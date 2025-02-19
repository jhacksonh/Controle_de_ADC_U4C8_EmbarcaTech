#include <stdio.h>
#include "pico/stdlib.h"
#include "oled.h"
#include "interrupt.h"
#include "pwm_util.h"
#include "adc_util.h"

// Definições dos pinos
#define PIN_I2C_SDA 14 // Pino SDA para comunicação I2C com o OLED
#define PIN_I2C_SCL 15 // Pino SCL para comunicação I2C com o OLED
#define PIN_JOY_X 27   // Pino do eixo X do joystick (conectado ao ADC)
#define PIN_JOY_Y 26   // Pino do eixo Y do joystick (conectado ao ADC)
#define PIN_JOY_BUTTON 22 // Pino do botão do joystick
#define PIN_BUTTON_A 5    // Pino do botão A

// Definições de variáveis globais
uint PIN_RGB_LED[3] = {13, 11, 12}; // Pinos do LED RGB (R, G, B)
uint PWM_WRAP = 4096; // Valor máximo do PWM (12 bits)
uint OLED_CENTER[2] = {(128 / 2 - 4), (64 / 2 - 4)}; // Centro do display OLED
uint JOY_CENTER_X = 2048, JOY_CENTER_Y = 2048; // Valores centrais do joystick (12 bits ADC)
bool LEDS_RB = true; // Controle para habilitar/desabilitar LEDs vermelho e azul
bool border_oled = true; // Controle para habilitar/desabilitar borda no OLED

// Protótipos das funções
void Callback_BTs(uint gpio, uint32_t events);
void manipulation_pixel_oled(int x, int y);
void create_border_oled();
void manipulation_RGBled_pwm(int x, int y);
void config_pin(uint pin, bool outPut, bool pullup);
void initialize_peripherals();

// Função para configurar um pino como entrada ou saída
void config_pin(uint pin, bool outPut, bool pullup) {
    gpio_init(pin); // Inicializa o pino
    gpio_set_dir(pin, outPut); // Configura como entrada ou saída
    if (pullup) gpio_pull_up(pin); // Habilita resistor de pull-up se necessário
    if (!pullup) gpio_put(pin, 0); // Define o pino como LOW se não for pull-up
}
// Função para inicializar todos os periféricos
void initialize_peripherals() {
    oled_Init(PIN_I2C_SDA, PIN_I2C_SCL); // Inicializa o display OLED
    setup_adc(PIN_JOY_X); // Configura o ADC para o eixo X do joystick
    setup_adc(PIN_JOY_Y); // Configura o ADC para o eixo Y do joystick
    config_pin(PIN_JOY_BUTTON, false, true); // Configura o botão do joystick como entrada com pull-up
    config_pin(PIN_BUTTON_A, false, true); // Configura o botão A como entrada com pull-up
    itr_SetCallbackFunction(Callback_BTs); // Define a função de callback para as interrupções
    itr_Interruption(PIN_BUTTON_A); // Habilita interrupção no botão A
    itr_Interruption(PIN_JOY_BUTTON); // Habilita interrupção no botão do joystick
    for (int i = 0; i < sizeof(PIN_RGB_LED) / sizeof(PIN_RGB_LED[0]); i++) {
        setup_pwm(PIN_RGB_LED[i], PWM_WRAP); // Configura os pinos do LED RGB para PWM
    }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial (para debug)
    initialize_peripherals(); // Inicializa todos os periféricos
    sleep_ms(1000); // Aguarda 1 segundo para estabilização

    // Lê os valores iniciais do joystick para calibrar o centro
    int x_joy = read_adc(1); // Lê o eixo X do joystick
    int y_joy = read_adc(0); // Lê o eixo Y do joystick
    
    JOY_CENTER_X = x_joy; // Define o centro do eixo X
    JOY_CENTER_Y = y_joy; // Define o centro do eixo Y

    // Loop principal
    while (true) {
        x_joy = read_adc(1); // Lê o eixo X do joystick
        y_joy = PWM_WRAP - read_adc(0); // Lê o eixo Y do joystick (invertido)
        manipulation_RGBled_pwm(x_joy, y_joy); // Controla o LED RGB com base no joystick
        manipulation_pixel_oled(x_joy, y_joy); // Atualiza o display OLED com base no joystick
        sleep_ms(10); // Pequena pausa para evitar leituras muito rápidas
    }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Função de callback para interrupções dos botões
void Callback_BTs(uint gpio, uint32_t events) {
    switch (gpio) {
        case PIN_BUTTON_A:
            LEDS_RB = !LEDS_RB; // Alterna o estado dos LEDs vermelho e azul
            break;
        case PIN_JOY_BUTTON:
            update_duty_cycle_pwm(PIN_RGB_LED[1], border_oled ? 100 : 0); // Alterna o LED verde
            border_oled = !border_oled; // Alterna o estado da borda do OLED
            break;
        default:
            break;
    }
}

// Função para controlar o LED RGB com base no joystick
void manipulation_RGBled_pwm(int x, int y) {
    static int center_tolerance = 250; // Tolerância para considerar o joystick no centro
    bool wrap_led_r = (x > JOY_CENTER_X + center_tolerance || x < JOY_CENTER_X - center_tolerance) && LEDS_RB; // Verifica se o eixo X está fora do centro
    bool wrap_led_b = (y > JOY_CENTER_Y + center_tolerance || y < JOY_CENTER_Y - center_tolerance) && LEDS_RB; // Verifica se o eixo Y está fora do centro
    update_duty_cycle_pwm(PIN_RGB_LED[0], wrap_led_r ? x : 0); // Atualiza o LED vermelho
    update_duty_cycle_pwm(PIN_RGB_LED[2], wrap_led_b ? y : 0); // Atualiza o LED azul
}

// Função para manipular o pixel no OLED com base no joystick
void manipulation_pixel_oled(int x, int y) {
    static int offset = 5; // Offset(deslocamento) para evitar que o pixel saia da tela
    int oledX = (x / 32)-1; // Converte o valor do joystick (0-4096) para a coordenada X do OLED (0-127)
    int oledY = (y / 64)-1; // Converte o valor do joystick (0-4096) para a coordenada Y do OLED (0-63)
    x = oledX < offset ? offset : (oledX > (127-8-offset)) ? (oledX - (8+offset)) : oledX-4; // Limita o valor de X para não sair da tela
    y = oledY < offset ? offset : (oledY > (63-8-offset)) ? (oledY - (8+offset)) : oledY-4; // Limita o valor de Y para não sair da tela
    oled_Clear(); // Limpa o display OLED
    create_border_oled(); // Desenha a borda no OLED
    oled_Draw_draw((uint8_t[]){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, x, y, 8, 8); //Desenha um bloco de 8x8 pixels na posição (x, y)
    oled_Update(); // Atualiza o display OLED
}

// Função para criar a borda no OLED
void create_border_oled() {
    if (border_oled) {
        oled_Draw_Rectangle(3, 3, 128 - 4, 64 - 4, border_oled, !border_oled); // Desenha uma borda simples
    } else {
        oled_Bold_Rectangle(2, 2, 124, 60); // Desenha uma borda em negrito
    }
}
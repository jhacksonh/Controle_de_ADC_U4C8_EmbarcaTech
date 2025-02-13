#include <stdio.h>
#include "pico/stdlib.h"
#include "oled.h"
#include "interrupt.h"
#include "pwm_util.h"
#include "adc_util.h"

// #define I2C_PORT i2c0
#define PIN_I2C_SDA 14
#define PIN_I2C_SCL 15

#define PIN_JOY_X 27 //JoyX
#define PIN_JOY_Y 26 //JoyY
#define PIN_JOY_BUTTON 22 //JoyButton
#define PIN_BUTTON_A 5

uint PIN_RGB_LED[3] = {13,11,12};
uint PWM_WRAP = 4096;
uint OLED_CENTER[2] = {(128/2-4),(64/2-4)};
uint JOY_CENTER_X = 2048,JOY_CENTER_Y = 2048;
bool LEDS_RB = true;
bool border_oled = false;

void Callback_BTs(uint gpio, uint32_t events);
void manipulation_pixel_oled(int x, int y);
void create_border_oled();
void manipulation_RGBled_pwm(int x, int y);

static void config_pin(uint pin, bool outPut, bool pullup) {
    gpio_init(pin); // Inicializa o pino
    gpio_set_dir(pin, outPut); // Configura a direção do pino (entrada ou saída)
    if(pullup)gpio_pull_up(pin); // Habilita o resistor de pull-up se necessário
    if(!pullup)gpio_put(pin, 0); // Define o pino como LOW se não for pull-up
}

void initialize_peripherals(){
    oled_Init(PIN_I2C_SDA, PIN_I2C_SCL);
    setup_adc(PIN_JOY_X);
    setup_adc(PIN_JOY_Y);
    config_pin(PIN_JOY_BUTTON, false, true);
    config_pin(PIN_BUTTON_A, false, true);
    itr_SetCallbackFunction(Callback_BTs);
    itr_Interruption(PIN_BUTTON_A);
    itr_Interruption(PIN_JOY_BUTTON);
    for (int i = 0; i < sizeof(PIN_RGB_LED)/sizeof(PIN_RGB_LED[0]); i++){
        setup_pwm(PIN_RGB_LED[i], PWM_WRAP);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(){
    stdio_init_all();
    
    initialize_peripherals();
    int x_joy = read_adc(1);
    int y_joy = read_adc(0);
    JOY_CENTER_X = x_joy;
    JOY_CENTER_Y = y_joy;
    
    while (true) {
        x_joy = read_adc(1);
        y_joy = 4096-read_adc(0);
        manipulation_RGBled_pwm(x_joy, y_joy);
        manipulation_pixel_oled(x_joy, y_joy);
        sleep_ms(5);
    }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Callback_BTs(uint gpio, uint32_t events){
    printf("Botao %d pressionado\n", gpio);
    switch(gpio){
        case PIN_BUTTON_A:
            LEDS_RB = !LEDS_RB;
            break;
        case PIN_JOY_BUTTON:
            update_duty_cycle_pwm(PIN_RGB_LED[1], border_oled?100:0);
            border_oled = !border_oled;
            break;
        default:
            break;
    }
}
void manipulation_pixel_oled(int x, int y){
    oled_Clear();
    int calcX = x/32-4;
    int calcY = y/64-4;
    x = calcX<5?5:calcX>=(127-4)?calcX-(5+4):calcX;
    y = calcY<5?5:calcY>=(63-4)?calcY-(5+4):calcY;
    create_border_oled();
    oled_Draw_draw((uint8_t[]){0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, x, y, 8, 8);
    oled_Update();
}
void create_border_oled(){
    oled_Draw_Rectangle(3,3,128-4,64-4, border_oled, !border_oled);
}
void manipulation_RGBled_pwm(int x, int y){
    bool wrap_led_r = (x > JOY_CENTER_X+200 || x < JOY_CENTER_X-200) && LEDS_RB;
    bool wrap_led_b = (y > JOY_CENTER_Y+200 || y < JOY_CENTER_Y-200) && LEDS_RB;
    update_duty_cycle_pwm(PIN_RGB_LED[0], wrap_led_r?x:0);
    update_duty_cycle_pwm(PIN_RGB_LED[2], wrap_led_b?y:0);
}
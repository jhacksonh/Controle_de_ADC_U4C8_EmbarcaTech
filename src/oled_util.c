#include "hardware/i2c.h"   // Biblioteca para comunicação I2C no Raspberry Pi Pico
#include "ssd1306.h"        // Biblioteca para controle do display OLED SSD1306
#include "font6x7.h"        // Fonte personalizada de 6x7 pixels
#include "oled.h"           // Cabeçalho para funções de controle do OLED

#define I2C_PORT i2c1       // Define a porta I2C usada (i2c1 no Raspberry Pi Pico)
#define ADDR 0x3C           // Endereço I2C do display OLED (0x3C é o padrão para muitos OLEDs)

// Dimensões do display OLED
uint8_t WIDTH = 128;        // Largura do display em pixels (128 para OLEDs comuns)
uint8_t HEIGHT = 64;        // Altura do display em pixels (64 para OLEDs comuns)

static ssd1306_t ssd;       // Estrutura para armazenar configurações e estado do display

// Definições de tamanho da fonte
static uint8_t font_width = 6;   // Largura de cada caractere da fonte em pixels
static uint8_t font_height = 7;  // Altura de cada caractere da fonte em pixels

// Índices de início das diferentes categorias de caracteres na fonte
static int FONT_START_0_9 = 16;   // Índice inicial para números (0-9) na fonte
static int FONT_START_ABC = 33;   // Índice inicial para letras maiúsculas (A-Z) na fonte
static int FONT_START_abc = 59;   // Índice inicial para letras minúsculas (a-z) na fonte

// Inicializa o display OLED via I2C
void oled_Init(uint pin_i2c_sda, uint pin_i2c_scl) {
  i2c_init(I2C_PORT, 400 * 1000);  // Inicializa o I2C a 400 kHz (velocidade padrão para OLEDs)
  gpio_set_function(pin_i2c_sda, GPIO_FUNC_I2C); // Define o pino SDA como função I2C
  gpio_set_function(pin_i2c_scl, GPIO_FUNC_I2C); // Define o pino SCL como função I2C
  gpio_pull_up(pin_i2c_sda); // Habilita resistor de pull-up no pino SDA
  gpio_pull_up(pin_i2c_scl); // Habilita resistor de pull-up no pino SCL

  ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDR, I2C_PORT); // Inicializa o display OLED com as configurações
  ssd1306_config(&ssd);     // Configura o display (inicializa o buffer e outras configurações)
  ssd1306_send_data(&ssd);  // Envia os dados iniciais para o display
  oled_Clear();             // Limpa o display (apaga todos os pixels)
}

// Desenha uma matriz de pixels no display
void oled_Draw_draw(uint8_t draw[], uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
  for (uint8_t i = 0; i < height; i++) {  // Itera sobre cada linha da matriz de pixels
    uint8_t line = draw[i];  // Obtém a linha de pixels do caractere
    for (uint8_t j = 0; j < width; j++) {  // Itera sobre cada pixel da linha
      uint rotX = x + width - j; // Inverte a ordem dos bits para o desenho correto (da direita para a esquerda)
      ssd1306_pixel(&ssd, rotX, y + i, line & (1 << j)); // Define o pixel no display (liga ou desliga)
    }
  }
}

// Escreve um caractere no display
void oled_Write_Char(char c, uint8_t x, uint8_t y) {
  uint16_t index = 0;  // Índice para acessar a fonte na memória
  if (c >= ' ' && c <= '/') index = (c - ' ') * font_height; // Caracteres especiais (espaço até '/')
  else if (c >= 'A' && c <= 'Z') index = (c - 'A' + FONT_START_ABC) * font_height; // Letras maiúsculas (A-Z)
  else if (c >= 'a' && c <= 'z') index = (c - 'a' + FONT_START_abc) * font_height; // Letras minúsculas (a-z)
  else if (c >= '0' && c <= '@') index = (c - '0' + FONT_START_0_9) * font_height; // Números e pontuações (0-9 e '@')

  for (uint8_t i = 0; i < font_height; i++) {  // Itera sobre cada linha do caractere
    uint8_t line = font[index + i];  // Obtém a linha do caractere da fonte
    for (uint8_t j = 0; j < font_width; j++) {  // Itera sobre cada pixel da linha
      uint rotX = x + (font_width - 1) - j; // Inverte a ordem dos bits (da direita para a esquerda)
      ssd1306_pixel(&ssd, rotX, y + i, line & (1 << j)); // Define o pixel no display (liga ou desliga)
    }
  }
}

// Escreve uma string de caracteres no display
void oled_Write_String(const char *str, uint8_t x, uint8_t y) {
  ssd1306_t *ssd_p = &ssd;  // Ponteiro para a estrutura do display
  uint8_t spacing = font_width + 1; // Espaço entre caracteres (1 pixel de espaçamento)

  while (*str) {  // Itera sobre cada caractere da string até o final ('\0')
    oled_Write_Char(*str++, x, y);  // Desenha o caractere atual e avança para o próximo
    x += spacing;  // Move a posição horizontalmente para o próximo caractere

    if (x + spacing >= ssd_p->width) {  // Se passar da largura do display
      x = 0;  // Reseta a posição horizontal para o início
      y += font_height + 1;  // Move para a próxima linha
    }
    if (y + font_height + 1 >= ssd_p->height) {  // Se ultrapassar a altura do display
      break;  // Sai do loop (não desenha mais caracteres)
    }
  }
}

// Desenha um retângulo no display
void oled_Draw_Rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool value, bool fill) {
    ssd1306_rect(&ssd, x, y, width, height, value, fill); // Desenha um retângulo (sólido ou contorno)
}

// Desenha um retângulo em destaque (alterna entre preenchido e contorno)
void oled_Bold_Rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
  static bool fill = true;  // Variável para alternar entre preenchido e contorno
  fill = !fill;  // Inverte o estado (preenchido/contorno)
  ssd1306_fill(&ssd, !fill); // Limpa o display (define todos os pixels como apagados ou ligados)
  ssd1306_rect(&ssd, x, y, width, height, fill, !fill); // Desenha o retângulo com o estado atual
}

// Atualiza o display após alterações
void oled_Update() {
    ssd1306_send_data(&ssd); // Envia os dados do buffer para o display (atualiza a tela)
}

// Limpa o display, apagando todos os pixels
void oled_Clear() {
    ssd1306_fill(&ssd, false); // Define todos os pixels como apagados (limpa a tela)
}
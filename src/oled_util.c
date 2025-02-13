#include "hardware/i2c.h"   // Biblioteca para comunicação I2C no Raspberry Pi Pico
#include "ssd1306.h"        // Biblioteca para controle do display OLED SSD1306
#include "font6x7.h"        // Fonte personalizada de 6x7 pixels
#include "oled.h"           // Cabeçalho para funções de controle do OLED

#define I2C_PORT i2c1       // Define a porta I2C usada
#define ADDR 0x3C           // Endereço I2C do display OLED

// Dimensões do display OLED
uint8_t WIDTH = 128;        
uint8_t HEIGHT = 64;        

static ssd1306_t ssd;       // Estrutura para armazenar configurações do display

// Definições de tamanho da fonte
static uint8_t font_width = 6;   
static uint8_t font_height = 7;  

// Índices de início das diferentes categorias de caracteres na fonte
static int FONT_START_0_9 = 16;   
static int FONT_START_ABC = 33;   
static int FONT_START_abc = 59;   

// Inicializa o display OLED via I2C
void oled_Init(uint pin_i2c_sda, uint pin_i2c_scl){
  i2c_init(I2C_PORT, 400 * 1000);  // Inicializa o I2C a 400 kHz
  gpio_set_function(pin_i2c_sda, GPIO_FUNC_I2C); // Define SDA como função I2C
  gpio_set_function(pin_i2c_scl, GPIO_FUNC_I2C); // Define SCL como função I2C
  gpio_pull_up(pin_i2c_sda); // Habilita pull-up no SDA
  gpio_pull_up(pin_i2c_scl); // Habilita pull-up no SCL

  ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDR, I2C_PORT); // Inicializa o display OLED
  ssd1306_config(&ssd);     // Configura o display
  ssd1306_send_data(&ssd);  // Atualiza os dados do display
  oled_Clear();             // Limpa o display
}

// Desenha uma matriz de pixels no display
void oled_Draw_draw(uint8_t draw[], uint8_t x, uint8_t y, uint8_t width, uint8_t height){
  for (uint8_t i = 0; i < height; i++){  
    uint8_t line = draw[i];  // Obtém a linha de pixels do caractere
    for (uint8_t j = 0; j < width; j++){  
      uint rotX = x + width - j; // Inverte a ordem dos bits para o desenho correto
      ssd1306_pixel(&ssd, rotX, y + i, line & (1 << j)); // Define o pixel no display
    }
  }
}

// Escreve um caractere no display
void oled_Write_Char(char c, uint8_t x, uint8_t y){
  uint16_t index = 0;
  if (c >= ' ' && c <= '/') index = (c - ' ') * font_height; // Caracteres especiais
  else if (c >= 'A' && c <= 'Z') index = (c - 'A' + FONT_START_ABC) * font_height; // Letras maiúsculas
  else if (c >= 'a' && c <= 'z') index = (c - 'a' + FONT_START_abc) * font_height; // Letras minúsculas
  else if (c >= '0' && c <= '@') index = (c - '0' + FONT_START_0_9) * font_height; // Números e pontuações

  for (uint8_t i = 0; i < font_height; i++){  
    uint8_t line = font[index + i];  // Obtém a linha do caractere
    for (uint8_t j = 0; j < font_width; j++){  
      uint rotX = x + (font_width - 1) - j; // Inverte a ordem dos bits
      ssd1306_pixel(&ssd, rotX, y + i, line & (1 << j)); // Define o pixel no display
    }
  }
}

// Escreve uma string de caracteres no display
void oled_Write_String(const char *str, uint8_t x, uint8_t y){
  ssd1306_t *ssd_p = &ssd;
  uint8_t spacing = font_width + 1; // Espaço entre caracteres (1 pixel)

  while (*str){  
    oled_Write_Char(*str++, x, y);  // Desenha o caractere atual e avança para o próximo
    x += spacing;  // Move a posição horizontalmente

    if (x + spacing >= ssd_p->width){  // Se passar da largura do display
      x = 0;  // Reseta a posição horizontal
      y += font_height + 1;  // Move para a próxima linha
    }
    if (y + font_height + 1 >= ssd_p->height){  // Se ultrapassar a altura do display
      break;  // Sai do loop
    }
  }
}

// Desenha um retângulo no display
void oled_Draw_Rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool value, bool fill){
    ssd1306_rect(&ssd, x, y, width, height, value, fill); // Desenha um retângulo sólido ou contorno
}

// Atualiza o display após alterações
void oled_Update(){
    ssd1306_send_data(&ssd); // Envia os dados do buffer para o display
}

// Limpa o display, apagando todos os pixels
void oled_Clear(){
    ssd1306_fill(&ssd, false); // Define todos os pixels como apagados
    ssd1306_send_data(&ssd);   // Atualiza o display
}

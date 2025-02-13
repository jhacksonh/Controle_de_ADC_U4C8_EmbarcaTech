#ifndef SSD1306_H  // Verifica se SSD1306_H não foi definido anteriormente
#define SSD1306_H  // Define SSD1306_H para evitar inclusão múltipla

#include <stdlib.h>  // Inclui funções de manipulação de memória e alocação
#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/i2c.h"  // Inclui a biblioteca I2C para comunicação com o dispositivo

extern uint8_t WIDTH, HEIGHT;
// Enumeração dos comandos para o controle do display SSD1306
typedef enum {
  SET_CONTRAST = 0x81,  // Define o comando para configurar o contraste
  SET_ENTIRE_ON = 0xA4,  // Comando para ativar todo o display
  SET_NORM_INV = 0xA6,  // Comando para definir a inversão de cores normal
  SET_DISP = 0xAE,  // Comando para ligar ou desligar o display
  SET_MEM_ADDR = 0x20,  // Comando para definir o tipo de endereçamento de memória
  SET_COL_ADDR = 0x21,  // Comando para configurar o endereço da coluna
  SET_PAGE_ADDR = 0x22,  // Comando para configurar o endereço da página
  SET_DISP_START_LINE = 0x40,  // Comando para definir a linha de início de exibição
  SET_SEG_REMAP = 0xA0,  // Comando para reverter o mapeamento dos segmentos
  SET_MUX_RATIO = 0xA8,  // Comando para configurar a razão de multiplexação
  SET_COM_OUT_DIR = 0xC0,  // Comando para configurar a direção das linhas de controle
  SET_DISP_OFFSET = 0xD3,  // Comando para configurar o deslocamento do display
  SET_COM_PIN_CFG = 0xDA,  // Comando para configurar os pinos de controle
  SET_DISP_CLK_DIV = 0xD5,  // Comando para configurar o divisor de clock
  SET_PRECHARGE = 0xD9,  // Comando para configurar o tempo de pré-carga
  SET_VCOM_DESEL = 0xDB,  // Comando para configurar a seleção de VCOM
  SET_CHARGE_PUMP = 0x8D  // Comando para ativar a bomba de carga
} ssd1306_command_t;
// Estrutura para representar o display SSD1306
typedef struct {
  uint8_t width, height, pages, address;  // Propriedades do display (largura, altura, páginas, endereço I2C)
  i2c_inst_t *i2c_port;  // Ponteiro para a porta I2C utilizada para comunicação
  bool external_vcc;  // Indica se o display usa fonte externa de alimentação
  uint8_t *ram_buffer;  // Buffer de memória RAM para armazenar os dados a serem exibidos
  size_t bufsize;  // Tamanho do buffer de dados
  uint8_t port_buffer[2];  // Buffer para armazenar dados e comandos para comunicação I2C
} ssd1306_t;
// Funções para inicializar e configurar o display SSD1306
void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c);
void ssd1306_config(ssd1306_t *ssd);  // Função de configuração do display
// Funções para enviar comandos e dados ao display
void ssd1306_command(ssd1306_t *ssd, uint8_t command);  // Envia um comando ao display
void ssd1306_send_data(ssd1306_t *ssd);  // Envia dados para o display
// Funções para desenhar no display (pixels, linhas, retângulos, etc.)
void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value);  // Desenha um pixel
void ssd1306_fill(ssd1306_t *ssd, bool value);  // Preenche o display com um valor
void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill);  // Desenha um retângulo
void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value);  // Desenha uma linha
void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value);  // Desenha uma linha horizontal
void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value);  // Desenha uma linha vertical

#endif  // Fim da definição de SSD1306_H

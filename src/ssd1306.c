#include "ssd1306.h"

// Função de inicialização do display SSD1306
void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c) {
  ssd->width = width;  // Define a largura do display
  ssd->height = height;  // Define a altura do display
  ssd->pages = height / 8U;  // Calcula o número de páginas (a altura dividida por 8, pois o display usa 8 bits por linha)
  ssd->address = address;  // Endereço I2C do display
  ssd->i2c_port = i2c;  // Porta I2C utilizada
  ssd->bufsize = ssd->pages * ssd->width + 1;  // Tamanho do buffer da RAM
  ssd->ram_buffer = calloc(ssd->bufsize, sizeof(uint8_t));  // Aloca memória para o buffer do display
  ssd->ram_buffer[0] = 0x40;  // Configura o primeiro byte do buffer para dados
  ssd->port_buffer[0] = 0x80;  // Configura o primeiro byte do buffer da porta para comandos
}
// Função de configuração do display SSD1306
void ssd1306_config(ssd1306_t *ssd) {
  // Envia uma série de comandos de inicialização ao display
  ssd1306_command(ssd, SET_DISP | 0x00);  // Desliga o display
  ssd1306_command(ssd, SET_MEM_ADDR);  // Define o modo de endereçamento da memória
  ssd1306_command(ssd, 0x01);  // Define o endereçamento horizontal
  ssd1306_command(ssd, SET_DISP_START_LINE | 0x00);  // Define a linha de início do display
  ssd1306_command(ssd, SET_SEG_REMAP | 0x01);  // Inverte a direção do mapeamento de segmentos
  ssd1306_command(ssd, SET_MUX_RATIO);  // Define a razão de multiplexação
  ssd1306_command(ssd, HEIGHT - 1);  // Define a altura do display
  ssd1306_command(ssd, SET_COM_OUT_DIR | 0x08);  // Define a direção das linhas de controle
  ssd1306_command(ssd, SET_DISP_OFFSET);  // Define o deslocamento do display
  ssd1306_command(ssd, 0x00);  // Define o deslocamento para 0
  ssd1306_command(ssd, SET_COM_PIN_CFG);  // Configura os pinos de controle
  ssd1306_command(ssd, 0x12);  // Configuração adicional dos pinos
  ssd1306_command(ssd, SET_DISP_CLK_DIV);  // Define a divisão do clock
  ssd1306_command(ssd, 0x80);  // Configuração do clock
  ssd1306_command(ssd, SET_PRECHARGE);  // Define o tempo de pré-carga
  ssd1306_command(ssd, 0xF1);  // Configuração do tempo de pré-carga
  ssd1306_command(ssd, SET_VCOM_DESEL);  // Configura a seleção de VCOM
  ssd1306_command(ssd, 0x30);  // Definição da tensão de VCOM
  ssd1306_command(ssd, SET_CONTRAST);  // Define o contraste
  ssd1306_command(ssd, 0xFF);  // Configura o contraste máximo
  ssd1306_command(ssd, SET_ENTIRE_ON);  // Define o display como "ligado totalmente"
  ssd1306_command(ssd, SET_NORM_INV);  // Define a inversão de cores (normal)
  ssd1306_command(ssd, SET_CHARGE_PUMP);  // Ativa a bomba de carga
  ssd1306_command(ssd, 0x14);  // Configuração da bomba de carga
  ssd1306_command(ssd, SET_DISP | 0x01);  // Liga o display
}
// Função para enviar comandos ao display SSD1306
void ssd1306_command(ssd1306_t *ssd, uint8_t command) {
  ssd->port_buffer[1] = command;  // Coloca o comando no buffer de dados
  i2c_write_blocking(  // Envia o comando via I2C
    ssd->i2c_port,
    ssd->address,
    ssd->port_buffer,
    2,
    false
  );
}
// Função para enviar dados (buffer) ao display SSD1306
void ssd1306_send_data(ssd1306_t *ssd) {
  ssd1306_command(ssd, SET_COL_ADDR);  // Define o endereço da coluna
  ssd1306_command(ssd, 0);  // Endereço inicial da coluna
  ssd1306_command(ssd, ssd->width - 1);  // Endereço final da coluna
  ssd1306_command(ssd, SET_PAGE_ADDR);  // Define o endereço da página
  ssd1306_command(ssd, 0);  // Página inicial
  ssd1306_command(ssd, ssd->pages - 1);  // Página final
  i2c_write_blocking(  // Envia os dados do buffer ao display
    ssd->i2c_port,
    ssd->address,
    ssd->ram_buffer,
    ssd->bufsize,
    false
  );
}
// Função para desenhar um pixel no display
void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value) {
  uint16_t index = (y >> 3) + (x << 3) + 1;  // Calcula o índice no buffer de RAM
  uint8_t pixel = (y & 0b111);  // Calcula a posição do pixel dentro do byte
  if (value)ssd->ram_buffer[index] |= (1 << pixel);// Se o valor for verdadeiro, acende o pixel
  else ssd->ram_buffer[index] &= ~(1 << pixel);// Se o valor for falso, apaga o pixel
}
// Função para preencher toda a tela com um valor (on/off)
void ssd1306_fill(ssd1306_t *ssd, bool value) {
    // Itera por todas as posições do display e define cada pixel
    for (uint8_t y = 0; y < ssd->height; ++y) {
        for (uint8_t x = 0; x < ssd->width; ++x) {
            ssd1306_pixel(ssd, x, y, value);
        }
    }
}
// Função para desenhar um retângulo no display
void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill) {
  // Desenha as bordas do retângulo
  for (uint8_t x = left; x < left + width; ++x) {
    ssd1306_pixel(ssd, x, top, value);
    ssd1306_pixel(ssd, x, top + height - 1, value);
  }
  for (uint8_t y = top; y < top + height; ++y) {
    ssd1306_pixel(ssd, left, y, value);
    ssd1306_pixel(ssd, left + width - 1, y, value);
  }
  // Preenche o retângulo se necessário
  if (fill) {
    for (uint8_t x = left + 1; x < left + width - 1; ++x) {
      for (uint8_t y = top + 1; y < top + height - 1; ++y) {
        ssd1306_pixel(ssd, x, y, value);
      }
    }
  }
}

// Função para desenhar uma linha entre dois pontos
void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    while (true) {
        ssd1306_pixel(ssd, x0, y0, value); // Desenha o pixel atual
        if (x0 == x1 && y0 == y1) break; // Termina quando alcança o ponto final
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}
// Função para desenhar uma linha horizontal
void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value) {
  // Desenha cada pixel na linha horizontal
  for (uint8_t x = x0; x <= x1; ++x)ssd1306_pixel(ssd, x, y, value);
}
// Função para desenhar uma linha vertical
void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value) {
  // Desenha cada pixel na linha vertical
  for (uint8_t y = y0; y <= y1; ++y)ssd1306_pixel(ssd, x, y, value);
}
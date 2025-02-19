#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "adc_util.h"

// Função para configurar o ADC (Conversor Analógico-Digital)
void setup_adc(uint pin) {
    adc_init();              // Inicializa o hardware do ADC
    adc_gpio_init(pin);      // Configura o pino GPIO especificado para funcionar como entrada analógica
}

// Função para ler o valor do ADC de um canal específico
int read_adc(uint input) {
    adc_select_input(input); // Seleciona o canal de entrada do ADC (0, 1, 2, etc.)
    return adc_read();       // Realiza a leitura do valor analógico e retorna o valor digital convertido
}
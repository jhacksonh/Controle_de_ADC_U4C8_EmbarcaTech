#include <stdio.h>              // Biblioteca padrão para funções de entrada e saída
#include "pico/stdlib.h"       // Biblioteca para funções específicas do Raspberry Pi Pico
#include "hardware/pwm.h"      // Biblioteca para controle de PWM (Pulse Width Modulation)
#include "pwm_util.h"           // Cabeçalho para funções de controle de PWM

// Função para configurar o PWM em um pino específico
void setup_pwm(uint pin, uint pwm_wrap) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura o pino para função PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o número do slice PWM associado ao pino
    pwm_set_clkdiv(slice_num, 1.0); // Define o divisor de clock do slice PWM como 1.0 (sem divisão)
    pwm_set_wrap(slice_num, pwm_wrap); // Define o valor de "wrap" (máximo) para o contador PWM
    pwm_set_gpio_level(pin, 0); // Inicializa o nível do PWM como 0 (duty cycle de 0%)
    pwm_set_enabled(slice_num, true); // Habilita o slice PWM
}

// Função para atualizar o duty cycle do PWM em um pino específico
void update_duty_cycle_pwm(uint pin, uint duty_cycle) {
    pwm_set_gpio_level(pin, duty_cycle); // Define o nível do PWM (duty cycle) no pino
}
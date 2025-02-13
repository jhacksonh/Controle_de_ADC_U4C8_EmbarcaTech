#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"     // Biblioteca padrão do Raspberry Pi Pico
#include "interrupt.h"       // Cabeçalho para funções de interrupção

static uint32_t LAST_TIME = 0;  // Variável para armazenar o último tempo da interrupção (para debounce)
static uint32_t WAIT_TIME = 200000;  // Tempo de debounce em microssegundos

// Estrutura que contém um ponteiro para função de callback
typedef struct{
    void (*func)(uint, uint32_t);  // Ponteiro para uma função que recebe um pino GPIO e um evento
} CallbackFunction;
CallbackFunction callback_function; // Instância da estrutura de callback

// Função para verificar se já passou tempo suficiente para considerar um novo acionamento (debounce)
static bool itr_Debounce(){
  uint32_t current_time = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual em microssegundos
  bool valid_time = current_time - LAST_TIME > WAIT_TIME; // Verifica se o tempo de debounce foi atingido
  if(valid_time) LAST_TIME = current_time; // Atualiza o tempo da última interrupção válida
  return valid_time; // Retorna se o evento é válido
}

// Callback executado quando ocorre uma interrupção no botão
static void itr_Button_Callback(uint gpio, uint32_t events) {
  if(itr_Debounce()){ // Executa a função de callback apenas se o debounce permitir
    callback_function.func(gpio, events);
  }
}

// Define a função de callback e o tempo de debounce(pode ser 0={fica 200000 padrão})
void itr_SetCallbackFunction(void (*func)(uint, uint32_t)){
    callback_function.func = func;
}

// Configura a interrupção em um pino específico
void itr_Interruption(uint pin){
  gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_FALL, true, &itr_Button_Callback);
  // Ativa a interrupção no pino, acionada na borda de descida (pressionar botão)
}

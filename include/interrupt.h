#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "pico/stdlib.h"

void itr_SetCallbackFunction(void (*func)(uint, uint32_t));
void itr_Interruption(uint pin);

#endif
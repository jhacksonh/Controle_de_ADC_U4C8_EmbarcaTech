#ifndef ADC_UTIL_H
#define ADC_UTIL_H

#include "pico/stdlib.h"

void setup_adc( uint pin);
int read_adc(uint input);

#endif
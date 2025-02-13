#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "adc_util.h"

void setup_adc( uint pin){
    adc_init(); 
    adc_gpio_init(pin); 
}
int read_adc(uint input){
    adc_select_input(input);
    return adc_read();
}
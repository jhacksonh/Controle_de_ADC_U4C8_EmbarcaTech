#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pwm_util.h"


void setup_pwm(uint pin, uint pwm_wrap) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice_num, 16);
    pwm_set_wrap(slice_num, pwm_wrap);
    pwm_set_gpio_level(pin, 0);
    pwm_set_enabled(slice_num, true);
}

void update_duty_cycle_pwm(uint pin, uint duty_cycle) {
    pwm_set_gpio_level(pin, duty_cycle);
}
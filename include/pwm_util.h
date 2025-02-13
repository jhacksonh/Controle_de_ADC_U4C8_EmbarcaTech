#ifndef PWM_UTIL_H
#define PWM_UTIL_H

#include <stdlib.h>

void setup_pwm(uint pin, uint pwm_wrap);
void update_duty_cycle_pwm(uint pin, uint duty_cycle);

#endif
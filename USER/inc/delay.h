#ifndef __SYSTEM_DELAY_H
#define __SYSTEM_DELAY_H

#include <stdint.h>
#include <stdbool.h>

#include "cs32f0xx_rcu.h"


void delay_init(uint32_t system_clock);
void delay_ms(uint32_t time);
bool wait_delay_time(uint32_t start_time, uint32_t delay_time);
uint32_t get_current_time(void);

#endif

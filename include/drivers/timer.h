#ifndef TIMER_H
#define TIMER_H

#include "../kernel.h"

// Timer constants
#define PIT_FREQUENCY       1193182
#define TIMER_IRQ           0
#define TIMER_PORT_CMD      0x43
#define TIMER_PORT_DATA     0x40

// Timer modes
#define TIMER_MODE_PERIODIC 0x36

// Function declarations
void timer_init(uint32_t frequency);
void timer_handler(void);
uint32_t timer_get_ticks(void);
void timer_wait(uint32_t ticks);
uint32_t timer_get_uptime_seconds(void);

#endif

#include "../../include/drivers/timer.h"
#include "../../include/vga.h"
#include "../../include/kernel.h"

static volatile uint32_t timer_ticks = 0;
static uint32_t timer_frequency = 0;

void timer_init(uint32_t frequency) {
    timer_frequency = frequency;
    uint32_t divisor = PIT_FREQUENCY / frequency;
    
    vga_puts("Initializing timer at ");
    vga_put_dec(frequency);
    vga_puts(" Hz...\n");
    
    // Send command byte
    outb(TIMER_PORT_CMD, TIMER_MODE_PERIODIC);
    
    // Send frequency divisor
    outb(TIMER_PORT_DATA, (uint8_t)(divisor & 0xFF));
    outb(TIMER_PORT_DATA, (uint8_t)((divisor >> 8) & 0xFF));
    
    vga_puts("Timer initialized\n");
}

void timer_handler(void) {
    timer_ticks++;
    
    // Display uptime every 100 ticks (approximately every second at 100Hz)
    if (timer_frequency > 0 && timer_ticks % timer_frequency == 0) {
        // Update uptime display could be added here
    }
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

void timer_wait(uint32_t ticks) {
    uint32_t start = timer_ticks;
    while (timer_ticks - start < ticks) {
        // Wait
    }
}

uint32_t timer_get_uptime_seconds(void) {
    if (timer_frequency == 0) return 0;
    return timer_ticks / timer_frequency;
}

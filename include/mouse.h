#ifndef MOUSE_H
#define MOUSE_H

#include "kernel.h"

// Mouse packet structure
typedef struct {
    uint8_t flags;      // Button states and overflow flags
    int16_t x_movement; // X movement delta
    int16_t y_movement; // Y movement delta
    int8_t z_movement;  // Scroll wheel movement (for wheel mice)
} mouse_packet_t;

// Mouse button flags
#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_RIGHT_BUTTON  0x02
#define MOUSE_MIDDLE_BUTTON 0x04
#define MOUSE_X_OVERFLOW    0x40
#define MOUSE_Y_OVERFLOW    0x80

// Mouse state structure
typedef struct {
    int16_t x;              // Current cursor X position
    int16_t y;              // Current cursor Y position
    uint8_t buttons;        // Current button states
    uint8_t initialized;    // Mouse initialization status
} mouse_state_t;

// Mouse controller ports
#define MOUSE_DATA_PORT     0x60
#define MOUSE_STATUS_PORT   0x64
#define MOUSE_COMMAND_PORT  0x64

// Mouse commands
#define MOUSE_CMD_ENABLE_AUX    0xA8
#define MOUSE_CMD_DISABLE_AUX   0xA7
#define MOUSE_CMD_TEST_AUX      0xA9
#define MOUSE_CMD_GET_STATUS    0x20
#define MOUSE_CMD_SET_STATUS    0x60
#define MOUSE_CMD_WRITE_AUX     0xD4

// PS/2 mouse commands
#define MOUSE_SET_RESOLUTION    0xE8
#define MOUSE_SET_SCALING       0xE6
#define MOUSE_SET_STREAM_MODE   0xEA
#define MOUSE_ENABLE_REPORTING  0xF4
#define MOUSE_DISABLE_REPORTING 0xF5
#define MOUSE_RESET             0xFF
#define MOUSE_GET_ID            0xF2

// Function declarations
int mouse_init(void);
void mouse_interrupt_handler(void);
mouse_state_t* mouse_get_state(void);
void mouse_set_cursor_position(int16_t x, int16_t y);
void mouse_show_cursor(void);
void mouse_hide_cursor(void);
int mouse_wait_input(void);
int mouse_wait_output(void);
void mouse_write_command(uint8_t command);
void mouse_write_data(uint8_t data);
uint8_t mouse_read_data(void);

#endif // MOUSE_H

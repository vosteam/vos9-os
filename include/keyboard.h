#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kernel.h"

// Keyboard driver functions
void keyboard_init(void);
char keyboard_getchar(void);
uint8_t keyboard_get_scancode(void);
void keyboard_handler(void);

// Keyboard event types
#define KEY_EVENT_PRESS   0x01
#define KEY_EVENT_RELEASE 0x02

// Special keys
#define KEY_ARROW_UP      0x48
#define KEY_ARROW_DOWN    0x50
#define KEY_ARROW_LEFT    0x4B
#define KEY_ARROW_RIGHT   0x4D
#define KEY_BACKSPACE     0x0E
#define KEY_DELETE        0x53
#define KEY_HOME          0x47
#define KEY_END           0x4F
#define KEY_PAGE_UP       0x49
#define KEY_PAGE_DOWN     0x51

// Structure to hold keyboard event information
typedef struct {
    uint8_t scancode;
    char    ascii;
    uint8_t type; // KEY_EVENT_PRESS or KEY_EVENT_RELEASE
    uint8_t special; // Non-zero if this is a special key
} key_event_t;

// Function to get a full key event
key_event_t keyboard_get_event(void);

// Keyboard constants
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#endif

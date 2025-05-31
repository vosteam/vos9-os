#include "../include/keyboard.h"

static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    0, 0, ' '
};

// Shifted scancode to ASCII mapping (US QWERTY layout)
static const char scancode_to_ascii_shifted[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, // 0-14
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', // 15-28 (Enter)
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', // 29-41 (LShift)
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, // 42-54 (RShift)
    0, 0, ' ' // 55-57 (Space)
    // Add more if needed for other keys
};

// Keep track of modifier keys
static uint8_t shift_pressed = 0;
// Add other modifiers like ctrl_pressed, alt_pressed if needed

#define SCANCODE_LSHIFT 0x2A
#define SCANCODE_RSHIFT 0x36
#define SCANCODE_LSHIFT_RELEASE (SCANCODE_LSHIFT | 0x80)
#define SCANCODE_RSHIFT_RELEASE (SCANCODE_RSHIFT | 0x80)

void keyboard_init(void) {
    // Initialize keyboard controller
    // For now, just a placeholder
}

uint8_t keyboard_get_scancode(void) {
    return inb(KEYBOARD_DATA_PORT);
}

// This function is now internal, keyboard_get_event should be used by external code
static char scancode_to_char(uint8_t scancode) {
    const char* map = shift_pressed ? scancode_to_ascii_shifted : scancode_to_ascii;
    size_t map_size = shift_pressed ? sizeof(scancode_to_ascii_shifted) : sizeof(scancode_to_ascii);

    if (scancode < map_size && map[scancode]) {
        return map[scancode];
    }
    return 0; // No valid character
}

// New function to get a full key event including press/release type
key_event_t keyboard_get_event(void) {
    key_event_t event = {0, 0, 0, 0};
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    event.scancode = scancode;

    // Check for extended scancode (0xE0 prefix)
    static uint8_t extended = 0;
    if (scancode == 0xE0) {
        extended = 1;
        // Return empty event, wait for next scancode
        return event;
    }

    // Check for Shift press/release
    if (scancode == SCANCODE_LSHIFT || scancode == SCANCODE_RSHIFT) {
        shift_pressed = 1;
        event.type = KEY_EVENT_PRESS;
        return event;
    } else if (scancode == SCANCODE_LSHIFT_RELEASE || scancode == SCANCODE_RSHIFT_RELEASE) {
        shift_pressed = 0;
        event.type = KEY_EVENT_RELEASE;
        return event;
    }

    if (scancode & 0x80) { // Key release (MSB is set)
        event.type = KEY_EVENT_RELEASE;
        event.scancode &= 0x7F; // Clear the MSB to get the make code
    } else { // Key press
        event.type = KEY_EVENT_PRESS;
    }

    // Handle extended scancodes (arrow keys, etc.)
    if (extended) {
        extended = 0; // Reset extended flag
        event.special = 1;
        switch (event.scancode) {
            case 0x48: // Up arrow
                event.scancode = KEY_ARROW_UP;
                break;
            case 0x50: // Down arrow
                event.scancode = KEY_ARROW_DOWN;
                break;
            case 0x4B: // Left arrow
                event.scancode = KEY_ARROW_LEFT;
                break;
            case 0x4D: // Right arrow
                event.scancode = KEY_ARROW_RIGHT;
                break;
            default:
                event.special = 0;
                break;
        }
    } else {
        // Handle normal keys
        if (event.scancode == KEY_BACKSPACE) {
            event.special = 1;
        } else {
            event.ascii = scancode_to_char(event.scancode);
        }
    }
    
    return event;
}

// keyboard_getchar is now a wrapper around keyboard_get_event for simplicity if only pressed chars are needed
char keyboard_getchar(void) {
    key_event_t event;
    // Loop until a key press event with a valid character occurs
    while(1) {
        // Wait for keyboard data to be available
        while (!(inb(KEYBOARD_STATUS_PORT) & 0x01)) {
            // You might want to add a short delay or a CPU relaxation instruction here
            // For now, just busy-wait
        }
        event = keyboard_get_event();
        if (event.type == KEY_EVENT_PRESS && event.ascii != 0 && !event.special) {
            return event.ascii;
        }
        // Optionally handle release events or other non-ASCII press events here
    }
}

void keyboard_handler(void) {
    if (inb(KEYBOARD_STATUS_PORT) & 0x01) { // Check if data is available
        key_event_t event = keyboard_get_event();

        if (event.type == KEY_EVENT_PRESS) {
            if (event.special) {
                // Handle special keys
                switch (event.scancode) {
                    case KEY_ARROW_UP:
                        extern void vga_move_cursor_up(void);
                        vga_move_cursor_up();
                        break;
                    case KEY_ARROW_DOWN:
                        extern void vga_move_cursor_down(void);
                        vga_move_cursor_down();
                        break;
                    case KEY_ARROW_LEFT:
                        extern void vga_move_cursor_left(void);
                        vga_move_cursor_left();
                        break;
                    case KEY_ARROW_RIGHT:
                        extern void vga_move_cursor_right(void);
                        vga_move_cursor_right();
                        break;
                    case KEY_BACKSPACE:
                        extern void vga_backspace(void);
                        vga_backspace();
                        break;
                    default:
                        break;
                }
            } else if (event.ascii != 0) {
                // Echo character to screen
                extern void vga_putchar(char c);
                vga_putchar(event.ascii);
            }
        } else if (event.type == KEY_EVENT_RELEASE) {
            // Optionally, handle key release events here
            // For example, print a message or update state
            // vga_puts("[Key Up]");
        }
        // Handle other events like Shift press/release if needed for direct feedback
    }
}

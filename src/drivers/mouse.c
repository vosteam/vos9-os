#include "mouse.h"
#include "vga.h"
#include "kernel.h"

// Global mouse state
static mouse_state_t mouse_state = {0};
static uint8_t mouse_packet_index = 0;
static uint8_t mouse_packet_data[4] = {0};
static int16_t screen_width = 80;
static int16_t screen_height = 25;

// Previous cursor position for restoration
static int16_t prev_cursor_x = 0;
static int16_t prev_cursor_y = 0;
static char prev_cursor_char = ' ';

// Mouse cursor character
#define MOUSE_CURSOR_CHAR 0xDB  // Solid block character

int mouse_wait_input(void) {
    int timeout = 100000;
    while (timeout--) {
        if ((inb(MOUSE_STATUS_PORT) & 0x01) != 0) {
            return 0;
        }
    }
    return -1; // Timeout
}

int mouse_wait_output(void) {
    int timeout = 100000;
    while (timeout--) {
        if ((inb(MOUSE_STATUS_PORT) & 0x02) == 0) {
            return 0;
        }
    }
    return -1; // Timeout
}

void mouse_write_command(uint8_t command) {
    mouse_wait_output();
    outb(MOUSE_COMMAND_PORT, command);
}

void mouse_write_data(uint8_t data) {
    mouse_wait_output();
    outb(MOUSE_DATA_PORT, data);
}

uint8_t mouse_read_data(void) {
    mouse_wait_input();
    return inb(MOUSE_DATA_PORT);
}

void mouse_send_command_to_mouse(uint8_t command) {
    mouse_write_command(MOUSE_CMD_WRITE_AUX);
    mouse_write_data(command);
    
    // Wait for acknowledgment
    uint8_t response = mouse_read_data();
    if (response != 0xFA) {
        // Command not acknowledged properly
    }
}

int mouse_init(void) {
    // Enable auxiliary mouse device
    mouse_write_command(MOUSE_CMD_ENABLE_AUX);
    
    // Get current status
    mouse_write_command(MOUSE_CMD_GET_STATUS);
    uint8_t status = mouse_read_data();
    
    // Enable interrupts for auxiliary device
    status |= 0x02; // Enable auxiliary interrupt
    mouse_write_command(MOUSE_CMD_SET_STATUS);
    mouse_write_data(status);
    
    // Reset mouse
    mouse_send_command_to_mouse(MOUSE_RESET);
    mouse_read_data(); // Read self-test result
    mouse_read_data(); // Read mouse ID
    
    // Set mouse resolution
    mouse_send_command_to_mouse(MOUSE_SET_RESOLUTION);
    mouse_send_command_to_mouse(0x03); // 8 counts/mm
    
    // Set scaling 1:1
    mouse_send_command_to_mouse(MOUSE_SET_SCALING);
    
    // Enable mouse reporting
    mouse_send_command_to_mouse(MOUSE_ENABLE_REPORTING);
    
    // Initialize mouse state
    mouse_state.x = screen_width / 2;
    mouse_state.y = screen_height / 2;
    mouse_state.buttons = 0;
    mouse_state.initialized = 1;
    
    mouse_packet_index = 0;
    
    return 0;
}

void mouse_hide_cursor(void) {
    if (prev_cursor_x >= 0 && prev_cursor_y >= 0 && 
        prev_cursor_x < screen_width && prev_cursor_y < screen_height) {
        // Restore original character with its original color
        uint8_t orig_color = vga_get_color_at(prev_cursor_x, prev_cursor_y);
        vga_put_char_at(prev_cursor_char, prev_cursor_x, prev_cursor_y, orig_color);
    }
}

void mouse_show_cursor(void) {
    // Save character at current position
    prev_cursor_char = vga_get_char_at(mouse_state.x, mouse_state.y);
    prev_cursor_x = mouse_state.x;
    prev_cursor_y = mouse_state.y;
    
    // Draw cursor with distinctive color
    vga_put_char_at(MOUSE_CURSOR_CHAR, mouse_state.x, mouse_state.y, 
                    VGA_COLOR_BLACK | (VGA_COLOR_WHITE << 4)); // Inverted colors
}

void mouse_set_cursor_position(int16_t x, int16_t y) {
    // Hide cursor at old position
    mouse_hide_cursor();
    
    // Handle scrolling when cursor goes beyond screen bounds
    if (y < 0) {
        // Scroll down to show content above
        vga_scroll_down();
        y = 0;
    } else if (y >= screen_height) {
        // Scroll up to show content below
        vga_scroll_up();
        y = screen_height - 1;
    }
    
    // Update position with bounds checking for X
    if (x < 0) x = 0;
    if (x >= screen_width) x = screen_width - 1;
    
    mouse_state.x = x;
    mouse_state.y = y;
    
    // Show cursor at new position
    mouse_show_cursor();
    
    // Update VGA cursor position for text input
    vga_set_cursor(x, y);
}

void mouse_process_packet(void) {
    if (mouse_packet_index < 3) {
        return; // Need complete packet
    }
    
    mouse_packet_t packet;
    packet.flags = mouse_packet_data[0];
    packet.x_movement = (int16_t)mouse_packet_data[1];
    packet.y_movement = (int16_t)mouse_packet_data[2];
    
    // Handle X overflow
    if (packet.flags & MOUSE_X_OVERFLOW) {
        packet.x_movement = (packet.flags & 0x10) ? -256 : 256;
    } else if (packet.flags & 0x10) {
        packet.x_movement |= 0xFF00; // Sign extend negative
    }
    
    // Handle Y overflow
    if (packet.flags & MOUSE_Y_OVERFLOW) {
        packet.y_movement = (packet.flags & 0x20) ? -256 : 256;
    } else if (packet.flags & 0x20) {
        packet.y_movement |= 0xFF00; // Sign extend negative
    }
    
    // Only process movement if there's actual movement
    if (packet.x_movement != 0 || packet.y_movement != 0) {
        // Calculate new position
        int16_t new_x = mouse_state.x + packet.x_movement;
        int16_t new_y = mouse_state.y - packet.y_movement; // Invert Y for screen coordinates
        
        // Update cursor position
        mouse_set_cursor_position(new_x, new_y);
    }
    
    // Update button states
    mouse_state.buttons = packet.flags & 0x07;
    
    // Reset packet index for next packet
    mouse_packet_index = 0;
}

void mouse_interrupt_handler(void) {
    if (!mouse_state.initialized) {
        return;
    }
    
    // Read data from mouse
    uint8_t data = inb(MOUSE_DATA_PORT);
    
    // Store in packet buffer
    mouse_packet_data[mouse_packet_index] = data;
    mouse_packet_index++;
    
    // Process complete packet
    if (mouse_packet_index >= 3) {
        mouse_process_packet();
    }
}

mouse_state_t* mouse_get_state(void) {
    return &mouse_state;
}

// Mouse click handlers for shell interaction
void mouse_handle_click(void) {
    // Обработка кликов без спама в консоль
    // События кликов можно обрабатывать через polling или callback
    // Пока что просто сохраняем состояние кнопок
}

// Get current mouse position for shell
void mouse_get_position(int16_t* x, int16_t* y) {
    if (x) *x = mouse_state.x;
    if (y) *y = mouse_state.y;
}

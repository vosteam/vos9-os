#include "../include/vga.h"

static uint16_t* const VGA_BUFFER = (uint16_t*)VGA_MEMORY;
static size_t vga_row = 0;
static size_t vga_column = 0;
static uint8_t vga_color = 0x07; // Light grey on black

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

void vga_init(void) {
    vga_row = 0;
    vga_column = 0;
    vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear_screen(); // This will also call vga_update_hw_cursor
    vga_enable_cursor(14, 15); // Enable cursor with typical scanlines (bottom of char cell)
}

void vga_clear_screen(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_column = 0;
    vga_update_hw_cursor(); // Update hardware cursor after clearing screen
}

// DOS-style clear function (alias for vga_clear_screen)
void vga_clear(void) {
    vga_clear_screen();
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_color = vga_entry_color(fg, bg);
}

void vga_set_cursor(uint8_t x, uint8_t y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        vga_column = x;
        vga_row = y;
        vga_update_hw_cursor(); // Update hardware cursor
    }
}

static void vga_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            const size_t next_index = (y + 1) * VGA_WIDTH + x;
            VGA_BUFFER[index] = VGA_BUFFER[next_index];
        }
    }
    
    // Clear last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        VGA_BUFFER[index] = vga_entry(' ', vga_color);
    }
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
            vga_row = VGA_HEIGHT - 1;
        }
    } else {
        const size_t index = vga_row * VGA_WIDTH + vga_column;
        VGA_BUFFER[index] = vga_entry(c, vga_color);
        
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
                vga_row = VGA_HEIGHT - 1;
            }
        }
    }
    vga_update_hw_cursor(); // Update hardware cursor after printing a char
}

void vga_puts(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

// Function to enable the hardware cursor
void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A); // Select cursor start register
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start); // Set cursor start scanline
    outb(0x3D4, 0x0B); // Select cursor end register
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);   // Set cursor end scanline
}

// Function to disable the hardware cursor
void vga_disable_cursor(void) {
    outb(0x3D4, 0x0A); // Select cursor start register
    outb(0x3D5, 0x20); // Set bit 5 to disable cursor
}

// Function to update the hardware cursor position
void vga_update_hw_cursor(void) {
    uint16_t pos = vga_row * VGA_WIDTH + vga_column;
    outb(0x3D4, 0x0F); // Select cursor location low register
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E); // Select cursor location high register
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// Function to get current cursor position
void vga_get_cursor_position(uint8_t* x, uint8_t* y) {
    *x = vga_column;
    *y = vga_row;
}

// Function to get current color
uint8_t vga_get_current_color(void) {
    return vga_color;
}

// Function to print a decimal number
void vga_put_dec(uint32_t n) {
    if (n == 0) {
        vga_putchar('0');
        return;
    }

    char buffer[11]; // Max 10 digits for uint32_t + null terminator
    int i = 0;
    while (n > 0) {
        buffer[i++] = (n % 10) + '0';
        n /= 10;
    }
    buffer[i] = '\0';

    // Reverse the buffer
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    vga_puts(buffer);
}

// Function to print a hexadecimal number
void vga_put_hex(uint32_t n) {
    vga_puts("0x");
    if (n == 0) {
        vga_putchar('0');
        return;
    }

    char buffer[9]; // Max 8 hex digits for uint32_t + null terminator
    int i = 0;
    const char* hex_chars = "0123456789ABCDEF";

    while (n > 0) {
        buffer[i++] = hex_chars[n % 16];
        n /= 16;
    }
    buffer[i] = '\0';

    // Reverse the buffer
    int start = 0;
    int end = i - 1;
    if (i == 0) { // Handle case where n was 0 initially, but we already printed "0x"
        vga_putchar('0');
        return;
    }
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    vga_puts(buffer);
}

// Cursor movement functions
void vga_move_cursor_up(void) {
    if (vga_row > 0) {
        vga_row--;
        vga_update_hw_cursor();
        vga_show_cursor_info();
    }
}

void vga_move_cursor_down(void) {
    if (vga_row < VGA_HEIGHT - 1) {
        vga_row++;
        vga_update_hw_cursor();
        vga_show_cursor_info();
    }
}

void vga_move_cursor_left(void) {
    if (vga_column > 0) {
        vga_column--;
        vga_update_hw_cursor();
        vga_show_cursor_info();
    } else if (vga_row > 0) {
        vga_row--;
        vga_column = VGA_WIDTH - 1;
        vga_update_hw_cursor();
        vga_show_cursor_info();
    }
}

void vga_move_cursor_right(void) {
    if (vga_column < VGA_WIDTH - 1) {
        vga_column++;
        vga_update_hw_cursor();
        vga_show_cursor_info();
    } else if (vga_row < VGA_HEIGHT - 1) {
        vga_row++;
        vga_column = 0;
        vga_update_hw_cursor();
        vga_show_cursor_info();
    }
}

void vga_backspace(void) {
    if (vga_column > 0) {
        vga_column--;
        const size_t index = vga_row * VGA_WIDTH + vga_column;
        VGA_BUFFER[index] = vga_entry(' ', vga_color);
        vga_update_hw_cursor();
    } else if (vga_row > 0) {
        vga_row--;
        vga_column = VGA_WIDTH - 1;
        const size_t index = vga_row * VGA_WIDTH + vga_column;
        VGA_BUFFER[index] = vga_entry(' ', vga_color);
        vga_update_hw_cursor();
    }
}

// Function to display cursor position information
void vga_show_cursor_info(void) {
    // Save current position and color
    uint8_t saved_row = vga_row;
    uint8_t saved_col = vga_column;
    uint8_t saved_color = vga_color;
    
    // Move to top-right corner to show cursor info
    vga_set_cursor(60, 0);
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLUE);
    vga_puts("Pos:");
    vga_put_dec(saved_col);
    vga_puts(",");
    vga_put_dec(saved_row);
    vga_puts("  ");
    
    // Restore position and color
    vga_color = saved_color;
    vga_set_cursor(saved_col, saved_row);
}

// Functions for mouse support

void vga_put_char_at(char c, uint8_t x, uint8_t y, uint8_t color) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return; // Out of bounds
    }
    
    const size_t index = y * VGA_WIDTH + x;
    VGA_BUFFER[index] = vga_entry(c, color);
}

char vga_get_char_at(uint8_t x, uint8_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return ' '; // Out of bounds, return space
    }
    
    const size_t index = y * VGA_WIDTH + x;
    return VGA_BUFFER[index] & 0xFF; // Extract character from entry
}

uint8_t vga_get_color_at(uint8_t x, uint8_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return VGA_COLOR_LIGHT_GREY; // Default color for out of bounds
    }
    
    const size_t index = y * VGA_WIDTH + x;
    return (VGA_BUFFER[index] >> 8) & 0xFF; // Extract color from entry
}

// Scrolling functions
void vga_scroll_up(void) {
    // Move all lines up by one
    for (size_t row = 1; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            size_t src_index = row * VGA_WIDTH + col;
            size_t dst_index = (row - 1) * VGA_WIDTH + col;
            VGA_BUFFER[dst_index] = VGA_BUFFER[src_index];
        }
    }
    
    // Clear the last line
    for (size_t col = 0; col < VGA_WIDTH; col++) {
        size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + col;
        VGA_BUFFER[index] = vga_entry(' ', vga_color);
    }
}

void vga_scroll_down(void) {
    // Move all lines down by one
    for (size_t row = VGA_HEIGHT - 1; row > 0; row--) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            size_t src_index = (row - 1) * VGA_WIDTH + col;
            size_t dst_index = row * VGA_WIDTH + col;
            VGA_BUFFER[dst_index] = VGA_BUFFER[src_index];
        }
    }
    
    // Clear the first line
    for (size_t col = 0; col < VGA_WIDTH; col++) {
        size_t index = col;
        VGA_BUFFER[index] = vga_entry(' ', vga_color);
    }
}

#ifndef VGA_H
#define VGA_H

#include "kernel.h"

// VGA driver functions
void vga_init(void);
void vga_clear_screen(void);
void vga_clear(void); // Alias for vga_clear_screen (DOS-style)
void vga_putchar(char c);
void vga_puts(const char* str);
void vga_set_color(uint8_t fg, uint8_t bg);
void vga_set_cursor(uint8_t x, uint8_t y);

// New functions for cursor control and info
void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void vga_disable_cursor(void);
void vga_get_cursor_position(uint8_t* x, uint8_t* y);
uint8_t vga_get_current_color(void);
void vga_update_hw_cursor(void); // Helper to update hardware cursor

// New functions for printing numbers
void vga_put_dec(uint32_t n);
void vga_put_hex(uint32_t n);

// Cursor movement functions
void vga_move_cursor_up(void);
void vga_move_cursor_down(void);
void vga_move_cursor_left(void);
void vga_move_cursor_right(void);
void vga_backspace(void);
void vga_show_cursor_info(void);

// Functions for mouse support
void vga_put_char_at(char c, uint8_t x, uint8_t y, uint8_t color);
char vga_get_char_at(uint8_t x, uint8_t y);
uint8_t vga_get_color_at(uint8_t x, uint8_t y);

// Scrolling functions
void vga_scroll_up(void);
void vga_scroll_down(void);

#endif

#include "../include/games.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/string.h"
#include "../include/drivers/timer.h"

// Simple pseudo-random number generator
static uint32_t rand_seed = 1;

static uint32_t simple_rand(void) {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed / 65536) % 32768;
}

void game_guess_number(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    // Initialize random seed with timer
    rand_seed = timer_get_ticks();
    
    uint32_t target = (simple_rand() % 100) + 1;
    uint32_t attempts = 0;
    uint32_t max_attempts = 7;
    
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_puts("\n=== Number Guessing Game ===\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("I'm thinking of a number between 1 and 100.\n");
    vga_puts("You have 7 attempts to guess it!\n");
    vga_puts("Type your guess and press Enter:\n\n");
    
    while (attempts < max_attempts) {
        vga_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
        vga_puts("Attempt ");
        vga_put_dec(attempts + 1);
        vga_puts("/");
        vga_put_dec(max_attempts);
        vga_puts(": ");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        // Simple input reading (simplified for demo)
        char input_buffer[10];
        int pos = 0;
        
        while (1) {
            if (inb(0x64) & 0x01) {
                key_event_t event = keyboard_get_event();
                if (event.type == KEY_EVENT_PRESS) {
                    if (event.ascii == '\n') {
                        input_buffer[pos] = '\0';
                        vga_putchar('\n');
                        break;
                    } else if (event.ascii >= '0' && event.ascii <= '9' && pos < 9) {
                        input_buffer[pos++] = event.ascii;
                        vga_putchar(event.ascii);
                    } else if (event.special && event.scancode == KEY_BACKSPACE && pos > 0) {
                        pos--;
                        vga_backspace();
                    }
                }
            }
        }
        
        // Convert string to number (simple implementation)
        uint32_t guess = 0;
        for (int i = 0; input_buffer[i]; i++) {
            if (input_buffer[i] >= '0' && input_buffer[i] <= '9') {
                guess = guess * 10 + (input_buffer[i] - '0');
            }
        }
        
        attempts++;
        
        if (guess == target) {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_puts("Congratulations! You guessed it in ");
            vga_put_dec(attempts);
            vga_puts(" attempts!\n");
            vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            return;
        } else if (guess < target) {
            vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
            vga_puts("Too low! Try a higher number.\n");
        } else {
            vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
            vga_puts("Too high! Try a lower number.\n");
        }
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
    
    vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
    vga_puts("Game Over! The number was ");
    vga_put_dec(target);
    vga_puts(".\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void game_calculator(int argc, char* argv[]) {
    if (argc != 4) {
        vga_puts("Usage: calc <number1> <operator> <number2>\n");
        vga_puts("Example: calc 10 + 5\n");
        vga_puts("Supported operators: +, -, *, /\n");
        return;
    }
    
    // Convert arguments to numbers (simplified)
    uint32_t num1 = 0, num2 = 0;
    char op = argv[2][0];
    
    // Simple string to number conversion
    for (int i = 0; argv[1][i]; i++) {
        if (argv[1][i] >= '0' && argv[1][i] <= '9') {
            num1 = num1 * 10 + (argv[1][i] - '0');
        }
    }
    
    for (int i = 0; argv[3][i]; i++) {
        if (argv[3][i] >= '0' && argv[3][i] <= '9') {
            num2 = num2 * 10 + (argv[3][i] - '0');
        }
    }
    
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_puts("Calculator: ");
    vga_put_dec(num1);
    vga_putchar(' ');
    vga_putchar(op);
    vga_putchar(' ');
    vga_put_dec(num2);
    vga_puts(" = ");
    
    uint32_t result = 0;
    int valid = 1;
    
    switch (op) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                result = num1 / num2;
            } else {
                vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
                vga_puts("Error: Division by zero!");
                valid = 0;
            }
            break;
        default:
            vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
            vga_puts("Error: Unknown operator!");
            valid = 0;
            break;
    }
    
    if (valid) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_put_dec(result);
    }
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("\n");
}

void game_snake_demo(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("\n=== Snake Game Demo ===\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("This would be a simple Snake game.\n");
    vga_puts("Use arrow keys to control the snake:\n");
    vga_puts("↑ ↓ ← → to move\n");
    vga_puts("Eat food (*) to grow!\n\n");
    
    // Simple demo field
    vga_puts("┌────────────────────┐\n");
    vga_puts("│                    │\n");
    vga_puts("│     ●●●→           │\n");
    vga_puts("│                    │\n");
    vga_puts("│         *          │\n");
    vga_puts("│                    │\n");
    vga_puts("└────────────────────┘\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("\n[This is a demo - full game not implemented yet]\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Press any key to continue...\n");
    
    // Wait for key press
    while (1) {
        if (inb(0x64) & 0x01) {
            key_event_t event = keyboard_get_event();
            if (event.type == KEY_EVENT_PRESS && (event.ascii != 0 || event.special)) {
                break;
            }
        }
    }
}

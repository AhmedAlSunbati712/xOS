#include <stdbool.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include "fb.h"
#include "fb_cursor.h"

/* ======== Initializing global variables ======= */
uint16_t terminal_bg = VGA_COLOR_BLACK;
volatile uint16_t* terminal_buffer = (volatile uint16_t*) VGA_MEMORY;
int curr_terminal_idx = 0;
const int tty_last_index = VGA_WIDTH * VGA_HEIGHT;

/* ======== Start of Forward Declarations ======= */
uint16_t pack_character_and_color(char c, uint16_t fg);
size_t strlen(char *string);
void scroll(void);
void kernel_main_fb(void);
int itoa(int value, char *buffer);
int itox(unsigned long long value, char *buffer);
/* ======== End of Forward Declarations ====== */

void write(char *buf, int len_to_print, int fg) {
    for (int idx = 0; idx < len_to_print; idx++) {
        if (buf[idx] == '\n') {
            int row = curr_terminal_idx / VGA_WIDTH;
            curr_terminal_idx = (row + 1) * VGA_WIDTH;
            move_cursor(curr_terminal_idx);
            continue;
        }
        terminal_putchar(buf[idx], fg);
    }
    terminal_buffer[curr_terminal_idx] = pack_character_and_color(' ', VGA_COLOR_GREEN);
    move_cursor((uint16_t) (curr_terminal_idx));
}

void initialize_terminal(void) {
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            size_t idx = row * VGA_WIDTH + col;
            uint16_t empty_space = pack_character_and_color(' ', VGA_COLOR_BLACK);
            terminal_buffer[idx] = empty_space;
        }
    }
}

void terminal_putchar_at(char c, int row, int col) {
    if (row >= VGA_HEIGHT || col >= VGA_WIDTH) return;

    /* FIX: Calculate index based on row/col, removed global 'index' variable */
    size_t idx = row * VGA_WIDTH + col;
    
    uint16_t packed_character_and_color = pack_character_and_color(c, VGA_COLOR_RED);
    terminal_buffer[idx] = packed_character_and_color;
}

void terminal_putchar(char c, int fg) {
    if (curr_terminal_idx == tty_last_index) {
        scroll();
    }
    uint16_t packed_character_and_color = pack_character_and_color(c, fg);
    terminal_buffer[curr_terminal_idx++] = packed_character_and_color;
}

void move_cursor(uint16_t pos) {
    outb((unsigned short) VGA_CURSOR_DESCRIPTOR_PORT, (unsigned char) POS_HIGH_BYTE_COMMAND);
    outb((unsigned short) VGA_CURSOR_DATA_PORT, (unsigned char) (pos & 0xFF));
    outb((unsigned short) VGA_CURSOR_DESCRIPTOR_PORT, (unsigned char) POS_LOW_BYTE_COMMAND);
    outb((unsigned short) VGA_CURSOR_DATA_PORT, (unsigned char) ((pos >> 8) & 0xFF));
}

void printf(char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    while (*format != '\0') {

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') format++;
            int amount = 0;
            while (format[amount] && format[amount] != '%')
                amount++;

            write(format, amount, VGA_COLOR_GREEN);
            format += amount;
            continue;
        }
        char *format_begun_at = format++;
        bool is_64bit = false;
        if (*format == 'l') {
            format++;
            if (*format == 'l') format++;
            is_64bit = true;
        }

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            write(&c, 1, VGA_COLOR_GREEN);
            continue;
        } else if (*format == 'd') {
            format++;
            int num = va_arg(parameters, int);

            char buf[32];
            int len = itoa(num, buf);

            write(buf, len, VGA_COLOR_GREEN);
            continue;
        } else if (*format == 'x') {
            format++;
            char buf[32];
            int len;

            if (is_64bit) {
                unsigned long long num = va_arg(parameters, unsigned long long);
                len = itox(num, buf);
            } else {
                unsigned int num = va_arg(parameters, unsigned int);
                len = itox((unsigned long long)num, buf);
            }
            write(buf, len, VGA_COLOR_GREEN);
            continue;
        } else {
            format = format_begun_at;
            write(format, 1, VGA_COLOR_GREEN);
            format++;
        }
    }

    va_end(parameters);
}

/* ============ Helper Functions ============ */

void kernel_main_fb(void) {
    initialize_terminal();
    int x = 1;
    int y = 2;
    int sum = x + y;
    printf("The\nsum\nof\n%d\nand\n%d\nis\n%d", x, y, sum);
    
}

size_t strlen(char *string) {
    size_t len = 0;
    while (string[len] != '\0') {
        len++;
    }
    return len;
}

void scroll(void) {
    // 1. Move everything up
    for (int i = 0; i < VGA_HEIGHT - 1; i++) {
        for (int j = 0; j < VGA_WIDTH; j++) {
            terminal_buffer[i * VGA_WIDTH + j] = terminal_buffer[(i + 1) * VGA_WIDTH + j];
        }
    }

    // 2. Clear the last line (fill with spaces)
    // If you don't do this, the bottom line will just contain copies of old text
    uint16_t blank = pack_character_and_color(' ', terminal_bg); // Use your BG color
    int last_row_start = (VGA_HEIGHT - 1) * VGA_WIDTH;
    
    for (int i = 0; i < VGA_WIDTH; i++) {
        terminal_buffer[last_row_start + i] = blank;
    }

    // 3. Reset cursor to start of the last line
    curr_terminal_idx = (VGA_HEIGHT - 1) * VGA_WIDTH;
}

uint16_t pack_character_and_color(char c, uint16_t fg) {
    /* VGA Entry Layout (16 bits):
       [ Blink (1) | BG Color (3) | FG Color (4) | Character (8) ]
    */
    return (uint16_t) c | (uint16_t)fg << 8 | (uint16_t)terminal_bg << 12;
}


int itoa(int value, char *buffer) {
    int is_negative = 0;
    int i = 0;

    if (value == 0) {
        buffer[i++] = '0';
        return i;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        int digit = value % 10;
        buffer[i++] = '0' + digit;
        value /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    // Reverse the buffer in-place
    for (int j = 0; j < i / 2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }

    return i; // number of chars written
}

int itox(unsigned long long value, char *buffer) {
    const char *hex = "0123456789ABCDEF";
    int i = 0;

    if (value == 0) {
        buffer[i++] = '0';
        return i;
    }

    while (value > 0) {
        buffer[i++] = hex[value & 0xF];
        value >>= 4;
    }

    // reverse the buffer
    for (int j = 0; j < i / 2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }

    return i;
}
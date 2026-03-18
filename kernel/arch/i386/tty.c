#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/tty.h>
#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;


static size_t tty_row;
static size_t tty_column;
static uint8_t tty_color;
static uint16_t* tty_buffer;


void terminal_initialize(void) {
    tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    tty_buffer = VGA_MEMORY;
    tty_row = 0;
    tty_column = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y += 1) {
        for (size_t x = 0; x < VGA_WIDTH; x += 1) {
            const size_t index = y * VGA_WIDTH + x;
            tty_buffer[index] = vga_entry(' ', tty_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    tty_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    tty_buffer[index] = vga_entry(c, color);
}

static inline void copy_to_prev_line(size_t line) {
    /* Helper inline function; copies line at index `line` (0-indexed) to the previous line*/
    if (line == 0) return;
    if (line >= VGA_HEIGHT) return;
    uint16_t* const curr_line = VGA_MEMORY + line * (VGA_WIDTH);
    uint16_t* const prev_line = VGA_MEMORY + (line - 1) * (VGA_WIDTH);
    for (size_t i = 0; i < VGA_WIDTH; i += 1) {
        prev_line[i] = curr_line[i];
    }
}

static inline void clear_last_line(void) {
    uint16_t* const last_line = VGA_MEMORY + (VGA_HEIGHT - 1) * (VGA_WIDTH);
    for (size_t i = 0; i < VGA_WIDTH; i += 1) {
        last_line[i] = vga_entry(' ', tty_color);
    }
}

void terminal_scroll(void) {
    for (size_t line = 0; line < VGA_HEIGHT; line += 1) {
        copy_to_prev_line(line);
    }
    clear_last_line();
    tty_row -= 1;
}


void terminal_putchar(char c) {
    /**
    We gotta handle a couple of cases here:
    1. First if it 's a new line character, then we just increement the row and reset the column
    2. otherwise, we just put the character at the current row and column, then we increment both
    the row and column.

        a. if the column is == VGA_WIDTH, then we increment tty_row and reset tty_column
         b. if the row is == VGA_HEIGHT, then we scroll the terminal (terminal_scroll handles decrementing the row)
    */

    if (c == '\n') {
        tty_row += 1;
        tty_column = 0;
    } else {
        terminal_putentryat(c, tty_color, tty_column, tty_row);
        tty_column += 1;
    }
    // After adding this character, we need to check if we've hit the end of the line
    if (tty_column == VGA_WIDTH) {
        tty_column = 0;
        tty_row += 1;
    }
    // If we've hit the end of the line and it was the last line, then we need to scroll the terminal
    if (tty_row == VGA_HEIGHT) {
        terminal_scroll();
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i += 1) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}
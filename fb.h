#ifndef FB_H
#define FB_H

/* Macros */
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000

/* Macros for moving cursor*/
#define VGA_CURSOR_DESCRIPTOR_PORT  0x3D4
#define VGA_CURSOR_DATA_PORT        0x3D5
#define POS_HIGH_BYTE_COMMAND       15
#define POS_LOW_BYTE_COMMAND        14

/* Externing important global variables */
extern uint16_t terminal_bg;
extern volatile uint16_t *terminal_buffer;
extern int curr_terminal_idx;
extern const int tty_last_index;

/* Defining an enum type for colors */
enum VGA_COLORS {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};


void initialize_terminal(void);
void write(char *buf, int len_to_print, int fg);
void terminal_putchar(char c, int fg);
void terminal_putchar_at(char c, int row, int col);
void move_cursor(uint16_t pos);
void printf(char* restrict format, ...);

#endif

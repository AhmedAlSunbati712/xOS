#include "fb_cursor.h"

void outb(unsigned short port, unsigned char data) {
    asm volatile ("outb %b0, %w1"
                  :
                  :"a" (data), "d" (port));
}
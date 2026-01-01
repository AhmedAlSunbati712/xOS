#ifndef GDT_H
#define GDT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NUM_GDT_ENTRIES 5


typedef struct gdt {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed)) gdt;

typedef struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry;

void gdt_init();
void load_gdt(uint32_t gdt_addr);
#endif
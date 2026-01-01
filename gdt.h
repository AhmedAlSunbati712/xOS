#ifndef GDT_H
#define GDT_H

#define NUM_GDT_ENTRIES 5

struct gdt {
    uint32_t offset;
    uint16_t size;
} __attribute__((packed));

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity; /* limit top 4 bits and then flags*/
    uint8_t base_high;
}__attribute__((packed));

void gdt_init();
void gdt_load(uint32_t gdt_addr);
#endif
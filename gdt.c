#include "gdt.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

gdt_entry gdt_entries[NUM_GDT_ENTRIES];
static struct gdt gdtr;
struct gdt_entry kernel_code;
struct gdt_entry kernel_data;

struct gdt_entry user_code;
struct gdt_entry user_data;

struct gdt_entry null_descriptor;

struct gdt_entry format_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

void gdt_init(void) {
    /* null descriptor */

    gdt_entries[0] = format_entry(0, 0, 0, 0);

    /* kernel mode code segment */

    gdt_entries[1] = format_entry(0, 0xFFFFF, 0x9A, 0xC);

    
    /* kernel mode data segment */

    gdt_entries[2] = format_entry(0, 0xFFFFF, 0x92, 0xC);


    /* user mode code segment */

    gdt_entries[3] = format_entry(0, 0xFFFFF, 0xFA, 0xC);


    /* user mode data segment */

    gdt_entries[4] = format_entry(0, 0xFFFFF, 0xF2, 0xC);

    gdtr.offset = (uint32_t)&gdt_entries;
    gdtr.size = sizeof(struct gdt_entry) * 5 - 1;
    load_gdt((uint32_t)&gdtr);
}

struct gdt_entry format_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    struct gdt_entry entry;
    entry.limit_low = limit & 0xFFFF;
    entry.base_low = base & 0xFFFF;
    entry.base_middle = (base >> 16) & 0xFF;
    entry.access = access;
    entry.granularity = (flags << 4) & 0xF0;
    entry.granularity |= (limit >> 16) & 0x0F;
    entry.base_high = (base >> 24) & 0xFF;
    return entry;
}
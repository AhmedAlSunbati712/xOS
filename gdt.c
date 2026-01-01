#include "gdt.h"

extern gdt_entry gdt_entries[NUM_GDT_ENTRIES];

gdt_entry format_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

void gdt_init(void) {
    /* null descriptor */
    struct gdt_entry null_descriptor;
    null_descriptor = format_entry(0, 0, 0, 0);
    gdt_entries[0] = null_descriptor;

    /* kernel mode code segment */
    struct gdt_entry kernel_code;
    kernel_code = format_entry(0, 0xFFFFF, 0x9A, 0xC);
    gdt_entries[1] = kernel_code;
    
    /* kernel mode data segment */
    struct gdt_entry kernel_data;
    kernel_data = format_entry(0, 0xFFFFF, 0x92, 0xC);
    gdt_entries[2] = kernel_data;

    /* user mode code segment */
    struct gdt_entry user_code;
    user_code = format_entry(0, 0xFFFFF, 0xFA, 0xC);
    gdt_entries[3] = user_code;

    /* user mode data segment */
    struct gdt_entry user_data;
    user_data = format_entry(0, 0xFFFFF, 0xF2, 0xC);
    gdt_entries[4] = user_data;

    gdt.offset = (uint32_t)&gdt_entries;
    gdt.size = sizeof(gdt_entries) - 1;
    gdt_load((uint32_t)&gdt);
}

gdt_entry format_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    struct gdt_entry entry;
    entry.limit_low = limit & 0xFFFF;
    entry.base_low = base & 0xFFFF;
    entry.base_middle = (base >> 16) & 0xFF;
    entry.access = access;
    entry.granularity = flags;
    entry.granularity |= (limit >> 16) & 0xF;
    entry.base_high = (base >> 24) & 0xFF;
    return entry;
}
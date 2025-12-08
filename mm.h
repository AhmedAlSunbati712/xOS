#ifndef MM_H
#define MM_H

#define MAX_NUM_PAGES 1048576
#define BITMAP_LENGTH (MAX_NUM_PAGES/8)
#define PAGESHIFT 12
#define PAGESIZE (1 << PAGESHIFT)
#define PAGEMASK (PAGESIZE - 1)
#define DOWN_TO_PAGE(ADDR) ((ADDR) & ~PAGEMASK)
#define UP_TO_PAGE(ADDR) ((ADDR + PAGEMASK) & ~PAGEMASK)


extern uint8_t bitmap[BITMAP_LENGTH];

bool initialize_pmm_bitmap(multiboot_info_t* mbd, uint64_t memory_limit);
#endif
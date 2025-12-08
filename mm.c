#include "mm.h"
#include <stdbool.h>
#include "multiboot.h"

uint8_t bitmap[BITMAP_LENGTH];

bool initialize_pmm_bitmap(multiboot_info_t* mbd, uint64_t memory_limit) {
	/*
	Setting all the pages to be occupied
	*/
    for (int i = 0; i < BITMAP_LENGTH; i++) {
		bitmap[i] = 255; // 11111111 = 255
	}
    // Finding the free frames
	for (unsigned int i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		multiboot_memory_map_t *mmmt = (multiboot_memory_map_t *) (mbd->mmap_addr + i);
		if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
			unsigned long long start_addr = mmmt->addr;
			unsigned long long end_addr = mmmt->addr + mmmt->len;
			printf("Free memory start address: %lx, end address: %lx, length: %lx\n", start_addr, end_addr, mmmt->len);
			// Round up the start address and round down the end addr
			unsigned long long start_aligned = UP_TO_PAGE(start_addr);
			unsigned long long end_aligned = DOWN_TO_PAGE(end_addr);
			unsigned long long curr_addr = start_aligned;
			while (curr_addr < end_aligned) {
				//printf("Address starting at %lx is Free ", curr_addr);
				int curr_page_num = (int) (curr_addr >> PAGESHIFT);
				int byte_idx = curr_page_num / 8;
				int byte_subidx = curr_page_num % 8;
				uint8_t bitmask = 1 << byte_subidx; // if for example byte_subidx = 1, then bitmask is 00000010
				// if this is the first page to be set free in this byte, then this is equivalent to 11111111 & ~(11111101) = 11111101
				bitmap[byte_idx] &= ~bitmask;
				curr_addr += PAGESIZE;
			}
		}
	}
}
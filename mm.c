#include "mm.h"
#include <stdbool.h>
#include <stdint.h>
#include "multiboot.h"
#include "fb.h"

uint8_t bitmap[BITMAP_LENGTH];
int num_free = 0;

bool initialize_pmm_bitmap(multiboot_info_t* mbd) {
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
			printf("[!] Free memory start address: %lx, end address: %lx, length: %lx\n", start_addr, end_addr, mmmt->len);
			// Round up the start address and round down the end addr
			printf("[!] Aligning the addresses to mark the pages\n");
			unsigned long long start_aligned = UP_TO_PAGE(start_addr);
			unsigned long long end_aligned = DOWN_TO_PAGE(end_addr);
			printf("[!] Aligned start address: %lx, Aligned end address: %lx\n", start_aligned, end_aligned);
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
				num_free += 1;
			}
		}
	}
	printf("[!] Number of free pages: %d.\n", num_free);
	return true;
}

bool mark_occupied(uint32_t start_address, uint32_t end_address) {
	uint32_t start_aligned = UP_TO_PAGE(start_address);
	uint32_t end_aligned = DOWN_TO_PAGE(end_address);
	for (uint32_t curr_addr = start_aligned; curr_addr < end_aligned; curr_addr += PAGESIZE) {
		int curr_page_num = (int) (curr_addr >> PAGESHIFT);
		if (!is_free(curr_page_num)) {
			printf("[!] page num %d is occupied\n", curr_page_num);
			// roll back
			return false;
		}
		int byte_idx = curr_page_num / 8;
		int byte_subidx = curr_page_num % 8;
		uint8_t bitmask = 1 << byte_subidx;
		bitmap[byte_idx] = bitmap[byte_idx] | bitmask;
	}
	return true;
}

bool is_free(int page_num) {
	int byte_idx = page_num / 8;
	int byte_subidx = page_num % 8;
	uint8_t bitmask = 1 << byte_subidx;
	uint8_t bitmasked_byte = bitmap[byte_idx] & bitmask;
	bitmasked_byte = bitmasked_byte >> byte_subidx;
	return bitmasked_byte == 0;
}
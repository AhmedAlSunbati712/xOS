#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "fb.h"
#include "mm.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

uint8_t bitmap[BITMAP_LENGTH];

void kernel_main(multiboot_info_t* mbd, unsigned int magic) 
{	
	/* Initialize terminal interface */
	initialize_terminal();

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf("Invalid magic number\n.");
		return;
	}
	if (!(mbd->flags >> 6 & 0x1)) {
		printf("Invalid memory map given by GRUB bootloader\n.");
		return;
	}

	/*need to find the limit of the memory. iterate through all of the entries
	 and calculate end address = start_address + length*/
	uint64_t memory_limit = 0;
	for (unsigned int i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		multiboot_memory_map_t *mmmt = (multiboot_memory_map_t *) (mbd->mmap_addr + i);
		printf("Start addr: %lx | length: %lx | size: %x | type: %d\n", mmmt->addr, mmmt->len, mmmt->size, mmmt->type);
		uint64_t current_limit = mmmt->addr + mmmt->len;
		if (current_limit > memory_limit) {
			memory_limit = current_limit;
		}
	}
	printf("Memory limit: %lx\n", memory_limit);
	int num_pages = (int)(memory_limit >> PAGESHIFT);
	printf("Number of pages: %d.\n", num_pages);

	/**
	Build the bitmap. We are going to have an array of bytes
	each byte controls 8 pages. The ith bit in the jth byte
	will reference the page with idx 8j+i-1
	11111111 11111111 11111111
	*/
	
	printf("Number of bytes in bitmap: %d.\n", BITMAP_LENGTH);
	
	/*
	Setting all the pages to be occupied
	*/
	for (int i = 0; i < BITMAP_LENGTH; i++) {
		bitmap[i] = 255; // 11111111 = 255
	}
	
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
	for (int i = 0; i < BITMAP_LENGTH; i++) {
		printf("Bitmap entry %d: %d", i, (int)bitmap[i]);
	}
	
}
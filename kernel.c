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
	uint8_t bitmap[num_pages];
	

	
}
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

extern uint32_t kernel_start_address;
extern uint32_t kernel_text_start;
extern uint32_t kernel_text_end;
extern uint32_t kernel_rodata_start;
extern uint32_t kernel_rodata_end;
extern uint32_t kernel_data_start;
extern uint32_t kernel_data_end;
extern uint32_t kernel_bss_start;
extern uint32_t kernel_bss_end;
extern uint32_t kernel_end_address;

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
	printf("1. kernel start address: %x, value: %x \n", &kernel_start_address, kernel_start_address);
	printf("2. kernel text start address: %x, value: %x \n", &kernel_text_start, kernel_text_start);
	printf("3. kernel text end address: %x, value: %x \n", &kernel_text_end, kernel_text_end);
	printf("4. kernel rodata start address: %x, value: %x \n", &kernel_rodata_start, kernel_rodata_start);
	printf("5. kernel rodata end address: %x, value: %x \n", &kernel_rodata_end, kernel_rodata_end);
	printf("6. kernel data start address: %x, value: %x \n", &kernel_data_start, kernel_data_start);
	printf("7. kernel data end address: %x, value: %x \n", &kernel_data_end, kernel_data_end);
	printf("8. kernel bss start address: %x, value: %x \n", &kernel_bss_start, kernel_bss_start);
	printf("9. kernel bss end address: %x, value: %x \n", &kernel_bss_end, kernel_bss_end);
	printf("10. kernel end address: %x, value: %x \n", &kernel_end_address, kernel_end_address);


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
	printf("[+] Memory limit: %lx\n", memory_limit);
	int num_pages = (int)(memory_limit >> PAGESHIFT);
	printf("[+] Number of pages: %d.\n", num_pages);

	/**
	Build the bitmap. We are going to have an array of bytes
	each byte controls 8 pages. The ith bit in the jth byte
	will reference the page with idx 8j+i-1
	11111111 11111111 11111111
	*/
	
	printf("[+] Number of bytes in bitmap: %d.\n", BITMAP_LENGTH);
	initialize_pmm_bitmap(mbd);
	// Marking kernel section stuff as occupied
	if (!mark_occupied((uint32_t)&kernel_text_start, (uint32_t)&kernel_text_end)) {
		printf("[!] [ERROR]: Kernel text section trying to take up addresses %x - %x but it's already occupied by something else!\n", &kernel_text_start, &kernel_text_end);
	} else {
		printf("[+] [SUCCESS]: Kernel text section successfully occupied %x - %x.\n", &kernel_text_start, &kernel_text_end);
	}

	if (!mark_occupied((uint32_t)&kernel_rodata_start, (uint32_t)&kernel_rodata_end)) {
		printf("[!] [ERROR]: Kernel read-only data section trying to take up addresses %x - %x but it's already occupied by something else!\n", &kernel_rodata_start, &kernel_rodata_end);
	} else {
		printf("[+] [SUCCESS]: Kernel read-only data section successfully occupied %x - %x.\n", &kernel_rodata_start, &kernel_rodata_end);
	}

	if (!mark_occupied((uint32_t)&kernel_data_start, (uint32_t)&kernel_data_end)) {
		printf("[!] [ERROR]: Kernel data section trying to take up addresses %x - %x but it's already occupied by something else!\n", &kernel_data_start, &kernel_data_end);
	} else {
		printf("[+] [SUCCESS]: Kernel data section successfully occupied %x - %x.\n", &kernel_data_start, &kernel_data_end);
	}

	if (!mark_occupied((uint32_t)&kernel_bss_start, (uint32_t)&kernel_bss_end)) {
		printf("[!] [ERROR]: Kernel bss section trying to take up addresses %x - %x but it's already occupied by something else!\n", &kernel_bss_start, &kernel_bss_start);
	} else {
		printf("[+] [SUCCESS]: Kernel bss section successfully occupied %x - %x.\n", &kernel_bss_start, &kernel_bss_end);

	}

	
}
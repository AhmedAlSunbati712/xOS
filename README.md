# xOS
`x` stands for unknown. This is a hobby project, I will start thinking about a name when I have a fully-working user-space with interrupts and VMM.
## Progress
### Sucessfull boot
Currently using grub multiboot specification to load my kernel. During boot, I initialize a stack of 16 `KiB`. Nothing too fancy in booting. Just calling kernel_main in `kernel.c`
### VGA Buffer
Started writing and finshed a basic driver to interact with the VGA buffer. Used Memory-mapped IO (MMIO) push the characters into the VGA buffer at address `0xB8000`.
Also, defined some helper functionality to move the cursor. This uses Port-mapped I/O. We send the position to the controller to update the position of the cursor. However, the controller only takes one byte at a time. We split the position in half and we send the higher byte and the lower bytes separately. Check `move_cursor` in `fb.h` and `outb` in `fb_cursor.s`.
### printf
Finally, got printf working with decimals, characters, long, and long long formatting. This is huge. can debug a lot with this now.
### Memory map
Grabbed the memory map from GRUB! Now need to start building the bitmap and my memory manager.
### Bitmap allocator
- Build the bitmap, marked pages not used by either kernel, hardware or multiboot info as free for use
- Extracted the kernel sections start and end addresses and marked the corresponding pages in the bitmap as used
## Need to do
### Memory manager
1. [x] ~~Build the bitmap~~
2. [x] Mark the bootloader info section and ~~the kernel stuff as used~~
3. Now, need to start thinking about other functionality like malloc and all of these stuff
### organizational work
Need to start organizing my code more and my build tools
## How to build
- You must first have a cross-compiler set up to be able to build and run this. Without using a cross-compiler, a lot of unexpected things can happen because the compiler assumes that the code is running on the host operating system.
- The compiler that I'm using targets a generic architecture (i686-elf)t hat allows you to leave the current operating system behind, meaning that no headers or libraries of the host operating system will be used.
- Call `make`.
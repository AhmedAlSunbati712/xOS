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
## Need to do
### Memory manager
1. Build the bitmap
2. Mark the bootloader info section and the kernel stuff as used
3. Now, need to start thinking about other functionality like malloc and all of these stuff
### organizational work
Need to start organizing my code more and my build tools
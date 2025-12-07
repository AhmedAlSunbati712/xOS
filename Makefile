# --- Configuration Variables ---
ISO := myos.iso
OS := myos.bin

# --- SRC files ---
C_SRCS := kernel.c test_stack.c fb.c spinlock.c
AS_SRCS := boot.s kernel_s.s fb_cursor.s

# -- OBJECT files ---
C_OBJS := $(C_SRCS:.c=.o)
AS_OBJS := $(AS_SRCS:.s=.o)
OBJS := $(C_OBJS) $(AS_OBJS)

# -- Toolchain ---
CC := i686-elf-gcc
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c

AS := i686-elf-as
ASFLAGS := -nostdlib 

LD := i686-elf-gcc 
LD_FLAGS := -T linker.ld -ffreestanding -O2 -nostdlib -lgcc
# --- Targets ---

.PHONY: all clean
all: $(ISO)

# ISO Target: Depends on the final kernel binary
$(ISO): $(OS)
	@echo ">> Creating ISO Image..."
	cp $(OS) isodir/boot/$(OS)
	grub-mkrescue -o $@ isodir -d ~/grub-x86/usr/lib/grub/i386-pc

# Kernel Binary Target: Links all object files
$(OS): $(OBJS)
	@echo ">> Linking Kernel Binary $@"
	$(LD) $(LD_FLAGS) $(OBJS) -o $@

# Object files from C files
%.o: %.c
	@echo ">> Compiling C $<"
	$(CC) $(CFLAGS) $< -o $@

# Object files from Assembly files.
%.o: %.S
	@echo ">> Compiling Assembly $<"
	$(AS) $(ASFLAGS) $< -o $@

# Clean Target: Removes generated files
clean:
	@echo ">> Cleaning generated files..."
	rm -rf $(C_OBJS) $(AS_OBJS) $(OS) $(ISO)
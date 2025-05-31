# VOS9 Build System
# Project structure:
# - src/boot/     : Bootloader source files
# - src/kernel/   : Kernel source files
# - src/drivers/  : Device drivers
# - src/lib/      : Library functions
# - include/      : Header files
# - build/        : Compiled objects and binaries
# - tools/        : Development tools and scripts
# - docs/         : Documentation

CC = gcc
AS = nasm
LD = ld

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
BOOT_SRC = $(SRC_DIR)/boot
KERNEL_SRC = $(SRC_DIR)/kernel
DRIVERS_SRC = $(SRC_DIR)/drivers
LIB_SRC = $(SRC_DIR)/lib

# Compiler flags
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -c -I$(INCLUDE_DIR)
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T $(KERNEL_SRC)/linker.ld
LDFLAGS_MULTIBOOT = -m elf_i386 -T $(KERNEL_SRC)/multiboot.ld

# Output files
BOOTLOADER = $(BUILD_DIR)/boot.bin
KERNEL = $(BUILD_DIR)/kernel.bin
KERNEL_MULTIBOOT = $(BUILD_DIR)/kernel_mb.bin
OS_IMAGE = $(BUILD_DIR)/vos9.img
ISO_IMAGE = $(BUILD_DIR)/vos9.iso
ISO_DIR = iso

# Object files
KERNEL_OBJECTS = $(BUILD_DIR)/kernel_entry.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/string.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/filesystem.o $(BUILD_DIR)/timer.o $(BUILD_DIR)/shell.o
MULTIBOOT_OBJECTS = $(BUILD_DIR)/multiboot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/string.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/filesystem.o $(BUILD_DIR)/timer.o $(BUILD_DIR)/shell.o

.PHONY: all clean run debug setup help iso run-iso

all: setup $(OS_IMAGE)

setup:
	@mkdir -p $(BUILD_DIR)

$(OS_IMAGE): $(BOOTLOADER) $(KERNEL)
	@echo "Creating OS image..."
	dd if=/dev/zero of=$(OS_IMAGE) bs=1024 count=1440 2>/dev/null
	dd if=$(BOOTLOADER) of=$(OS_IMAGE) bs=512 count=1 conv=notrunc 2>/dev/null
	dd if=$(KERNEL) of=$(OS_IMAGE) bs=512 seek=1 conv=notrunc 2>/dev/null
	@echo "OS image created: $(OS_IMAGE)"

$(BOOTLOADER): $(BOOT_SRC)/boot.asm
	@echo "Building bootloader..."
	$(AS) -f bin $(BOOT_SRC)/boot.asm -o $(BOOTLOADER)

$(KERNEL): $(KERNEL_OBJECTS)
	@echo "Linking kernel..."
	$(LD) $(LDFLAGS) $(KERNEL_OBJECTS) -o $(KERNEL)

$(BUILD_DIR)/kernel_entry.o: $(BOOT_SRC)/kernel_entry.asm
	@echo "Assembling kernel entry..."
	$(AS) $(ASFLAGS) $(BOOT_SRC)/kernel_entry.asm -o $(BUILD_DIR)/kernel_entry.o

$(BUILD_DIR)/kernel.o: $(KERNEL_SRC)/kernel.c $(INCLUDE_DIR)/kernel.h
	@echo "Compiling kernel..."
	$(CC) $(CFLAGS) $(KERNEL_SRC)/kernel.c -o $(BUILD_DIR)/kernel.o

$(BUILD_DIR)/vga.o: $(DRIVERS_SRC)/vga.c $(INCLUDE_DIR)/vga.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling VGA driver..."
	$(CC) $(CFLAGS) $(DRIVERS_SRC)/vga.c -o $(BUILD_DIR)/vga.o

$(BUILD_DIR)/keyboard.o: $(DRIVERS_SRC)/keyboard.c $(INCLUDE_DIR)/keyboard.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling keyboard driver..."
	$(CC) $(CFLAGS) $(DRIVERS_SRC)/keyboard.c -o $(BUILD_DIR)/keyboard.o

$(BUILD_DIR)/string.o: $(LIB_SRC)/string.c $(INCLUDE_DIR)/string.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling string library..."
	$(CC) $(CFLAGS) $(LIB_SRC)/string.c -o $(BUILD_DIR)/string.o

$(BUILD_DIR)/memory.o: $(SRC_DIR)/memory.c $(INCLUDE_DIR)/memory.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling memory manager..."
	$(CC) $(CFLAGS) $(SRC_DIR)/memory.c -o $(BUILD_DIR)/memory.o

$(BUILD_DIR)/filesystem.o: $(SRC_DIR)/filesystem.c $(INCLUDE_DIR)/filesystem.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling filesystem..."
	$(CC) $(CFLAGS) $(SRC_DIR)/filesystem.c -o $(BUILD_DIR)/filesystem.o

$(BUILD_DIR)/sata.o: $(DRIVERS_SRC)/sata.c $(INCLUDE_DIR)/drivers/sata.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling SATA driver..."
	$(CC) $(CFLAGS) $(DRIVERS_SRC)/sata.c -o $(BUILD_DIR)/sata.o

$(BUILD_DIR)/timer.o: $(DRIVERS_SRC)/timer.c $(INCLUDE_DIR)/drivers/timer.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling timer driver..."
	$(CC) $(CFLAGS) $(DRIVERS_SRC)/timer.c -o $(BUILD_DIR)/timer.o

$(BUILD_DIR)/shell.o: $(SRC_DIR)/shell.c $(INCLUDE_DIR)/shell.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling shell..."
	$(CC) $(CFLAGS) $(SRC_DIR)/shell.c -o $(BUILD_DIR)/shell.o

$(BUILD_DIR)/games.o: $(SRC_DIR)/games.c $(INCLUDE_DIR)/games.h $(INCLUDE_DIR)/kernel.h
	@echo "Compiling games..."
	$(CC) $(CFLAGS) $(SRC_DIR)/games.c -o $(BUILD_DIR)/games.o

# Multiboot kernel target
$(KERNEL_MULTIBOOT): $(MULTIBOOT_OBJECTS)
	@echo "Linking multiboot kernel..."
	$(LD) $(LDFLAGS_MULTIBOOT) $(MULTIBOOT_OBJECTS) -o $(KERNEL_MULTIBOOT)

$(BUILD_DIR)/multiboot.o: $(BOOT_SRC)/multiboot.asm
	@echo "Assembling multiboot entry..."
	$(AS) $(ASFLAGS) $(BOOT_SRC)/multiboot.asm -o $(BUILD_DIR)/multiboot.o

# ISO image target
iso: setup $(ISO_IMAGE)

$(ISO_IMAGE): $(KERNEL_MULTIBOOT)
	@echo "Creating ISO directory structure..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_MULTIBOOT) $(ISO_DIR)/boot/kernel.bin
	@echo "Generating GRUB ISO image..."
	grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR)
	@echo "ISO image created: $(ISO_IMAGE)"

clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)/*
	@echo "Clean complete."

run: $(OS_IMAGE)
	@echo "Running VOS9 in QEMU..."
	qemu-system-i386 -fda $(OS_IMAGE)

debug: $(OS_IMAGE)
	@echo "Running VOS9 in debug mode..."
	qemu-system-i386 -fda $(OS_IMAGE) -nographic -s -S

run-iso: $(ISO_IMAGE)
	@echo "Running VOS9 ISO in QEMU..."
	qemu-system-i386 -cdrom $(ISO_IMAGE) -nographic

debug-iso: $(ISO_IMAGE)
	@echo "Running VOS9 ISO in debug mode..."
	qemu-system-i386 -cdrom $(ISO_IMAGE) -nographic -s -S

help:
	@echo "VOS9 Build System"
	@echo "================="
	@echo "Available targets:"
	@echo "  all       - Build the complete OS (floppy image)"
	@echo "  iso       - Build ISO image with GRUB bootloader"
	@echo "  clean     - Remove all build files"
	@echo "  run       - Run OS floppy image in QEMU"
	@echo "  run-iso   - Run OS ISO image in QEMU"
	@echo "  debug     - Run OS floppy in debug mode"
	@echo "  debug-iso - Run OS ISO in debug mode"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Project structure:"
	@echo "  src/boot/     - Bootloader source files"
	@echo "  src/kernel/   - Kernel source files"
	@echo "  src/drivers/  - Device drivers"
	@echo "  src/lib/      - Library functions"
	@echo "  include/      - Header files"
	@echo "  build/        - Compiled objects and binaries"
	@echo "  tools/        - Development tools and scripts"
	@echo "  docs/         - Documentation"

# Makefile for OM3 OS

# Tools
CC = gcc
LD = ld
ASM = nasm

# Flags (Crucial for bare metal!)
# -m32: Compile for 32-bit
# -ffreestanding: Don't assume standard libraries exist
# -fno-pie: Disable Position Independent Executable (confuses raw binaries)
CFLAGS = -m32 -ffreestanding -fno-pie -c
LDFLAGS = -m elf_i386 -Ttext 0x1000 --oformat binary

all: om3os.bin

om3os.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > om3os.bin

boot.bin: boot.asm
	$(ASM) -f bin boot.asm -o boot.bin

kernel.bin: kernel.o
	$(LD) $(LDFLAGS) -o kernel.bin kernel.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) kernel.c -o kernel.o

clean:
	rm -f *.bin *.o
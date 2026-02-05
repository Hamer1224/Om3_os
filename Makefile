# Makefile for OM3 OS (Windows Version)

# Tools
CC = gcc
LD = ld
ASM = nasm

# Compiler Flags
CFLAGS = -m32 -ffreestanding -fno-pie -c
# Linker Flags (i386pe is the format for Windows MinGW trying to mimic elf)
LDFLAGS = -m i386pe -Ttext 0x1000

all: om3os.bin

# Combine files using Windows CMD syntax
om3os.bin: boot.bin kernel.bin
	cmd /c copy /b boot.bin+kernel.bin om3os.bin

boot.bin: boot.asm
	$(ASM) -f bin boot.asm -o boot.bin

# Note: We link to .bin directly here if possible, 
# or use objcopy if your linker forces .exe extensions
kernel.bin: kernel.o
	$(LD) $(LDFLAGS) -o kernel.tmp kernel.o
	objcopy -O binary kernel.tmp kernel.bin

kernel.o: kernel.c
	$(CC) $(CFLAGS) kernel.c -o kernel.o

# Windows cleanup command
clean:
	del *.bin *.o *.tmp
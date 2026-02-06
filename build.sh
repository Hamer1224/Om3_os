#!/bin/bash
# OM3 OS Linux/Tablet Build Script

echo "[0/6] Cleaning up..."
rm -f *.o *.bin *.exe kernel.bin om3os.bin

echo "[1/6] Assembling Kernel Entry..."
nasm -f elf32 entry.asm -o entry.o

echo "[2/6] Compiling Kernel Modules..."
# We use x86_64-elf-gcc or standard gcc -m32
gcc -m32 -ffreestanding -c utils.c -o utils.o
gcc -m32 -ffreestanding -c screen.c -o screen.o
gcc -m32 -ffreestanding -c keyboard.c -o keyboard.o
gcc -m32 -ffreestanding -c disk.c -o disk.o
gcc -m32 -ffreestanding -c fs.c -o fs.o
gcc -m32 -ffreestanding -c holyhamer.c -o holyhamer.o
gcc -m32 -ffreestanding -c kernel.c -o kernel.o

echo "[3/6] Linking..."
# We link directly to a flat binary for the kernel
ld -m elf_i386 -T link.ld -o kernel.bin entry.o kernel.o utils.o screen.o keyboard.o disk.o fs.o holyhamer.o --oformat binary

echo "[4/6] Bootloader..."
nasm -f bin boot.asm -o boot.bin

echo "[5/6] Building Hard Drive Image..."
# Create a 10MB empty file
dd if=/dev/zero of=padding.bin bs=1M count=10
# Combine bootloader + kernel + padding
cat boot.bin kernel.bin padding.bin > om3os.bin

echo "[6/6] Launching QEMU..."
# On a tablet/cloud, you might need -display vnc=:1 or -display gtk
qemu-system-i386 -drive file=om3os.bin,format=raw
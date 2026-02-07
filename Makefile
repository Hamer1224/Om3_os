# List all object files
OBJ = entry.o kernel.o utils.o screen.o keyboard.o disk.o fs.o holyhamer.o editor.o
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -c
# Build the final OS image
all: om3os.bin

om3os.bin: boot.bin kernel.bin
	dd if=/dev/zero of=padding.bin bs=1M count=10
	cat boot.bin kernel.bin padding.bin > om3os.bin
	rm padding.bin

			# Link the kernel
kernel.bin: $(OBJ)
	ld -m elf_i386 -T link.ld -o kernel.bin $(OBJ) --oformat binary

				# Compile C files
%.o: %.c
	gcc $(CFLAGS) $< -o $@

entry.o: entry.asm
	nasm -f elf32 entry.asm -o entry.o

						# Assemble bootloader
boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin

							# The "Clean" command to fix build errors
clean:
	rm -f *.o *.bin om3os.bin
release: all
	@echo "Packaging release version..."
	# This creates a copy and renames it with a version number
	cp om3os.bin om3os_v1.2_release.bin
	@echo "Release created: om3os_v1.2_release.bin"
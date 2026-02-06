C_SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJ = ${C_SOURCES:.c=.o} entry.o

# Flags for 32-bit cross-compilation on 64-bit Linux
CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdinc -I.

all: om3os.bin

om3os.bin: boot.bin kernel.bin
	dd if=/dev/zero of=padding.bin bs=1M count=10
		cat boot.bin kernel.bin padding.bin > om3os.bin
			rm padding.bin

			kernel.bin: ${OBJ}
				ld -m elf_i386 -T link.ld -o $@ $^ --oformat binary

				%.o: %.c ${HEADERS}
					gcc ${CFLAGS} -c $< -o $@

					entry.o: entry.asm
						nasm -f elf32 $< -o $@

						boot.bin: boot.asm
							nasm -f bin $< -o $@

							clean:
								rm -rf *.o *.bin kernel.bin om3os.bin
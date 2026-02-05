// disk.c - Simple ATA PIO Driver
#include "headers.h"

// Wait for the drive to be ready (BSY bit clear, DRQ bit set)
void ata_wait_ready() {
    while ((port_byte_in(0x1F7) & 0xC0) != 0x40);
}

void ata_read_sector(int sector, void* buffer) {
    ata_wait_ready();
    port_byte_out(0x1F6, 0xE0 | ((sector >> 24) & 0x0F)); // Master drive + top 4 bits
    port_byte_out(0x1F2, 1);        // Read 1 sector count
    port_byte_out(0x1F3, (unsigned char) sector);
    port_byte_out(0x1F4, (unsigned char)(sector >> 8));
    port_byte_out(0x1F5, (unsigned char)(sector >> 16));
    port_byte_out(0x1F7, 0x20);     // Command 0x20 = READ SECTOR

    ata_wait_ready();
    // Read 256 words (512 bytes)
    port_word_in(0x1F0, buffer, 256);
}

void ata_write_sector(int sector, void* buffer) {
    ata_wait_ready();
    port_byte_out(0x1F6, 0xE0 | ((sector >> 24) & 0x0F));
    port_byte_out(0x1F2, 1);        // Write 1 sector count
    port_byte_out(0x1F3, (unsigned char) sector);
    port_byte_out(0x1F4, (unsigned char)(sector >> 8));
    port_byte_out(0x1F5, (unsigned char)(sector >> 16));
    port_byte_out(0x1F7, 0x30);     // Command 0x30 = WRITE SECTOR

    ata_wait_ready();
    // Write 256 words (512 bytes)
    port_word_out(0x1F0, buffer, 256);
}
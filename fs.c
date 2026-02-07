// fs.c - Om File System
#include "headers.h"

#define FS_START_SECTOR 100
#define MAX_FILES 64
#define FILE_NAME_LEN 16

// The structure of one file entry (Total 32 bytes)
struct FileEntry
{
    char name[FILE_NAME_LEN]; // 16 bytes
    int sector_id;            // 4 bytes (Where the data is)
    int size;                 // 4 bytes
    char padding[8];          // Padding to align to 32 bytes
};

// We need a buffer to hold the file table (512 bytes)
struct FileEntry file_table[MAX_FILES];
char sector_buffer[512]; // General purpose buffer

// Load the table from disk into memory
void load_table()
{
    ata_read_sector(FS_START_SECTOR, file_table);
}

// Save the memory table back to disk
void save_table()
{
    ata_write_sector(FS_START_SECTOR, file_table);
}

// 1. FORMAT: Wipe the table
void fs_format()
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        file_table[i].name[0] = 0; // Empty name = Free slot
        file_table[i].sector_id = 0;
        file_table[i].size = 0;
    }
    save_table();
    print_string("omfs formatted. all data wiped.\n");
}

// 2. LIST: Show files
void fs_list()
{
    load_table();
    print_string("--- omfs files ---\n");
    int found = 0;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (file_table[i].name[0] != 0)
        {
            print_string(file_table[i].name);
            print_string(" (size: ");
            char size_str[10];
            int_to_string(file_table[i].size, size_str);
            print_string(size_str);
            print_string(" bytes)\n");
            found = 1;
        }
    }
    if (!found)
        print_string("(empty)\n");
}

// 3. CREATE: Write a new file
void fs_create(char *name, char *content)
{
    load_table();

    // Find free slot
    int slot = -1;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (file_table[i].name[0] == 0)
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
    {
        print_string("error: disk full (max 16 files).\n");
        return;
    }

    // Write content to the data sector
    // Data sectors start at 101, 102, etc. (100 + 1 + slot)
    int data_sector = FS_START_SECTOR + 1 + slot;

    // Clear buffer and copy content
    for (int i = 0; i < 512; i++)
        sector_buffer[i] = 0;

    int len = 0;
    while (content[len] != 0 && len < 512)
    {
        sector_buffer[len] = content[len];
        len++;
    }

    ata_write_sector(data_sector, sector_buffer);

    // Update Table
    memory_copy(name, file_table[slot].name, FILE_NAME_LEN);
    file_table[slot].sector_id = data_sector;
    file_table[slot].size = len;
    save_table();

    print_string("file saved: ");
    print_string(name);
    print_string("\n");
}

// 4. READ: Cat a file
void fs_read(char *name)
{
    load_table();
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (strcmp(file_table[i].name, name) == 0)
        {
            // Found it! Read the data sector.
            ata_read_sector(file_table[i].sector_id, sector_buffer);
            print_string("content: ");
            print_string(sector_buffer);
            print_string("\n");
            return;
        }
    }
    print_string("error: file not found.\n");
}

// A simple structure for a file


char *fs_read_file(char *filename)
{
    load_table();
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (strcmp(file_table[i].name, filename) == 0)
        {
            print_string("found file");
            ata_read_sector(file_table[i].sector_id, sector_buffer);
            return (char*)sector_buffer;
        }
    }
    return (char *)0; // File not found
}


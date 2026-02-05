#ifndef HEADERS_H
#define HEADERS_H

// --- SCREEN SETTINGS ---
#define VIDEO_ADDRESS 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// --- GLOBAL VARIABLES ---
extern int cursor_col;
extern int cursor_row;
extern unsigned char current_color;
extern int holyhamer_mode;
extern int is_root;

// --- UTILS (utils.c) ---
unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
void port_word_in(unsigned short port, void* buffer, int count);
void port_word_out(unsigned short port, void* buffer, int count);
void memory_copy(char* source, char* dest, int no_bytes);
int strcmp(char s1[], char s2[]);
int starts_with(char *main, char *prefix);
void int_to_string(int n, char str[]);
int string_to_int(char* str);

// --- SCREEN (screen.c) ---
void clear_screen();
void print_string(char* str);
void print_char(char c);
void print_backspace();
void update_cursor();
void set_terminal_color(char* color_name);

// --- KEYBOARD (keyboard.c) ---
char get_key_from_scancode(unsigned char scancode);
void set_keyboard_layout(char* layout);

// --- DISK (disk.c) ---
void ata_read_sector(int sector, void* buffer);
void ata_write_sector(int sector, void* buffer);

// --- FILESYSTEM (fs.c) ---
void fs_format();
void fs_list();
void fs_create(char* name, char* content);
void fs_read(char* name);
void fs_delete(char* name);

// --- HOLYHAMER (holyhamer.c) ---
void hh_init();
void run_holyhamer_code();

#endif
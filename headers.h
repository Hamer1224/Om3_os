// headers.h - Function Declarations
#ifndef HEADERS_H
#define HEADERS_H

// --- TYPES ---
// We define "Video Memory" location here so screen.c knows it
#define VIDEO_ADDRESS 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// --- GLOBAL VARIABLES (Shared across files) ---
// "extern" means: "This variable exists, but it's defined in another file"
extern int cursor_col;
extern int cursor_row;
extern unsigned char current_color;
extern int holyhamer_mode;
extern int is_root;

// --- FUNCTION PROTOTYPES ---

// utils.c
unsigned char port_byte_in(unsigned short port);
int strcmp(char s1[], char s2[]);
int starts_with(char *main, char *prefix);
void memory_copy(char* source, char* dest, int no_bytes);

// screen.c
void clear_screen();
void print_string(char* str);
void print_char(char c);
void update_cursor();
void set_terminal_color(char* color_name);

// keyboard.c
char get_key_from_scancode(unsigned char scancode);

#endif
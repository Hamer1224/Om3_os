// kernel.c - OM3 OS (Lowercase Version)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile char* video_memory = (volatile char*)0xb8000;
int cursor_col = 0;
int cursor_row = 0;

// --- MEMORY ---
char key_buffer[256];
int buffer_index = 0;

// GLOBAL FLAGS
int holyhamer_mode = 0;
int is_root = 0; 

// --- 1. LOW LEVEL I/O ---
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// --- 2. STRING TOOLS ---
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

int starts_with(char *main, char *prefix) {
    while (*prefix) {
        if (*prefix++ != *main++) return 0;
    }
    return 1;
}

// --- 3. KEYBOARD MAP (No Shift Support Yet) ---
unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ',   0,
};

// --- 4. VIDEO DRIVER ---
void update_cursor() {
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = '_';
    video_memory[offset + 1] = 0x0F;
}

void clear_cursor_visual() {
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = ' ';
    video_memory[offset + 1] = 0x0F;
}

void print_char(char c) {
    if (c == '\n') {
        clear_cursor_visual();
        cursor_row++;
        cursor_col = 0;
        return;
    }
    clear_cursor_visual();
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = c;
    video_memory[offset + 1] = 0x0F;
    cursor_col++;
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }
}

void print_string(const char* str) {
    int i = 0;
    while (str[i] != 0) {
        print_char(str[i]);
        i++;
    }
}

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = 0x0F;
    }
    cursor_col = 0;
    cursor_row = 0;
}

// --- 5. HOLYHAMER ENGINE (Simplified) ---
void run_holyhamer_code() {
    print_string("\n"); 

    if (strcmp(key_buffer, "exit") == 0) {
        holyhamer_mode = 0;
        print_string("exiting holyhamer environment...\n");
    }
    // NEW SYNTAX: "print <text>" (No parentheses or quotes needed)
    else if (starts_with(key_buffer, "print ")) {
        // Skip first 6 chars ("print ")
        char* content = key_buffer + 6;
        print_string(content);
        print_string("\n");
    }
    else {
        print_string("hh error: syntax error.\n");
    }
}

// --- 6. SHELL LOGIC ---
void execute_command() {
    print_string("\n"); 

    if (holyhamer_mode == 1) {
        run_holyhamer_code();
    }
    else {
        // --- COMMANDS ---
        if (strcmp(key_buffer, "help") == 0) {
            print_string("  holyhamer - programming mode\n");
            print_string("  super     - become root (sudo)\n");
            print_string("  whoami    - show current user\n");
            print_string("  clear     - clear screen\n");
        } 
        else if (strcmp(key_buffer, "holyhamer") == 0) { // LOWERCASE
            holyhamer_mode = 1; 
            print_string("holyhamer compiler v0.1 initialized.\n");
            print_string("type 'exit' to return.\n");
        }
        else if (strcmp(key_buffer, "super") == 0) {
            if (is_root == 0) {
                is_root = 1;
                print_string("password accepted. you are now root.\n");
            } else {
                is_root = 0;
                print_string("you are now a standard user.\n");
            }
        }
        else if (strcmp(key_buffer, "whoami") == 0) {
            if (is_root == 1) print_string("root\n");
            else print_string("user\n");
        }
        else if (strcmp(key_buffer, "clear") == 0) {
            clear_screen();
            cursor_row = -1; 
        }
        else if (buffer_index > 0) {
            print_string("unknown command.\n");
        }
    }

    // Reset buffer
    for(int i=0; i<256; i++) key_buffer[i] = 0;
    buffer_index = 0;

    // --- PROMPT DISPLAY ---
    if (cursor_row < 0) cursor_row = 0;

    if (holyhamer_mode == 1) {
        print_string("hh> "); // Lowercase prompt
    } else {
        if (is_root == 1) print_string("root# ");
        else print_string("om3$ ");
    }
}

// --- 7. MAIN KERNEL ---
void kmain() {
    clear_screen();
    print_string("welcome to om3 os v1.2\n");
    print_string("type 'help' for commands.\n\n");
    print_string("om3$ ");
    
    update_cursor();

    while(1) {
        if (port_byte_in(0x64) & 0x1) {
            unsigned char scan_code = port_byte_in(0x60);
            if (scan_code < 128) {
                char c = keyboard_map[scan_code];

                if (c == '\b') {
                    if (buffer_index > 0) {
                        clear_cursor_visual();
                        cursor_col--;
                        int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
                        video_memory[offset] = ' '; video_memory[offset + 1] = 0x0F;
                        buffer_index--;
                        key_buffer[buffer_index] = 0;
                        update_cursor();
                    }
                }
                else if (c == '\n') {
                    clear_cursor_visual();
                    execute_command(); // Logic handles mode check
                    update_cursor();
                }
                else if (c != 0) {
                    print_char(c);      
                    update_cursor();
                    if (buffer_index < 255) {
                        key_buffer[buffer_index] = c;
                        buffer_index++;
                    }
                }
            }
        }
        for(int i = 0; i < 500000; i++) { __asm__ volatile ("nop"); }
    }
}
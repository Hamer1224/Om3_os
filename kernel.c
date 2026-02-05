// kernel.c - OM3 OS with Command Shell
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile char* video_memory = (volatile char*)0xb8000;
int cursor_col = 0;
int cursor_row = 0;

// --- THE MEMORY (Command Buffer) ---
char key_buffer[256]; // Stores the current command
int buffer_index = 0;

// --- 1. LOW LEVEL I/O ---
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// --- 2. STRING TOOLS (Since we don't have <string.h>) ---
// Returns 0 if strings are same, non-zero if different
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

// --- 3. KEYBOARD MAP ---
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

// --- 5. SHELL LOGIC ( The New Brain ) ---
void execute_command() {
    print_string("\n"); // Move to new line

    // Compare buffer with known commands
    if (strcmp(key_buffer, "help") == 0) {
        print_string("Available commands:\n");
        print_string("  help   - Show this list\n");
        print_string("  clear  - Clear the screen\n");
        print_string("  hello  - Say hi\n");
    } 
    else if (strcmp(key_buffer, "clear") == 0) {
        clear_screen();
        cursor_row = -1; // Hack: It will become 0 after the newline below
    }
    else if (strcmp(key_buffer, "hello") == 0) {
        print_string("Hello! Welcome to OM3 OS.\n");
    }
    else if (buffer_index > 0) {
        print_string("Unknown command: ");
        print_string(key_buffer);
        print_string("\n");
    }

    // Reset the buffer for the next command
    for(int i=0; i<256; i++) key_buffer[i] = 0;
    buffer_index = 0;

    if (cursor_row >= 0) print_string("OM3> ");
    else {
         // Handle special case for clear screen
         cursor_row = 0; 
         print_string("OM3> ");
    }
}

// --- 6. MAIN KERNEL ---
void kmain() {
    clear_screen();
    print_string("Welcome to OM3 OS v1.0\n");
    print_string("Type 'help' for commands.\n\n");
    print_string("OM3> ");
    
    update_cursor();

    while(1) {
        if (port_byte_in(0x64) & 0x1) {
            unsigned char scan_code = port_byte_in(0x60);
            if (scan_code < 128) {
                char c = keyboard_map[scan_code];

                // Case 1: BACKSPACE
                if (c == '\b') {
                    if (buffer_index > 0) {
                        // Remove from screen
                        clear_cursor_visual();
                        cursor_col--;
                        int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
                        video_memory[offset] = ' ';
                        video_memory[offset + 1] = 0x0F;
                        
                        // Remove from memory buffer
                        buffer_index--;
                        key_buffer[buffer_index] = 0;
                        
                        update_cursor();
                    }
                }
                // Case 2: ENTER
                else if (c == '\n') {
                    clear_cursor_visual();
                    execute_command(); // Run the logic!
                    update_cursor();
                }
                // Case 3: NORMAL LETTER
                else if (c != 0) {
                    print_char(c);      
                    update_cursor();
                    
                    // Save to memory buffer
                    if (buffer_index < 255) {
                        key_buffer[buffer_index] = c;
                        buffer_index++;
                    }
                }
            }
        }
        // Delay loop
        for(int i = 0; i < 500000; i++) { __asm__ volatile ("nop"); }
    }
}
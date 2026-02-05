// kernel.c - OM3 OS with Cursor & Backspace
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile char* video_memory = (volatile char*)0xb8000;
int cursor_col = 0;
int cursor_row = 0;

// --- 1. LOW LEVEL I/O ---
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// --- 2. KEYBOARD MAP ---
unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', /* Backspace */
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter */
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ',   0,
    // ... (rest is 0 for simplicity)
};

// --- 3. CURSOR FUNCTION (The Visual Line) ---
void update_cursor() {
    // Calculate where the cursor should be
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    
    // Draw the underscore '_'
    video_memory[offset] = '_';
    video_memory[offset + 1] = 0x0F; // White text
}

void clear_cursor_visual() {
    // Erase the underscore (replace with space) before moving
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = ' ';
    video_memory[offset + 1] = 0x0F;
}

// --- 4. VIDEO DRIVER ---
void print_char(char c) {
    // 1. Handle Backspace
    if (c == '\b') {
        if (cursor_col > 0) {
            clear_cursor_visual(); // Erase current cursor
            cursor_col--;
            
            // Delete the character at the new position
            int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
            video_memory[offset] = ' '; 
            video_memory[offset + 1] = 0x0F;
        }
        return;
    }

    // 2. Handle Newline
    if (c == '\n') {
        clear_cursor_visual(); // Erase old cursor
        cursor_row++;
        cursor_col = 0;
        return;
    }

    // 3. Normal Character
    clear_cursor_visual(); // Erase the '_' to make room for the letter
    
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = c;
    video_memory[offset + 1] = 0x0F; // White on Black

    cursor_col++;
    
    // Wrap to next line if we hit the edge
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

// --- 5. MAIN KERNEL ---
void kmain() {
    clear_screen();
    print_string("OM3 OS - Command Line\n");
    print_string("Type something > ");
    
    // Draw the initial cursor
    update_cursor();

    while(1) {
        if (port_byte_in(0x64) & 0x1) {
            unsigned char scan_code = port_byte_in(0x60);

            if (scan_code < 128) {
                char c = keyboard_map[scan_code];
                if (c != 0) {
                    print_char(c);      // 1. Print the letter (erases old cursor)
                    update_cursor();    // 2. Draw new cursor at new spot
                }
            }
        }
        
        // Delay loop
        for(int i = 0; i < 500000; i++) { __asm__ volatile ("nop"); }
    }
}
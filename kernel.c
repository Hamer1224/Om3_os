// kernel.c - OM3 OS v1.5 (Caps Lock + Turkish Keyboard)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile char* video_memory = (volatile char*)0xb8000;
int cursor_col = 0;
int cursor_row = 0;

// --- MEMORY ---
char key_buffer[256];
int buffer_index = 0;

// --- STATE FLAGS ---
int holyhamer_mode = 0;
int is_root = 0; 
unsigned char current_color = 0x0F; 
int caps_lock = 0; // 0 = Off, 1 = On

// --- 1. LOW LEVEL I/O ---
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// --- 2. KEYBOARD MAPS ---

// POINTER to the currently active map
unsigned char* current_key_map; 

// MAP 1: US QWERTY (Standard)
unsigned char us_key_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ',   0,
};

// MAP 2: TURKISH Q (Adapted for VGA Font)
// Changes: i->ı, ;->ş, '->i, [->ğ, ]->ü, ,->ö, .->ç, /->.
unsigned char tr_key_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'g', 129, '\n', // 129 = ü
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 's', 'i', ',',  0, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', 148, 135, '.',   0, '*',   0, ' ',   0, // 148=ö, 135=ç
};

// --- 3. STRING TOOLS ---
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

// --- 4. VIDEO DRIVER ---
void scroll() {
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++) {
        video_memory[i] = video_memory[i + (VGA_WIDTH * 2)];
    }
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i < VGA_HEIGHT * VGA_WIDTH * 2; i += 2) {
        video_memory[i] = ' '; video_memory[i + 1] = current_color;
    }
}

void update_cursor() {
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = '_'; video_memory[offset + 1] = current_color;
}

void clear_cursor_visual() {
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = ' '; video_memory[offset + 1] = current_color;
}

void print_char(char c) {
    if (c == '\n') {
        clear_cursor_visual();
        cursor_row++; cursor_col = 0;
    } else {
        clear_cursor_visual();
        int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
        video_memory[offset] = c;
        video_memory[offset + 1] = current_color; 
        cursor_col++;
    }
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0; cursor_row++;
    }
    if (cursor_row >= VGA_HEIGHT) {
        scroll();
        cursor_row = VGA_HEIGHT - 1;
    }
}

void print_string(const char* str) {
    int i = 0;
    while (str[i] != 0) {
        print_char(str[i]); i++;
    }
}

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' '; video_memory[i + 1] = current_color;
    }
    cursor_col = 0; cursor_row = 0;
}

void set_terminal_color(char* color_name) {
    if (strcmp(color_name, "red") == 0) current_color = 0x04;
    else if (strcmp(color_name, "blue") == 0) current_color = 0x01;
    else if (strcmp(color_name, "green") == 0) current_color = 0x02;
    else if (strcmp(color_name, "cyan") == 0) current_color = 0x03;
    else if (strcmp(color_name, "white") == 0) current_color = 0x0F;
    else if (strcmp(color_name, "purple") == 0) current_color = 0x05;
    else print_string("unknown color.\n");
}

// --- 5. HOLYHAMER ENGINE ---
void run_holyhamer_code() {
    print_string("\n"); 
    if (strcmp(key_buffer, "exit") == 0) {
        holyhamer_mode = 0;
        print_string("exiting holyhamer...\n");
    }
    else if (starts_with(key_buffer, "print ")) {
        print_string(key_buffer + 6);
        print_string("\n");
    }
    else if (starts_with(key_buffer, "paint ")) {
        set_terminal_color(key_buffer + 6);
        print_string("color changed.\n");
    }
    else {
        print_string("hh error.\n");
    }
}

// --- 6. SHELL LOGIC ---
void execute_command() {
    print_string("\n"); 
    if (holyhamer_mode == 1) {
        run_holyhamer_code();
    }
    else {
        if (strcmp(key_buffer, "help") == 0) {
            print_string("  holyhamer    - programming mode\n");
            print_string("  keyboard tr  - turkish q layout\n");
            print_string("  keyboard us  - us layout\n");
            print_string("  color <x>    - change color\n");
        } 
        else if (strcmp(key_buffer, "holyhamer") == 0) {
            holyhamer_mode = 1; 
            print_string("holyhamer compiler v0.2\n");
        }
        else if (strcmp(key_buffer, "keyboard tr") == 0) {
            current_key_map = tr_key_map;
            print_string("keyboard set to turkish q.\n");
        }
        else if (strcmp(key_buffer, "keyboard us") == 0) {
            current_key_map = us_key_map;
            print_string("keyboard set to us qwerty.\n");
        }
        else if (starts_with(key_buffer, "color ")) {
            set_terminal_color(key_buffer + 6);
        }
        else if (strcmp(key_buffer, "clear") == 0) {
            clear_screen(); cursor_row = -1; 
        }
        else if (buffer_index > 0) {
            print_string("unknown command.\n");
        }
    }
    
    // Clear buffer
    for(int i=0; i<256; i++) key_buffer[i] = 0;
    buffer_index = 0;

    if (cursor_row < 0) cursor_row = 0;
    
    if (holyhamer_mode == 1) print_string("hh> ");
    else if (is_root == 1) print_string("root# ");
    else print_string("om3$ ");
}

// --- 7. MAIN KERNEL ---
void kmain() {
    // Default to US map
    current_key_map = us_key_map;
    
    clear_screen();
    print_string("welcome to om3 os v1.5\n");
    print_string("caps lock & turkish keyboard supported.\n");
    print_string("om3$ ");
    
    update_cursor();

    while(1) {
        if (port_byte_in(0x64) & 0x1) {
            unsigned char scan_code = port_byte_in(0x60);
            
            // --- CAPS LOCK TOGGLE (0x3A) ---
            if (scan_code == 0x3A) {
                caps_lock = !caps_lock; // Toggle On/Off
                
                // Hack: Read it again or delay slightly to avoid bouncing
                for(int i=0; i<10000; i++) __asm__ volatile ("nop");
                continue; 
            }

            if (scan_code < 128) {
                // Use the ACTIVE map (US or TR)
                char c = current_key_map[scan_code];

                // --- CAPS LOCK LOGIC ---
                if (caps_lock == 1) {
                    // Lowercase a-z to Uppercase A-Z
                    if (c >= 'a' && c <= 'z') c -= 32;
                    
                    // Turkish Specific Capitalization (for supported chars)
                    else if (c == 135) c = 128; // ç -> Ç
                    else if (c == 129) c = 154; // ü -> Ü
                    else if (c == 148) c = 153; // ö -> Ö
                }

                if (c == '\b') {
                    if (buffer_index > 0) {
                        clear_cursor_visual();
                        cursor_col--;
                        int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
                        video_memory[offset] = ' '; video_memory[offset + 1] = current_color;
                        buffer_index--;
                        key_buffer[buffer_index] = 0;
                        update_cursor();
                    }
                }
                else if (c == '\n') {
                    clear_cursor_visual();
                    execute_command();
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
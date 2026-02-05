// kernel.c - OM3 OS Main Logic
#include "headers.h"

// Internal Memory
char key_buffer[256];
int buffer_index = 0;

// Flags
int holyhamer_mode = 0;
int is_root = 0;

// Forward declarations for set_keyboard_layout (since it's internal to keyboard.c logic usually)
// In a pro setup, we'd add it to headers.h, but we can do it here:
void set_keyboard_layout(char* layout);

// --- HOLYHAMER ENGINE ---
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

// --- SHELL ---
void execute_command() {
    print_string("\n"); 
    if (holyhamer_mode) {
        run_holyhamer_code();
    } else {
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
            set_keyboard_layout("tr");
            print_string("keyboard set to turkish q.\n");
        }
        else if (strcmp(key_buffer, "keyboard us") == 0) {
            set_keyboard_layout("us");
            print_string("keyboard set to us qwerty.\n");
        }
        else if (starts_with(key_buffer, "color ")) {
            set_terminal_color(key_buffer + 6);
        }
        else if (strcmp(key_buffer, "clear") == 0) {
            clear_screen(); 
            // We can't access cursor_row directly if we didn't extern it, 
            // but we did in headers.h, so this is valid:
            extern int cursor_row;
            cursor_row = -1; 
        }
        else if (buffer_index > 0) {
            print_string("unknown command.\n");
        }
    }
    
    // Clear buffer
    for(int i=0; i<256; i++) key_buffer[i] = 0;
    buffer_index = 0;

    extern int cursor_row;
    if (cursor_row < 0) cursor_row = 0;
    
    if (holyhamer_mode) print_string("hh> ");
    else if (is_root) print_string("root# ");
    else print_string("om3$ ");
}

void kmain() {
    clear_screen();
    print_string("welcome to om3 os v1.6 (modular)\n");
    print_string("om3$ ");
    
    update_cursor();

    while(1) {
        if (port_byte_in(0x64) & 0x1) {
            unsigned char scan_code = port_byte_in(0x60);
            char c = get_key_from_scancode(scan_code);

            if (c == 0) continue; // Handled internally (Caps lock, etc)

            if (c == '\b') {
                if (buffer_index > 0) {
                    // Backspace logic manually here or move to screen.c later
                    // For now, let's keep it simple:
                    print_char('\b'); // Not implemented visually yet, so we do manual:
                    
                    extern int cursor_col;
                    extern int cursor_row;
                    int offset = (cursor_row * VGA_WIDTH + (cursor_col-1)) * 2;
                    // Visual hack for now (we should move backspace logic to screen.c eventually)
                     // Re-implementing visual backspace briefly here for safety:
                    if (cursor_col > 0) {
                         cursor_col--;
                         offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
                         char* vid = (char*)VIDEO_ADDRESS;
                         vid[offset] = ' ';
                         update_cursor();
                    }

                    buffer_index--;
                    key_buffer[buffer_index] = 0;
                }
            }
            else if (c == '\n') {
                execute_command();
                update_cursor();
            }
            else {
                print_char(c);      
                update_cursor();
                if (buffer_index < 255) {
                    key_buffer[buffer_index] = c;
                    buffer_index++;
                }
            }
        }
        // Delay
        for(int i = 0; i < 500000; i++) { __asm__ volatile ("nop"); }
    }
}
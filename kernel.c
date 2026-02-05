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

unsigned char get_rtc_register(int reg) {
    port_byte_in(0x70);             // NMI disable isn't strictly needed but good practice
    __asm__ volatile("outb %0, $0x70" : : "a"((unsigned char)reg));
    unsigned char ret = port_byte_in(0x71);
    return ret;
}

// CMOS sends "BCD" numbers (e.g. 15 is stored as hex 0x15, not 0x0F)
// We need to decode this weird format.
int bcd_to_bin(unsigned char bcd) {
    return ((bcd / 16) * 10) + (bcd & 0x0F);
}

void print_time() {
    // Read values
    int second = bcd_to_bin(get_rtc_register(0x00));
    int minute = bcd_to_bin(get_rtc_register(0x02));
    int hour   = bcd_to_bin(get_rtc_register(0x04));
    
    // Buffer to hold the number strings
    char buf[10];

    // Print Hour
    int_to_string(hour, buf);
    print_string(buf);
    print_string(":");

    // Print Minute
    if (minute < 10) print_string("0"); // Padding zero
    int_to_string(minute, buf);
    print_string(buf);
    print_string(":");

    // Print Second
    if (second < 10) print_string("0"); // Padding zero
    int_to_string(second, buf);
    print_string(buf);
    
    print_string(" UTC\n");
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
        else if (strcmp(key_buffer, "time") == 0) {
            print_string("current rtc time: ");
            print_time();
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
                    print_backspace();
                
                    // Remove character from Memory (Buffer)
                    buffer_index--;
                    key_buffer[buffer_index] = 0;
                    
                    update_cursor();
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
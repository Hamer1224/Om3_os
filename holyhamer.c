// holyhamer.c - The HolyHamer Scripting Language
#include "headers.h"

// The Memory: 26 integer slots for variables 'a' through 'z'
int variables[26]; 

void hh_init() {
    // Clear memory on startup
    for(int i=0; i<26; i++) variables[i] = 0;
}

// Helper: Get array index from char (e.g., 'a' -> 0, 'z' -> 25)
int get_var_index(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a';
    return -1;
}

void run_holyhamer_code() {
    // We need to access key_buffer from kernel.c
    // Since we didn't extern it in headers, let's grab it directly here or pass it in.
    // BETTER WAY: Let's assume key_buffer is available via a getter or extern.
    // For simplicity in this step, let's declare it extern here manually:
    extern char key_buffer[];
    extern int holyhamer_mode;

    print_string("\n"); 

    if (strcmp(key_buffer, "exit") == 0) {
        holyhamer_mode = 0;
        print_string("exiting holyhamer...\n");
    }
    // --- 1. VAR: var x 10 ---
    else if (starts_with(key_buffer, "var ")) {
        char var_char = key_buffer[4]; // The letter (e.g. 'x')
        char* val_str = key_buffer + 6; // The value (e.g. "10")
        
        int idx = get_var_index(var_char);
        if (idx != -1) {
            int val = string_to_int(val_str);
            variables[idx] = val;
            print_string("saved.\n");
        } else {
            print_string("hh error: use lowercase a-z.\n");
        }
    }
    // --- 2. ADD: add x 5 ---
    else if (starts_with(key_buffer, "add ")) {
        char var_char = key_buffer[4];
        char* val_str = key_buffer + 6;
        
        int idx = get_var_index(var_char);
        if (idx != -1) {
            int val = string_to_int(val_str);
            variables[idx] += val;
            print_string("updated.\n");
        }
    }
    // --- 3. SUB: sub x 5 ---
    else if (starts_with(key_buffer, "sub ")) {
        char var_char = key_buffer[4];
        char* val_str = key_buffer + 6;
        
        int idx = get_var_index(var_char);
        if (idx != -1) {
            int val = string_to_int(val_str);
            variables[idx] -= val;
            print_string("updated.\n");
        }
    }
    // --- 4. PRINT: print $x OR print hello ---
    else if (starts_with(key_buffer, "print ")) {
        char* content = key_buffer + 6;
        
        // Check if it's a variable (starts with $)
        if (content[0] == '$') {
            char var_char = content[1];
            int idx = get_var_index(var_char);
            if (idx != -1) {
                char num_str[16];
                int_to_string(variables[idx], num_str);
                print_string(num_str);
                print_string("\n");
            } else {
                print_string("0\n");
            }
        } else {
            // Normal text
            print_string(content);
            print_string("\n");
        }
    }
    // --- PAINT ---
    else if (starts_with(key_buffer, "paint ")) {
        set_terminal_color(key_buffer + 6);
        print_string("color changed.\n");
    }
    else {
        print_string("hh error: unknown command.\n");
    }
}
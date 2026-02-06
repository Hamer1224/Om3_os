// holyhamer.c - The HolyHamer Scripting Language for OM3 OS
#include "headers.h"

// The Memory: 26 integer slots for variables 'a' through 'z'
int variables[26];

void hh_init()
{
    // Initialize all variables to zero
    for (int i = 0; i < 26; i++)
    {
        variables[i] = 0;
    }
}

// Helper: Get array index from character (e.g., 'a' -> 0, 'z' -> 25)
int get_var_index(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a';
    }
    return -1;
}

void run_holyhamer_code()
{
    // Access global variables from kernel.c
    extern char key_buffer[];
    extern int holyhamer_mode;

    print_string("\n");

    // --- EXIT ---
    if (strcmp(key_buffer, "exit") == 0)
    {
        holyhamer_mode = 0;
        print_string("exiting holyhamer...\n");
    }

    // --- CLEAR ---
    else if (strcmp(key_buffer, "clear") == 0)
    {
        clear_screen();
    }

    // --- VAR: var x 10 OR var x = 10 ---
    else if (starts_with(key_buffer, "var "))
    {
        char var_char = key_buffer[4]; // The variable letter
        char *val_ptr = key_buffer + 5;

        // Skip spaces and equals signs to find the number
        while (*val_ptr == ' ' || *val_ptr == '=')
            val_ptr++;

        int idx = get_var_index(var_char);
        if (idx != -1)
        {
            variables[idx] = string_to_int(val_ptr);
            print_string("saved.\n");
        }
        else
        {
            print_string("hh error: variable name must be a-z.\n");
        }
    }

    // --- ADD: add x 5 ---
    else if (starts_with(key_buffer, "add "))
    {
        char var_char = key_buffer[4];
        char *val_ptr = key_buffer + 5;
        while (*val_ptr == ' ' || *val_ptr == '=')
            val_ptr++;

        int idx = get_var_index(var_char);
        if (idx != -1)
        {
            variables[idx] += string_to_int(val_ptr);
            print_string("updated.\n");
        }
    }

    // --- SUB: sub x 5 ---
    else if (starts_with(key_buffer, "sub "))
    {
        char var_char = key_buffer[4];
        char *val_ptr = key_buffer + 5;
        while (*val_ptr == ' ' || *val_ptr == '=')
            val_ptr++;

        int idx = get_var_index(var_char);
        if (idx != -1)
        {
            variables[idx] -= string_to_int(val_ptr);
            print_string("updated.\n");
        }
    }

    // --- PRINT: print hello OR print y ---
    else if (starts_with(key_buffer, "print "))
    {
        char *content = key_buffer + 6;

        // Check if content is a single variable name (e.g., 'y')
        if (content[0] >= 'a' && content[0] <= 'z' && content[1] == '\0')
        {
            int idx = get_var_index(content[0]);
            char num_str[16];
            int_to_string(variables[idx], num_str);
            print_string(num_str);
            print_string("\n");
        }
        else
        {
            // Otherwise, treat it as normal text
            print_string(content);
            print_string("\n");
        }
    }

    // --- PAINT: paint <color_name> ---
    else if (starts_with(key_buffer, "paint "))
    {
        set_terminal_color(key_buffer + 6);
        print_string("color updated.\n");
    }

    else if (key_buffer[0] != '\0')
    {
        print_string("hh error: unknown command.\n");
    }
}
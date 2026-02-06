#include "headers.h"

extern char key_buffer[256];
extern int holyhamer_mode;

int variables[26];

void hh_init()
{
    for (int i = 0; i < 26; i++)
        variables[i] = 0;
}

int get_var_index(char c)
{
    if (c >= 'a' && c <= 'z')
        return c - 'a';
    return -1;
}

void run_holyhamer_code()
{
    print_string("\n");

    // --- EXIT ---
    if (strcmp(key_buffer, "exit") == 0)
    {
        holyhamer_mode = 0;
        print_string("exiting holyhamer...\n");
    }

    // --- CLEAR SCREEN (cls) ---
    else if (strcmp(key_buffer, "cls") == 0)
    {
        clear_screen();
    }

    // --- HELP ---
    else if (strcmp(key_buffer, "help") == 0)
    {
        print_string("HolyHamer Commands:\n");
        print_string("- var <name> <val> : Set variable\n");
        print_string("- add/sub/mul/div  : Math ops\n");
        print_string("- print <name/txt> : Show value\n");
        print_string("- list             : Show all vars\n");
        print_string("- bg <color>       : Change background\n");
        print_string("- cls              : Clear screen\n");
    }

    // --- BACKGROUND COLOR (bg) ---
    else if (starts_with(key_buffer, "bg "))
    {
        // This assumes you have a function 'set_background_color' in screen.c
        // If not, you can reuse set_terminal_color with a logic change
        set_terminal_background(key_buffer + 3);
        clear_screen();
        print_string("Background updated.\n");
    }

    // --- LIST VARIABLES ---
    else if (strcmp(key_buffer, "list") == 0)
    {
        for (int i = 0; i < 26; i++)
        {
            if (variables[i] != 0)
            {
                char out[16];
                char name[4] = {(char)('a' + i), ':', ' ', '\0'};
                print_string(name);
                int_to_string(variables[i], out);
                print_string(out);
                print_string("  ");
            }
        }
        print_string("\n");
    }

    // --- VARIABLE ASSIGNMENT ---
    else if (starts_with(key_buffer, "var "))
    {
        char var_char = key_buffer[4];
        char *val_ptr = key_buffer + 5;
        while (*val_ptr == ' ' || *val_ptr == '=')
            val_ptr++;

        int idx = get_var_index(var_char);
        if (idx != -1)
        {
            variables[idx] = string_to_int(val_ptr);
            print_string("saved.\n");
        }
    }

    // --- MATH ---
    else if (starts_with(key_buffer, "add ") || starts_with(key_buffer, "sub ") ||
             starts_with(key_buffer, "mul ") || starts_with(key_buffer, "div "))
    {
        char var_char = key_buffer[4];
        char *val_ptr = key_buffer + 5;
        while (*val_ptr == ' ' || *val_ptr == '=')
            val_ptr++;

        int idx = get_var_index(var_char);
        int val = string_to_int(val_ptr);

        if (idx != -1)
        {
            if (starts_with(key_buffer, "add "))
                variables[idx] += val;
            else if (starts_with(key_buffer, "sub "))
                variables[idx] -= val;
            else if (starts_with(key_buffer, "mul "))
                variables[idx] *= val;
            else if (starts_with(key_buffer, "div ") && val != 0)
                variables[idx] /= val;
            print_string("done.\n");
        }
    }

    else if (starts_with(key_buffer, "echo "))
    {
        print_string(key_buffer + 5);
        print_string("\n");
    }
    else if (strcmp(key_buffer, "info") == 0)
    {
        print_string("Om3 OS - HolyHamer v1.2\n");
        print_string("Memory: 26 Registers Active\n");
    }

    // --- PRINTING ---
    else if (starts_with(key_buffer, "print "))
    {
        char *content = key_buffer + 6;
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
            print_string(content);
            print_string("\n");
        }
    }
}
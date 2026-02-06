#include "headers.h"

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
    extern char key_buffer[];
    extern int holyhamer_mode;

    print_string("\n");

    if (strcmp(key_buffer, "exit") == 0)
    {
        holyhamer_mode = 0;
        print_string("exiting holyhamer...\n");
    }
    // --- SET VARIABLE (var a = 10) ---
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
    // --- MATH OPERATIONS ---
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
            if (starts_with(key_buffer, "sub "))
                variables[idx] -= val;
            if (starts_with(key_buffer, "mul "))
                variables[idx] *= val;
            if (starts_with(key_buffer, "div ") && val != 0)
                variables[idx] /= val;
            print_string("done.\n");
        }
    }
    // --- AUTOMATIC PRINT (print a) ---
    else if (starts_with(key_buffer, "print "))
    {
        char *content = key_buffer + 6;

        // If it's a single letter, print the variable value
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
    else if (key_buffer[0] != '\0')
    {
        print_string("hh error: unknown command.\n");
    }
}
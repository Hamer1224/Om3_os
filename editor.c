#include "headers.h"

#define MAX_BUFFER 1024
char edit_buf[MAX_BUFFER];

void start_text_editor(char *filename)
{
        print_string("--- om3 hlmr editor ---\n");
        print_string("Type 'SAVE' to finish script.\n");

        int pos = 0;
        edit_buf[0] = '\0';

        while (1)
        {
                print_string("edit> ");

                // Wait for the keyboard driver to set a 'ready' flag (Enter pressed)
                // You'll need to set this to 1 in your keyboard.c Enter key logic

                // Check for exit command
                if (strcmp(key_buffer, "SAVE") == 0)
                {
                        break;
                }

                // Copy key_buffer into the editor storage
                int len = strlen(key_buffer);
                if (pos + len + 2 < MAX_BUFFER)
                {
                        for (int i = 0; i < len; i++)
                        {
                                edit_buf[pos++] = key_buffer[i];
                        }
                        edit_buf[pos++] = '\n'; // Add newline for holyhamer
                        edit_buf[pos] = '\0';
                }

                
                // Note: You might need a clear_key_buffer() function here
        }

        fs_write(filename, edit_buf);
        print_string("File saved.\n");
}
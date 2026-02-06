// screen.c - Video Driver
#include "headers.h"

volatile char *video_memory = (volatile char *)VIDEO_ADDRESS;
unsigned char terminal_attribute = 0x0F; // 0 is black background, F is white text
int cursor_col = 0;
int cursor_row = 0;
unsigned char current_color = 0x0F; // White on Black

void scroll()
{
    // Move every line up
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++)
    {
        video_memory[i] = video_memory[i + (VGA_WIDTH * 2)];
    }
    // Clear bottom line
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i < VGA_HEIGHT * VGA_WIDTH * 2; i += 2)
    {
        video_memory[i] = ' ';
        video_memory[i + 1] = current_color;
    }
}
void set_terminal_background(char *color)
{
    if (strcmp(color, "blue") == 0)
        terminal_attribute = (terminal_attribute & 0x0F) | 0x10;
    else if (strcmp(color, "green") == 0)
        terminal_attribute = (terminal_attribute & 0x0F) | 0x20;
    else if (strcmp(color, "red") == 0)
        terminal_attribute = (terminal_attribute & 0x0F) | 0x40;
    else if (strcmp(color, "black") == 0)
        terminal_attribute = (terminal_attribute & 0x0F) | 0x00;
    clear_screen(); // Refresh screen with new color
}

void update_cursor()
{
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = '_';
    video_memory[offset + 1] = current_color;
}

void clear_cursor_visual()
{
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = ' ';
    video_memory[offset + 1] = current_color;
}

void print_char(char c)
{
    if (c == '\n')
    {
        clear_cursor_visual();
        cursor_row++;
        cursor_col = 0;
    }
    else
    {
        clear_cursor_visual();
        int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
        video_memory[offset] = c;
        video_memory[offset + 1] = current_color;
        cursor_col++;
    }

    if (cursor_col >= VGA_WIDTH)
    {
        cursor_col = 0;
        cursor_row++;
    }
    if (cursor_row >= VGA_HEIGHT)
    {
        scroll();
        cursor_row = VGA_HEIGHT - 1;
    }
}

void print_backspace()
{
    // 1. First, erase the current cursor visual (the underscore)
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = ' ';
    video_memory[offset + 1] = current_color;

    // 2. Decrease the cursor position
    cursor_col--;

    // 3. Handle Reverse Wrapping (Moving back to the previous line)
    if (cursor_col < 0)
    {
        cursor_col = VGA_WIDTH - 1; // Go to end of previous line
        cursor_row--;
        if (cursor_row < 0)
        {
            cursor_row = 0; // Don't go off screen at the top
            cursor_col = 0;
        }
    }

    // 4. Erase the character at the new position
    offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = ' ';
    video_memory[offset + 1] = current_color;
}

void print_string(char *str)
{
    int i = 0;
    while (str[i] != 0)
    {
        print_char(str[i]);
        i++;
    }
}

void clear_screen()
{
    // 0xB8000 is the start of VGA video memory
    unsigned char *vga_mem = (unsigned char *)0xB8000;

    // There are 80x25 characters on a standard screen
    for (int i = 0; i < 80 * 25; i++)
    {
        // Set the character to a blank space
        vga_mem[i * 2] = ' ';
        // Set the color to our GLOBAL attribute variable
        vga_mem[i * 2 + 1] = terminal_attribute;
    }

}


void set_terminal_color(char *color_name)
{
    if (strcmp(color_name, "red") == 0)
        current_color = 0x04;
    else if (strcmp(color_name, "blue") == 0)
        current_color = 0x01;
    else if (strcmp(color_name, "green") == 0)
        current_color = 0x02;
    else if (strcmp(color_name, "cyan") == 0)
        current_color = 0x03;
    else if (strcmp(color_name, "white") == 0)
        current_color = 0x0F;
    else if (strcmp(color_name, "purple") == 0)
        current_color = 0x05;
}
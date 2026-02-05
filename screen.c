// screen.c - Video Driver
#include "headers.h"

volatile char* video_memory = (volatile char*)VIDEO_ADDRESS;
int cursor_col = 0;
int cursor_row = 0;
unsigned char current_color = 0x0F; // White on Black

void scroll() {
    // Move every line up
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++) {
        video_memory[i] = video_memory[i + (VGA_WIDTH * 2)];
    }
    // Clear bottom line
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i < VGA_HEIGHT * VGA_WIDTH * 2; i += 2) {
        video_memory[i] = ' '; 
        video_memory[i + 1] = current_color;
    }
}

void update_cursor() {
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = '_'; 
    video_memory[offset + 1] = current_color;
}

void clear_cursor_visual() {
    int offset = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[offset] = ' '; 
    video_memory[offset + 1] = current_color;
}

void print_char(char c) {
    if (c == '\n') {
        clear_cursor_visual();
        cursor_row++;
        cursor_col = 0;
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

void print_string(char* str) {
    int i = 0;
    while (str[i] != 0) {
        print_char(str[i]);
        i++;
    }
}

void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = current_color;
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
}
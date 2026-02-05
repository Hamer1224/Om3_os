// kernel.c
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile char* video_memory = (volatile char*)0xb8000;

// Forward declaration
void clear_screen_blue();
void print_string(const char* str, int row, int col);

// MAIN FUNCTION
void kmain() {
    // 1. Turn the screen BLUE immediately
    clear_screen_blue();
    
    // 2. Print text
    print_string("OM3 OS IS ALIVE!", 0, 0);
    print_string("If the background is blue, C is working.", 2, 0);

    while(1);
}

// Helper: Fills screen with Blue background
void clear_screen_blue() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';      // Space
        video_memory[i + 1] = 0x1F; // Blue Background (1), White Text (F)
    }
}

void print_string(const char* str, int row, int col) {
    int offset = (row * VGA_WIDTH + col) * 2;
    int i = 0;
    while (str[i] != 0) {
        video_memory[offset + (i * 2)] = str[i];
        video_memory[offset + (i * 2) + 1] = 0x1F; // Matches blue background
        i++;
    }
}
// kernel.c - OM3 OS
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile char* video_memory = (volatile char*)0xb8000;

// Forward declarations (tell the compiler these exist so we can put kmain first)
void clear_screen();
void print_string(const char* str, int row, int col);

// CRITICAL: kmain must be the first function in the file!
void kmain() {
    clear_screen();
    
    print_string("Welcome to OM3 OS", 0, 0);
    print_string("Kernel loaded successfully.", 1, 0);
    print_string("This is 32-bit Protected Mode!", 3, 0);

    while(1);
}

// Helper functions go BELOW kmain
void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = 0x07; // Light grey
    }
}

void print_string(const char* str, int row, int col) {
    int offset = (row * VGA_WIDTH + col) * 2;
    int i = 0;
    while (str[i] != 0) {
        video_memory[offset + (i * 2)] = str[i];
        video_memory[offset + (i * 2) + 1] = 0x0F; // White text
        i++;
    }
}
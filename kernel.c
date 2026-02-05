// kernel.c - OM3 OS
// We define the height and width of the text mode screen
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Pointer to video memory
volatile char* video_memory = (volatile char*)0xb8000;

// Function to clear the screen
void clear_screen() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';     // Space character (blank)
        video_memory[i + 1] = 0x07; // Light grey on black
    }
}

// Function to print a string at a specific location
void print_string(const char* str, int row, int col) {
    int offset = (row * VGA_WIDTH + col) * 2;
    int i = 0;
    while (str[i] != 0) {
        video_memory[offset + (i * 2)] = str[i];
        video_memory[offset + (i * 2) + 1] = 0x0F; // White on Black
        i++;
    }
}

void kmain() {
    clear_screen();
    
    print_string("Welcome to OM3 OS", 0, 0);
    print_string("Kernel loaded successfully.", 1, 0);
    print_string("We are now running in 32-bit Protected Mode!", 3, 0);

    while(1);
}
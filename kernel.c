// kernel.c
// GCC provides these header files for freestanding apps
#include <stdint.h> 

void kmain() {
    // 0xb8000 is the address where the text screen video memory begins
    volatile char* video_memory = (volatile char*)0xb8000;
    
    const char* str = "OM3 OS - Now in C!";
    int i = 0;
    
    // Simple loop to write characters to video memory
    // Even bytes are the character, Odd bytes are the color
    while (str[i] != '\0') {
        video_memory[i * 2] = str[i];      // Character
        video_memory[i * 2 + 1] = 0x0F;    // Color (White on Black)
        i++;
    }
    
    // Loop forever so the OS doesn't crash
    while(1);
}
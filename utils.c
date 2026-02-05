// utils.c - Low level & String helpers
#include "headers.h"

// Read from hardware port
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void reverse(char s[]) {
    int i, j;
    char c;
    for (i = 0, j = 0; s[j] != '\0'; j++); // Find length
    j--; // Go to last char
    
    // Swap characters from outside in
    for (; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void int_to_string(int n, char str[]) {
    int i = 0;
    int sign;

    if ((sign = n) < 0) n = -n; // Handle negative numbers

    // Extract digits in reverse order
    do {
        str[i++] = n % 10 + '0'; // Get last digit and turn into char
    } while ((n /= 10) > 0);     // Remove last digit

    if (sign < 0) str[i++] = '-';
    
    str[i] = '\0'; // Null terminator
    reverse(str);  // Flip it around to be correct
}

// Compare two strings
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

// Check prefix
int starts_with(char *main, char *prefix) {
    while (*prefix) {
        if (*prefix++ != *main++) return 0;
    }
    return 1;
}
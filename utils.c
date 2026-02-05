// utils.c - Low level & String helpers
#include "headers.h"

// Read from hardware port
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
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
// keyboard.c - Keyboard Driver
#include "headers.h"

// US QWERTY
unsigned char us_key_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' ',   0,
};

// TURKISH Q
unsigned char tr_key_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'g', 129, '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 's', 'i', ',',  0, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', 148, 135, '.',   0, '*',   0, ' ',   0,
};

// Pointer to active map
unsigned char* current_key_map = us_key_map;
int caps_lock = 0;

void set_keyboard_layout(char* layout) {
    if (strcmp(layout, "tr") == 0) current_key_map = tr_key_map;
    else current_key_map = us_key_map;
}

char get_key_from_scancode(unsigned char scan_code) {
    // Handle Caps Lock
    if (scan_code == 0x3A) {
        caps_lock = !caps_lock;
        return 0;
    }
    
    if (scan_code > 128) return 0; // Ignore key releases

    char c = current_key_map[scan_code];

    if (caps_lock) {
        if (c >= 'a' && c <= 'z') c -= 32;
        else if (c == 135) c = 128; // ç -> Ç
        else if (c == 129) c = 154; // ü -> Ü
        else if (c == 148) c = 153; // ö -> Ö
    }
    return c;
}
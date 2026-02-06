// utils.c - Low level & String helpers
#include "headers.h"

// --- I/O PORTS ---

// Read byte from port
unsigned char port_byte_in(unsigned short port)
{
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write byte to port
void port_byte_out(unsigned short port, unsigned char data)
{
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Read words (16-bit) from port (for HDD)
void port_word_in(unsigned short port, void *buffer, int count)
{
    __asm__ volatile("rep insw" : "+D"(buffer), "+c"(count) : "d"(port) : "memory");
}

// Write words (16-bit) to port (for HDD)
void port_word_out(unsigned short port, void *buffer, int count)
{
    __asm__ volatile("rep outsw" : "+S"(buffer), "+c"(count) : "d"(port));
}

// --- MEMORY ---

void memory_copy(char *source, char *dest, int no_bytes)
{
    for (int i = 0; i < no_bytes; i++)
    {
        dest[i] = source[i];
    }
}

// --- STRINGS & CONVERSION ---

int strcmp(char s1[], char s2[])
{
    int i;
    for (i = 0; s1[i] == s2[i]; i++)
    {
        if (s1[i] == '\0')
            return 0;
    }
    return s1[i] - s2[i];
}

int starts_with(char *main, char *prefix)
{
    while (*prefix)
    {
        if (*prefix++ != *main++)
            return 0;
    }
    return 1;
}

void reverse(char s[])
{
    int i, j;
    char c;
    for (i = 0, j = 0; s[j] != '\0'; j++)
        ;
    j--;
    for (; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void int_to_string(int n, char str[])
{
    int i = 0;
    int sign;
    if ((sign = n) < 0)
        n = -n;
    do
    {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';
    reverse(str);
}
int strlen(char *s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}

void strcat(char *dest, char *src)
{
    int i = 0;
    int j = 0;
    // Find the end of dest
    while (dest[i] != '\0')
        i++;
    // Append src to dest
    while (src[j] != '\0')
    {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0'; // Always null-terminate!
}

int string_to_int(char *str)
{
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            res = res * 10 + (str[i] - '0');
        }
        else if (str[i] == ' ')
        {
            break;
        }
    }
    return res;
}

void filesystem_cd(char *path)
{
    // 1. Handle "cd .." (Go back)
    if (strcmp(path, "..") == 0)
    {
        if (strcmp(current_path, "/") == 0)
        {
            print_string("Already at root.\n");
            return;
        }
        // Logic to find the last '/' and terminate the string there
        int len = strlen(current_path);
        for (int i = len - 2; i >= 0; i--)
        {
            if (current_path[i] == '/')
            {
                current_path[i + 1] = '\0';
                break;
            }
        }
    }
    // 2. Handle root "cd /"
    else if (strcmp(path, "/") == 0)
    {
        current_path[0] = '/';
        current_path[1] = '\0';
    }
    // 3. Handle specific directory
    else
    {
        // Simple logic: append the folder name to the current path
        // In a real FS, you would first check if the directory exists!
        strcat(current_path, path);
        strcat(current_path, "/");
    }

    print_string("Path: ");
    print_string(current_path);
    print_string("\n");
}

int ends_with(char *str, char *suffix)
{
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    if (suffix_len > str_len)
        return 0;
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}
void strcpy(char *dest, char *src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

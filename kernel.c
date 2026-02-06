// kernel.c - OM3 OS Main Logic
#include "headers.h"

// Internal Memory (Must NOT be static so holyhamer.c can read it)
char key_buffer[256];
char current_path[256] = "/";
int buffer_index = 0;

// Flags
int holyhamer_mode = 0;
int is_root = 0;

// --- TIME HELPER ---
unsigned char get_rtc_register(int reg)
{
    port_byte_out(0x70, reg);
    return port_byte_in(0x71);
}

int bcd_to_bin(unsigned char bcd)
{
    return ((bcd / 16) * 10) + (bcd & 0x0F);
}

void print_time()
{
    int second = bcd_to_bin(get_rtc_register(0x00));
    int minute = bcd_to_bin(get_rtc_register(0x02));
    int hour = bcd_to_bin(get_rtc_register(0x04));

    char buf[10];

    int_to_string(hour, buf);
    print_string(buf);
    print_string(":");

    if (minute < 10)
        print_string("0");
    int_to_string(minute, buf);
    print_string(buf);
    print_string(":");

    if (second < 10)
        print_string("0");
    int_to_string(second, buf);
    print_string(buf);

    print_string(" UTC\n");
}

// --- SHELL ---
void execute_command()
{
    print_string("\n");
    if (holyhamer_mode)
    {
        // This function is now inside holyhamer.c!
        run_holyhamer_code();
    }
    else
    {
        if (strcmp(key_buffer, "help") == 0)
        {
            print_string("  holyhamer    - programming mode\n");
            print_string("  write <n> <c>- save file\n");
            print_string("  ls           - list files\n");
            print_string("  cat <n>      - read file\n");
            print_string("  format       - wipe disk\n");
        }
        // Inside your command processing function
        else if (strcmp(key_buffer, "holyhamer") == 0)
        {
            holyhamer_mode = 1;
            print_string("holyhamer compiler v1.0\n");
            print_string("commands: var, add, sub, print\n");
        }
        else if (strcmp(key_buffer, "time") == 0)
        {
            print_string("current time: ");
            print_time();
        }
        else if (strcmp(key_buffer, "format") == 0)
        {
            if (is_root)
                fs_format();
            else
                print_string("error: root required.\n");
        }
        else if (strcmp(key_buffer, "ls") == 0)
        {
            fs_list();
        }
        else if (starts_with(key_buffer, "write "))
        {
            char *name = key_buffer + 6;
            char *content = name;
            while (*content != ' ' && *content != 0)
                content++;
            if (*content == ' ')
            {
                *content = 0;
                content++;
                fs_create(name, content);
            }
            else
            {
                print_string("usage: write <name> <content>\n");
            }
        }
        else if (starts_with(key_buffer, "cd "))
        {
            filesystem_cd(key_buffer + 3);
        }
        else if (starts_with(key_buffer, "cat "))
        {
            char *name = key_buffer + 4;
            fs_read(name);
        }
        else if (strcmp(key_buffer, "keyboard tr") == 0)
        {
            set_keyboard_layout("tr");
            print_string("keyboard: tr\n");
        }
        else if (strcmp(key_buffer, "keyboard us") == 0)
        {
            set_keyboard_layout("us");
            print_string("keyboard: us\n");
        }
        else if (starts_with(key_buffer, "color "))
        {
            set_terminal_color(key_buffer + 6);
        }
        else if (ends_with(key_buffer, ".hlmr"))
        {
            // Only run this if it's NOT a write command
            run_hlmr_file(key_buffer);
        }
        else if (strcmp(key_buffer, "clear") == 0)
        {
            clear_screen();
        }
        else if (strcmp(key_buffer, "super") == 0)
        {
            if (is_root)
            {
                is_root = 0;
                print_string("now user.\n");
            }
            else
            {
                is_root = 1;
                print_string("now root.\n");
            }
        }
        else if (buffer_index > 0)
        {
            print_string("unknown command.\n");
        }
    }

    // Clear buffer
    for (int i = 0; i < 256; i++)
        key_buffer[i] = 0;
    buffer_index = 0;

    if (cursor_row < 0)
        cursor_row = 0;

    if (holyhamer_mode)
        print_string("hh> ");
    else if (is_root)
        print_string("root# ");
    else
        print_string("om3$ ");
}

void kmain()
{
    // Initialize HolyHamer Memory
    hh_init();
    fs_init();
    clear_screen();
    print_string("welcome to om3 os v2.1 (holyhamer enabled)\n");
    print_string("om3$ ");

    update_cursor();

    while (1)
    {
        if (port_byte_in(0x64) & 0x1)
        {
            unsigned char scan_code = port_byte_in(0x60);
            char c = get_key_from_scancode(scan_code);

            if (c == 0)
                continue;

            if (c == '\b')
            {
                if (buffer_index > 0)
                {
                    print_backspace();
                    buffer_index--;
                    key_buffer[buffer_index] = 0;
                    update_cursor();
                }
            }
            else if (c == '\n')
            {
                execute_command();
                update_cursor();
            }
            else
            {
                print_char(c);
                update_cursor();
                if (buffer_index < 255)
                {
                    key_buffer[buffer_index] = c;
                    buffer_index++;
                }
            }
        }
        // Delay
        for (int i = 0; i < 500000; i++)
        {
            __asm__ volatile("nop");
        }
    }
}
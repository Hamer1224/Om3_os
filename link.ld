OUTPUT_FORMAT("pei-i386")
ENTRY(kmain)

SECTIONS
{
    /* Load the kernel at address 0x1000 */
    . = 0x1000;

    /* Code goes first */
    .text : ALIGN(4K)
    {
        *(.text)
    }

    /* Read-only data (strings) goes next */
    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Read-write data */
    .data : ALIGN(4K)
    {
        *(.data)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
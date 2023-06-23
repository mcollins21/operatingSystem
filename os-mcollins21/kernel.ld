/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
    /* Begin putting sections at 1 MiB, a conventional place for kernels to be
       loaded at by the bootloader. */
    . = 1M;
    /* link the multiboot struct here */
    . = ALIGN(8);
    .multiboot : { *(.multiboot) }
/*    KEEP(*(.multiboot)); */
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    _start_bss = . ;
    .bss  : { *(.bss)  }
    _end_bss = ADDR(.bss) + SIZEOF(.bss) ;
    
    . = ALIGN(16);

    .stack : { *(.stack) }
    _end_kernel = .;
}

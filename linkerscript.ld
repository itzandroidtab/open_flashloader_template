SEARCH_DIR(.);

/*
Format configurations
*/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm");
OUTPUT_ARCH(arm);


/* 
The stack size used by the application. NOTE: you need to adjust according to your application. 
*/
STACK_SIZE = 0x100;

/*
Memories definitions
*/
MEMORY
{
    ram (rwx) : org = 0x20000000, len = 32k
}

/*
Entry point
*/
ENTRY( __reset_handler );

/*
Sections
*/
SECTIONS
{
    /* Flash os information */
    PrgCode :
    {
        . = ALIGN(4);
        KEEP(*(PrgCode PrgCode.*));
        . = ALIGN(4);
    } > ram

    /* Vector table. Has the initial stack pointer and the initial 
       structure for the arm interrupts */
    .vectors :
    {
        . = ALIGN(128);
        /* vector table */
        KEEP(*(.vectors .vectors.*));
        . = ALIGN(4);
    } > ram

    /* Text segment, stores all user code */
    .text :
    {
        . = ALIGN(4);
        /* text segment */
        *(.text .text.* .gnu.linkonce.t.*);
        /* glue arm to thumb code, glue thumb to arm code*/
        *(.glue_7t .glue_7);
        /* c++ exceptions */
        *(.eh_frame .eh_frame_hdr)
        /* exception unwinding information */
        
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*);
        *(.ARM.exidx*)

        . = ALIGN(4);
        KEEP(*(.init));
        . = ALIGN(4);
        KEEP(*(.fini));

        . = ALIGN(4);
    } > ram

    /* Read only data */
    .rodata : 
    {
        . = ALIGN(4);

        /* Constands, strings, etc */
        *(.rodata .rodata.* .gnu.linkonce.r.*);

        . = ALIGN(4);
    } > ram

    /* Support C constructors, and C destructors in both user code
       and the C library. This also provides support for C++ code. */    
    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE(__preinit_array_start = .);

        KEEP(*(.preinit_array))

        PROVIDE(__preinit_array_end = .);
    } > ram

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE(__init_array_start = .);

        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))

        PROVIDE(__init_array_end = .);
    } > ram

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE(__fini_array_start = .);

        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        
        PROVIDE(__fini_array_end = .);
    } > ram

    PrgData :
    {
        . = ALIGN(4);
        KEEP(*(PrgData PrgData.*))
        . = ALIGN(4);
    } > ram

    /* Data that needs to be initialized to a value different than 0 */
    .data :
    {
        . = ALIGN(4);
        PROVIDE(__data_init_start = LOADADDR(.data));
        PROVIDE(__data_start = .);

        . = ALIGN(4);
        *(.data .data.* .gnu.linkonce.d.*)
        . = ALIGN(4);
        
        PROVIDE(__data_end = .);
        PROVIDE(__data_init_end = LOADADDR(.data));
    } > ram

    /* Data that needs to be initialized to 0 */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(__bss_start = .);

        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON);
        . = ALIGN(4);

        PROVIDE(__bss_end = .);
    } > ram

    /* Stack segment */
    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        PROVIDE(__stack_start = .);

        . = . + STACK_SIZE;        
        PROVIDE(__stack_end = .);
    } > ram

    /* Heap segment */
    .heap (NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(__heap_start = .);
        PROVIDE(__heap_end = (ORIGIN(ram) + LENGTH(ram)));
    } > ram

    /* Flash device information */
    DevDscr :
    {
        . = ALIGN(4);
        KEEP(*(DevDscr DevDscr.*));
        . = ALIGN(4);
    } > ram

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
        *(.note.GNU-stack)
    }

    .ARM.attributes 0 : { KEEP(*(.ARM.attributes)) }    
}

/* U-Boot loadb places the image at 0xc0008000 in DDR. */
MEMORY
{
    DDR (rwx) : ORIGIN = 0xC0008000, LENGTH = 1M
}

/* Keep the stack above the image and leave room for a small bare-metal app. */
_estack = ORIGIN(DDR) + LENGTH(DDR);

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.text.startup))
        *(.text*)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } > DDR

    /* Place ARM unwind tables before data to avoid overlapping load images. */
    .ARM.extab :
    {
        *(.ARM.extab*)
    } > DDR

    .ARM.exidx :
    {
        *(.ARM.exidx*)
    } > DDR

    /* The complete binary is loaded into DDR, so data has equal VMA/LMA. */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > DDR

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > DDR

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}

PROVIDE(_sidata = LOADADDR(.data));

/* Reserve at least 4 KiB for the stack within the declared DDR region. */
ASSERT(_ebss <= (_estack - 0x1000), "Image overlaps reserved stack")

/* End of map.lds. */

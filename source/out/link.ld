/*
+
+	Linker Option
+	Linker Script
+						February.04.2014  t_sato
*/

/*OUTPUT_FORMAT("elf32-i386","elf32-i386","elf32-i386")*/
OUTPUT_FORMAT("pe-i386")
/*OUTPUT_FORMAT("elf32-i386")*/
OUTPUT_ARCH(i386)
/*ENTRY(_skernel_main)*/
ENTRY(_init_kernel)
/*
start = _boot_main;
*/

SECTIONS
{
	/*. = 0x00010400;*/
	/*. = 0x00011800;*/
	/*. = 0x00013000;*/
	. = 0x00012000;
	.text : { *(.text) }
	
	. = ALIGN(16);
	.rodata : { *(.rodata*) }
	
	
	. = ALIGN(16);
	.data : { *(.data*) }
	
	. = ALIGN(16);
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
}

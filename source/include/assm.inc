%define MBR_PHYSICAL_ADD		0x7c00
%define BS_LOGICAL_LOAD_ADD		0x1000
%define BS_PHYSICAL_ADD			0x010000
;%define SKERNEL_PHYSICAL_ADD	0x010400
;%define SKERNEL_PHYSICAL_ADD	0x011800
;%define SKERNEL_PHYSICAL_ADD	0x00013000
%define SKERNEL_PHYSICAL_ADD	0x00012000
%define PHYSICAL_LOAD_ADD_MAX	0x100000
;%define BOOT_INFO_ADDR			0x101c0
%define BOOT_INFO_ADDR			0x10800

%define FIRST_PARTITION			0x1BE
%define SECOND_PARTITION		0x1CE
%define THIRD_PARTITION			0x1DE
%define FOURTH_PARTITON			0x1EE

%define BOOT_CODE_SELECTOR		0x08
%define BOOT_DATA_SELECTOR		0x10

%define PIC0_IMR				0x21
%define PIC1_IMR				0xa1
%define PIC0_OCW2				0x20
%define PIC1_OCW2				0xa0
%define PIC0_ICW1				0x20
%define PIC0_ICW2				0x21
%define PIC0_ICW3				0x21
%define PIC0_ICW4				0x21
%define PIC1_ICW1				0xa0
%define PIC1_ICW2				0xa1
%define PIC1_ICW3				0xa1
%define PIC1_ICW4				0xa1

%define E820_OUTPUT_OFFSET_ADDR	8
%define E820_OUTPUT_ADDR		(BOOT_INFO_ADDR+E820_OUTPUT_OFFSET_ADDR)
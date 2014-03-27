/*---------------------------------------------------------------------*/
/*!
 * @file	boot.h
 * @brief	boot
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _BOOT_H_
#define _BOOT_H_

#include "type.h"
#include "define.h"


#define E820ENTRY_MAX	128

#define E820_TYPE_MEMORY	0x01	/* available for os */
#define E820_TYPE_RESERVE	0x02	/* not available for os */
#define E820_TYPE_ACPI		0x03	/* available after reading ACPI table */
#define E820_TYPE_NVS		0x04	/* ACPI NVS memory */
#define E820_TYPE_UNUSABLE	0x05	/**/
#define E820_TYPE_DISABLE	0x06	/**/

struct e820_entry {
	// 
	u32 size;
	
	// total 64bit starting address.
	u32 addr_low;
	u32 addr_high;
	
	// total 64bit length
	u32 length_low;
	u32 length_high;
	
	u32		type;
	u32		extention;
};

struct e820_map {
	u32					num;
	struct e820_entry	entry[E820ENTRY_MAX];
};


//#define BOOT_INFO_ADDR 0x0101c0
#define BOOT_INFO_ADDR 0x10800

// boot information
struct boot_info {
	u16 screen_width;			// 0 - 1
	u16 screen_height;			// 2 - 3
	u32 vram;					// 4 - 7
	struct e820_map e820map;	// 8 - 272
	
	//TODO::provisional
	u32 padding[12];
};

extern struct boot_info *g_boot_info;


#endif // _BOOT_H_

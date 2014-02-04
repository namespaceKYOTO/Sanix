/*---------------------------------------------------------------------*/
/*!
 * @file	define.h
 * @brief	define
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _DEFINE_H_
#define _DEFINE_H_

#include "type.h"

// ÇªÇÃëºÉ|Å[Égî‘çÜ
// other ports
#define KEY_PORT  0x60

#define BYTE_4K		(0x00001000)

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
struct descriptor{
	union {
		// gdt
		struct {
			u16		limit_low;	// limit  0-15bit
			u16		base_low;	// base address 0-15bit
			u8		base_hlow;	// base address 16-23bit
			u8		type;		// gdt type
			u8		limit_high;	// limit 16-19bit,  flag
			u8		base_hhigh;	// base address 24-31bit
		};
		// idt
		struct {
			u16		offset_l;
			u16		ds;			// selector
			u8		zero;		// unused
			u8		param;
			u16		offset_h;
		};
		// simple access
		u8 _byte[8];	// 1byte access
		u64 _8byte;		// 8byte access
	};
};

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
typedef struct _descriptor_table {
	u16			descr_size;		// descriptor size
	u32			phy_addr;		// physical address
} gdt_t, idt_t;

/*---------------------------------------------------------------------*/
/*!
 * @brief	tss (Task state segment)
 */
/*---------------------------------------------------------------------*/
struct tss {
	u32		task__back_link;
	u32		esp0;
	u32		ss0;
	u32		esp1;
	u32		ss1;
	u32		esp2;
	u32		ss2;
	u32		cr3;
	u32		eip;
	u32		eflags;
	u32		eax;
	u32		ecx;
	u32		edx;
	u32		ebx;
	u32		esp;
	u32		ebp;
	u32		esi;
	u32		edi;
	u32		es;
	u32		cs;
	u32		ss;
	u32		ds;
	u32		fs;
	u32		gs;
	u32		ldt_segment_sel;
	u32		io_base_addr;
};

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
struct regs {
	union {
		//32 bit 
		struct {
			u32 edi;		// +  0
			u32 esi;		// +  4
			u32 ebp;		// +  8
			u32 esp;		// + 12
			u32 ebx;		// + 16
			u32 edx;		// + 20
			u32 ecx;		// + 24
			u32 eax;		// + 28
			u32 eflags;		// + 32
		};
		//16 bit
		struct {
			u16 di, hdi;		// +  0
			u16 si, hsi;		// +  4
			u16 bp, hbp;		// +  8
			u16 sp, hsp;		// + 12
			u16 bx, hbx;		// + 16
			u16 dx, hdx;		// + 20
			u16 cx, hcx;		// + 24
			u16 ax, hax;		// + 28
			u16 flags, hflags;	// + 32
		};
		//8 bit
		struct {
			u8 dil, dih, hdil, hdih;		// +  0
			u8 sil, sih, hsil, hsih;		// +  4
			u8 bpl, bph, hbpl, hbph;		// +  8
			u8 spl, sph, hspl, hsph;		// + 12
			u8 bh, bl, hbl, hbh;			// + 16
			u8 dh, dl, hdl, hdh;			// + 20
			u8 ch, cl, hcl, hch;			// + 24
			u8 ah, al, hal, hah;			// + 28
			u8 flag0, flag1, flag2, flag3;	// + 32
		};
	};
};

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
void init_reg( struct regs *reg );


#endif	// _DEFINE_H_

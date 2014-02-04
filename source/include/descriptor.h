#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include "type.h"
#include "define.h"

// descriptor
#define GDT_MAX		512 /*max 8192*/
#define IDT_MAX		128 /*max 256*/

/** +++++++++++++++++++++++++++++++++++++++
* @brief GDT content
* ----------------------------------------
* 00 : NULL
* 01 : 4GB code segment for kernel
* 02 : 4GB data segment for kernel
* 03 : 4GB code segment for user
* 04 : 4GB data segment for user
* 
* ----------------------------------------
* 
* 05 : unused
* 06 : unused
* 07 : unused
* 08 : unused
* 09 : unused
* 10 : unused
* 11 : unused
* 12 : unused
* 13 : unused
* 14 : unused
*
*--- TLS (Thread Local Storage) ----------
*
* 15 : TLS
*
*--- User TSS (Task State Segment) ------------
*
* User Tss Begin
* 16 : User TSS
*    :
*    :
* 256: User TSS
*
*--- Kernel TSS (Task State Segment) ------------
* 
* 257: Kernel TSS
* 
* @author t_sato
* ++++++++++++++++++++++++++++++++++++++++ */

#define GDT_PM_CS_SELECTOR	1
#define _GDT__PM_CS_SELECTOR	(PM_CODE_SELECTOR*8)
#define GDT_PM_DS_SELECTOR	2
#define _GDT__PM_DS_SELECTOR	(PM_DATA_SELECTOR*8)

#define KERNEL_CS	0x08
#define KERNEL_DS	0x10
#define USER_CS		0x18
#define USER_DS		0x20

#define GDT_TLS_SELECTOR	15
#define _GDT_TSL_SELECTOR	(GDT_TLS_SELECTOR*8)
#define GDT_USER_TSS_SEL_BIGIN	16
//#define GDT_TSS_SELECTOR	GDT_USER_TSS_SEL_BIGIN
#define _GDT_TSS_SELECTOR	(GDT_TSS_SELECTOR*8)
#define GDT_KERNEL_TSS_SEL_BIGIN	257
#define _GDT_TSS_SELECTOR	(GDT_KERNEL_TSS_SEL_BIGIN*8)


void init_gdt( void );
void init_idt( void );
void set_gdt_descr( struct descriptor *gd, u32 limit, s32 base, u8 type, u8 flag );
void set_idt_descr( struct descriptor *id, void* handler, u8 type, u16 ds );

inline struct descriptor* get_gdt( u32 idx );
inline struct descriptor* get_idt( u32 idx );

#endif // _DESCRIPTOR_H_

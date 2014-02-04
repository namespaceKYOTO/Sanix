/*---------------------------------------------------------------------*/
/*!
 * @file	interrupt.h
 * @brief	PIC, PIT, IRQ, ISR, etc...
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "type.h"
#include "util.h"

//
#define IRQ_TIME	(0x01)
#define IRQ_KEYBORD	(0x02)
#define IRQ_ALL		(0xff)
#define IRQ_ENABLE_MASK(f)	(~((f)&IRQ_ALL))
#define IRQ_DISABLE_MASK(f)	((f)&IRQ_ALL)


// I/O port for PIC
#define PIC0_IMR  0x21
#define PIC1_IMR  0xa1

#define PIC0_OCW2 0x20
#define PIC1_OCW2 0xa0

#define PIC0_ICW1 0x20
#define PIC0_ICW2 0x21
#define PIC0_ICW3 0x21
#define PIC0_ICW4 0x21

#define PIC1_ICW1 0xa0
#define PIC1_ICW2 0xa1
#define PIC1_ICW3 0xa1
#define PIC1_ICW4 0xa1


void init_pic( void );
void init_pit( void );

void enable_interrupt( u8 pic0_mask, u8 pic1_mask );
void disable_interrupt( u8 pic0_mask, u8 pic1_mask );

void isr_test( void );

void isr_divide_error( u32 cs );
void isr_nmi( void );
void isr_break_point( void );
void isr_overflow( void );
void isr_bound_range_exceeded_exception( void );
void ist_invalid_opcode( void );
void isr_disable_device( void );
void isr_double_fault( void );
void isr_coprocessor_segment_overrun( void );
void isr_invalid_tss( u32 error_code );
void isr_segment_not_present( void );
void isr_stack_fault( void );
void isr_general_protection( u32 error_code, u32 eip, u32 cs, u32 eflag );
void isr_page_fault( void );
void isr_floating_point_mf( void );
void isr_floating_point_xf( void );


// user
void isr_timer( void );
void isr_kerboard( void );

void isr_ata_master( void );
void isr_ata_slave( void );

// system call 
void isr_sys_call( u32 idx );


extern squeue_t key_queue;

#endif // _INTERRUPT_H_

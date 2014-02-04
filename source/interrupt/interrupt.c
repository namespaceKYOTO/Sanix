/*---------------------------------------------------------------------*/
/*!
 * @file	interrupt.c
 * @brief	PIC, PIT, IRQ, ISR, etc...
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#include "define.h"
#include "fasm.h"
#include "graphic.h"
#include "time.h"
#include "memory.h"
#include "task.h"
#include "process.h"
#include "console.h"
#include "interrupt.h"


u8 key_buff[32];
squeue_t key_queue;

//!< initialize Programmable Interrupt Controller
void init_pic( void )
{
	// PIC0(master) initialize
	io_out8( PIC0_ICW1, 0x11 );	// エッジトリガーリング ICW4も使用
	io_out8( PIC0_ICW2, 0x20 );	// IRQ割り込み信号0～7は INT20-27で受け取る
	io_out8( PIC0_ICW3, 0x02 );	// PIC0のIRQ2がPIC1に接続されている
	io_out8( PIC0_ICW4, 0x01 );	// 8086 mode
	// PIC1(slave) initialize
	io_out8( PIC1_ICW1, 0x11 );	// エッジトリガーリング ICW4も使用
	io_out8( PIC1_ICW2, 0x28 );	// IRQ割り込み信号8～15は INT28-2fで受け取る
	io_out8( PIC1_ICW3, 0x02 );	// PIC1はPIC0のIRQ2に接続されている
	io_out8( PIC1_ICW4, 0x01 );	// 8086 mode
	
	smemset( key_buff, 0, 32 );
	init_squeue( key_queue, key_buff, 0, 0, 32 );
}
/*---------------------------------------------------------------------*/
/*!
*@brief	initialize Programmable Interval Timer
*@note	clock 1.19318MHz
*@note	determination the interrupt cycle, clock / setting counter
*/
/*---------------------------------------------------------------------*/
void init_pit( void )
{
	io_out8( 0x43, 0x34 );	// 
#if 0
	// call the IRQ0 at interval of 1/100 sec
	io_out8( 0x40, 0x9c );	// low 8bit
	io_out8( 0x40, 0x2e );	// high 8biy
#else
	// call the IRQ0 at interval of 1/1000 sec
	io_out8( 0x40, 0xA9 );	// low 8bit
	io_out8( 0x40, 0x04 );	// high 8biy
#endif
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	enable interrupt
 */
/*---------------------------------------------------------------------*/
void enable_interrupt( u8 pic0_mask, u8 pic1_mask)
{
	io_out8( PIC0_IMR, pic0_mask );
	io_out8( PIC1_IMR, pic1_mask );
	sti();
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	disable interrupt
 */
/*---------------------------------------------------------------------*/
void disable_interrupt( u8 pic0_mask, u8 pic1_mask)
{
	io_out8( PIC0_IMR, pic0_mask );
	io_out8( PIC1_IMR, pic1_mask );
	cli();
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	interrupt service routine test
 */
/*---------------------------------------------------------------------*/
void isr_test( void )
{
	puts_ex("test Exception", 0, 0, COLOR_RED, COLOR_BLUE );
	for(;;){};
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	divide error
 */
/*---------------------------------------------------------------------*/
void isr_divide_error( u32 cs )
{
//	puts_ex("Diveide Exception", 0, 0, COLOR_RED, COLOR_BLUE );
//	putn(cs, 16*12, 0 );
	// スタック値から例外が起きたprocessがわかるはず!!
	proc_struct_t *current_proc = get_current_process();
//	delete_process_by_proc_struct( current_proc );
	puts_console("divide error :: ");
	puts_console( current_proc->debug_name );

	schedule_exception();
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	Non Maskable Intrrupt
 */
/*---------------------------------------------------------------------*/
void isr_nmi( void )
{
	
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	break point
 */
/*---------------------------------------------------------------------*/
void isr_break_point( void )
{
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	overflow
 */
/*---------------------------------------------------------------------*/
void isr_overflow( void )
{
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	BOUND range exceeded exception
 */
/*---------------------------------------------------------------------*/
void isr_bound_range_exceeded_exception( void )
{
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	invalid opcode exception
 */
/*---------------------------------------------------------------------*/
void ist_invalid_opcode( void )
{
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	disable device
 */
/*---------------------------------------------------------------------*/
void isr_disable_device( void )
{
	puts_ex("Desable Device Exception", 0, 0, COLOR_RED, COLOR_BLUE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	double fault
 */
/*---------------------------------------------------------------------*/
void isr_double_fault( void )
{
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	coprocessor segment overrun
 */
/*---------------------------------------------------------------------*/
void isr_coprocessor_segment_overrun( void )
{
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	invalid tss
 */
/*---------------------------------------------------------------------*/
void isr_invalid_tss( u32 error_code )
{
	puts_ex("Invalid TSS Exception", 0, 0, COLOR_RED, COLOR_BLUE );
	puts_ex("Error Code", 0, 16*1, COLOR_RED, COLOR_BLUE );
	putn_col( error_code, 8*20, 16*1, 10 );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	segment not present
 */
/*---------------------------------------------------------------------*/
void isr_segment_not_present( void )
{
	puts_ex("Segment not present Exception", 0, 0, COLOR_RED, COLOR_BLUE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	stack segment fault
 */
/*---------------------------------------------------------------------*/
void isr_stack_fault( void )
{
	puts_ex("Stack Fault Exception", 0, 0, COLOR_RED, COLOR_BLUE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	general protection exception
 */
/*---------------------------------------------------------------------*/
void isr_general_protection( u32 error_code, u32 eip, u32 cs, u32 eflag )
{
	u32 idx = 0;
	puts_ex("General Protection Exception", 0, 0, COLOR_RED, COLOR_BLUE );

	++idx;
	puts_ex("Error Code", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );

	++idx;
	puts_ex("                  ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puts_ex("EXT", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	putn_col(((error_code>>0) & 0x00000001), DEBUG_FONT_SIZE_X*18, DEBUG_FONT_SIZE_Y*idx, 1 );

	++idx;
	puts_ex("                  ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puts_ex("IDT", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	putn_col(((error_code>>1) & 0x00000001), DEBUG_FONT_SIZE_X*18, DEBUG_FONT_SIZE_Y*idx, 1 );

	++idx;
	puts_ex("                  ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puts_ex("TI ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	putn_col(((error_code>>2) & 0x00000001), DEBUG_FONT_SIZE_X*18, DEBUG_FONT_SIZE_Y*idx, 1 );

	++idx;
	puts_ex("                  ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puts_ex("SEGUMENT", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	putn_col(((error_code>>3) & 0x00001fff), DEBUG_FONT_SIZE_X*18, DEBUG_FONT_SIZE_Y*idx, 4 );
	
	++idx;
	puts_ex("                                ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puts_ex("EIP", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puth_col( eip, DEBUG_FONT_SIZE_X*18, DEBUG_FONT_SIZE_Y*idx, 8 );
	
	++idx;
	puts_ex("                                ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puts_ex("CS", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puth_col( cs, DEBUG_FONT_SIZE_X*18, DEBUG_FONT_SIZE_Y*idx, 8 );
	
	++idx;
	puts_ex("                                ", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puts_ex("EFLAG", 0, DEBUG_FONT_SIZE_Y*idx, COLOR_RED, COLOR_BLUE );
	puth_col( eflag, DEBUG_FONT_SIZE_X*18, DEBUG_FONT_SIZE_Y*idx, 8 );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	page falut
 */
/*---------------------------------------------------------------------*/
void isr_page_fault( void )
{
	puts_ex("Page Fault Exception", 0, 0, COLOR_RED, COLOR_BLUE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	floating point exception #MF
 */
/*---------------------------------------------------------------------*/
void isr_floating_point_mf( void )
{
	puts_ex("Floating Point Exception MF", 0, 0, COLOR_RED, COLOR_BLUE );
	for(;;){};
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	floating point exception #XF
 */
/*---------------------------------------------------------------------*/
void isr_floating_point_xf( void )
{
	puts_ex("Floating Point Exception XF", 0, 0, COLOR_RED, COLOR_BLUE );
	for(;;){};
}


/*---------------------------------------------------------------------*/
/*!
 * @brief	timer
 */
/*---------------------------------------------------------------------*/
void isr_timer( void )
{
	time_count();
	
	schedule();
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	keyboard
 */
/*---------------------------------------------------------------------*/
void isr_kerboard( void )
{
	u8 code = io_in8( 0x60 );
#if 0
	puts_ex("Keyboard Interrput", 0, 0, COLOR_RED, COLOR_BLUE );
	puth_ex( code, 8*22, 0, COLOR_RED, COLOR_BLUE, 2 );
#endif
	enqueue( &key_queue, code );
}

//
void isr_ata_master( void )
{
	puts_ex("ATA Master Interrupt", 0, 0, COLOR_RED, COLOR_BLUE );
}

//
void isr_ata_slave( void )
{
	puts_ex("ATA Slave Interrupt", 0, 0, COLOR_RED, COLOR_BLUE );
}

// system call
void isr_sys_call( u32 idx )
{
	puts_ex("Syscall", 0, 0, COLOR_RED, COLOR_BLUE );
	putn_ex( idx, 8*10, 0, COLOR_RED, COLOR_BLUE, 3 );
}

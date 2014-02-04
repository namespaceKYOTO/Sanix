/*---------------------------------------------------------------------*/
/*!
* @file		skenel.c
* @brief	Sanix kernel
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "define.h"
#include "boot.h"
#include "fasm.h"
#include "interrupt.h"
#include "descriptor.h"
#include "graphic.h"
#include "graphic2d.h"
#include "debug.h"
#include "time.h"
#include "memory.h"
#include "page.h"
#include "math.h"
#include "ata.h"
#include "error.h"
#include "util.h"
#include "console.h"
#include "list.h"
#include "process.h"
#include "task.h"
#include "lock.h"
#include "pci.h"


void skernel_main( void );
void ata_test( void );


void init_kernel( void )
{
	disable_interrupt( IRQ_DISABLE_MASK(IRQ_ALL), IRQ_DISABLE_MASK(IRQ_ALL) );
	{
		init_gdt();
		init_idt();
		init_pic();
		init_pit();
		init_graphic();
		init_page();
//		init_smalloc();
		set_enable_sched( false );
	}
	enable_interrupt( IRQ_ENABLE_MASK(IRQ_TIME|IRQ_KEYBORD), IRQ_DISABLE_MASK(IRQ_ALL) );

	init_console();
	
	init_memory();
	
	disable_interrupt( IRQ_DISABLE_MASK(IRQ_TIME|IRQ_KEYBORD), IRQ_DISABLE_MASK(IRQ_ALL) );
	init_ata();
	enable_interrupt( IRQ_ENABLE_MASK(IRQ_TIME|IRQ_KEYBORD), IRQ_DISABLE_MASK(IRQ_ALL) );
	
	init_time();

	// initialize process and start multi task
	init_process();

	create_process( console, CLEATE_PROC_KERNEL );
//	create_process( ata_test, CLEATE_PROC_KERNEL );
	start_multi_proc();


	// provisional
	skernel_main();
}

void skernel_main( void )
{
	struct timer _time;
	u32 count = 0;
	u32 rot = 0;
	get_timer( &_time );
	hlt();
	puts("Performance", 0, 16*4 );

	for(;;){
		
		struct timer tmp;
		get_timer( &tmp );
	};
}

void ata_test( void )
{
	u8 buff[512] = {0};
	struct ata_address ata_addr;
	u32 ret = 0;
	ata_addr.cylinder = 0;
	ata_addr.header = 0;
	ata_addr.sector = 0;
#if 1
	ATA_ADRESS_TO_CHS( ata_addr, 1024 );
	ret = write_ata_data( buff, 1, &ata_addr );
	if( ret != ENONE ) {
		puts_console("failed write ata data\n");
	} else {
		puts_console("success write ata data\n");
	}

	ATA_ADRESS_TO_CHS( ata_addr, 1024 );
	ret = read_ata_data( buff, 1, &ata_addr );
	
	if( ret != ENONE ) {
		puts_console("failed read ata data\n");
	} else {
		puts_console("success read ata data\n");
	}
#endif
	s32 counter = 0;
	s32 idx = 0;
	for(;;) {
		if( ++counter >= 100000 ) {
			puth_console( buff[idx++] );
			puts_console('\n');
			counter = 0;
			if( idx >= 512 ) {
				puts_console("end\n");
				idx = 0;
				break;
			}
		}
	}
}


/*---------------------------------------------------------------------*/
/*!
* @file		descriptor.c
* @brief	GDT, IDT, TS, etc....
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "fasm.h"
#include "debug.h"
#include "inthandler.h"
#include "interrupt.h"
#include "descriptor.h"

static struct descriptor g_gdt[GDT_MAX] __attribute__((alignment(16)));
static struct descriptor g_idt[IDT_MAX] __attribute__((alignment(16)));

void init_gdt( void )
{
	// NULL
	set_gdt_descr( &g_gdt[0], 0x00000000, 0x00000000, 0x00, 0x00 );
	
	// 4GB code selector for user
	set_gdt_descr( &g_gdt[1], 0xffffffff, 0x00000000, 0x9a, 0x0c );
	// 4GB data selector for user
	set_gdt_descr( &g_gdt[2], 0xffffffff, 0x00000000, 0x92, 0x0c );
	
	// 4GB code selector for user
	set_gdt_descr( &g_gdt[3], 0xffffffff, 0x00000000, 0xfa, 0x0c );
	// 4GB data selector for user
	set_gdt_descr( &g_gdt[4], 0xffffffff, 0x00000000, 0xf2, 0x0c );

#if 0
	// tls  タスク用共有メモリ、セグメント!?
	set_gdt_descr( &g_gdt[GDT_TLS_SELECTOR], 0x00000000, 0x00000000, 0x00, 0x00 );
	// tss
//	set_gdt_descr( &g_gdt[GDT_TSS_SELECTOR], 0x00000067, (s32)(&g_default_proc.reg), 0x89, 0x08 );
#endif


	lgdt( sizeof(g_gdt), (u32)g_gdt );
}

void init_idt( void )
{
// Exception
	set_idt_descr( &g_idt[ 0], (void*)inthandler00, 0x8e, 0x08 ); // 除算エラー
//	set_idt_descr( &g_idt[ 1], (void*)inthandler01, 0x8e, 0x08 ); // ### reserved ###
//	set_idt_descr( &g_idt[ 2], (void*)inthandler02, 0x8e, 0x08 ); // NMI割り込み
//	set_idt_descr( &g_idt[ 3], (void*)inthandler03, 0x8e, 0x08 ); // ブレークポイント
//	set_idt_descr( &g_idt[ 4], (void*)inthandler04, 0x8e, 0x08 ); // オーバーフロー
//	set_idt_descr( &g_idt[ 5], (void*)inthandler05, 0x8e, 0x08 ); // BOUNDの範囲外
//	set_idt_descr( &g_idt[ 6], (void*)inthandler06, 0x8e, 0x08 ); // 無効オペコード(未定義オペコード)
	set_idt_descr( &g_idt[ 7], (void*)inthandler07, 0x8e, 0x08 ); // デバイス使用不可(マス・コプロセッサがない)
//	set_idt_descr( &g_idt[ 8], (void*)inthandler08, 0x8e, 0x08 ); // ダブルフォルト
//	set_idt_descr( &g_idt[ 9], (void*)inthandler09, 0x8e, 0x08 ); // ### コプロセッサ・セグメント・オーバーラン, reserved #####
	set_idt_descr( &g_idt[10], (void*)inthandler0a, 0x8e, 0x08 ); // 無効TSS
	set_idt_descr( &g_idt[11], (void*)inthandler0b, 0x8e, 0x08 ); // セグメント不在
	set_idt_descr( &g_idt[12], (void*)inthandler0c, 0x8e, 0x08 ); // スタック・セグメント・フォルト
	set_idt_descr( &g_idt[13], (void*)inthandler0d, 0x8e, 0x08 ); // 一般保護
	set_idt_descr( &g_idt[14], (void*)inthandler0e, 0x8e, 0x08 ); // ページフォルト
//	set_idt_descr( &g_idt[15], (void*)inthandler0f, 0x8e, 0x08 ); // ### Intel reserved ###
	set_idt_descr( &g_idt[16], (void*)inthandler10, 0x8e, 0x08 ); // x87 FPU 浮動小数点エラー(マスフォルト)
//	set_idt_descr( &g_idt[17], (void*)inthandler11, 0x8e, 0x08 ); // アライメント・チェック
//	set_idt_descr( &g_idt[18], (void*)inthandler12, 0x8e, 0x08 ); // マシンチェック
	set_idt_descr( &g_idt[19], (void*)inthandler13, 0x8e, 0x08 ); // SIMD 浮動小数点例外
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  0x14 - 0x1f : ### Intel reserved ###
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
//  user define   0x20 - 0x2f (IRQ)
	set_idt_descr( &g_idt[0x20], (void*)inthandler20, 0x8e, 0x08 );	// timer
	set_idt_descr( &g_idt[0x21], (void*)inthandler21, 0x8e, 0x08 );	// keyboard
	
	set_idt_descr( &g_idt[0x2e], (void*)inthandler2e, 0x8e, 0x08  );// ATA master
	set_idt_descr( &g_idt[0x2f], (void*)inthandler2f, 0x8e, 0x08  );// ATA slave
	
// system call
	set_idt_descr( &g_idt[0x30], (void*)inthandler30, 0x8e, 0x08 );


	lidt( sizeof(g_idt), (u32)g_idt );
}

void set_gdt_descr( struct descriptor *gd, u32 limit, s32 base, u8 type, u8 flag )
{
	gd->limit_low	= limit & 0x0000ffff;
	gd->base_low	= base & 0x0000ffff;
	gd->base_hlow	= (base & 0x00ff0000) >> 16;
	gd->type		= type;
	gd->limit_high	= (limit & 0x000f0000) >> 16 | ((flag<<4) & 0xf0);
	gd->base_hhigh	= (base & 0xff000000) >> 24;
}

void set_idt_descr( struct descriptor *id, void *handler, u8 type, u16 ds )
{
	u32 handler_addr	= (u32)handler;
	id->offset_l		= (u16)(handler_addr & 0x0000ffff);
	id->ds				= ds;
	id->zero			= 0;
	id->param			= type;//0x8e;
	id->offset_h		= (u16)(handler_addr>>16);
}


struct descriptor* get_gdt( u32 idx )
{
	S_ASSERT( 0 <= idx && idx < GDT_MAX, "INVALID GET GDT INDEX" );
	return &g_gdt[idx];
}
struct descriptor* get_idt( u32 idx )
{
	S_ASSERT( 0 <= idx && idx < IDT_MAX, "INVALID IDT GDT INDEX" );
	return &g_idt[idx];
}


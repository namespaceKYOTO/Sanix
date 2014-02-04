#ifndef _FASM_H_
#define _FASM_H_

#include "type.h"
#include "define.h"

void hlt( void );
void cli( void );
void sti( void );
u32  get_cr0( void );
void set_cr0( u32 flag );
void cr3( u32 page_dire );
void lgdt( u32 limit, u32 addr );
void lidt( u32 limit, u32 addr );
void ltr( u32 index );
void io_in( struct regs *ireg, struct regs *oreg );
u8   io_in8( u32 port );
u16  io_in16( u32 port );
u32  io_in32( u32 port );
void io_out8( u32 port, u8 data );
void io_out16( u32 port, u16 data );
void io_out32( u32 port, u32 data );
void io_in10( struct regs *ireg, struct regs *oreg );
void far_jmp( u32 offset, u16 selector );
void sys_call( u32 idx );
u32 get_eflag( void );


#endif // _FASM_H_

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "type.h"
#include "list.h"
#include "util.h"

void init_console( void );
void console( void );
void putc_console( u8 c );
void puts_console( u8* str );
void putn_console( u32 n );
void puth_console( u32 n );

void putc_console_ex( u8 c, u8 color );
void puts_console_ex( u8* str, u8 color );
void putn_console_ex( u32 n, u8 color );
void puth_console_ex( u32 n, u8 color );

#endif // _CONSOLE_H_

/*---------------------------------------------------------------------*/
/*!
* @file		graphic.c
* @brief	graphic
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "define.h"
#include "debug.h"
#include "boot.h"
#include "fasm.h"
#include "util.h"
#include "graphic2d.h"
#include "graphic.h"

static struct gra_info s_info;

static const u8 pallet_table[COLOR_NUM * _RGB] = {
	0x00,0x00,0x00,		// black
	0xff,0xff,0xff,		// white
	0xff,0x00,0x00,		// red
	0x00,0xff,0x00,		// green
	0x00,0x00,0xff,		// blue
	0xff,0xff,0x00,		// yellow
	0xff,0x00,0xff,		// pink
	0x00,0xff,0xff,		// lightblue
	0xf0,0xf0,0xf0,		// gray1
	0x0f,0x0f,0x0f,		// gray2
};

#if 0
static const u8 DEBUG_FONTS[_DEBUG_FONT_TBL_SIZE][__DEBUG_FONT_ARRAY_SIZE] = {
#if defined(__USE_FONT_16x16__)
#include "font_16x16.tbl"
#else
//#include "font_8x16.tbl"
#include "font_8x16_0__64.tbl"
#include "font_8x16_65__127.tbl"
#endif
};
#else
static const u8 DEBUG_FONTS_65x127[_DEBUG_FONT_TBL_SIZE/2][__DEBUG_FONT_ARRAY_SIZE] = {
#include "font_8x16_65__127.tbl"
};
static const u8 DEBUG_FONTS_0x64[_DEBUG_FONT_TBL_SIZE/2][__DEBUG_FONT_ARRAY_SIZE] = {
#include "font_8x16_0__64.tbl"
};
#endif

static u8 hex_tbl[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F', };

void _set_pallet( u8* pallet )
{
	io_out8( 0x03c9, pallet[0] );
	io_out8( 0x03c9, pallet[1] );
	io_out8( 0x03c9, pallet[2] );
}
void _init_pallet( void )
{
	io_out8( 0x03c8, 0 );
	_set_pallet( (u8*)&pallet_table[COLOR_BLACK		* _RGB] );	// black
	_set_pallet( (u8*)&pallet_table[COLOR_WHITE		* _RGB] );	// white
	_set_pallet( (u8*)&pallet_table[COLOR_RED		* _RGB] );	// red
	_set_pallet( (u8*)&pallet_table[COLOR_GREEN		* _RGB] );	// green
	_set_pallet( (u8*)&pallet_table[COLOR_BLUE		* _RGB] );	// blue
	_set_pallet( (u8*)&pallet_table[COLOR_YELLOW	* _RGB] );	// yellow
	_set_pallet( (u8*)&pallet_table[COLOR_PINK		* _RGB] );	// pink
	_set_pallet( (u8*)&pallet_table[COLOR_LIGHTBLUE	* _RGB] );	// lkightblue
	_set_pallet( (u8*)&pallet_table[COLOR_GRAY1		* _RGB] );	// gray1
	_set_pallet( (u8*)&pallet_table[COLOR_GRAY2		* _RGB] );	// gray2
}

void init_graphic( void )
{
	_init_pallet();
	
#if 1
	s_info.screen_width		= g_boot_info->screen_width;
	s_info.screen_height	= g_boot_info->screen_height;
	s_info.vram_begin		= g_boot_info->vram;
	s_info.vram_end			= s_info.vram_begin + (s_info.screen_width * s_info.screen_height);
#else
	s_info.screen_width		= SCREEN_WIDTH;
	s_info.screen_height	= SCREEN_HEIGHT;
	s_info.vram_begin		= VGA_ADDR_BEGIN;
	s_info.vram_end			= VGA_ADDR_END;
#endif
	
	screen_clear( COLOR_BLUE );
	
	init_sg2d();
	
	puts("graphic initialize", 0, 0);
}

void set_video( void )
{
#if 0
	struct regs ireg, oreg;
//	init_reg( &ireg );
//	init_reg( &oreg );

	ireg.ah = 0x00;
	ireg.al = 0x13;		// vga 320*200*8bit
//	io_out8( &ireg, &oreg );
#endif

	return;
}

bool change_video_mode( u32 video_mode )
{
	struct regs ireg, oreg;
	u8 buff[512];
	
	// Determine whether VBE are present
	init_reg( &ireg );
	init_reg( &oreg );
	ireg.eax = 0x4f00;
	ireg.edi = (u32)buff;
	io_in10( &ireg, &oreg );
	for(;;){};
	if( oreg.eax != 0x004f) {
		S_ASSERT( 0, "Change video mode1");
		return false;
	}
	
	// Examine the attribute of the specified video mode
	init_reg( &ireg );
	init_reg( &oreg );
	ireg.eax = 0x4f01;
	ireg.ecx = video_mode;
	ireg.edi = (u32)buff;
	io_in10( &ireg, &oreg );
	if(	oreg.eax != 0x004f
	||	!(buff[ 0x00 ] == 0x00 && buff[ 0x01 ] == 0x80	// linear framebuffer mode supported?
	&&	buff[ 0x19 ] == 0x08							// bit per pixel
	&&	buff[ 0x1b ] == 0x04							// packed pixel
	)) {
		S_ASSERT( 0, "Change video mode2");
		return false;
	}
	
	// set video mode 
	init_reg( &ireg );
	init_reg( &oreg );
	ireg.eax = 0x4f02;
	ireg.ebx = 0x4000 + video_mode;
	ireg.edi = (u32)buff;
	io_in10( &ireg, &oreg );
	
	s_info.screen_width = *((u16*)&buff[0x12]);
	s_info.screen_height = *((u16*)&buff[0x14]);
	s_info.vram_begin = *((u32*)&buff[0x28]);
	s_info.vram_end = s_info.vram_begin + (s_info.screen_width * s_info.screen_height);
	S_ASSERT( 0, "Change video mode3");
}

void set_font( void )
{
#if 0
	struct regs ireg, oreg;
	
	// 80*50 vga font
	
#endif
}

void get_info( struct gra_info *des )
{
	des->screen_width = s_info.screen_width;
	des->screen_height = s_info.screen_height;
	des->vram_begin = s_info.vram_begin;
	des->vram_end = s_info.vram_end;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	put charactor
 */
/*---------------------------------------------------------------------*/
void putc_ex( s8 c, s16 posx, s16 posy, u8 color_char, u8 color_back )
{
	if( /*!is_alphanumeric(c) ||*/ posx > s_info.screen_width || posy > s_info.screen_height ) {
		return;
	}
	u32 idx = 0, write_line = 0;
#if defined(__USE_FONT_16x16__)
	u16 col = 0;
#else
	u8 col = 0;
#endif // __USE_FONT_16x16__
	u8 *addr = s_info.vram_begin + ((posy*s_info.screen_width) + posx);
	u8 i, j;
	
#if 0		// September.02.2013 t_sato
	if( is_lower(c) )		idx = (c - 'a') + _LOWER_OFFSET;
	else if( is_upper(c) )	idx = c - 'A';
	else if( is_number(c) )	idx = (c - '0') + _NUMBER_OFFSET;
	else					idx = DEBUG_FONT_NUM - 1;
#endif
	if( is_ascii(c) )	idx = c;
	else				idx = 0;
	
//	S_ASSERT( 0 <= idx && idx < DEBUG_FONT_NUM, "GRAPHIC INDEX ERROR" );
	
	for( i = 0; i < DEBUG_FONT_SIZE_Y; ++i ) {
#if defined(__USE_FONT_16x16__)
		col = ((DEBUG_FONTS[idx][i*2] << 8) | (DEBUG_FONTS[idx][i*2+1]));
#else
	#if 0
		col = DEBUG_FONTS[idx][i];
	#else
		if( idx < 65 )
			col = DEBUG_FONTS_0x64[idx][i];
		else
			col = DEBUG_FONTS_65x127[idx-65][i];
	#endif
#endif // __USE_FONT_16x16__
		write_line = (u32)addr/s_info.screen_width;
		for( j = 0; j < DEBUG_FONT_SIZE_X; ++j ) {
			if( s_info.vram_begin <= addr + j && addr + j <= s_info.vram_end ) {
				if( (col<<1)&__DEBUG_FONT_COL_MASK
				&&	write_line == (u32)(addr+j)/s_info.screen_width
				) {
					// write
					*(addr + j) = color_char;
				} else {
					// no write
					*(addr + j) = color_back;
				}
			}
			col <<= 1;
		}
		addr += s_info.screen_width;
	}
}
void putc( s8 c, s16 posx, s16 posy )
{
	putc_ex( c, posx, posy, COLOR_BLACK, COLOR_BLUE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	put string
 */
/*---------------------------------------------------------------------*/
void puts_col( s8 *str, s16 posx, s16 posy, u8 color_char, u8 color_back, s32 colum )
{
	s16 _posx = posx, _posy = posy;
	bool check_col = (colum >= 1);
	u32 col;

	while( *str ) {
		
		if( is_linefeed(str) ) {
			_posx = posx;
			_posy += DEBUG_FONT_SIZE_Y;
		} /*else
		if( *str == ' ' ) {
			_posx += DEBUG_FONT_SIZE_X;
		} */else {
			putc_ex( *str, _posx, _posy, color_char, color_back );
			_posx += DEBUG_FONT_SIZE_X;
		}
		++str;
		if( check_col && ++col > colum ) break;
	}
}

void puts_ex( s8 *str, s16 posx, s16 posy, u8 color_char, u8 color_back )
{
	puts_col( str, posx, posy, color_char, color_back, -1 );
}

void puts( s8 *str, s16 posx, s16 posy )
{
	puts_ex( str, posx, posy, COLOR_BLACK, COLOR_BLUE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	put number
 */
/*---------------------------------------------------------------------*/
void putn_ex( u32 num, s16 posx, s16 posy, u8 color_char, u8 color_back, u32 column )
{
	u8 digit = 0;
	bool write = false;
	while( column-- > 0 || num > 0 ) {
		digit = num % 10;
		if( num <= 0 && write ) {
			putc_ex( ' ', posx, posy, color_char, color_back );
		} else {
			putc_ex( '0' + digit, posx, posy, color_char, color_back );
		}
		posx -= DEBUG_FONT_SIZE_X;
		num /= 10;
		write = true;
	}
}
void putn( u32 num, s16 posx, s16 posy )
{
	putn_ex( num, posx, posy, COLOR_BLACK, COLOR_BLUE, 10 );
}
void putn_col( u32 num, s16 posx, s16 posy, u32 column )
{
	putn_ex( num, posx, posy, COLOR_BLACK, COLOR_BLUE, column );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	put hexadecimal
 */
/*---------------------------------------------------------------------*/
void puth_ex( u32 num, s16 posx, s16 posy, u8 color_char, u8 color_back, u32 column )
{
	u8 digit = 0;
	bool write = false;
	while( column-- > 0 || num > 0 ) {
		digit = num & 0x0000000F;
		S_ASSERT( 0 <= digit && digit < 16, "PUTH ERROR");
		if( num <= 0 && write ) {
			putc_ex( ' ', posx, posy, color_char, color_back );
		} else {
			putc_ex( hex_tbl[digit], posx, posy, color_char, color_back );
		}
		posx -= DEBUG_FONT_SIZE_X;
		num >>= 4;
		write = true;
	}
}
void puth( u32 num, s16 posx, s16 posy )
{
	puth_ex( num, posx, posy, COLOR_BLACK, COLOR_BLUE, 8 );
}
void puth_col( u32 num, s16 posx, s16 posy, u32 column )
{
	puth_ex( num, posx, posy, COLOR_BLACK, COLOR_BLUE, column );
}


void line_clear( u32 line, u8 color )
{
//	putc( ' ', 0, line, color );
}

void screen_clear( u8 color )
{
#if 1
	u8 *addr = s_info.vram_begin;
	for( ; addr <= s_info.vram_end; ++addr ) {
		*addr = color;
	}
#else
	u32 *addr = (u32*)s_info.vram_begin;
	u32 color_32 = (color << 24 | color << 16 | color << 8 | color << 0 );
	for( ; addr + sizeof(u32) <= s_info.vram_end; ++addr ) {
		*addr = color_32;
	}
#endif
}

void debug_flag( s32 flag )
{
#if 0
	u16 i = 0;
	u8 color;
	rect _rect = { 0, 0, 99/*64*/, 14 };
	_write_square( &_rect, COLOR_WHITE );

	_rect.width = 1;
	for( ; i < 32 ; ++i ) {
		if( flag & 0x80 ) {
			color = COLOR_RED;
		} else {
			color = COLOR_BLACK;
		}
		_write_square( &_rect, color );
		_rect.posx += 2;
		flag <<= 1;
	}
#endif
}


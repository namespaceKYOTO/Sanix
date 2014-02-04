/*---------------------------------------------------------------------*/
/*!
 * @file	graphic.h
 * @brief	graphic
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "define.h"
#include "graphicUtil.h"
#include "math.h"

#define COLOR_WHITE			0
#define COLOR_BLACK			1
#define COLOR_RED			2
#define COLOR_GREEN			3
#define COLOR_BLUE			4
#define COLOR_YELLOW		5
#define COLOR_PINK			6
#define COLOR_LIGHTBLUE		7
#define COLOR_GRAY1			8
#define COLOR_GRAY2			9
#define COLOR_NUM			10
#define _RGB				3

//-----------------------------------
// provisional
// movde 320x200x256
#define VGA_ADDR_BEGIN		0x0a0000
#define VGA_ADDR_END		0x0affff
#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		200
//-----------------------------------

#define __USE_FONT_16x16__
#undef __USE_FONT_16x16__
#if defined(__USE_FONT_16x16__)
#define DEBUG_FONT_SIZE_X		16
#define DEBUG_FONT_SIZE_Y		16
#define DEBUG_FONT_NUM			36
#define __DEBUG_FONT_ARRAY_SIZE	32
#define __DEBUG_FONT_COL_MASK	0x8000
#define _NUMBER_OFFSET			26
#define _LOWER_OFFSET			0
#define _DEBUG_FONT_TBL_SIZE	128
#define 
#else
#define DEBUG_FONT_SIZE_X		8
#define DEBUG_FONT_SIZE_Y		16
#define DEBUG_FONT_NUM			63
#define __DEBUG_FONT_ARRAY_SIZE	16
#define __DEBUG_FONT_COL_MASK	0x80
#define _NUMBER_OFFSET			26
#define _LOWER_OFFSET			36
#define _DEBUG_FONT_TBL_SIZE	128
#endif	//__USE_FONT_16x16__

enum {
	eVIDEO_MODE_640x400x256		= 0x100,
	eVIDEO_MODE_640x480x256		= 0x101,
	eVIDEO_MODE_800x600x16		= 0x102,
	eVIDEO_MODE_800x600x256		= 0x103,
	eVIDEO_MODE_1024x768x16		= 0x104,
	eVIDEO_MODE_1024x768x256	= 0x105,
	eVIDEO_MODE_1280x1024x16	= 0x106,
	eVIDEO_MODE_1280x1024x256	= 0x107,
	eVIDEO_MODE_80x60_text		= 0x108,
	eVIDEO_MODE_132x25_text		= 0x109,
	eVIDEO_MODE_132x43_text		= 0x10A,
	eVIDEO_MODE_132x50_text		= 0x10B,
	eVIDEO_MODE_132x60_text		= 0x10C,
};

struct gra_info {
	u32 screen_width;
	u32 screen_height;
	u32 vram_begin;
	u32 vram_end;
};

#define is_linefeed( str )		(str[0] == 0x0A)

void init_graphic( void );

void set_video( void );
bool change_video_mode( u32 video_mode );
void set_font( void );

void get_info( struct gra_info *des );

/*---------------------------------------------------------------------*/
/*!
 * @brief	put charactor
 */
/*---------------------------------------------------------------------*/
void putc_ex( s8 c, s16 posx, s16 posy, u8 color_char, u8 color_back );
void putc( s8 c, s16 posx, s16 posy );

/*---------------------------------------------------------------------*/
/*!
 * @brief	put string
 */
/*---------------------------------------------------------------------*/
void puts_col( s8 *str, s16 posx, s16 posy, u8 color_char, u8 color_back, s32 colum );
void puts_ex( s8 *str, s16 posx, s16 posy, u8 color_char, u8 color_back );
void puts( s8 *str, s16 posx, s16 posy );

/*---------------------------------------------------------------------*/
/*!
 * @brief	put decimal
 */
/*---------------------------------------------------------------------*/
void putn_ex( u32 num, s16 posx, s16 posy, u8 color_char, u8 color_back, u32 column );
void putn( u32 num, s16 posx, s16 posy );
void putn_col( u32 num, s16 posx, s16 posy, u32 column );

/*---------------------------------------------------------------------*/
/*!
 * @brief	put hexadecimal
 */
/*---------------------------------------------------------------------*/
void puth_ex( u32 num, s16 posx, s16 posy, u8 color_char, u8 color_back, u32 column );
void puth( u32 num, s16 posx, s16 posy );
void puth_col( u32 num, s16 posx, s16 posy, u32 column );

void screen_clear( u8 color );

// debug
void debug_flag( s32 flag );

#endif // _GRAPHIC_H_

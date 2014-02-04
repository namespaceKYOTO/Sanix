/*---------------------------------------------------------------------*/
/*!
* @file		graphic2d.c
* @brief	Sanix Graphic 2D
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/

#include "graphic.h"
#include "math.h"
#include "debug.h"
#include "graphic2d.h"

static struct gra_info _info;

#define get_addr( posy, posx ) \
	_info.vram_begin + (((u32)posy * _info.screen_width) + (u32)posx)

#define is_valid_address( addr ) \
	(_info.vram_begin <= addr && addr <= _info.vram_end)

void init_sg2d( void )
{
//	struct gra_info _info;
	get_info( &_info );
}


/*---------------------------------------------------------------------*/
/*!
 * @brief	
 * @note	widget ŠÖ˜A‚È‹C‚à‚·‚é
 */
/*---------------------------------------------------------------------*/
void sg2d_fill_pixel( const struct rect *_rect, u8 color )
{
	u32 screen_ofsx = 0;//_info.screen_width >> 1;
	u32 screen_ofsy = 0;//_info.screen_height >> 1;
	u8 *addr = NULL;
	u32 y,x;
//	vector2_t pos, base;
	
	for( y = 0; y < _rect->height; ++y ) {
//		base.y = _rect->posy + y;
		for( x = 0; x < _rect->width; ++x ) {
//			base.x = _rect->posx + x;
//			rotate( &pos, &base, rot );
			addr = get_addr( (_rect->posy+y), (_rect->posx+x) );
			if( is_valid_address(addr) ) {
				*addr = color;
			}
		}
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
void sg2d_move_pixel( u32 dstx, u32 dsty, const struct rect *src, u8 src_color )
{
//	S_ASSERT( 0, "error" );
	u32 x, y;
	u8 *dst_addr, *src_addr;
	for( y = 0; y < src->height; ++y ) {
		for( x = 0; x < src->width; ++x ) {
			dst_addr = get_addr((dsty+y), (dstx+x));
			src_addr = get_addr((src->posy+y), (src->posx+x));
			if( is_valid_address(src_addr) ) {
				if( is_valid_address(dst_addr) ) {
					*dst_addr = *src_addr;//src_color;//COLOR_BLUE;//*src_addr;
				}
				*src_addr = src_color;
			}
		}
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
void sg2d_line( const struct rect *_rect, u8 color )
{
//	struct gra_info _info;
//	get_info( &_info );

	u32 length = (_rect->width >= _rect->height) ? _rect->width : _rect->height;
	f32 _posx = _rect->posx;
	f32 _posy = _rect->posy; 
	f32 stepx = (f32)_rect->width / length;
	f32 stepy = (f32)_rect->height / length;
	u8 *addr = get_addr( _posy, _posx);
	u16 i = 0;

//	putn_col( (u32)_posx, 16*10, 16*1, 3 );
//	putn_col( (u32)_posy, 16*10, 16*2, 3 );
//	putn_col( (u32)stepx*100, 16*5, 16*1, 5 );
//	putn_col( (u32)stepy*100, 16*5, 16*2, 5 );
	for( ; i < length; ++i ) {
		if( is_valid_address(addr+i) ) {
			*addr = color;
		}
		_posx += stepx;
		_posy += stepy;
		addr = get_addr( _posy, _posx);
	}
//	putn_col( (u32)_posx, 16*22, 16*1, 3 );
//	putn_col( (u32)_posy, 16*22, 16*2, 3 );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
void sg2d_square( const struct rect *_rect, f32 rot, u8 color )
{
//	struct gra_info _info;
//	get_info( &_info );
	u32 screen_ofsx = _info.screen_width >> 1;
	u32 screen_ofsy = _info.screen_height >> 1;
	u8 *addr = NULL;
	u32 y,x;
	vector2_t pos, base;
	
	for( y = 0; y < _rect->height; ++y ) {
		base.y = _rect->posy + y;
		for( x = 0; x < _rect->width; ++x ) {
			base.x = _rect->posx + x;
			rotate( &pos, &base, rot );
			addr = get_addr( (pos.y + screen_ofsy), (pos.x + screen_ofsx) );
			/*_info.vram_begin + 
				((((u32)pos.y + screen_ofsy) * _info.screen_width) + ((u32)pos.x + screen_ofsx));*/
			if( is_valid_address(addr) ) {
				*addr = color;
			}
		}
	}
	putn( (u32)pos.x, 16*10, 16*9 );
	putn( (u32)pos.y, 16*10, 16*10 );
}


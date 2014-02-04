#ifndef _GRAPHIC_2D_H_
#define _GRAPHIC_2D_H_

#include "define.h"
#include "graphicUtil.h"

void init_sg2d( void );
void sg2d_fill_pixel( const struct rect *_rect, u8 color );
void sg2d_move_pixel( u32 dstx, u32 dsty, const struct rect *src, u8 src_color );
void sg2d_line( const struct rect *_rect, u8 color );
void sg2d_square( const struct rect *_rect, f32 rot, u8 color );

#endif	// _GRAPHIC_2D_H_

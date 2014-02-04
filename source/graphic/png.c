/*---------------------------------------------------------------------*/
/*!
* @file		png.c
* @brief	Portable Network Graphics(PNG)
* @note		Feb.04.2014 ver 1.0 support
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "png.h"

u32 pnt_file_signature[] = { 0x89504e47,0x0d0a1a0a };
u8 allowed_bit_depths[] =  {
	0x1f,// color type 0 : depth 1,2,4,8,16
	0xff,// color type 1 : invalid
	0x18,// color type 2 : depth 8,16
	0x0f,// color type 3 : depth 1,2,4,8
	0x18,// color type 4 : depth 8,16
	0xff,// color type 5 : invalid
	0x18,// color type 6 : depth 8.16
};

void _get_chunk( chunk *dst, u8* file )
{
	dst->length = *((u32*)file);
	dst->type = *((u32*)file + 1);
	dst->data = file + 8;
	dst->crc = *(file + dst->length + 8);
}

bool _check_png_signature( u8* file )
{
	if( *((u32*)_file) != png_file_signature[0]
	&&	*((u32*)_file + 1) != png_file_signature[1] )
	{
		return true;
	} else {
		return false;
	}
}

bool open_png_file( png_file *dst )
{
	chunk _chunk;
	u8* _file = 0x00;
	
	
	// png file signature check
	if( _check_png_signature( _file ) ) {
		return false;
	}
	_file += 8;
	
	// get IHDR
	_get_chunk( _chunk, _file );
	
	switch( _chunk.type ) {
		// IHDR
		case 0x49484452:
			break;
		
		// IEND
		case 0x49454e44:
			break;
		
		
		
		default;
			break;
	}
}



#ifndef _PNG_H_
#define _PNG_H_

#include "type.h"

#define _CUNK_TYPE_MASK 0x20

typedef struct _chunk {
	u32 length;
	u32 type;
	u8 *data;
	u32 crc;
} chunk;

/* IHDR
width, height : mage dimensions in pixels

bit depth : number of bits per sample or per palette index(not per pixel)
 color type  allowed bit depths
     0     : 1,2,4,8,16
     2     : 8,16
     3     : 1,2,4,8
     4     : 8,16
     6     : 8.16

color type : color type codes represent sums of the following values
 1 : palette used
 2 : color used
 4 : alpha channel used

compression method : method used to compression the image data

filter method : preprocessing method applied to the image data befor compression

interlace method : transmission order of the image data
*/
typedef struct _ihdr_data {
	u32 width;
	u32 height;
	u8 bit_depth;
	u8 color_type; 
	u8 compression_method;
	u8 filter_method;
	u8 interlace_method;
} ihdr_data;

typedef struct _png_file {
	u8* file;
	ihdr_data ihdr;
	chunk idat;
} png_file;

#endif // _PNG_H_

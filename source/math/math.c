/*---------------------------------------------------------------------*/
/*!
* @file		math.c
* @brief	math
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "math.h"

static f32 trigonometric_function_tbl[][3] = {
#include "trigonometricfunction.tbl"
};

f32 sin( s32 rot )
{
	if( rot >= 0 ) {
		return trigonometric_function_tbl[ (rot % 181) ][0];
	}
	return 0.0f;
}
f32 cos( s32 rot )
{
	if( rot >= 0 ) {
		return trigonometric_function_tbl[ (rot % 181) ][1];
	}
	return 0.0f;
}
f32 tan( s32 rot )
{
	if( rot >= 0 ) {
		return trigonometric_function_tbl[ (rot % 181) ][2];
	}
	return 0.0f;
}

void rotate( vector2_t *des, const vector2_t *src, s32 rot )
{
	f32 sine = sin( rot );
	f32 cosine = cos( rot );
	f32 fx = src->x;
	f32 fy = src->y;
	vector2_t offset = {
		.x = 0/*src->x*/ + 0.5f,
		.y = 0/*src->y*/ + 0.5f,
	};
	fx -= offset.x;
	fy -= offset.y;
	
	des->x = cosine * fx - sine * fy;
	des->y = sine * fx + cosine * fy;
	
	des->x += offset.x;
	des->y += offset.y;
}

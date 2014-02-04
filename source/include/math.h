#ifndef _MATH_H_
#define _MATH_H_

#include "type.h"

typedef struct {
	f32 x;
	f32 y;
} vector2_t;

typedef struct {
	f32 x;
	f32 y;
	f32 z;
} vector3_t;


f32 sin( s32 rot );
f32 cos( s32 rot );
f32 tan( s32 rot );

void rotate( vector2_t *des, const vector2_t *src, s32 rot );

#endif // _MATH_H_

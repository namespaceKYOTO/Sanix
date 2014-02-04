/*---------------------------------------------------------------------*/
/*!
* @file		debug.c
* @brief	debug
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "define.h"
#include "graphic.h"
#include "debug.h"


void assert( u8 *error_put )
{
	puts( error_put, 0, 0 );
	for(;;){};
}



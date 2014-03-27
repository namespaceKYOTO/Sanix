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
	puts_ex( "Assert : ", 0, 0, COLOR_RED, COLOR_BLUE );
	puts_ex( error_put, 9*8, 0, COLOR_RED, COLOR_BLUE );
	for(;;){};
}



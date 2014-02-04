/*---------------------------------------------------------------------*/
/*!
* @file		sfs.c
* @brief	Sanix File System
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/

#include "define.h"
#include "fs/sfs.h"

struct super_block* sb_get( void )
{
	u32 addr = 512 * 5;
	
	return (struct super_block*)addr;
}

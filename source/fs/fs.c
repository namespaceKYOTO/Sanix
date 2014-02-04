/*---------------------------------------------------------------------*/
/*!
* @file		fs.c
* @brief	file system
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/

#include "define.h"
#include "fs.h"

bool _file_open( const char *name, file *_file )
{
	
}

file* fopen( const char* name, file *_file )
{
	if( _file_open( name, _file ) ) {
		return _file;
	}
	return 0;
}




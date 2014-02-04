/*---------------------------------------------------------------------*/
/*!
* @file		page.h
* @brief	paging
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#ifndef _PAGE_H_
#define _PAGE_H_


#define PAGE_DIRECTORY_ENTRY_NUM	1024
#define PAGE_TABLE_ENTRY_NUM		1024
#define PDE_ADD	0x00100000
#define PTE_ADD	0x00101000


void init_page( void );

#endif //_PAGE_H_

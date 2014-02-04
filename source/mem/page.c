/*---------------------------------------------------------------------*/
/*!
* @file		page.c
* @brief	paging
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "define.h"
#include "fasm.h"
#include "graphic.h"
#include "memory.h"
#include "debug.h"
#include "page.h"


//u32 s_pde[PAGE_DIRECTORY_ENTRY] __attribute__((aligned(4096)));						// PDE 4K byte alignment
//u32 s_pte[PAGE_DIRECTORY_ENTRY][PAGE_TABLE_ENTRY] __attribute__((aligned(4096)));	// PTE 4K byte alignment
static u32 *s_pde = (u32*)PDE_ADD;	// 1.000 Mbyte
static u32 *s_pte = (u32*)PTE_ADD;	// 1.004 Mbyte


//!<
void _set_page( u32 *pde, u32 *pte, u32 page_base_addr, u32 pde_info, u32 pte_info )
{
	*pde = (((u32)pte & 0xFFFFF000) | (pde_info & 0x0FFF));		// PDE information
	
	// PTE set in the sequence number
	u32 i = 0;
	u32 size = 0x1000;	// 4K byte
	for( ; i < PAGE_TABLE_ENTRY_NUM; ++i ) {
		*(pte + i) = ((page_base_addr & 0xFFFFF000) | (pte_info & 0x0FFF));	// PTE information
		page_base_addr += size;												// next addr
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize paging
 */
/*---------------------------------------------------------------------*/
void init_page( void )
{
	u32 i = 0;
	u32 page_base_addr = 0x00;
	u32 size = 0x400000;		// 4M byte
	for( ; i < PAGE_DIRECTORY_ENTRY_NUM; ++i ) {
		_set_page( s_pde + i, s_pte + (i*PAGE_TABLE_ENTRY_NUM), page_base_addr, /*0x0013*/0x0001, /*0x0013*/0x0001 );
		page_base_addr += size;
	}
	
	cr3( (u32)s_pde );	// set PDBR
//	cr0( 0x80000000 );	// use paging
	u32 flag = get_cr0();
	set_cr0( flag | 0x80000000 );
}


/*---------------------------------------------------------------------*/
/*!
 * @file	pci.h
 * @brief	peripheral component interconnect
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#include "type.h"
#include "memory.h"
#include "console.h"
#include "pci.h"

struct pci_device_man g_pci_man;
#define _add_pci_device( pci_dev )		\
{										\
	if( g_pci_man.top != NULL ) {		\
		g_pci_man.last->next = pci_dev;	\
		g_pci_man.last = pci_dev;		\
	} else {							\
		g_pci_man.top = pci_dev;		\
		g_pci_man.last = pci_dev;		\
	}									\
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	pci word read
 * @param bus	: number of bus
 * @param slot	: number of device
 * @param func	: number of function
 * @param offset: register address
 * @return read double word
 */
/*---------------------------------------------------------------------*/
u32 __pci_read32( u32 bus, u32 slot, u32 func, u8 offset )
{
	u32 data = 0x80000000;
	data = data | ((0xFF & bus) << 16) | ((0x1F & slot) << 11) | ((0x07 & func) << 8) | (offset & 0xfc);
	io_out32( PCI_CONFIG_REG_ADDR, data );
	
	u32 ret = io_in32( PCI_CONFIG_REG_DATA );
	return ret;
/*
%bit0-1：0に固定
%bit2-7：レジスタアドレス
%bit8-10：機能番号
%bit11-15：デバイス番号
%bit16-23：バス番号
%bit24-30：リザーブで、0に固定
%bit31：イネーブルビットで、1に固定
*/
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	PCI initialize
 */
/*---------------------------------------------------------------------*/
void init_pci( void )
{
	g_pci_man.top = 0;
	g_pci_man.last = 0;

	// check up pci device
	u32 i = 0;
	u32 j = 0;
	u32 ret = 0;
	for( ; j < 8; ++j ) {
		for( ; i < 32; ++i ) {
			ret = __pci_read32( j, i, 0, 0 );
			u16 vid = (u16)(ret & 0x0000FFFF);
			u16 did = (u16)((ret >> 16) & 0x0000FFFF);
			if( vid != 0xFFFF ) {
				struct pci_device *pci = (struct pci_device *)alloc_gp_slab_mem(sizeof(struct pci_device));
				pci->vendor_id = vid;
				pci->device_id = did;
				pci->next = NULL;
				_add_pci_device( pci );
				puts_console("vender id  ");puth_console(vid);
				putc_console('\n');
				puts_console("devide id  ");puth_console(did);
				putc_console('\n');
				putc_console('\n');
			}
		}
	}
}

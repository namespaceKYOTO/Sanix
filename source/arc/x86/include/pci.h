/*---------------------------------------------------------------------*/
/*!
 * @file	pci.h
 * @brief	peripheral component interconnect
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _PCI_H_
#define _PCI_H_

//#include "type.h"
//#include "define.h"

#define PCI_IOPORT_ADDR		0x000100
#define PCI_IOPORT_BASE		0x000200
#define PCI_CONFIG_REG_ADDR		0x0cf8
#define PCI_CONFIG_REG_DATA		0x0cfc


struct pci_device {
	u32					vendor_id;
	u32					device_id;
	struct pci_device	*next;
};

struct pci_device_man {
	struct pci_device	*top;
	struct pci_device	*last;
};

void init_pci( void );

#endif // _PCI_H_

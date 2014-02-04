/*---------------------------------------------------------------------*/
/*!
 * @file	ata.h
 * @brief	ata
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _ATA_H_
#define _ATA_H_

#include "type.h"

#define ATA_DEV_PATA	0	// parallel ATA
#define ATA_DEV_PATAPI	1	// parallel ATAPI
#define ATA_DEV_SATA	2	// serial ATA
#define ATA_DEV_SAPAPI	3	// serial ATAPI
#define ATA_DEV_UNKNOWN	4	

#define ATA_ADDRESSING_SCHEME_CHS   1
#define ATA_ADDRESSING_SCHEME_LBA28 2
#define ATA_ADDRESSING_SCHEME_LBA48 4

#define ATA_TRANSFER_MODE_PIO 0
#define ATA_TRANSFER_MODE_DMA 1
#define ATA_TRANSFER_MODE_UDMA 2

#define ATA_DEVICE_MASTER 0
#define ATA_DEVICE_SLAVE 1

#define ATA_CMD_IDENTIFY_DEVICE		0xec
#define ATA_CMD_IDENTIFY_PACKET_DEVICE	0xa1
#define ATA_CMD_READ_PIO		0x20
#define ATA_CMD_READ_VERIFY_PIO		0x24
#define ATA_CMD_WRITE_PIO		0x30
#define ATA_CMD_READ_DMA		0xcb
#define ATA_CMD_WRITE_DMA		0xca

/*
 * Port
 * 0x01F0 Data
 * 0x01F1 Features / Error
 * 0x01F2 Sector Count
 * 0x01F3 Sector Number / LBAlo
 * 0x01F4 Cylinder Low / LBAmid
 * 0x01F5 Cylinder High / LBAhi
 * 0x01F6 Drive / Head
 * 0x01F7 Command / Status
 * 0x03F6 Device Controller / Alternate
 */

#define _DEBUG_ATA 1

struct ata_dev_info {
	u8 dev_type;
	u8 serial_number[20];
	u8 support_addressing_scheme;
	u8 support_transfer_mode;
	u32 transfer_mode;
	u32 lba28_total_sectors;
	u64 lba48_total_sectors;
};

struct ata_address {
	union {
		struct {
			u16 cylinder;
			u8 header;
			u8 sector;
		};
		u32 lba28;
		u64 lba48;
	};
};

#define ATA_ADRESS_TO_CHS( ata, address ) \
	ata.cylinder = ((address>>9) / 63 ) / 255;	\
	ata.header = ((address>>9) / 63 ) % 255;	\
	ata.sector = (address>>9) % 63 + 1;

extern struct ata_dev_info g_ata_dev_info;

u8 get_ata_devtype( s32 slave );
void init_ata( void );
u32 read_ata_data( u8 *dst, u32 nr_read_sector, struct ata_address *addr );
u32 write_ata_data( u8 *src, u32 nr_write_sector, struct ata_address *addr );


#endif // _ATA_H_

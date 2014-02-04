/*---------------------------------------------------------------------*/
/*!
* @file		ata.h
* @brief	ata
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
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
#include "graphic.h"
#include "error.h"
#include "time.h"
#include "ata.h"


#ifdef _DEBUG_ATA
#define __ATA_PUTS( str, posx, posy )		\
	puts_ex( str, posx*8, posy*16, COLOR_RED, COLOR_BLUE );
#define __ATA_PUTN( n, posx, posy, col )	\
	putn_ex( n, posx*8, posy*16, COLOR_RED, COLOR_BLUE, col );
#define __ATA_PUTH( n, posx, posy, col )	\
	puth_ex( n, posx*8, posy*16, COLOR_RED, COLOR_BLUE, col );
#else
#define __ATA_PUTS( str, posx, posy )	{}
#define __ATA_PUTN( n, posx, posy, col ){}
#define __ATA_PUTH( n, posx, posy, col ){}
#endif // _DEBUG_ATA


struct ata_dev_info g_ata_dev_info;

/*---------------------------------------------------------------------*/
/*!
 * @brief	get ATA device type
 * @param	slave : is slave
 */
/*---------------------------------------------------------------------*/
u8 get_ata_devtype( s32 device )
{
	u8 status = 0;
	u8 ret = 0;
	io_out8( 0x01F6, 0xa0 | device << 4 );
	ndelay( 400 );	// must wait 400ns
	
	
#if 0
	/* check power mode */
	io_out8( 0x01F1, 0x00 );
	io_out8( 0x01F2, 0x00 );
	io_out8( 0x01F3, 0x00 );
	io_out8( 0x01F7, 0xE5 );	/* check power mode */
	status = io_in8( 0x01F7 );
	if( status & 0x01 ) {
		__ATA_PUTS("Check Power Mode Error", 0, 8 );
	} else {
		ret = io_in8( 0x01F2 );
		__ATA_PUTS("Check Power Mode", 0, 8 );
		__ATA_PUTH( ret, 20, 8, 2 );
	}
#endif
	
	
	status = io_in8( 0x01F7 );
	if( status & 0x80 || !(status & 0x70) ) {
		__ATA_PUTS("Device busy", 0, 9 );
//		for(;;){};
		return ATA_DEV_UNKNOWN;
	}
	__ATA_PUTN( status, 15, 9, 2 );
	
	
	/* software reset */
//	io_out8( 0x3F6, 0x04 );
//	__ATA_PUTS("software reset", 0, 10 );
	/* execute device diagnostic */
	io_out8( 0x01F1, 0x00 );
	io_out8( 0x01F2, 0x00 );
	io_out8( 0x01F3, 0x00 );
//	io_out8( 0x01F6, 0x04 );	/* master  @must wait 400ns */
//	ndelay( 400 );			// must wait 400ns
	io_out8( 0x01F7, 0x90 );	/* execute device diagnostic command */
	__ATA_PUTS("execute device diagnostic", 0, 10 );
	
	status = io_in8( 0x01F7 );
	if( (status & 0x89) == 0x01 ) {
		ret = io_in8( 0x01F1 );			/* error */
		// (ret == 0x01 || ret == 0x81) is successful
		__ATA_PUTS("ERROR", 30, 10 );
		__ATA_PUTH( ret, 45, 10, 2 );
//		for(;;){};
//		return  ATA_DEV_UNKNOWN;
	}
	
	ndelay( 400 );	// master/slave select   must wait 400ns
//	io_out8( 0x1F6, slave << 4 );
//	__ATA_PUTS("master slave select", 0, 11 );
	
	/* get signature */
	u8 cl = io_in8( 0x01F4 );
	u8 ch = io_in8( 0x01F5 );
	if( cl == 0x00 && ch == 0x00 ) return ATA_DEV_PATA;
	if( cl == 0x14 && ch == 0xee ) return ATA_DEV_PATAPI;
	
	
	io_out8( 0x3F6, 0x00 );

	return ATA_DEV_UNKNOWN;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize ATA
 */
/*---------------------------------------------------------------------*/
void init_ata( void )
{
	u16 dev_info[256];
	u32 i;
	
#if 0
	g_ata_dev_info.dev_type = get_ata_devtype( ATA_DEVICE_MASTER );	/* get master device type */
	if( g_ata_dev_info.dev_type == ATA_DEV_UNKNOWN ) {
		__ATA_PUTS("ATA device type unknown", 0, 12 );
		return ;
	}
#else
	// provisional
	g_ata_dev_info.dev_type = ATA_DEV_PATA;
#endif
	
	io_out8( 0x01F2, 0x00 );
	io_out8( 0x01F3, 0x00 );
	io_out8( 0x01F4, 0x00 );
	io_out8( 0x01F5, 0x00 );
	
	/* identify device of master */
	switch( g_ata_dev_info.dev_type ) {
		case ATA_DEV_PATA:
			io_out8( 0x017f, ATA_CMD_IDENTIFY_DEVICE );	// IDENTIFY DEVICE command
			__ATA_PUTS("IDENTIFY DEVICE command", 0, 12 );
			break;
		
		case ATA_DEV_PATAPI:
			io_out8( 0x017f, ATA_CMD_IDENTIFY_PACKET_DEVICE );	// IDENTIFY PACKET DEVICE command
			__ATA_PUTS("IDENTIFY PACKET DEVICE command", 0, 12 );
			break;
	}
	
	/* wait busy */
	u8 status;
	do {
		status = io_in8( 0x01F7 );
		__ATA_PUTS("polling", 0, 13 );
		puth_ex( status, 8*15,16*13, COLOR_RED, COLOR_BLUE, 2 );
	} while( status & 0x80 );
	
	/* get device info */
	for( i = 0; i < 256; ++i ) {
		dev_info[i] = io_in16( 0x01F0 );
	}
	__ATA_PUTS("get device info", 0, 14 );
	
	/* addressing scheme that supports */
	g_ata_dev_info.support_addressing_scheme = ATA_ADDRESSING_SCHEME_CHS;
	if( dev_info[49] & 0x0100 )
		g_ata_dev_info.support_addressing_scheme |= ATA_ADDRESSING_SCHEME_LBA28;
	if( dev_info[83] & 0x0200 )
		g_ata_dev_info.support_addressing_scheme |= ATA_ADDRESSING_SCHEME_LBA48;
	__ATA_PUTS("addressing scheme", 0, 15 );
	__ATA_PUTN(g_ata_dev_info.support_addressing_scheme, 20, 15, 3 );
	
	/* total sectors */
	g_ata_dev_info.lba28_total_sectors = ((u32)dev_info[60] << 16) | (u32)dev_info[61];
	g_ata_dev_info.lba48_total_sectors = ((u64)dev_info[100] << 48) | ((u64)dev_info[101] << 32) |
								((u64)dev_info[102] << 16) | (u64)dev_info[103];
//	if( lba28_total_sectors != 0 ) addressing_scheme |= ATA_ADDRESSING_SCHEME_LBA28;
//	if( lba48_total_sectors != 0 ) addressing_scheme |= ATA_ADDRESSING_SCHEME_LBA48;
	__ATA_PUTS("LBA28 toral sector", 0, 16 );
	__ATA_PUTN(g_ata_dev_info.lba28_total_sectors, 28, 16, 10 );
	__ATA_PUTS("LBA48 toral sector", 0, 17 );
	__ATA_PUTN(g_ata_dev_info.lba48_total_sectors, 28, 17, 10 );
	
	/* set transfer mode */
	g_ata_dev_info.transfer_mode = ATA_TRANSFER_MODE_PIO;
	u16 mdm_support = dev_info[61]; /* multiple word DMA */
	u16 umd_support = dev_info[88];	/* Ultra DMA mode */
	__ATA_PUTS("set transfer mode", 0, 18 );
	
	/* Support ATA/ATAPI */
	__ATA_PUTS("Support ATA ATAPI", 0, 19 );
	__ATA_PUTH(dev_info[80], 30, 19, 8 );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	read ATA data
 * @param	dst : out data
 * @param	nr_read_sector : number of read sector
 * @param	addr : ata adress info
 */
/*---------------------------------------------------------------------*/
u32 read_ata_data( u8 *dst, u32 nr_read_sector, struct ata_address *addr )
{
	u8 status;
	u32 i;
	
	/* busy check */
//	status = io_in8( 0x01F7 );
//	if( status & 0xc8 ) return EBUSY;
	
	ndelay( 400 );	// master select  must wait 400ns
	io_out8( 0x1F6, 0x00 );
	
	/*  busy check */
//	status = io_in8( 0x01F7 );
//	if( status & 0xc8 ) return EBUSY2;
	
//	read_size = nr_read_sector * 512;	/* 512 is sector size */
	u32 read_size = (nr_read_sector << 9); /* nr_read_sector * 512 */
	__ATA_PUTN( read_size, 30, 21, 10 );
	
	/* parameter set */
	io_out8( 0x01F2, nr_read_sector );
	io_out8( 0x01F3, addr->sector );
	io_out8( 0x01F4, (u8)(addr->cylinder & 0x00FF) );
	io_out8( 0x01F5, (u8)((addr->cylinder & 0xFF00) >> 8) );
	io_out8( 0x01F6, 0x00 | addr->header );
	
	/* command */
	io_out8( 0x01F7, ATA_CMD_READ_PIO );	/* read sector */
	
	for( i = 0; i < read_size; ++i ) {
		/* status check */
		do {
			status = io_in8( 0x01F7 );
			if( status & 0x01 ) return EREAD;
 		} while( (status & 0x88) == 0x88 );
		
		/* read data */
		dst[i] = io_in8( 0x01F0 );
	}	
	
	status = io_in8( 0x01F7 );
	if( status & 0x80 ) return EBUSY3;
	
	return ENONE;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	write ATA
 */
/*---------------------------------------------------------------------*/
u32 write_ata_data( u8 *src, u32 nr_write_sector, struct ata_address *addr )
{
	u8 status;
	u32 i;
	
	/* busy check */
//	status = io_in8( 0x01F7 );
//	if( status & 0xc8 ) return EBUSY;
	
	ndelay( 400 );	// master select  must wait 400ns
	io_out8( 0x1F6, 0x00 );
	
	/*  busy check */
//	status = io_in8( 0x01F7 );
//	if( status & 0xc8 ) return EBUSY2;
	
//	u32 nr_sector = size / 512;	/* 512 is sector size */
	u32 write_size = (nr_write_sector << 9); /* nr_write_sector * 512 */
	__ATA_PUTN( write_size, 30,20, 10 );
	
	/* parameter set */
	io_out8( 0x01F2, nr_write_sector );
	io_out8( 0x01F3, addr->sector );
	io_out8( 0x01F4, (u8)(addr->cylinder & 0x00FF) );
	io_out8( 0x01F5, (u8)((addr->cylinder & 0xFF00) >> 8) );
	io_out8( 0x01F6, 0x00 | addr->header );
	
	/* command */
	io_out8( 0x01F7, ATA_CMD_WRITE_PIO );	/* write sector */
	
	for( i = 0; i < write_size; ++i ) {
		/* status check */
		do {
			status = io_in8( 0x01F7 );
			if( status & 0x01 ) return EWRITE;
		} while( (status & 0x88) == 0x88 );
		
		/* write data */
		io_out8( 0x01F0, src[i] );
	}
	
	status = io_in8( 0x01F7 );
	if( status & 0x80 ) return EBUSY3;
	
	return ENONE;
}

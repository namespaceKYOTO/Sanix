#include "type.h"
#include "define.h"

#define IDE_PRIMARY_PORRT		0x01f0
#define IDE_PRIMARY_CONTROL_PORT	0x03f6
#define IDE_SECONDARY_PORT		0x0170
#define IDE_SECONDARY_CONTROL_PORT	0x0376

#define IDE_	0xcf8

#if 0
	s32 data = (1<<31) | (bus<<16) | (device<<11) | (func<<8);// | 8;
	io_out32( 0x0cf8, data | 8);
	
	s32 ret = io_in32( 0x0cfc );
	
	// if device exists
	if( (ret>>16) != 0xFFFF ) {
		// Check if this device  need an IRQ assinment
		io_out32( 0x0cf8, data  |0x3c );	// read the interrupt line field
		io_in8(0x0cfc , 0xfe  );		// change the IRQ field to 0xfe
		io_out32( 0x0cf8, data  |0x3c );	// read the interrupt line field
		if( io_in32(0x0cfc) & 0xff == 0xfe ) {
			// this device need an IRQ assignment
		} else {
		// The device does'nt use IRQs, check if this is an Parallel IDE
			if(class == 0x01 && subclass  == 0x01 && (ProgIF == 0x8a || ProgIF == 0x80)) {
				// This is a Parallel IDE Controller which uses IRQs 14 and 15
			}
		}
	}
#endif

void ide_controller_initialize( void )
{
}

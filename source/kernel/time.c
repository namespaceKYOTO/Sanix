/*---------------------------------------------------------------------*/
/*!
 * @file	time.c
 * @brief	time
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#include "graphic.h"
#include "interrupt.h"
#include "time.h"

//static u32 s_time_count;
static struct time s_time;
#define __TIME_COUNT_UP	(1)

u32 __nr_tick_loop = 0;
static struct timer s_timer;


void init_time( void )
{
//	s_time_count = 0;
	s_time.hour = 0;
	s_time.minute = 0;
	s_time.second = 0;
//	s_time.millisecond = 0;

	s_timer.tick = 0;
//	s_timer.nr_tick_loop = 0;
	
	u32 loop = 0;
	u32 prev = s_timer.tick;
	while( s_timer.tick == prev ){}
	prev = s_timer.tick;
	while( s_timer.tick == prev ){ ++loop; }
	__nr_tick_loop  = loop;
//	s_timer.nr_tick_loop = loop;
}

void get_timer( struct timer *dst )
{
	dst->tick = s_timer.tick;
}

void get_time( struct time *dst )
{
}

void time_count( void ) {
	++s_timer.tick;
}

//!< actual delay processing
void __delay( s32 loop )
{
	s32 i = 0;
	for( ; i < loop; ++i ) {
	}
}
//!< nano second delay
void ndelay( s32 nanosec )
{
	__delay( (__nr_tick_loop  / 1000) * nanosec );//	1tick‚Ìloop”‚©‚çloop‰ñ”‚ðŒˆ‚ß‚Ädelayˆ—
}
//!< milli second delay
void mdelay( s32 millisec )
{
	__delay( (__nr_tick_loop ) * millisec ); //	1tick‚Ìloop”‚©‚çloop‰ñ”‚ðŒˆ‚ß‚Ädelayˆ—
}

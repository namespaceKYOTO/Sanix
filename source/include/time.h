/*---------------------------------------------------------------------*/
/*!
 * @file	time.h
 * @brief	time
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _TIME_H_
#define _TIME_H_

#include "define.h"

struct time {
	u8 second;
	u8 minute;
	u16 hour;
};

struct timer {
	u32 tick;
//	u32 nr_tick_loop;
};


inline void get_timer( struct timer *dst );
void get_time( struct time *dst );
void time_count( void );

inline void ndelay( s32 nanosec );
inline void mdelay( s32 millisec );

#endif // _TIME_H_

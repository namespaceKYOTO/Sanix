/*---------------------------------------------------------------------*/
/*!
* @file		descriptor.c
* @brief	spinlock, semaphore, etc...
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/

#include "lock.h"

#if 0
void spin_lock( struct spin_lock *dst )
{
	/*
	 * u8 lock = 1;
	 * dst->lock = lock;
	*/
	// damn described by AT&T
	u8 lock = 1;
	__asm__ (
		""
		"xchg %[lock],%[dst_lcok]"
		""
		: [lock] "+q" (lock),
		  [dst_lcok] "+m" (dst->lock)
		:
		: "memory");
}
void spin_unlock( struct spin_lock *dst )
{
	/*
	 * u8 lock = 0;
	 * des->lock = lock;
	*/
	// damn described by AT&T
	u8 lock = 0;
	__asm__ (
		"xchg %[lock],%[dst_lock]"
		: [lock] "+q" (lock),
		  [dst_lock] "+m" (dst->lock)
		:
		: "memory");
}
#endif


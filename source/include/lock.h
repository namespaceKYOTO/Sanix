/*---------------------------------------------------------------------*/
/*!
* @file		lock.h
* @brief	synchronous processing
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#ifndef _LOCK_H_
#define _LOCK_H_

#include "type.h"


/*---------------------------------------------------------------------*/
/*!
 * @brief	spin lock structure
 * @note	0:free  1:lcok
 */
/*---------------------------------------------------------------------*/
struct spin_lock {
	u8 lock;
};
/*---------------------------------------------------------------------*/
/*!
 * @brief	spin lock macro
 * @note	start:
 * 			  u8 lock = 1;
 * 			  xcang slk->lock,lock;
 * 			  if( lock == 0 ) goto end;
 * 			  while( slk->lock == 1 ) pause;
 * 			  goto start;//start all over again
 * 			end:
 */
/*---------------------------------------------------------------------*/
// ...damn described by AT&T
#define spin_lock( slk )			\
{									\
	u8 __lock = 1;					\
	__asm__ __volatile__(			\
		"1:\n\t"					\
		"xchgb %[lock],%[slock]\n\t"\
		"test %[lock],%[lock]\n\t"	\
		"jz 3f\n\t"					\
		"2:\n\t"					\
		"pause\n\t"					\
		"test %[lock],%[slock]\n\t"	\
		"jz 1b\n\t"					\
		"jmp 2b\n\t"				\
		"3:\n\t"					\
		: [lock] "+q" (__lock),		\
		  [slock] "+m" (slk->lock)	\
		:							\
		: "memory");				\
}
/*---------------------------------------------------------------------*/
/*!
 *	@bried	spin unlock macro
 *	@note	u8 lock = 0;
 *			des->lock = lock;
 */
/*---------------------------------------------------------------------*/
// damn described by AT&T
#define spin_unlock( slk )			\
{									\
	u8 __lock = 0;					\
	__asm__ __volatile__(			\
		"xchgb %[lock],%[slock]\n\t"\
		: [lock] "+q" (__lock),		\
		  [slock] "+m" (slk->lock)	\
		:							\
		: "memory");				\
}

/*---------------------------------------------------------------------*/
/*!
 *	@brief	atomic structure
 */
/*---------------------------------------------------------------------*/
typedef struct {
	u32 var;
} atomic_t;

/*---------------------------------------------------------------------*/
/*!
 *	@brief	atomic read
 */
/*---------------------------------------------------------------------*/
static inline u32 atomic_read( atomic_t *src )
{
	return (*(volatile u32*)&src->var);
}
/*---------------------------------------------------------------------*/
/*!
 *	@brief	atomic set
 */
/*---------------------------------------------------------------------*/
static inline void atomic_set( atomic_t *dst, u32 val )
{
	dst->var = val;
}
/*---------------------------------------------------------------------*/
/*!
 *	@brief	atomic addition
 */
/*---------------------------------------------------------------------*/
static inline void atomic_add( atomic_t *dst, u32 value )
{
	__asm__( "lock addl %[val],%[var]\n\t"
		:[var]"+m"(dst->var)
		:[val]"Ir"(value)
	);
}
/*---------------------------------------------------------------------*/
/*!
 *	@brief	atomic subtranction
 */
/*---------------------------------------------------------------------*/
static inline void atomic_sub( atomic_t *dst, u32 value )
{
	__asm__( "lock subl %[val],%[var]\n\t"
		:[var]"+m"(dst->var)
		:[val]"Ir"(value)
	);
}
/*---------------------------------------------------------------------*/
/*!
 *	@brief	atomic increment
 */
/*---------------------------------------------------------------------*/
static inline void atomic_inc( atomic_t *dst )
{
	atomic_add( dst, 1 );
}
/*---------------------------------------------------------------------*/
/*!
 *	@brief	atomic decrement
 */
/*---------------------------------------------------------------------*/
static inline void atomic_dec( atomic_t *dst )
{
	atomic_sub( dst, 1 );
}


#endif // _LOCK_H_

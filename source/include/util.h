/*---------------------------------------------------------------------*/
/*!
 * @file	util.h
 * @brief	util
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _UTIL_H_
#define _UTIL_H_

#include "type.h"


#define S_ARRAY_SIZE( array ) \
	sizeof(array) / sizeof(array[0])

enum {
	NUMBER_TYPE_NONE = 0,
	NUMBER_TYPE_DECIMAL,
	NUMBER_TYPE_HEXADECIMAL,
};

/*---------------------------------------------------------------------*/
/*!
 * @brief	Queue
 */
/*---------------------------------------------------------------------*/
typedef struct {
	u8* buff;
	u32 start ,end;
	u32 size;
	bool is_space;
} squeue_t;

//!<
#define queue_size( queue ) \
	((queue.start <= queue.end && queue.is_space ) ? \
		queue.end - queue.start : (queue.size - queue.start) + queue.end)

//!<
#define init_squeue( queue, _buff, _start, _end, _size ) \
	queue.buff=_buff; queue.start=_start; queue.end=_end; queue.size=_size; queue.is_space = true;

void enqueue( squeue_t *dst, u8 data );
u8 dequeue( squeue_t *dst );
void do_empty_queue( squeue_t *dst );
bool do_replace_queue( squeue_t *src, u8 old, u8 new );
bool get_contents_queue( squeue_t *src, s32 get_size, u8* dst, u32 dst_size, bool inc );
s32 get_queue_element_size( squeue_t *src );
s32 get_queue_free_size( squeue_t *src );
bool is_exist_elements_queue( squeue_t *dst, u8 element );

/*---------------------------------------------------------------------*/
/*!
 * @brief	Hash
 */
/*---------------------------------------------------------------------*/
static inline u32 hash_u32( u32 val, u32 bits )
{
	u32 hash = val * 0x9e370001ul;
	return hash >> (32 - bits);
}


/*---------------------------------------------------------------------*/
/*!
 * @brief	string operation
 */
/*---------------------------------------------------------------------*/
void strcpy( u8* dst, const u8* src );
void strncpy( u8* dst, const u8* src, s32 num );
s32 strcmp( const u8* src1, const u8* src2 );
u8* strchr( const u8* src, u8 code );
s32 strlen( const u8* src );
void sprintf( u8* dst, const u8* format, ... );

#define is_lower(c)				('a' <= c && c <= 'z')
#define is_upper(c)				('A' <= c && c <= 'Z')
#define is_alphabet(c) 			(is_lower(c) || is_upper(c))
#define is_number(c) 			('0' <= c && c <= '9')
#define is_alphanumeric( c )	(is_alphabet(c) || is_number(c))
#define is_decimal(c)			is_number(c)
#define is_hexadecimal(c)		(is_number(c) || ('A' <= c && 'F') || ('a' <= c && c <= 'f'))
#define is_ascii(c)				(0 <= c && c <= 127)


/*---------------------------------------------------------------------*/
/*!
 * @brief	Other
 */
/*---------------------------------------------------------------------*/
register u32 stack_pointer asm("esp") __attribute__((__used__));

s32 sdtoi( const u8* src );
s32 shtoi( const u8* src );

s32 get_number_type( const u8* src );
#endif // _UTIL_H_

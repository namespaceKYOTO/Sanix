/*---------------------------------------------------------------------*/
/*!
* @file		util.c
* @brief	Utility
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#include "memory.h"
#include "util.h"


/*---------------------------------------------------------------------*/
/*!
 * @brief	Queue
 */
/*---------------------------------------------------------------------*/
void enqueue( squeue_t *dst, u8 data )
{
//	u32 __size = queue_size( dst );
	if( get_queue_free_size( dst ) > 0 ) {
		dst->buff[dst->end] = data;
		if( ++dst->end >= dst->size ) dst->end = 0;
		if( dst->start == dst->end ) dst->is_space = false;
	} else {
		dst->buff[dst->end] = data;
		if( ++dst->start >= dst->size ) dst->start = 0;
		if( ++dst->end >= dst->size ) dst->end = 0;
	}
};

u8 dequeue( squeue_t *dst )
{
//	u32 __size = queue_size( dst );
	if( get_queue_element_size( dst ) > 0 ) {
		u8 data = dst->buff[dst->start];
		if( ++dst->start >= dst->size ) dst->start = 0;
		dst->is_space = true;
		return data;
	}
	return 0x00;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	empty the queue
 */
/*---------------------------------------------------------------------*/
void do_empty_queue( squeue_t *dst )
{
	if( dst->size <= 0 )
		return;

	smemset( dst->buff, 0x00, dst->size );
	dst->start = 0;
	dst->end = 0;
	dst->is_space = true;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	replace the code
 */
/*---------------------------------------------------------------------*/
bool do_replace_queue( squeue_t *src, u8 old, u8 new )
{
	s32 i = 0;
	bool ret = false;
	for( ; i < src->size; ++i ) {
		if( src->buff[i] == old ) {
			src->buff[i] = new;
			ret = true;
		}
	}
	return ret;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	get the contents of the queue
 */
/*---------------------------------------------------------------------*/
bool get_contents_queue( squeue_t *src, s32 get_size, u8* dst, u32 dst_size, bool inc )
{
	if( src->size < get_size || get_queue_element_size(src) == 0 )
		return false;

	s32 i = 0;
	s32 qi = src->start;
	for( ; i < dst_size && i < get_size && i < dst_size; ++i ) {
		dst[i] = src->buff[qi];
		if( ++qi >= src->size ) qi = 0;
		if( qi == src->end && src->is_space == true ) return;
		if( inc ) {
			++src->start;
			if( src->start >= src->size ) src->start = 0;
			if( src->start == src->end ) return;
		}
	}
	return true;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	get the element size ot the queue
 */
/*---------------------------------------------------------------------*/
s32 get_queue_element_size( squeue_t *src )
{
	if( src->start == src->end && src->is_space == true )
		return 0;

	if( src->is_space == false ) {
		return src->size;
	} else
	if( src->end >= src->start ) {
		return src->end - src->start;
	} else {
		return (src->end + src->size) - src->start;
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	get the free size of the queue
 */
/*---------------------------------------------------------------------*/
s32 get_queue_free_size( squeue_t *src )
{
	if( src->is_space == false )
		return 0;

	if( src->end >= src->start ) {
		return src->size - (src->end - src->start);
	} else {
		return src->size - ((src->end + src->size) - src->start);
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	element of the queue exist
 */
/*---------------------------------------------------------------------*/
bool is_exist_elements_queue( squeue_t *dst, u8 element )
{
	if( get_queue_element_size( dst ) == 0 )
		return false;

	s32 i = dst->start;
	for( ; i < dst->size; ++i ) {
		if( i >= dst->size ) i = 0;
		if( dst->buff[i] == element ) {
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	String operation
 */
/*---------------------------------------------------------------------*/
void strcpy( u8* dst, const u8* src )
{
	while( *src != 0 ){
		*dst = *src;
		++dst;
		++src;
	}
}

void strncpy( u8* dst, const u8* src, s32 num )
{
	s32 count = 0;
	while( *src != 0 && count < num ) {
		*dst = *src;
		++dst;
		++src;
		++count;
	}
}

s32 strcmp( const u8* src1, const u8* src2 )
{
	s32 ret = 0;
	while( *src1 || *src2 ) {
		ret = *src1 - *src2;
		if( ret != 0 ) return ret;
		++src1;
		++src2;
	}
	return 0;
}

u8* strchr( const u8* src, u8 code )
{
	while( *src ) {
		if( *src == code )
			return src;
		++src;
	}
	return NULL;
}

s32 strlen( const u8* src )
{
	s32 ret = 0;
	while( *src != NULL ) {
		++src;
		++ret;
	}
	return ret;
}

void sprintf( u8* dst, const u8* format, ... )
{
}

/*---------------------------------------------------------------------*/
/*!
* @brief	convert string(decimal) to integer
 */
/*---------------------------------------------------------------------*/
s32 sdtoi( const u8* src )
{
	if( src == NULL ) 
		return 0;

	s32 ret = 0;
	while( *src != NULL ) {
		if( is_number( *src ) ) {
			ret *= 10;
			ret += *src - '0';
		} else
			return ret;
		++src;
	}
	return ret;
}
/*---------------------------------------------------------------------*/
/*!
* @brief	convert string(hexadecimal) to integer
 */
/*---------------------------------------------------------------------*/
s32 shtoi( const u8* src )
{
	if( src == NULL )
		return 0;

	if( src[0] == '0' && src[1] == 'x' )
		src += 2;
	
	s32 ret = 0;
	while( *src != NULL ) {
		if( is_hexadecimal(*src) ) {
			ret *= 10;
			if( is_number(*src) )				ret += *src - '0';
			else if('A' <= *src && *src <= 'F')	ret += *src - 'A';
			else								ret += *src - 'a';
		} else
			return ret;
		++src;
	}
	return ret;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	get type of number
 * @note	prefix 0x is hexadecimal
 */
/*---------------------------------------------------------------------*/
s32 get_number_type( const u8* src )
{
	s32 ret = NUMBER_TYPE_DECIMAL;
	bool hex = false;

	if( strlen(src) >= 3 && src[0] == '0' && src[1] == 'x' ) {
		hex = true;
		ret = NUMBER_TYPE_HEXADECIMAL;
		src += 2;
	}

	while( *src != NULL ) {
		if( hex ) {
			if( is_hexadecimal(*src) )	++src;
			else						return NUMBER_TYPE_NONE;
		} else {
			if( is_decimal(*src) )	++src;
			else  					return NUMBER_TYPE_NONE;
		}
	}
	return ret;
}

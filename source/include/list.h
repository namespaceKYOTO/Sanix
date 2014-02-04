/*---------------------------------------------------------------------*/
/*!
 * @file	list.h
 * @brief	list
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _LIST_H_
#define _LIST_H_

#include "type.h"

//@@@ we can use "for_each_list"  September.25.t_sato
#define SLIST_REPEAT_FUNC( head, proc )	\
{										\
	struct slist *__top_list = head;	\
	struct slist *__now_list = head;	\
	do {								\
		proc;							\
		__now_list = __now_list->next;	\
	}while( __top_list != __now_list );	\
}
//@@@

#define for_each_list( top, val ) \
	for( (val) = (top)->next; (top) != (val) && (val) != NULL; (val) = (val)->next)

//#define for_each_list_and_top( top, val ) \
//	for( (val) = (top)->next; (top) != (val)->prev && (val) != NULL; (val) = (val)->next)
#define for_each_list_do \
	do
#define for_each_list_while( top, val ) \
	while( ((val) = (val)->next) != NULL && (top) != (val) );

//!< general purpose list
typedef struct slist {
	struct slist	*prev;
	struct slist	*next;
} slist_t;

//!< general purpose hash list
typedef struct shash_list {
	struct shash_list **prev;
	struct shash_list *next;
} shash_list_t;

//!<
static inline void slist_init( struct slist *list )
{
	list->prev = list;
	list->next = list;
}

//!< connect list
static inline void __slist_connect( struct slist *prev, struct slist *next )
{
	prev->next = next;
	next->prev = prev;
}

//!< add list
static inline void slist_add( struct slist *des, struct slist *src )
{
	// dst -> src -> dst->next
	__slist_connect( src, des->next );
	__slist_connect( des, src );
//	__slist_connect( src->next, des );
}

static inline void slist_add_tail( struct slist *head, struct slist *src )
{
	// dst->prev(tail) -> src -> dst
	__slist_connect( head->prev, src );
	__slist_connect( src, head );
}

//!< delete list
static inline void slist_delete( struct slist *list )
{
	__slist_connect( list->prev, list->next );
	list->prev = NULL;
	list->next = NULL;
}

//!< list empty?
static inline bool slist_empty( struct slist *list )
{
	return (list == list->next);
}

//!< exist list?
static inline bool slist_exist( struct slist *head, struct slist *list )
{
	SLIST_REPEAT_FUNC( head, 
		if( __now_list == list ) return true;
	)
	return false;
}



#endif // _LIST_H_

/*---------------------------------------------------------------------*/
/*!
* @file		list.c
* @brief	related generic list
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
#if 0
#include "list.h"

void slist_add( struct slist *dst, struct slist *src )
{
	struct slist *prev = dst;
	if( dst->prev != NULL ) {
		prev = dst->prev;
	}
	dst->prev = src;
	src->prev = prev;
	src->next = dst;
	prev->next = src;
}

void slist_delete( struct slist *dst, struct slist *src )
{
	if( dst->next == NULL ) {
		return;
	}
	
	struct slist *curr = dst;
	do {
		if( curr == src ) {
			if( curr->prev == curr->next ) {
				curr->prev->prev = NULL;
				curr->prev->next = NULL;
			} else {
				curr->prev->next = curr->next;
				curr->next->prev = curr->prev;
			}
			break;
		}
		curr = curr->next;
	} while( dst != curr );
}
#endif

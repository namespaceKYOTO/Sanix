/*---------------------------------------------------------------------*/
/*!
 * @file	memory.h
 * @brief	memory
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "type.h"
#include "list.h"
#include "lock.h"
#include "boot.h"



//!< 4K Byte alignment
#define MEM_ALIGNMENT_4KBYTE( addr ) (addr & 0xFFFFe000)

#define MEMORY_4KBYTE	0x00001000

//!<
#define MEM_ROUNDED_UP_MASK( mask, value ) \
	((value + (mask >> 1)) & mask)

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize memory
 */
/*---------------------------------------------------------------------*/
void init_memory( void );


/*========================================================================*/
// Momory map
/*========================================================================*/
#define MEM_MEMORY	0x00000001
#define MEM_RESERVE	0x00000002
#define MEM_USE		0x00000004

#define MEM_FLAG( falg, MEM_XXX ) (falg & MEM_XXX)
#define ON_MEM_FLAG( flag, MEM_XXX ) (flag = flag | MEM_XXX)
#define OFF_MEM_FLAG( flag, MEM_XXX ) (flag &= ~MEM_XXX)

#define MEMORY_MAP_ADDRESS	0x02000000			// provisional

//!<
typedef struct _mem_block {
	u32				flag;		//!< 1byte order  3byte flag
	u32				private;
	struct slist	list;
} mem_block_t;

#define MEM_ORDER_START_BIT (24)
//!< structure-dependent conversion
#define MEM_BLOCK( _list ) \
	( (mem_block_t*)( ((u8*)(_list+1))-sizeof(mem_block_t) ) )
//!< structure-dependent conversion
#define MEM_BLOCK_TO_ADDR( _block ) \
	((u32)((((u32)_block-(u32)mem_map->mem_head)/sizeof(mem_block_t))*0x00001000))
//!< structure-dependent conversion
#define MEM_ADDR_TO_BLOCK( _addr ) \
	((mem_block_t*)((u32)mem_map + sizeof(struct mem_map))+(_addr>>12))

//!<
struct mem_map {
	u32 				nr_mem_block;
	u32 				available_mem;
	u32 				unavailable_mem;
	struct spin_lock	slock;
	mem_block_t			*mem_head;
};

inline const struct mem_map* get_mem_map( void );
inline u32 get_available_mem_block( void );
inline u32 get_unavailable_mem_block( void );
void create_memory_map( void );


/*========================================================================*/
// Momory buddy system
/*========================================================================*/
enum {
	FREE_BLOCK_4x1K = 0,	// 1 x 4 Kbyte
	FREE_BLOCK_4x2K,
	FREE_BLOCK_4x4K,
	FREE_BLOCK_4x8K,
	FREE_BLOCK_4x16K,
	FREE_BLOCK_4x32K,
	FREE_BLOCK_4x64K,
	FREE_BLOCK_4x128K,
	FREE_BLOCK_4x256K,
	FREE_BLOCK_4x512K,
	FREE_BLOCK_4x1024K,
	FREE_BLOCK_TYPE_NUM,
};
struct mem_free_block {
	u32 nr_free_block;
	u32 *head;			// start address of the mem block
};
struct mem_buddy {
	struct mem_free_block free_block[ FREE_BLOCK_TYPE_NUM ];
};

void init_mem_buddy( void );
u32 alloc_mem_block( u32 order );
void free_mem_block( u32 addr, u32 order );
u32 get_nr_free_block( u32 order );
u32 get_addr_free_block( u32 order );


/*========================================================================*/
// Momory cache? :: provisional
// 4K byte 未満sizeのメモリを管理
// 幾何学的sizeで割り当て
// general purpose geometric memory
/*========================================================================*/
//!<
#define SLAB_OBJ_LIST_END_SIGNATURE 0xffff

//!<
/* structure-dependent conversion */
#define LIST_TO_SLAB( list ) \
	((struct slab*)list)

//!<
#define request_for_each_slab( head, proc )	\
{											\
	struct slab *__slab = NULL;				\
	SLIST_REPEAT_FUNC( head, 				\
		__slab = LIST_TO_SLAB(__now_list );	\
		proc )								\
}

//!<
struct slab {
	struct slist	list;
	u32				nr_use_obj;		//!< number of objects in use
	u32				top_obj_off;	//!< offset of top object
	u32				top_obj_addr;	//!< actual address of top object
	u32				free;			//!< index
};
//!<
struct slab_lists {
	struct slist		lsit_full;		//!< full used memory list
	struct slist		lsit_partial;	//!< partial used memory list
	struct slist		lsit_free;		//!< unused list
	u32					nr_free_obj;	//!< number of free objects
};
//!<
struct slab_cache {
	u32					size;			//!< the total slab size
	struct spin_lock	splock;			//!< cache spin lock
	struct slist		cache_list;		//!< slab cache list
	
	u32					obj_size;		//!< object size
	u32					nr_obj;			//!< the total number of objects
	
	u32					nr_need_block;	//!< order of the number of blocks required
	
	struct slab_lists	slab_lists;		//!< slab lists
};
//!<
typedef u16 slab_obj_list_t;

void init_gp_slab_mem( void );
void* alloc_gp_slab_mem( u32 size );
void free_gp_slab_mem( void *addr, u32 size );

/*========================================================================*/
// Momory heap :: provisional
// future...task単位でヒープの管理を
/*========================================================================*/
#define SMALLOC_SIGNATURE		0xFFEEDD00
#define SMALLOC_SIGNATURE_MASK	0xFFFFFF00
#define SMALLOC_UNUSED			(0x00 | SMALLOC_SIGNATURE)
#define SMALLOC_USE				(0x01 | SMALLOC_SIGNATURE)

typedef struct _alloc_head {
	u32					state;
	u32					size;
	struct _alloc_head	*prev;
	struct _alloc_head	*next;
} alloc_head_t;

typedef struct {
	u32				heap_size;
	alloc_head_t	*begin;
	alloc_head_t	*end;
} heap_allocator_t;

//void init_smalloc( void );
void init_heap_allocator( heap_allocator_t *allocater, u8 *heap, u32 size );
void* smalloc( heap_allocator_t *allocater, u32 size );
void sfree( heap_allocator_t *allocater, void* src );
void smemset( u8* dst, u8 code, u32 size );

void memory_put( void );

#endif // _MEMORY_H_

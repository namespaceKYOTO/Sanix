/*---------------------------------------------------------------------*/
/*!
 * @file	memory.c
 * @brief	memory management
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#include "define.h"
#include "graphic.h"
#include "debug.h"
#include "console.h"
#include "util.h"
#include "memory.h"


static struct mem_map	*mem_map;
struct mem_buddy		mem_buddy;
struct slab_cache		gp_slab_cache[8];
static u32 _slab_obj_size[ ] = { 32,64,128,256,512,1024,2048,4096, };
static u32 _slab_need_block[ ] = { 
	// provisional
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x1K,
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x1K,
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x1K,
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x1K,
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x1K,
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x1K,
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x2K,
	FREE_BLOCK_4x1024K,//FREE_BLOCK_4x2K,
};

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize memory managerment
 */
/*---------------------------------------------------------------------*/
void init_memory( void )
{
	// create memory map
	create_memory_map();
	puts("create memory map", 0, 0);
	
	// buddy system
	smemset( (u8*)&mem_buddy, 0x00, sizeof(struct mem_buddy) );
	init_mem_buddy();
	puts("initialize memory buddy system", 0, 0);
	
	// slab system
	init_gp_slab_mem();
	puts("initialize gp slab memory", 0, 0);
}


/*========================================================================*/
// Momory map
/*========================================================================*/
const struct mem_map* get_mem_map( void )
{
	return mem_map;
}
u32 get_available_mem_block( void )
{
	return (mem_map->available_mem>>12);
}
u32 get_unavailable_mem_block( void )
{
	return (mem_map->unavailable_mem>>12);
}

u32 __get_mem_state( u32 addr )
{
#if 1
	// skernel used
	if( addr <= MEM_ALIGNMENT_4KBYTE(0x03001000) ) {
		return MEM_RESERVE;
	}else
#endif
	if( g_boot_info->e820map.num >= 0 ) {
		s32 i = 0;
		for( ; i < g_boot_info->e820map.num; ++i ) {
			if( g_boot_info->e820map.entry[i].addr_low <= addr && addr < (g_boot_info->e820map.entry[i].addr_low + g_boot_info->e820map.entry[i].length_low) ) {
				return g_boot_info->e820map.entry[i].type;
			}
		}
		return 0x00000002;
	} else {
		// sample memory map
//		mem_entry[0].type = MEM_TYPE_MEMORY;
//		mem_entry[0].addr_low = 0x00000000;
//		mem_entry[0].length_low = 639 * 1024;	// 639K
//		mem_entry[1].type = MEM_TYPE_RESERVE;
//		mem_entry[1].addr_low = 0x0009FC00;
//		mem_entry[1].length_low = 1024;			// 1K
//		mem_entry[2].type = MEM_TYPE_RESERVE;
//		mem_entry[2].addr_low = 0x000F0000;
//		mem_entry[2].length_low = 64 * 1024;	// 64K
//		mem_entry[3].type = MEM_TYPE_MEMORY;
//		mem_entry[3].addr_low = 0x00100000;
//		mem_entry[3].length_low = 0x00700000;	// 7M
//		mem_entry[4].type = MEM_TYPE_RESERVE;
//		mem_entry[4].addr_low = 0x00800000;
//		mem_entry[4].length_low = 0x00400000;	// 4M
//		mem_entry[5].type = MEM_TYPE_MEMORY;
//		mem_entry[5].addr_low = 0x01000000;
//		mem_entry[5].length_low = 0x08000000;	// 128M
//		mem_entry[6].type = MEM_TYPE_RESERVE;
//		mem_entry[6].addr_low = 0xFEC00000;
//		mem_entry[6].length_low = 4 * 1024;		// 4K
//		mem_entry[7].type = MEM_TYPE_RESERVE;
//		mem_entry[7].addr_low = 0xFEE00000;
//		mem_entry[7].length_low = 4 * 1024;		// 4K
//		mem_entry[8].type = MEM_TYPE_RESERVE;
//		mem_entry[8].addr_low = 0xFFFF0000;
//		mem_entry[8].length_low = 64 * 1024;	// 64K
		
		// provisional
		if(	MEM_ALIGNMENT_4KBYTE(0x0009FC00) <= addr && addr <= MEM_ALIGNMENT_4KBYTE(0x000a0000)
		||	MEM_ALIGNMENT_4KBYTE(0x000F0000) <= addr && addr <= MEM_ALIGNMENT_4KBYTE(0x00100000)
		){
			return MEM_RESERVE;
		}else
		if( MEM_ALIGNMENT_4KBYTE(0x00800000) <= addr && addr <= MEM_ALIGNMENT_4KBYTE(0x00c00000) ) {
			return MEM_RESERVE;
		}else
		if(	MEM_ALIGNMENT_4KBYTE(0xFEC00000) <= addr && addr <= MEM_ALIGNMENT_4KBYTE(0xFEC01000)
		||	MEM_ALIGNMENT_4KBYTE(0xFEE00000) <= addr && addr <= MEM_ALIGNMENT_4KBYTE(0xFEE01000)
		||	MEM_ALIGNMENT_4KBYTE(0xFFFF0000) <= addr && addr <= MEM_ALIGNMENT_4KBYTE(0xFFFFFFFF)
		){
			return MEM_RESERVE;
		} else {
			return MEM_MEMORY;
		}
	}
}

void create_memory_map( void )
{
	mem_map = (struct mem_map*)MEMORY_MAP_ADDRESS;
//	mem_map->mem_desc.flag;
//	mem_map->mem_desc.private;
	mem_map->available_mem = 0x00;
	mem_map->unavailable_mem = 0x00;
	mem_map->slock.lock = 0;
	mem_map->mem_head = (struct mem_block*)((u32)mem_map + sizeof(struct mem_map));
	slist_init( &mem_map->mem_head->list );
	
	struct mem_block *prev_block = mem_map->mem_head;
	struct mem_block *now_block = mem_map->mem_head; //(struct mem_block*)((u32)mem_map + sizeof(struct mem_map));
//	struct mem_block *now_block = mem_map->mem_desc;

	//
	u32 count = 1024 * 1023;
	u32 i;
	u32 addr = 0;
#if 1
//	struct e820_map *e820map = g_boot_info->e820map;
	putn( g_boot_info->e820map.num, DEBUG_FONT_SIZE_X*10, DEBUG_FONT_SIZE_Y*2 );

	puts("addr_low", DEBUG_FONT_SIZE_X*0, DEBUG_FONT_SIZE_Y*3 );
	putn( g_boot_info->e820map.entry[0].addr_low, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*3 );
	
	puts("addr_high", DEBUG_FONT_SIZE_X*0, DEBUG_FONT_SIZE_Y*4 );
	putn( g_boot_info->e820map.entry[0].addr_high, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*4 );
	
	puts("lengt low", DEBUG_FONT_SIZE_X*0, DEBUG_FONT_SIZE_Y*5 );
	putn( g_boot_info->e820map.entry[0].length_low, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*5 );

	puts("length high", DEBUG_FONT_SIZE_X*0, DEBUG_FONT_SIZE_Y*6 );
	putn( g_boot_info->e820map.entry[0].length_high, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*6 );

	puts("type", DEBUG_FONT_SIZE_X*0, DEBUG_FONT_SIZE_Y*7 );
	putn( g_boot_info->e820map.entry[0].type, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*7 );

	puts("extention", DEBUG_FONT_SIZE_X*0, DEBUG_FONT_SIZE_Y*8 );
	putn( g_boot_info->e820map.entry[0].extention, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*8 );
#endif
	for( i = 0; i < count; ++i ) {
		// Analyze memory
		//now_block->now_block
		now_block->flag = __get_mem_state( addr );	// i * 4K;
		now_block->private = 0x00;
		
		switch( now_block->flag )
		{
			case MEM_MEMORY:
				mem_map->available_mem += MEMORY_4KBYTE;
				break;
			
			case MEM_RESERVE:
			case MEM_USE:
				mem_map->unavailable_mem += MEMORY_4KBYTE;
				break;
			
			default:
				S_ASSERT(0, "Analyze memory error");
				break;
		}
		
		slist_add( &prev_block->list, &now_block->list );
		prev_block = now_block;
		++now_block;
		slist_init( &now_block->list );
		addr += MEMORY_4KBYTE;
	}
	mem_map->nr_mem_block = i;
}


/*========================================================================*/
// Momory buddy system
/*========================================================================*/
void init_mem_buddy( void )
{
	u32 order = 0;
	u32 size = 0;
	u32 addr = 0;
	u32 fb_addr[FREE_BLOCK_TYPE_NUM];
	smemset( (u8*)fb_addr, 0x00, sizeof(fb_addr));

	/* analyze memory map */
	slist_t* _list = &mem_map->mem_head->list;
	for_each_list_do
	{
		if( MEM_FLAG( MEM_BLOCK( _list )->flag, MEM_MEMORY) ) {
			size += MEMORY_4KBYTE;
			if( (MEMORY_4KBYTE << order) < size ){
				++order;
				addr = (u32)MEM_BLOCK( _list );
			}
			if( order > FREE_BLOCK_4x1024K ) {
				if( fb_addr[order-1] == 0x00 ) {
					fb_addr[order-1] = addr;
					mem_buddy.free_block[order-1].head = (u32*)addr;
				} else {
					((struct mem_block*)fb_addr[order-1])->flag |= ((order-1)<<24);
					((struct mem_block*)fb_addr[order-1])->private = addr;
					fb_addr[order-1] = addr;
				}
				++mem_buddy.free_block[order-1].nr_free_block;
				order = 0;
				size = 0;
			}
		} else
		if(size != 0) {
			if( fb_addr[order-1] == 0x00 ) {
				fb_addr[order-1] = addr;
				mem_buddy.free_block[order-1].head = (u32*)addr;
			} else {
				((struct mem_block*)fb_addr[order-1])->flag |= ((order-1)<<24);
				((struct mem_block*)fb_addr[order-1])->private = addr;
				fb_addr[order-1] = addr;
			}
			++mem_buddy.free_block[order-1].nr_free_block;
			u32 remainder = size - (MEMORY_4KBYTE << (order-1));
			s32 nr = order-1;
			addr += sizeof(struct mem_block) * (0x01 << (order-1));
			for( ; nr >= 0; --nr ) {
			//while( remainder ){
				if(remainder >= (MEMORY_4KBYTE << nr)) {
					++mem_buddy.free_block[nr].nr_free_block;
					remainder -= (MEMORY_4KBYTE << nr);
					if( fb_addr[nr] == 0x00 ) {
						fb_addr[nr] = addr;
						mem_buddy.free_block[nr].head = (u32*)addr;
					} else {
						((struct mem_block*)fb_addr[nr])->flag |= (nr<<24);
						((struct mem_block*)fb_addr[nr])->private = addr;
						fb_addr[nr] = addr;
					}
					++mem_buddy.free_block[nr].nr_free_block;
					addr += sizeof(struct mem_block) * (0x01 << (nr));
				} /*else {
					--nr;
				}*/
			}
			//++mem_buddy.free_block[order-1].nr_free_block;
			order = 0;
			size = 0;
		}
	}
	for_each_list_while( &mem_map->mem_head->list, _list )
}

//!< split buddy blocks
void _split_buddy_block( s32 split_order, s32 order )
{
	u32 split_front = 0;
	u32 split_back = 0;
	while( split_order > order ) {
		S_ASSERT( mem_buddy.free_block[split_order].nr_free_block > 0, "buddy split error");
		split_front = (u32)mem_buddy.free_block[split_order].head;
		split_back = split_front + sizeof(struct mem_block) * (0x01 << (split_order-1));
		if( split_front == 0 || split_back == 0) {
			puts("split_front", 8*0, 16*1 );
			puth( split_front, 8*21, 16*1 );
			puts("split_back", 8*0, 16*2 );
			puth( split_back, 8*21, 16*2 );
			S_ASSERT( split_front > 0, "buddy split front error");
			S_ASSERT( split_back > 0, "buddy split back error");
		}

		mem_buddy.free_block[split_order].head = (u32*)((struct mem_block*)split_front)->private;
		--mem_buddy.free_block[split_order].nr_free_block;
#if 0	// free_block数が1の場合は 0 になることもあり得る
		if( mem_buddy.free_block[split_order].head == 0 ) {
			puts("split_order", 8*0, 16*1 );
			puth( split_order, 8*21, 16*1 );
			puts("split_front", 8*0, 16*2 );
			puth( split_front, 8*21, 16*2 );
			S_ASSERT( 0, "buddy split head error");
		}
#endif
		
		((struct mem_block*)split_front)->private = split_back;
		((struct mem_block*)split_back)->private = (u32)mem_buddy.free_block[split_order - 1].head;
		mem_buddy.free_block[split_order - 1].head = (u32*)split_front;
		mem_buddy.free_block[split_order - 1].nr_free_block += 2;
		
		--split_order;
	}
}
//!< integration buddy block
u32 _integration_buddy_block( u32 addr, u32 order )
{
	/* 隣接する */
	u32 prev_addr = addr - (0x00001000 << order);
	u32 next_addr = addr + (0x00001000 << order);
	struct mem_block *prev = MEM_ADDR_TO_BLOCK( prev_addr );
	struct mem_block *next = MEM_ADDR_TO_BLOCK( next_addr );
	struct mem_block *current = MEM_ADDR_TO_BLOCK( addr );
	u32 ret = 0;
	
	if( ((prev->flag & 0xFF000000) >> 24) == order && prev->private != 0 ) {
		// prevと統合
		current->flag &= 0x00FFFFFF;
		current->private = 0;
		prev->flag += 0x01000000;
		ret = prev_addr;
	}else
	if( ((next->flag & 0xFF000000) >> 24) == order && next->private != 0 ) {
		// nextと統合
		next->flag &= 0x00FFFFFF;
		next->private = 0;
		current->flag += 0x01000000;
		ret = (u32)addr;
	}
	
	return ret;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	alloc memory block
 * @return	allocated memory block
 * @retval	null : failed to allocate memory block
 * @retval	other : successful
 */
/*---------------------------------------------------------------------*/
u32 alloc_mem_block( u32 order )
{
	struct mem_block *mem = NULL;
	struct mem_block *block = NULL;
	
	spin_lock( (&mem_map->slock) );
	
	if( mem_buddy.free_block[order].nr_free_block ) {
		--mem_buddy.free_block[order].nr_free_block;
		mem = (struct mem_block*)mem_buddy.free_block[order].head;
		mem_buddy.free_block[order].head =
			(u32*)((struct mem_block*)mem_buddy.free_block[order].head)->private;
	} else {
		S_ASSERT(order < FREE_BLOCK_4x1024K, "order size error");
		// ないので,大きい所から分けてもらう...
		s32 split_order = order + 1;
		for( ; split_order < FREE_BLOCK_TYPE_NUM; ++split_order ) {
			if( mem_buddy.free_block[split_order].nr_free_block ) {
				_split_buddy_block( split_order, order );
				--mem_buddy.free_block[order].nr_free_block;
				mem = (struct mem_block*)mem_buddy.free_block[order].head;
				mem_buddy.free_block[order].head =
					(u32*)((struct mem_block*)mem_buddy.free_block[order].head)->private;
				break;
			}
		}
	}
	
	spin_unlock( (&mem_map->slock) );
	
	if( mem ) {
		mem_map->available_mem -= (0x00001000<<order);
		mem_map->unavailable_mem += (0x00001000<<order);
		ON_MEM_FLAG( mem->flag, MEM_USE );
		return MEM_BLOCK_TO_ADDR( mem );
	}
	return NULL;
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	free memory block
 */
/*---------------------------------------------------------------------*/
void free_mem_block( u32 addr, u32 order )
{
	spin_lock( (&mem_map->slock) );
	
	struct mem_block *block = MEM_ADDR_TO_BLOCK( addr );
	if( MEM_FLAG(block->flag, MEM_USE) ) {
		OFF_MEM_FLAG(block->flag, MEM_USE);
		mem_map->available_mem += (0x00001000 << order);
		mem_map->unavailable_mem -= (0x00001000 << order);
		
		u32 ret = 0;
		u32 i = order;
		bool is_integration;
		for( ; i < FREE_BLOCK_TYPE_NUM; ++i ) {
			ret = _integration_buddy_block( addr, i );
			if( ret == 0 ) {
				break;
			}
			addr = ret;
			mem_buddy.free_block[i].nr_free_block -= 2;
			++mem_buddy.free_block[i+1].nr_free_block;
			MEM_ADDR_TO_BLOCK( addr )->private = (u32)mem_buddy.free_block[i+1].head;
			mem_buddy.free_block[i+1].head = (u32*)MEM_ADDR_TO_BLOCK( addr );
			is_integration = true;
		}
		
		if( is_integration == false ) {
			++mem_buddy.free_block[order].nr_free_block;
			block->private = (u32)mem_buddy.free_block[order].head;
			mem_buddy.free_block[order].head = (u32*)block;
		}
	}
	
	spin_unlock( (&mem_map->slock) );
}

u32 get_nr_free_block( u32 order )
{
	struct mem_block *block = (struct mem_block*)mem_buddy.free_block[order].head;
	u32 nr = 0;
	while( block ) {
		block = (struct mem_block*)block->private;
		++nr;
	}
	return nr;
}
u32 get_addr_free_block( u32 order )
{
	return (u32)mem_buddy.free_block[order].head;
}


/*========================================================================*/
// Momory cache? :: provisional
// 4K byte 未満sizeのメモリを管理
// 幾何学的sizeで割り当て
// general purpose geometric memory
/*========================================================================*/

//!< 
inline u32 __size_to_index( u32 size )
{
	     if( size <= _slab_obj_size[0] ) return 0;
	else if( size <= _slab_obj_size[1] ) return 1;
	else if( size <= _slab_obj_size[2] ) return 2;
	else if( size <= _slab_obj_size[3] ) return 3;
	else if( size <= _slab_obj_size[4] ) return 4;
	else if( size <= _slab_obj_size[5] ) return 5;
	else if( size <= _slab_obj_size[6] ) return 6;
	else if( size <= _slab_obj_size[7] ) return 7;
}
//!<
inline u32 __index_to_size( u32 index )
{
	switch(index) {
		case 0: return   32;
		case 1: return   64;
		case 2: return  128;
		case 3: return  256;
		case 4: return  512;
		case 5: return 1024;
		case 6: return 2048;
		case 7: return 4096;
	}
}
//!< slab allocation
void __alloc_slab( u32 block_mem, u32 size, u32 order )
{
	u32 idx = __size_to_index( size );
	struct slab *_slab;
	u32 nr_obj = 0;
	if( size <= 512 ) {
		// inside slab cache
		nr_obj = (0x00001000 << order) / (_slab_obj_size[idx] + sizeof(slab_obj_list_t));
		_slab = (struct slab*)block_mem;
		_slab->top_obj_off = 0;
		_slab->top_obj_addr = block_mem + sizeof(struct slab) + (sizeof(slab_obj_list_t) * nr_obj);
	} else {
		// outside slab cahce
		nr_obj = (0x00001000 << order) / _slab_obj_size[idx];
		_slab = (struct slab*)alloc_gp_slab_mem( sizeof(struct slab) );
		_slab->top_obj_off = 0;
		_slab->top_obj_addr = block_mem;
	}

	slist_init( &_slab->list );
	_slab->nr_use_obj = 0;
	_slab->free = 0;

	// initialize slab object list
	slab_obj_list_t *sobj;
	sobj = (slab_obj_list_t*)(_slab + 1);
	u32 i = 0;
	for( ; i < nr_obj; ++i ) {
		sobj[i] = i + 1;
	}
	sobj[nr_obj - 1] = SLAB_OBJ_LIST_END_SIGNATURE;

//	gp_slab_cache[idx].nr_obj += nr_obj;
	gp_slab_cache[idx].slab_lists.nr_free_obj += nr_obj;
	slist_add( &gp_slab_cache[idx].slab_lists.lsit_free, &_slab->list );	// add to free list

	S_ASSERT( gp_slab_cache[idx].slab_lists.lsit_free.next == &_slab->list, "failed list add");

//	return _slab;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize general purpose slab memory
 */
/*---------------------------------------------------------------------*/
void init_gp_slab_mem( void )
{
	u8 i = 0;
	u32 size = S_ARRAY_SIZE( _slab_obj_size );
	for( ; i < size; ++i ) {
		gp_slab_cache[i].size = 0;
		gp_slab_cache[i].splock.lock = 0;
		slist_init( &gp_slab_cache[i].cache_list );
		gp_slab_cache[i].obj_size = _slab_obj_size[i];
		gp_slab_cache[i].nr_obj = 0;
		gp_slab_cache[i].nr_need_block = _slab_need_block[i];
		slist_init( &gp_slab_cache[i].slab_lists.lsit_full );
		slist_init( &gp_slab_cache[i].slab_lists.lsit_partial );
		slist_init( &gp_slab_cache[i].slab_lists.lsit_free );
		gp_slab_cache[i].slab_lists.nr_free_obj = 0;
	}
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	allocate from general purpose slab memory
 * @return	allocated memory
 * @retval	null : failed
 * @retval	other : successful
 */
/*---------------------------------------------------------------------*/
void* alloc_gp_slab_mem( u32 size )
{
	u32 idx = __size_to_index( size );
	
	spin_lock( (&gp_slab_cache[idx].splock) );
	
	if( gp_slab_cache[idx].slab_lists.nr_free_obj == 0 ) {
		__puts("slab free object zero");
		__putcr
		u32 mem_block = alloc_mem_block( gp_slab_cache[idx].nr_need_block );	// get block
		if( mem_block == 0 ) return NULL;
		__alloc_slab( mem_block, size, gp_slab_cache[idx].nr_need_block );
#if 0		// memory block descriptorにslab addressを持たせたい  March.11.2013 t_sato
		s32 i = 0;
		for( ; i < (1<<gp_slab_cache[idx].slab_lists.nr_free_obj); ++i, mem_block += 0x00001000 ) {
			MEM_ADDR_TO_BLOCK( mem_block )->
		}
#endif
	}
	
	// allocate
	void *_mem = NULL;
	struct slab *_slab = NULL;
	
	--gp_slab_cache[idx].slab_lists.nr_free_obj;
	
	slab_obj_list_t *_top_obj_list = NULL;//(slab_obj_list_t*)(_slab + 1);
	if( !slist_empty( &gp_slab_cache[idx].slab_lists.lsit_partial ) ) {
		//!< partial list
		_slab = LIST_TO_SLAB( gp_slab_cache[idx].slab_lists.lsit_partial.next );
		_top_obj_list = (slab_obj_list_t*)(_slab + 1);
		if( _top_obj_list[_slab->free] == SLAB_OBJ_LIST_END_SIGNATURE ) {
			slist_delete( &_slab->list );
			slist_add( &gp_slab_cache[idx].slab_lists.lsit_full, &_slab->list );
		}
		__puts("use partial list\n");
	} else {
		//!< free list
		_slab = LIST_TO_SLAB( gp_slab_cache[idx].slab_lists.lsit_free.next );
		_top_obj_list = (slab_obj_list_t*)(_slab + 1);
		slist_delete( &_slab->list );
		slist_add( &gp_slab_cache[idx].slab_lists.lsit_partial, &_slab->list );
		__puts("use free list\n");
	}
	++_slab->nr_use_obj;
	__puts("slab address    ");
	__putn((u32)_slab)
	__putcr
//	_mem = (void*)(_slab->top_obj_addr + _slab->free * (sizeof(slab_obj_list_t)*idx));
	_mem = (void*)(_slab->top_obj_addr + _slab->free * _slab_obj_size[idx]);

	// used to release
	MEM_ADDR_TO_BLOCK( MEM_ALIGNMENT_4KBYTE( ((u32)_mem) ) )->private = (u32)_slab;

	_slab->free = _top_obj_list[_slab->free];

	spin_unlock( (&gp_slab_cache[idx].splock) );
	
//	S_ASSERT( 0, "failed search list");
	return _mem;
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	free the allocated memory
 */
/*---------------------------------------------------------------------*/
void free_gp_slab_mem( void *addr, u32 size )
{
	u32 idx = __size_to_index( size );
	
	struct slab *_slab = (struct slab*)MEM_ADDR_TO_BLOCK( MEM_ALIGNMENT_4KBYTE(((u32)addr)))->private;
#if 1
	__puts("release slab addr    ");
	__putn(_slab);
	__putcr
#endif
	slab_obj_list_t *sobj = (slab_obj_list_t *)(_slab + 1);
	u32 release_idx = (((u32)addr) - _slab->top_obj_addr) / size;
	
	--_slab->nr_use_obj;
	sobj[release_idx] = _slab->free;
	_slab->free = sobj[release_idx];
	
	bool ret = slist_exist( &gp_slab_cache[idx].slab_lists.lsit_full, &_slab->list );
	if( ret ) {
		slist_delete( &_slab->list );
		slist_add( &gp_slab_cache[idx].slab_lists.lsit_partial, &_slab->list );
	}
	if( _slab->nr_use_obj == 0 ) {
		slist_delete( &_slab->list );
		slist_add( &gp_slab_cache[idx].slab_lists.lsit_free, &_slab->list );
	}
	++gp_slab_cache[idx].slab_lists.nr_free_obj;
}


/*========================================================================*/
// Momory heap :: provisional
// future...task単位でヒープの管理を
/*========================================================================*/
/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize heap allocater
 */
/*---------------------------------------------------------------------*/
void init_heap_allocator( heap_allocator_t *allocater, u8 *heap, u32 size )
{
	allocater->heap_size = size;
	allocater->begin = (alloc_head_t *)heap;
	allocater->end = (alloc_head_t *)(heap + size);
	
	allocater->begin->state = SMALLOC_UNUSED;
	allocater->begin->size = 0;
	allocater->begin->prev = 0;
	allocater->begin->next = allocater->end;
	
	allocater->end->state = SMALLOC_UNUSED;
	allocater->end->size = 0;
	allocater->end->prev = allocater->begin;
	allocater->end->next = 0;
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	memory allocate
 */
/*---------------------------------------------------------------------*/
void* smalloc( heap_allocator_t *allocator, u32 size )
{
	alloc_head_t* now_head = allocator->begin;
	alloc_head_t* prev_head = 0;
	alloc_head_t* next_head = 0;
	alloc_head_t* split_point_head = 0;

	while( now_head != allocator->end/*s_smalloc_end_head*/ ) {
		if( now_head->state == SMALLOC_UNUSED
		&&	(u32)now_head->next > (u32)now_head + size + sizeof(alloc_head_t)
		){
			// allocate
			now_head->state = SMALLOC_USE;
			now_head->size = size;
			prev_head = now_head;
			next_head = now_head->next;
			split_point_head = (alloc_head_t*)((u32)now_head + size + sizeof(alloc_head_t));
			
			// split
			if( (u32)next_head > (u32)split_point_head + sizeof(alloc_head_t) ) {
				prev_head->next = split_point_head;
				next_head->prev = split_point_head;
				
				split_point_head->state = SMALLOC_UNUSED;
				split_point_head->size = 0;
				split_point_head->prev = prev_head;
				split_point_head->next = next_head;
			}
			return (void*)((u32)now_head + sizeof(alloc_head_t));
		}
		now_head = now_head->next;
	}
	return NULL;	// failed
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	memory free
 */
/*---------------------------------------------------------------------*/
void sfree( heap_allocator_t *allocator, void* src )
{
	alloc_head_t* now_head = (alloc_head_t*)((u32)src - sizeof(alloc_head_t));
	alloc_head_t* prev_head = now_head->prev;
	alloc_head_t* next_head = now_head->next;
	
	puth( allocator->begin, 16*12, 16*7 );
	puth( now_head, 16*12, 16*8 );
	S_ASSERT( now_head == allocator->begin, "ERROR SFREE1" );
	
	now_head->state = SMALLOC_UNUSED;
	now_head->size = 0;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	memory set
 */
/*---------------------------------------------------------------------*/
void smemset( u8* dst, u8 code, u32 size )
{
	s32 i = 0;
	for( ; i < size; ++i ) {
		*(dst+i) = code;
	}
}

//!< provisional
void memory_put( void )
{
	u32 use_mem = 0;
	u32 count = 0;
#if defined(__SMALLOC_HEAP__)
	alloc_head *head = s_smalloc_begin_head;
	
	S_ASSERT( s_smalloc_end_head->size == 0, "ERROR MEMORY PUT1" );
	S_ASSERT( s_smalloc_end_head->next == 0, "ERROR MEMORY PUT1" );
	while( head != s_smalloc_end_head ) {
		if( head->state == SMALLOC_USE ) {
			++count;
			use_mem += head->size;
		}
		head = head->next;
		S_ASSERT( head != NULL, "ERROR MEMORY PUT2" );
	}
#endif //__SMALLOC_HEAP__
	
	puts("USE BLOCK", 0, 16*9 );
	putn( count, 16*15, 16*9 );
	
	puts("USE", 0, 16*10 );
	putn( use_mem, 16*12, 16*10 );
}


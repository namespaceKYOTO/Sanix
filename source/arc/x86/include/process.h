/*---------------------------------------------------------------------*/
/*!
 * @file	process.h
 * @brief	process
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "list.h"
#include "util.h"
#include "memory.h"
#include "debug.h"


#define PROC_STATE_RUNNING	0x00000001
#define PROC_STATE_IDLE		0x00000002
#define PROC_STATE_EXIT		0x00000004

#define CLEATE_PROC_KERNEL	0x00000001
#define CLEATE_PROC_USER	0x00000002

//!< get pid hash (0-1024)
#define pid_hash( val ) hash_u32( val, 10 )

register u32 stack_pointer asm("esp") __attribute__((__used__));

//<! structure-dependent conversion
#define STATE_LIST_TO_PROC_STRUCT( list ) \
	((proc_struct_t*)((u32*)list - 1))

#define PROC_LIST_TO_PROC_STRUCT( list ) \
	((proc_struct_t*)((u32*)list - 3))

//! context は hcontextのアドレスを指していなければならない
#define CONTEXT_TO_PROC_STRUCT( context )\
	((proc_struct_t*)((u8*)context - (sizeof(u32) + sizeof(struct slist) *2)))


//!< structure-dependent conversion
#define PROC_LIST_TO_PID( _list ) \
	((pid_t*)((u32*)_list -1))

#define PID_CHAIN_LIST_TO_PID_T( _list ) \
	((pid_t*)((u32*)_list - 1))

#define PID_HEAD_LIST_TO_PID_T( _list ) \
	((pid_t*)((u32*)_list - 2))

//!<
#define switch_proc( _prev, _next )							\
do {														\
	u32 ebx, ecx, edx, esi, edi;							\
	__asm__ __volatile__(									\
		"pushfl\n\t"					/* eflag save */	\
		"pushl %%ebp\n\t"				/* ebp save */		\
		"movl $1f,%[prev_eip]\n\t"		/* eip save */		\
		"pushl %[prev_eip]\n\t"			/* eip push */		\
		"movl %%esp,%[prev_esp]\n\t"	/* esp save */		\
		"movl %[next_esp],%%esp\n\t"	/* esp restore */	\
		"pushl %[next]\n\t"				/* arg 2 */			\
		"pushl %[prev]\n\t"				/* arg 1 */			\
		/*"pushl %[next_eip]\n\t"			 eip restore */	\
		/*"jmp ___switch_to\n\t"		*/					\
		"call ___switch_to\n\t"								\
		"popl %%eax\n\t"				/* arg1 */			\
		"popl %%eax\n\t"				/* arg2 */			\
		"ret\n\t"						/* return 1f */		\
		"1:\t"												\
		"popl %%ebp\n\t"	/* ebp restore */				\
		"popfl\n\t"			/* eflag restore */				\
		/* output operand */								\
		:[prev_esp] "=m" (_prev->context.esp),				\
		 [prev_eip] "=m" (_prev->context.eip),				\
															\
		/* register */										\
		"=b" (ebx), "=c" (ecx), "=d" (edx),					\
		"=S" (esi), "=D" (edi)								\
															\
		/* input operand */									\
		:[next_esp] "m" (_next->context.esp),				\
		 [next_eip] "m" (_next->context.eip),				\
		 /* regparm parameters for __switch_to */			\
		 [prev] "a" (_prev),								\
		 [next] "d" (_next)									\
		/* work register */									\
		: "memory");										\
} while(0)

typedef void (*handler_t)(void);
typedef void (*main_t)( void );


//typedef struct pid_t;
//typedef struct proc_context_t;
//typedef struct proct_struct_t;

//!< hardware context
struct hard_context {
	u32 esp;
	u32 eip;
	u16 ds;
	u16 es;
	u16 fs;
	u16 gs;
	
	// fpu
};

struct proc_struct;

//!< process id
typedef struct {
	u32					pid;
	struct slist		chain_list;
	struct slist		pid_head;
	struct proc_struct	*pstr;
} pid_t;

//!< process infomation
typedef struct {
	u8						stack[BYTE_4K - (sizeof(struct  hard_context))];
	struct  hard_context	context;		//!< hardwear context
} proc_context_t;

//!< process
typedef struct proc_struct {
	u32					state;			//!< run,wait,etc... 
	struct slist		state_list;		//!< state(run,wait,etc...) list
	struct slist		proc_list;		//!< process all chain list
	proc_context_t		*hcontext;		//!< process info
	heap_allocator_t	heap_allocator;	//!< process memory allocater
	pid_t				*pid_chain;		//!< process id chain list

#if defined __SDEBUG__
	u8					proc_name[128];//!< process name
#endif	//__SDEBUG__
	
//	signal;
//	file system;
	
} proc_struct_t;

//!< process descriptor table
struct proc_desc_table {
	pid_t			pid_list[1];	//!< process id lists
	struct slist	run_list;		//!< process run list
	struct slist	wait_list;		//!< process wait list
	struct slist	all_list;		//!< process all list
	proc_struct_t	*current_proc;	//!< current process
};

//!< (new) pid bit map
struct pid_bit_map {
	u32		size;
	u8		*map;
};


void start_multi_proc( void );
void schedule( void );

void schedule_exception( void );

void init_process( void );

#if defined __SDEBUG__
#define create_process( func, flag )	\
	_create_process( func, flag , #func );
#else
#define create_process( func, flag )	\
	_create_process( func, flag );
#endif // __SDEBUG__		\

proc_struct_t* _create_process( main_t func, u32 proc_flag
#if defined __SDEBUG__
, const char* proc_name
#endif // __SDEBUG__
);

void delete_process_by_proc_struct( proc_struct_t* proc );
void delete_process_by_pid( s32 pid );

//static inline proc_context_t* get_current_process()
//{
//	return (proc_context_t*)((stack_pointer & 0xFFFFE000));
//}

static inline void hard_context_stack_push( struct hard_context* p, s32 val )
{
	p->esp -= 4;
	*((u32*)p->esp) = val;
}
static inline u32 hard_context_stack_pop( struct hard_context* p )
{
	u32 ret = *((u32*)p->esp);
	p->esp += 4;
	return ret;
}

inline void set_enable_sched( bool b );
inline bool get_enable_sched();

proc_struct_t* __switch_to( proc_struct_t *prev, proc_struct_t *next ) ;//__attribute__((regparm(3)));
//void __switch_to( proc_struct_t *prev, proc_struct_t *next ) ;//__attribute__((regparm(3)))

proc_struct_t *get_proc_strcut_by_pid( s32 pid );

inline proc_struct_t *get_current_process( void );

#if defined __SDEBUG__
void debug_print_proc_all( void );
#endif // __SDEBUG__


#endif // _PROCESS_H_

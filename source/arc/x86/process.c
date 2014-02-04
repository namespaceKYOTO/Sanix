/*---------------------------------------------------------------------*/
/*!
 * @file	process.h
 * @brief	process
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/

/*=====================================================================*/
// include
/*=====================================================================*/
#include "memory.h"
#include "descriptor.h"
#include "console.h"
#include "task.h"
#include "graphic.h"
#include "console.h"
#include "fasm.h"
#include "inthandler.h"
#include "interrupt.h"
#include "page.h"
#include "process.h"


/*=====================================================================*/
// define variable
/*=====================================================================*/
//static proc_lists_t proc_lists;
static struct		proc_desc_table proc_dtbl;
static struct		pid_bit_map pbit_map;
static bool			enable_sched = false;	//!< enable schedule
static u32			_count = 0;
static struct tss	_tss;
static u8			_tss_stack[256];

/*=====================================================================*/
// define macro
/*=====================================================================*/
#define for_each_hash_list( head, p ) \
	for_each_list( head, p )

/*=====================================================================*/
// 
/*=====================================================================*/
void _proc_exit( void );
void proc0( void );
void proc1( void );
void test_proc( void );

/*=====================================================================*/
// define function
/*=====================================================================*/
//!<
void __add_hash_list( pid_t *_pid, u32 hash )
{
	struct slist *_list = NULL;
	
	if( slist_empty( &proc_dtbl.pid_list[0].chain_list ) ) {
		slist_add( &proc_dtbl.pid_list[0].chain_list, &_pid->chain_list );
	}
	
	for_each_hash_list( (&proc_dtbl.pid_list[0].chain_list), _list ) {
		pid_t *_p = PID_CHAIN_LIST_TO_PID_T( _list );
		u32 _hash = pid_hash( _p->pid );
		if( hash == _hash ) {
			// add chain list
			slist_add( &_p->pid_head, &_pid->pid_head );
			break;
		} else
		if( hash > _hash ) {
			slist_add( _list->prev, &_pid->chain_list );
			break;
		}
	}
}

void __release_hash_list( pid_t *_pid )
{
	slist_delete( &_pid->chain_list );
	slist_delete( &_pid->pid_head );
}

//!< get unique process id
inline u32 __bit_to_index( u8 bit )
{
	if( bit == 0x01 ) return 0;
	if( bit == 0x02 ) return 1;
	if( bit == 0x04 ) return 2;
	if( bit == 0x08 ) return 3;
	if( bit == 0x10 ) return 4;
	if( bit == 0x20 ) return 5;
	if( bit == 0x40 ) return 6;
	if( bit == 0x80 ) return 7;
	return 0;
}

//!< get process id
inline u32 __get_pid( void )
{
//	pid_bit_map
	u32 i = 0;
	for( ; i < pbit_map.size; ++i ) {
		if( pbit_map.map[i] != 0xFF ) {
			
			u8 bit = pbit_map.map[i] ^ (pbit_map.map[i] + 1);
			bit = bit & (pbit_map.map[i] + 1);
			
			pbit_map.map[i] = bit | pbit_map.map[i];
			return __bit_to_index( bit ) + i * 8;
		}
	}
	return 0;
}
inline void __release_pid( u32 pid )
{
	u32 index = pid >> 3; // pid / 8
	u32 offset = pid -  index;
	pbit_map.map[index] = pbit_map.map[index] & ~offset;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	schedule process
 */
/*---------------------------------------------------------------------*/
inline void start_multi_proc( void )
{
#if 0
	proc0();
#else
	#if defined __SDEBUG__
	s32 idx = 20;
	struct slist *list = proc_dtbl.run_list.next;
	for_each_list( &proc_dtbl.run_list, list )
	{
		proc_struct_t *proc = STATE_LIST_TO_PROC_STRUCT( list );
		puts_console("create proc" );
		puts_console(proc->debug_name);
		++idx;
	}
#endif // __SDEBUG__

	proc_context_t *context= proc_dtbl.current_proc->hcontext;
	u8 *_stack = &context->stack[sizeof(context->stack) - 1];
	__asm__ __volatile__(
		"movl %[stack],%%esp\n\t"
		"jmp %[func]\n\t"
		:[stack] "=m" (_stack),
		 [func] "=m" (context->context.eip)
		:
		:"memory"
	);
#endif
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	schedule process
 */
/*---------------------------------------------------------------------*/
void schedule( void )
{
#if 1
	if( enable_sched == false )
		return;
#endif

#if 0
	++_count;
	puts_console("schedule count");
	putn( _count, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*14);
#endif

//	proc_struct_t *current = CONTEXT_TO_PROC_STRUCT( get_current_process() );	// ‹‚ß‚ç‚ê‚È‚¢... 
	proc_struct_t *current = proc_dtbl.current_proc;
	proc_struct_t *next = STATE_LIST_TO_PROC_STRUCT( &current->state_list.next );
	if( &proc_dtbl.run_list == current->state_list.next ) {
		next = STATE_LIST_TO_PROC_STRUCT( proc_dtbl.run_list.next );
	} else {
		next = STATE_LIST_TO_PROC_STRUCT( current->state_list.next );
	}
	
#if defined __SDEBUG__
//	puts_console("next process name", 0, DEBUG_FONT_SIZE_Y*16);
//	puts_console( next->debug_name, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*16);
#endif	//__SDEBUG__

	proc_dtbl.current_proc = next;

//	puth_console(current, 0, DEBUG_FONT_SIZE_Y*15);
//	puts_console(current->debug_name, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*15);
//	puth_console(next, 0, DEBUG_FONT_SIZE_Y*16);
//	puts_console(next->debug_name, DEBUG_FONT_SIZE_X*20, DEBUG_FONT_SIZE_Y*16);
/*************************************/
// ¦–â‘è“_ July.01.2013 ‰ðŒˆ!?
#if 0
	s32 idx = 17;
//	puts_console("schedule current stack", 0, DEBUG_FONT_SIZE_Y*idx);
//	puth_console(stack_pointer, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*idx);
	
	++idx;
	puts_console("schedule current context", 0, DEBUG_FONT_SIZE_Y*idx);
	puth_console(current, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*idx);
	++idx;
	puts_console("schedule current stack", 0, DEBUG_FONT_SIZE_Y*idx);
	puth_console(current->hcontext->context.esp, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*idx);
#if defined __SDEBUG__
	++idx;
	puts_console("schedule current proc", 0, DEBUG_FONT_SIZE_Y*idx);
	puts_console(current->debug_name, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*idx);
	++idx;
	puts_console("schedule current eip", 0, DEBUG_FONT_SIZE_Y*idx);
	puth_console(current->hcontext->context.eip, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*idx);
	++idx;
	puts_console("schedule proc0", 0, DEBUG_FONT_SIZE_Y*idx);
	puth_console(proc0, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*idx);
#endif // __SDEBUG__
	
	++idx;
//	puts_console("schedule next context", 0, DEBUG_FONT_SIZE_Y*idx);
//	puth_console(next, DEBUG_FONT_SIZE_X*30, DEBUG_FONT_SIZE_Y*idx);
	puts_console("schedule next context");
	puth_console(next);
	++idx;
	puts_console("schedule next stack");
	puth_console(*((u32*)next->hcontext->context.esp));
#if defined __SDEBUG__
	++idx;
	puts_console("schedule next proc");
	puts_console(next->debug_name);
	++idx;
	puts_console("schedule next eip");
	puth_console(next->hcontext->context.eip);
	++idx;
	puts_console("schedule proc1");
	puth_console(proc1);
#endif // __SDEBUG__
	
	puts_console("proc dtbl address ");
	puth_console(&proc_dtbl);
	puts_console("current proc");
	puth_console(proc_dtbl.current_proc);
#endif
/*************************************/
	
	/*--- provisional ---*/
//	enable_sched = false;
//	return;
	/*-------------------*/
	
	switch_proc( current->hcontext, next->hcontext );
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	Schedule in the exception
 */
/*---------------------------------------------------------------------*/
void schedule_exception( void )
{
	proc_struct_t	*current = proc_dtbl.current_proc;
	proc_struct_t	*next = STATE_LIST_TO_PROC_STRUCT( &current->state_list.next );

	u32 *esp = (u32*)_tss.esp;
//	*(esp +  4) = next->hcontext->context;	// ss
	*(esp +  8) = next->hcontext->context.esp;	// esp
//	*(esp + 12) = next->hcontext->context;	// eflags
//	*(esp + 16) = next->hcontext->context;	// cs
	*(esp + 20) = next->hcontext->context.eip;	// eip
//	*(esp + 24) = next->hcontext->context;	// error code

	delete_process_by_proc_struct( current );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	
 */
/*---------------------------------------------------------------------*/
void init_process( void )
{
	enable_sched = false;
	
	// 
	proc_dtbl.pid_list[0].pid = 0;
	slist_init( &proc_dtbl.pid_list[0].chain_list);
	slist_init( &proc_dtbl.pid_list[0].pid_head );
	slist_init( &proc_dtbl.run_list );
	slist_init( &proc_dtbl.wait_list );
	slist_init( &proc_dtbl.all_list );
	proc_dtbl.current_proc = NULL;
	
	
	// initialize pid bit map
	pbit_map.size = BYTE_4K >> 3;	// BYTE_4K / 8 ??
	pbit_map.map = (u8*)alloc_mem_block( FREE_BLOCK_4x1K );		// 
//	pbit_map.map = (u8*)alloc_mem_block( FREE_BLOCK_4x1024K );	// provisional 4x1024
	smemset( pbit_map.map, 0x00, BYTE_4K );
	
	// kernel prcess zero
	proc_struct_t *proc = NULL;
	proc = create_process( proc0, CLEATE_PROC_KERNEL );
	puth_console(proc);
	puts_console("proc context");
	puth_console(proc->hcontext);

	proc = create_process( proc1, CLEATE_PROC_KERNEL );
	puth_console(proc);
	puts_console("proc context");
	puth_console(proc->hcontext);

	create_process( test_proc, CLEATE_PROC_KERNEL );

	puts_console("proc dtbl address ");
	puth_console(&proc_dtbl);
	puts_console("current proc");
	puth_console(proc_dtbl.current_proc);

	// user task
	smemset( _tss_stack, 0x00, sizeof(_tss_stack));
	_tss.cr3 = PDE_ADD;
	_tss.eflags = 0x0202;	// provisional
	_tss.eip = 0;
	_tss.eax = 0;
	_tss.ecx = 0;
	_tss.edx = 0;
	_tss.ebx = 0;
	_tss.esp = (u32)(&_tss_stack[sizeof(_tss_stack) - 1]);
	_tss.ebp = (u32)(&_tss_stack[sizeof(_tss_stack) - 1]);
	_tss.esi = 0;
	_tss.edi = 0;
	_tss.es = USER_DS;
	_tss.cs = USER_CS;
	_tss.ss = USER_DS;
	_tss.ds = USER_DS;
	_tss.fs = USER_DS;
	_tss.gs = USER_DS;
	_tss.io_base_addr = 0x40000000;	// provisional
	set_gdt_descr( get_gdt(GDT_USER_TSS_SEL_BIGIN), 0x67, &_tss, 0x89, 0x00 );	// provisional
	ltr( (GDT_USER_TSS_SEL_BIGIN*8) );

	// start multi task
//	start_multi_proc();
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	create process
 */
/*---------------------------------------------------------------------*/
proc_struct_t* _create_process( main_t func, u32 create_flag
#if defined __SDEBUG__
, const char* proc_name
#endif // __SDEBUG__
)
{
	bool _enable_sched = enable_sched;
	enable_sched = false;

	// allocate memory
	proc_context_t	*context = (proc_context_t*)alloc_mem_block( FREE_BLOCK_4x1K );		// 
//	proc_context_t	*context = (proc_context_t*)alloc_mem_block( FREE_BLOCK_4x1024K );	// provisional 4x1024K
	proc_struct_t	*pstr = (proc_struct_t*)alloc_gp_slab_mem( sizeof(proc_struct_t) );
	pid_t			*pid = (pid_t*)alloc_gp_slab_mem( sizeof(pid_t) );
	
	// initialize task struct
	pstr->state = PROC_STATE_RUNNING;
	slist_init( &pstr->state_list );
	slist_init( &pstr->proc_list );
	pstr->hcontext = context;
	slist_init( &pid->chain_list );
	slist_init( &pid->pid_head );
	pstr->pid_chain = pid;
	pid->pstr = pstr;
	
//	u8 *heap = (u8*)init_heap_allocator();
//	init_heap_allocator( pstr->heap_allocator, heap, );

#if defined __SDEBUG__
	smemset( pstr->debug_name, 0, 128 );
	strcpy( pstr->debug_name, proc_name);
#endif // __SDEBUG__

	// initialize context
	u16 dseg = create_flag & CLEATE_PROC_KERNEL ? KERNEL_DS : USER_DS;
	context->context.esp = (u32)(&context->stack[BYTE_4K - (sizeof(struct  hard_context)) - 1]);
	context->context.eip = func;
	context->context.ds = dseg;
	context->context.es = dseg;
	context->context.fs = dseg;
	context->context.gs = dseg;
	if( context->context.esp == 0 ) {
		puts_console("esp zero");
	}
	
	//¦ callŽž‚Ìstack‚ð–Y‚ê‚È‚¢‚æ‚¤‚É
	u32 eflag = 0;
	u16 cseg = create_flag & CLEATE_PROC_KERNEL ? KERNEL_CS : USER_CS;
	eflag = get_eflag();
	if( (eflag & 0x00000200) == 0 ) {
		puts_console( "interrupt flag zero");
		puth_console( eflag );
	} else {
		puts_console( "eflag");
		puth_console( eflag );
	}
	hard_context_stack_push( &context->context, _proc_exit );			// _proc_exit
	hard_context_stack_push( &context->context, eflag );				// eflag
	hard_context_stack_push( &context->context, cseg );					// code segment
	hard_context_stack_push( &context->context, context->context.eip );	// eip
	// interrupt pushad
	hard_context_stack_push( &context->context, inthandler_ret ); // call 
	
	
	// connect state list
	if( proc_dtbl.current_proc == NULL )
		proc_dtbl.current_proc = pstr;
	slist_add_tail( &proc_dtbl.run_list, &pstr->state_list );
	slist_add_tail( &proc_dtbl.all_list, &pstr->proc_list );

	// connect hash list
	pid->pid = __get_pid();
	__add_hash_list( pid , pid_hash( pid->pid ) );

	enable_sched = _enable_sched;

	return pstr;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	delete process by process struct
 */
/*---------------------------------------------------------------------*/
void delete_process_by_proc_struct( proc_struct_t* proc )
{
//	_exit( proc );
	proc->state = PROC_STATE_EXIT;
	slist_delete( &proc->state_list );							// deleter run state list
	slist_add_tail( &proc_dtbl.wait_list, &proc->state_list );	// connect wait state list
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	delete process by process id
 */
/*---------------------------------------------------------------------*/
void delete_process_by_pid( s32 pid )
{
	
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	actual switch process
 */
/*---------------------------------------------------------------------*/
proc_struct_t* __switch_to( proc_struct_t *prev, proc_struct_t *next ) //__attribute__((regparm(3)))
//void __switch_to( struct process_struct *prev, struct process_struct *next ) //__attribute__((regparm(3)))
{
//	u32 eax,edx;

#if 0
	s32 idx = 28;
	puts_console("prev addr", 0, DEBUG_FONT_SIZE_Y*idx);
	puth_console( (u32)prev, DEBUG_FONT_SIZE_X*40, DEBUG_FONT_SIZE_Y*idx);
	++idx;
	puts_console("next addr", 0, DEBUG_FONT_SIZE_Y*idx);
	puth_console( (u32)next, DEBUG_FONT_SIZE_X*40, DEBUG_FONT_SIZE_Y*idx);
	
	++idx;
	puts_console("process_struct", 0, DEBUG_FONT_SIZE_Y*idx);
#endif

	return prev;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	process exit
 */
/*---------------------------------------------------------------------*/
void _proc_exit( void )
{
	puts_console("\nooo Proc Exit\n ooo");
#if defined __SDEBUG__
	puts_console( proc_dtbl.current_proc->debug_name );
	putc_console('\n');
#endif

	// disable timer interrupt
	disable_interrupt( IRQ_DISABLE_MASK(IRQ_TIME), IRQ_DISABLE_MASK(IRQ_ALL) );
	
	proc_struct_t *_proc = proc_dtbl.current_proc;
	__release_hash_list( _proc->pid_chain );
	slist_delete( &_proc->state_list );
	
	free_gp_slab_mem( (void*)_proc->pid_chain, sizeof(pid_t) );
	free_mem_block( (u32)_proc->hcontext, FREE_BLOCK_4x1024K );	// provisional 4x1024K
	free_gp_slab_mem( (void*)_proc->pid_chain, sizeof(proc_struct_t) );
	
	// enable timer itnerrupt
	enable_interrupt( IRQ_ENABLE_MASK(IRQ_TIME), IRQ_DISABLE_MASK(IRQ_ALL) );

	for(;;){
	}
}


/*---------------------------------------------------------------------*/
/*!
 * @brief	process zero
 */
/*---------------------------------------------------------------------*/
void proc0( void )
{
#if 0
	puts_console("current stack", 0, DEBUG_FONT_SIZE_Y*10);
	puth_console(stack_pointer, DEBUG_FONT_SIZE_X*40, DEBUG_FONT_SIZE_Y*10);
	
	proc_struct_t* proc = STATE_LIST_TO_PROC_STRUCT(proc_dtbl.run_list.prev);
	__asm__ __volatile__("movl %[proc_esp],%%esp": :[proc_esp] "m" (proc->hcontext->context.esp): "memory");

//	puts_console("process stack", 0, DEBUG_FONT_SIZE_Y*10);
//	puth_console( &proc->hcontext->stack[BYTE_4K - (sizeof(struct  hard_context)) -1], DEBUG_FONT_SIZE_X*40, DEBUG_FONT_SIZE_Y*10);
	puts_console("after change the stack", 0, DEBUG_FONT_SIZE_Y*11);
	puth_console(stack_pointer, DEBUG_FONT_SIZE_X*40, DEBUG_FONT_SIZE_Y*11);
	puts_console("proc address", 0, DEBUG_FONT_SIZE_Y*12);
	puth_console(proc, DEBUG_FONT_SIZE_X*40, DEBUG_FONT_SIZE_Y*12);
#endif
/*
	for(;;){
	}
*/
	puts_console("process0");
	enable_sched = true;
	s32 n = 0;
	for(;;){
		putn_console( ++n );
	}
}

void proc1( void )
{
	puts_console("process1");
	s32 n = 0;
	for(;;){
		/*
		if( enable_sched ){
			puts_console( "enable sched true ", 0, DEBUG_FONT_SIZE_Y*15 );
		} else {
			puts_console( "enable sched false", 0, DEBUG_FONT_SIZE_Y*15 );
		}
		*/
		putn_console( ++n );
	}
}

void test_proc( void )
{
//	s32 i = 1 / 0;
	for(;;) {
	}
}



void set_enable_sched( bool b )
{
	enable_sched = b;
}
bool get_enable_sched()
{
	return enable_sched;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	get process by pid
 */
/*---------------------------------------------------------------------*/
proc_struct_t *get_proc_strcut_by_pid( s32 pid )
{
	u32 hash = pid_hash( pid );
	struct slist *_list = NULL;

#if 0
	proc_struct_t
		proc_list‚©‚ç‚½‚Ç‚é‚×‚«H
#endif
	for_each_hash_list( (&proc_dtbl.pid_list[0].chain_list), _list ) {
		pid_t *_p = PID_CHAIN_LIST_TO_PID_T( _list );
		u32 _hash = pid_hash( _p->pid );
		if( hash == _hash ) {
			if( _p->pid == pid )
				return _p->pstr;
			
			for_each_hash_list( &_p->pid_head, _list ) {
				pid_t *_p = PID_HEAD_LIST_TO_PID_T( _list );
				if( _p->pid == pid ) {
					return _p->pstr;
				}
			}
		}
	}
	return NULL;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	get current process
 */
/*---------------------------------------------------------------------*/
proc_struct_t *get_current_process( void )
{
	return proc_dtbl.current_proc;
}


#if defined __SDEBUG__
/*---------------------------------------------------------------------*/
/*!
 * @brief	debug process all print
 */
/*---------------------------------------------------------------------*/
void debug_print_proc_all( void )
{
#if 0	// September.05.2013 t_sato
	struct slist *_list = 0;//proc_dtbl.run_list.next;
	proc_struct_t *_proc = 0;
	for_each_list( &proc_dtbl.run_list, _list ) {
		_proc = STATE_LIST_TO_PROC_STRUCT(_list);
		if( _proc ) {
			puts_console( _proc->debug_name );
			puts_console("    ");
			puth_console( _proc->pid_chain->pid );
			putc_console('\n');
		}
	}
#endif
#if 1
	struct slist *_list = 0;//proc_dtbl.all_list.next;
	proc_struct_t *_proc = 0;
	for_each_list( &proc_dtbl.all_list, _list ) {
		_proc = PROC_LIST_TO_PROC_STRUCT(_list);
		if( _proc ) {
			puts_console( _proc->debug_name );
			puts_console("    ");
			puth_console( _proc->pid_chain->pid );
			putc_console('\n');
		}
	}
#endif
}
#endif // __SDEBUG__

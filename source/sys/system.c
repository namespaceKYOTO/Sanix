/*---------------------------------------------------------------------*/
/*!
 * @file	system.h
 * @brief	system
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#include "console.h"
#include "process.h"
#include "system.h"


/*---------------------------------------------------------------------*/
/*!
 * @brief	process kill
 */
/*---------------------------------------------------------------------*/
void kill( s32 pid )
{
	proc_struct_t* _pstr = get_proc_strcut_by_pid( pid );
	
	if( _pstr ) {
		puts_console("proces kill :: ");
#if defined __SDEBUG__
		puts_console( _pstr->proc_name );
		putc_console('\n');
#endif // __SDEBUG__
		return;
		_pstr->state = PROC_STATE_EXIT;
	}
	
	puts_console("kill process Failed :: ");
	puth_console( pid );
	putc_console('\n');
}


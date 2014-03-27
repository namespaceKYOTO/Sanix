/*---------------------------------------------------------------------*/
/*!
* @file		console.c
* @brief	provisional console
* @note		Copyright (C) 2014 t_sato
*/
/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
// include
/*---------------------------------------------------------------------*/
#include "time.h"
#include "graphic.h"
#include "memory.h"
#include "widget.h"
#include "interrupt.h"
#include "debug.h"
#include "process.h"
#include "system.h"
#include "console.h"


/*---------------------------------------------------------------------*/
// define
/*---------------------------------------------------------------------*/
bool init_cons = false;
//struct widget *cons;
//struct text_box *textbox;
struct widget cons;
struct text_box textbox;
struct touch_state t_state;
struct timer l_time;
u8 cmd_stack[32];
squeue_t cmd_queue;
bool cmd_mode;

static const u8 __key1[] = {
	'1','2','3','4','5','6','7','8','9','0', 0, 0, 0, 0,
	'Q','W','E','R','T','Y','U','I','O','P', 0, 0, 0, 0,
	'A','S','D','F','G','H','J','K','L', 0, 0, 0, 0, 0,
	'Z','X','C','V','B','N','M',
};
static const u8 __key2[] = {
	'1','2','3','4','5','6','7','8','9','0', 0, 0, 0, 0,
	'q','w','e','r','t','y','u','i','o','p', 0, 0, 0, 0,
	'a','s','d','f','g','h','j','k','l', 0, 0, 0, 0, 0,
	'z','x','c','v','b','n','m',
};
static u8 *__key_ptr;

#define is_valid_key( code ) \
	( 0x02 <= code && code <= 0x32 )
#define key2char( code ) \
	__key_ptr[code - 0x02]


#define __putcl \
	putc_console('\n');
#define __puts( str ) \
	puts_console(str); __putcl
#define __putn( n ) \
	putn_console(n); __putcl
#define __puth( n ) \
	puth_console(n); __putcl

#define __TEST_MEM_BLOCK		0
#define __TEST_GP_SLAB_MEM		0
#define __TEST_GET_PID			0

#define _KEY_ENTER_RELEASED				0x9C
#define _KEY_HANKAKU_ZENKAKU_PRESSED	0x29
#define _KEY_SPACE_RELEASED				0xB9


#define _CMD_ARGS_ARGC_MAX	32
#define _CMD_ARGS_BUFF_SIZE	128
struct cmd_args {
	s32 argv;
	u8* argc[_CMD_ARGS_ARGC_MAX];	// provisional
	u8 buff[_CMD_ARGS_BUFF_SIZE];
};
void _init_cmd_args( struct cmd_args *_ca )
{
	_ca->argv = 0;
	smemset( _ca->argc, 0x00, _CMD_ARGS_ARGC_MAX );
	smemset( _ca->buff, 0x00, _CMD_ARGS_BUFF_SIZE );
}
void _set_cmd_args( struct cmd_args *_ca, u8* cmd )
{
	smemset( _ca->buff, 0x00, _CMD_ARGS_BUFF_SIZE );
	strncpy( _ca->buff, cmd, _CMD_ARGS_BUFF_SIZE );
	
	_ca->argv = 0;
	u8* _prev = _ca->buff;
	u8* _next = NULL;
	while( (_next = strchr(_prev, ' ')) != NULL ) {
		*_next = NULL;
		_ca->argc[ _ca->argv ] = _prev;
		_prev = _next + 1;
		if( ++_ca->argv >= _CMD_ARGS_ARGC_MAX ) {
			return;
		}
	}
	_ca->argc[ _ca->argv ] = _prev;
	++_ca->argv;
}
const u8* _get_cmd_arg( struct cmd_args *_ca, s32 idx )
{
	if( idx >= _ca->argv )
		return NULL;

	return _ca->argc[idx];
}
struct cmd_args _cmd_args;

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize console
 */
/*---------------------------------------------------------------------*/
void init_console( void )
{
	struct gra_info _gra_info;
	get_info( &_gra_info );
	__key_ptr = __key1;

#if 0
	cons = create_widget( eWIDGET_BASE );
	textbox = (struct text_box*)create_widget( eWIDGET_TEXT_BOX );
#endif
	
	init_widget( &cons );
	init_widget( &textbox.base );
	add_widget( &cons, &textbox.base );
	
	cons.data.rect.posx = _gra_info.screen_width>>1;
	cons.data.rect.posy = 0;
	cons.data.rect.width = 40 * DEBUG_FONT_SIZE_X;//200;
	cons.data.rect.height = 30 * DEBUG_FONT_SIZE_Y;//200;

	textbox.base.data.rect.posx = _gra_info.screen_width>>1;
	textbox.base.data.rect.posy = 0;
	textbox.base.data.rect.width = cons.data.rect.width ;
	textbox.base.data.rect.height = cons.data.rect.height;
	textbox.col = textbox.base.data.rect.width / DEBUG_FONT_SIZE_X;//col;
	textbox.row = textbox.base.data.rect.height/ DEBUG_FONT_SIZE_Y;//row:
	textbox.now_col = 0;
	textbox.now_row = 0;
	
	t_state.pos.x = 0;
	t_state.pos.y = 0;
	t_state.is_touch = false;
	
	init_cons = true;
	cmd_mode = false;
	
	// command queue initialize
	init_squeue( cmd_queue, cmd_stack, 0, 0, sizeof(cmd_stack) );
	
	_init_cmd_args( &_cmd_args );
	
	puts("console initialize", 0, DEBUG_FONT_SIZE_Y*1);
}


/*---------------------------------------------------------------------*/
/*!
 * @brief	update console
 */
/*---------------------------------------------------------------------*/
void update_console()
{
	update_widget_all( &cons, &t_state );
	
	u8 code = 0;
	u32 addr = 0;
	u32 id = 0;
	{
		code = dequeue( &key_queue );
		if( code != 0 ) {
			if( code == _KEY_HANKAKU_ZENKAKU_PRESSED ) {
				__key_ptr = __key_ptr == __key1 ? __key2 : __key1;
			} else
			if( is_valid_key(code) || code == _KEY_ENTER_RELEASED || code == _KEY_SPACE_RELEASED ) {
				if( is_valid_key(code) ) {
					code = key2char( code );
					putc_console( code );
				} else
				if( code == _KEY_ENTER_RELEASED ) {
					putc_console( '\n' );
				} else
				if( code == _KEY_SPACE_RELEASED ) {
					code = ' ';
					putc_console( code );
				}
				enqueue( &cmd_queue, code );
			}
		}
		cmd_parsing();
		
		get_timer( &l_time );
		putn_col( l_time.tick, 8*15, 16*3, 10 );
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	draw console
 */
/*---------------------------------------------------------------------*/
void draw_console()
{
	draw_widget_all( &cons );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	console
 */
/*---------------------------------------------------------------------*/
void console( void )
{	
	while( !init_cons ){};

	get_timer( &l_time );
	
	draw_widget( &cons, true );
	
	s32 _i = 'Z';
	for( ; _i < 'c'/*_DEBUG_FONT_TBL_SIZE*/; ++_i ) {
		putc_console( _i ); putc_console(' ');
		putn_console( _i ); putc_console('\n');
	}
	
	for(;;) {
		update_console();
		draw_console();
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	print console
 */
/*---------------------------------------------------------------------*/
void putc_console( u8 c )
{
	putc_console_ex( c, COLOR_WHITE );
}
void puts_console( u8* str )
{
	puts_console_ex( str, COLOR_WHITE );
}
void putn_console( u32 n )
{
	putn_console_ex( n, COLOR_WHITE );
}
void puth_console( u32 n )
{
	puth_console_ex( n, COLOR_WHITE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	print console
 */
/*---------------------------------------------------------------------*/
void putc_console_ex( u8 c, u8 color )
{
	put_c( &textbox, c, color );
}
void puts_console_ex( u8* str, u8 color )
{
	put_str( &textbox, str, color );
}
void putn_console_ex( u32 n, u8 color )
{
	if( n == 0 ) {
		putc_console_ex('0', color);
	}
	u32 div = 1000000000;
	u8 i = 0;
	while( div /*&& n*/ ) {
		i = n / div;
		n -= (i*div);
		div /= 10;
		putc_console_ex( '0' + i, color );
	}
}
void puth_console_ex( u32 n, u8 color )
{
	putc_console_ex('0', color);
	putc_console_ex('x', color);
	u32 digit = 0;
	u32 hex = n;
	while( hex > 0 ) {
		++digit;
		hex >>= 4;
	}
	u32 i = 1;
	u8 c[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	for( ; i <= digit; ++i) {
		putc_console_ex( c[((n >> (digit*4 - i*4)) & 0x0000000F)], COLOR_WHITE );
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	command parsing
 */
/*---------------------------------------------------------------------*/
void cmd_parsing( void )
{
	u8 buff[_CMD_ARGS_ARGC_MAX];
	smemset( buff, 0x00, sizeof(buff) );
	bool _enter = do_replace_queue( &cmd_queue, _KEY_ENTER_RELEASED, 0 );
	get_contents_queue( &cmd_queue, sizeof(buff), buff, sizeof(buff), false );
	
	_set_cmd_args( &_cmd_args, buff );

	if( _enter )
		do_empty_queue( &cmd_queue );

	if( _enter ) {
		if( cmd_mode == false ) {
			if( strcmp( _get_cmd_arg(&_cmd_args, 0), "cmd" ) == 0 ) {
				__puts("ooooooooooooooooooooooooooooooooooooooo");
				__puts("         Start Command Mode");
				__puts("ooooooooooooooooooooooooooooooooooooooo");
				cmd_mode = true;
			}
		} else {
			// view process all
			if( strcmp( _get_cmd_arg(&_cmd_args, 0), "PROCALL") == 0 ){
			//	__puts("view all process");
			#if defined __SDEBUG__
				debug_print_proc_all( );
			#endif // __SDEBUG__
			} else
			// kill specific process
			if( strcmp( _get_cmd_arg(&_cmd_args, 0), "KILL") == 0 ){
				const u8* _num = _get_cmd_arg(&_cmd_args, 1);
				s32 _type = get_number_type( _num );
				s32 _pid = 0;
				switch( _type ) {
					case NUMBER_TYPE_DECIMAL:
						_pid = sdtoi( _num );
						__puts("number type is decimal");
						break;
					
					case NUMBER_TYPE_HEXADECIMAL:
						_pid = shtoi( _num );
						__puts("number type is hexadecimal");
						break;
					
					default : __puts("none number type");
				}
				if( _pid ) {
					kill( _pid );
				}
			} else
			// help
			if( strcmp( _get_cmd_arg(&_cmd_args, 0), "HELP") == 0 ) {
			}
		}
	}
}

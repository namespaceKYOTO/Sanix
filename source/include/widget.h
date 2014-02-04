/*---------------------------------------------------------------------*/
/*!
 * @file	widget.h
 * @brief	widget
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef __SIPL_WIDGET__
#define __SIPL_WIDGET__


/*---------------------------------------------------------------------*/
// include
/*---------------------------------------------------------------------*/
#include "type.h"
#include "math.h"
#include "list.h"
#include "graphicUtil.h"


/*---------------------------------------------------------------------*/
// define
/*---------------------------------------------------------------------*/
#define _WIDGET_CAST( type, widget ) ((type*)widget)
#define CHILD_LIST_TO_WIDGET( _list ) \
		((struct widget*)((u8*)_list - sizeof(struct rect) - sizeof(struct widget*)  - sizeof(struct widget*)))

enum WidgetType {
	eWIDGET_BASE = 0,
	eWIDGET_TEXT_BOX,
};

struct touch_state {
	vector2_t	pos;
	bool		is_touch;
};

struct _data {
	// information
	struct rect rect;
	struct widget	*parent;
	struct widget	*child;
	struct slist	brother;
	
	// state
	bool	is_touch;
	bool	is_visible;
	bool	is_active;
	bool	is_redraw;
};

struct widget;
typedef void (*init_func)( struct widget* );
typedef void (*update_func)( struct _data*, struct touch_state* );
typedef void (*draw_func)( struct _data*, bool );

struct _operation {
	init_func	init;
	update_func	update;
	draw_func	draw;
};

struct widget {
	struct _data		data;
	struct _operation	ope;
};

struct text_box {
	struct widget base;
	u32 col, now_col;
	u32 row, now_row;
	bool is_cursor;
	u16 cursor_count;
};

#define add_child( this, _child )								\
	if( !this->child ) this->child = _child;					\
	else  slist_add( this->child->brother, child->brother );	\
	_child->parent = this;

//void delete_child( struct widget *this, struct widget *child );

//!< base widget
void init_widget( struct widget *widget );
void update_widget( struct _data*, struct touch_state* );
void draw_widget( struct _data*, bool );

//!< textbox widget
void init_text_box( struct widget *widget );
void update_text_box( struct _data*, struct touch_state* );
void draw_text_box( struct _data*, bool );
void put_c( struct text_box *text_box, u8 c, u8 color );
void put_str( struct text_box *text_box, u8 *str, u8 color );
void scroll_text_box( struct text_box *text_box );

//!< all widget
inline u32 get_widget_size( s32 type );
struct widget* create_widget( s32 type );
void update_widget_all( struct widget *root,  struct touch_state* touch_state );
void draw_widget_all( struct widget *root );
void add_widget( struct widget *dst, struct widget *child );
void set_widget_operation( s32 type, struct widget* dst );


#endif // __SIPLE_WIDGET__

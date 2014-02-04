/*---------------------------------------------------------------------*/
/*!
 * @file	widget.c
 * @brief	widget
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#include "graphic.h"
#include "graphic2d.h"
#include "console.h"
#include "widget.h"

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize base widget
 */
/*---------------------------------------------------------------------*/
void init_widget( struct widget *widget )
{
//	widget->pos = vector3_t( 0.0f, 0.0f, 0.0f );
	widget->data.parent = NULL;
	widget->data.child = NULL;
//	INIT_SLIST( (&widget->data.brother) );
	slist_init( &widget->data.brother );
	
	widget->data.is_touch = false;
	widget->data.is_visible = false;
	widget->data.is_active = false;
	widget->data.is_redraw = true;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	update base widget
 */
/*---------------------------------------------------------------------*/
void update_widget( struct _data *data, struct touch_state* touch_state )
{
	data->is_redraw = false;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	draw base widget
 */
/*---------------------------------------------------------------------*/
void draw_widget( struct _data *data, bool visible )
{
	if( visible && data->is_redraw ) {
//		sg2d_square( &data->rect, 0, COLOR_BLACK );
		sg2d_fill_pixel( &data->rect, COLOR_BLACK );
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	initialize textbox
 */
/*---------------------------------------------------------------------*/
void init_text_box( struct widget *widget )
{
	struct text_box* textbox = (struct text_box*)widget;
	
	init_widget(widget);
	textbox->col = 0;
	textbox->now_col = 0;
	textbox->row = 0;
	textbox->now_row = 0;
	textbox->is_cursor = true;
	textbox->cursor_count = 0;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	update textbox
 */
/*---------------------------------------------------------------------*/
void update_text_box( struct _data* data, struct touch_state* touch_state )
{
	puts( "text box update", 0, 16*20);
	struct text_box* text_box = (struct text_box*)data;
	if( text_box->is_cursor && (++text_box->cursor_count & 0x0FFF) > 30) {
		text_box->cursor_count += 0x8000;
		text_box->cursor_count = (text_box->cursor_count & 0xF000);
		put_str(text_box, "chage cursor", COLOR_WHITE );
	}else{
		put_str(text_box, "not change cursor", COLOR_WHITE );
	}
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	draw textbox
 */
/*---------------------------------------------------------------------*/
void draw_text_box( struct _data* data, bool visible )
{
	struct text_box* text_box = (struct text_box*)data;
	if( text_box->is_cursor ) {
		struct rect rect = {
			.posx = text_box->now_col,
			.posy = text_box->now_row,
			.width = DEBUG_FONT_SIZE_X,
			.height = DEBUG_FONT_SIZE_Y,
		};
		if( text_box->cursor_count & 0x8000 ) {
			sg2d_fill_pixel( &rect, COLOR_BLACK );
		} else {
			sg2d_fill_pixel( &rect, COLOR_WHITE );
		}
	}
//	struct text_box *text_box = _WIDGET_CAST( struct textbox, data);
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	put charactor to textbox
 */
/*---------------------------------------------------------------------*/
void put_c( struct text_box *text_box, u8 c, u8 color )
{
	s16 posx = text_box->base.data.rect.posx + (text_box->now_col * DEBUG_FONT_SIZE_X);
	s16 posy = text_box->base.data.rect.posy + (text_box->now_row * DEBUG_FONT_SIZE_Y);
	
	if( c == 0x0A/*is_linefeed(c)*/ ) {
		text_box->now_col = 0;
		++text_box->now_row;
	} else {
		putc_ex( c, posx, posy, color, COLOR_BLACK );
		++text_box->now_col;
	}
	
	if( text_box->col <= text_box->now_col ) {
		text_box->now_col = 0;
		++text_box->now_row;
	}
	if( text_box->row <= text_box->now_row ) {
		text_box->now_row = text_box->row - 1;
		scroll_text_box( text_box );
	}
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	put string to textbox
 */
/*---------------------------------------------------------------------*/
void put_str( struct text_box *text_box, u8 *str, u8 color )
{
	while( *str ) {
		put_c( text_box, *str, color );
		++str;
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	scroll text box
 */
/*---------------------------------------------------------------------*/
void scroll_text_box( struct text_box *text_box )
{
//	sg2d_move_pixel( text_box->col, text_box->row - 1, &text_box->base.data.rect, COLOR_BLACK );
	sg2d_move_pixel( text_box->base.data.rect.posx, text_box->base.data.rect.posy - DEBUG_FONT_SIZE_Y, &text_box->base.data.rect, COLOR_BLACK );
	struct rect rec = {
		.posx = text_box->base.data.rect.posx,
		.posy = text_box->base.data.rect.posy - DEBUG_FONT_SIZE_Y,
		.width = text_box->base.data.rect.width,
		.height = DEBUG_FONT_SIZE_Y,
	};
	sg2d_fill_pixel( &rec, COLOR_BLUE );
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	get widget size
 */
/*---------------------------------------------------------------------*/
u32 get_widget_size( s32 type )
{
	switch( type ) {
		case eWIDGET_BASE:		return sizeof(struct widget);
		case eWIDGET_TEXT_BOX:	return sizeof(struct text_box);
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	create widget
 */
/*---------------------------------------------------------------------*/
struct widget* create_widget( s32 type )
{
	struct widget* widget = (struct widget*)alloc_gp_slab_mem( get_widget_size(type) );
	
	set_widget_operation( type, widget);
	widget->ope.init( widget );
	return widget;
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	update widget all
 */
/*---------------------------------------------------------------------*/
void update_widget_all( struct widget *root,  struct touch_state* touch_state )
{
#if 0
	puts_console("update widget all  ");
	putn_console(root);
	putc_console('\n');
#endif
	if( root->ope.update ) {
		root->ope.update( &root->data, touch_state );
	}
	
	// child
	if( root->data.child ) {
		struct slist *child_top = &root->data.child->data.brother;
		struct slist *child = root->data.child->data.brother.next;
		for_each_list( child_top, child ) {
			update_widget_all( CHILD_LIST_TO_WIDGET(child), touch_state);
			child = child->next;
		}
	}
}
/*---------------------------------------------------------------------*/
/*!
 * @brief	draw widget all
 */
/*---------------------------------------------------------------------*/
void draw_widget_all( struct widget *root )
{
	if( root->ope.draw ) {
		root->ope.draw( &root->data, root->data.is_visible );
	}
	
	// child
	if( root->data.child ) {
		struct slist *child_top = &root->data.child->data.brother;
		struct slist *child = root->data.child->data.brother.next;
		for_each_list( child_top, child ) {
			draw_widget_all( CHILD_LIST_TO_WIDGET(child) );
			child = child->next;
		}
	}
	
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	add child widget
 */
/*---------------------------------------------------------------------*/
void add_widget( struct widget *dst, struct widget *child )
{
	if( dst->data.child == NULL ) {
		dst->data.child = child;
	} else {
		slist_add( &dst->data.child->data.brother, &child->data.brother );
	}
}

/*---------------------------------------------------------------------*/
/*!
 * @brief	set widget operation
 */
/*---------------------------------------------------------------------*/
void set_widget_operation( s32 type, struct widget* dst )
{
	switch( type ) {
		case eWIDGET_BASE: {
			dst->ope.init = init_widget;
			dst->ope.update = update_widget;
			dst->ope.draw = draw_widget;
		} break;
		
		case eWIDGET_TEXT_BOX: {
			dst->ope.init = init_text_box;
			dst->ope.update = update_text_box;
			dst->ope.draw = draw_text_box;
		} break;
	}
}

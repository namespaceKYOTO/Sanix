#ifndef _DEBUG_H_
#define _DEBUG_H_

#define S_ASSERT( terms, error_put ) \
	if( !(terms) ) assert( error_put );


void assert( u8 *error_put );

#endif // _DEBUG_H_

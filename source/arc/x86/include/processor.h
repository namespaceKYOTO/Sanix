/*---------------------------------------------------------------------*/
/*!
 * @file	processor.h
 * @brief	processor
 * @note	Copyright (C) 2014 t_sato
 */
/*---------------------------------------------------------------------*/
#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "type.h"

struct fpu_fsave_struct {
	u32 fpu_cw;		//!< FPU Control Word
	u32 fpu_sw;		//!< FPU Status Word
	u32 fpu_tw;		//!< FPU Tag Word
	u32 fpu_dp;		//!< FPU Data Pointer
	u32 fpu_ip;		//!< FPU Instruction Pointer
	u32 fpu_lip;	//!< FPU Last Instruction Opcode
	u32 st[8];		//!< st
};

struct fpu_fxsave_struct {
	u16 fpu_cw;		//!< Control Word
	u16 fpu_sw;		//!< Status Word
	u16 fpu_tw;		//!< Tag Word
	u16 fpu_op;		//!< Opcode
	
	u32 fpu_ip;		//!< Instruction Pointer Offset
	
	u16 fpu_cs;		//!< code selector
	u16 _reserve1;
	
	u32 fpu_dp;		//!< Operand Pointer Offset
	
	u16 fpu_ds;		//!< data selector
	u16 _reserve2;
	
	u32 mxcsr;		//!< MXCSR register state
	u32 mxcsr_mask;	//!< MXCSR mask
	
	/* st0/mm0 - st7/mm7  16*8 = 128 bytes */
	u8 st_mm[128];
	
	/* XMM0 - XMM7  16*8 = 128 bytes*/
	u8 xmm[128];
	
	/* reserve */
	u8 reserve[224];
}  __attribute__((aligned(16)));

#endif // _PROCESSOR_H_

;--------------------------------------------------------------------
;
;    Copyright (C) 2014 t_sato
;
;    function assembler(NASM)
;
;--------------------------------------------------------------------
%include "assm.inc"

[BITS 32]

[extern _inthandler30]

[global _hlt]
[global _cli]
[global _sti]
[global _get_cr0]
[global _set_cr0]
[global _cr3]
[global _lgdt]
[global _lidt]
[global _ltr]
[global _io_in]
[global _io_in8]
[global _io_in16]
[global _io_in32]
[global _io_out8]
[global _io_out16]
[global _io_out32]
[global _io_in10]
[global _far_jmp]
[global _memset]
[global _sys_call]
[global _get_eflag]

;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
segment .text

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void hlt( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_hlt:
	hlt
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void cli( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; interrupt enable flag clear(Changing the IF flag)
_cli:
	cli
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void sti( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; interrupt enable flag set(Changing the IF flag)
_sti:
	sti
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void get_cr0( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_get_cr0:
	mov		eax,cr0
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void set_cr0( u32 flag )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_set_cr0:
	mov		eax,cr0
	mov		edx,[esp+0x04]
	mov		cr0,eax
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void cr3( u32 page_dire ) PDBR
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_cr3:
	mov		eax,[esp+0x04]
	mov		cr3,eax
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void lgdt( u32 limit, u32 addr )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_lgdt:
	mov		ax,[esp+0x04]
	mov		[esp+0x06],ax
	lgdt	[esp+0x06]
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void lidt( u32 limit, u32 addr )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_lidt:
	mov		ax,[esp+0x04]
	mov		[esp+0x06],ax
	lidt	[esp+0x06]
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void ltr( u32 index )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_ltr:
	ltr		[esp+0x04]
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void _io_in( struct regs *in_reg, struct regs *out_reg )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_in:
	push	ebp
	mov		ebp,esp

	pushfd
	pushad

	push	eax
	mov		esi,[esp+4]			; first parameter to function
	mov		eax,dword[esi+36]	; al = in_reg->work
	mov		byte [__ope],al
	
	; it check the correct?
;	mov		al,byte[__ope]
	cmp		eax,10h
	je		_next
	cli
	jmp		$
	
_next:	pop		eax

	mov		eax,ebp
	mov		[esi+8],eax
	mov		eax,[esi]
	mov		esp,eax
	mov		esp,ebp
	mov		eax,[esi+28]
	popad
	
	
;	jmp		$

	; int al
		db	0xcd	; int
__ope:	db	0x00	; al

	; store registers
	mov		esi,[ebp+8]		; second parameter to function
	add		esi,0x36		; struct bottom address
	mov		ebp,esp
	mov		esp,esi
	pushfd	; push all falags
	pushad	; push all general-purpose register
	mov		esp,ebp
;	leave
	
	
	popad
	popfd
	
	pop		ebp

	ret


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; u8 _io_in8( u32 port )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_in8:
	mov		edx,[esp+4]
	xor		eax,eax
	in		al,dx
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; u16 _io_in16( u32 port )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_in16:
	mov		edx,[esp+4]
	xor		eax,eax
	in		ax,dx
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; u32 _io_in32( u32 port )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_in32:
	mov		edx,[esp+4]
	xor		eax,eax
	in		eax,dx
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void _io_out8( u32 port, u8 data )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_out8:
	mov		edx,[esp+4]
	mov		al,[esp+8]
	out		dx,al
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void _io_out16( u32 port, u16 data )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_out16:
	mov		edx,[esp+4]
	mov		ax,[esp+8]
	out		dx,ax
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void _io_out32( u32 port, u32 data )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_out32:
	mov		edx,[esp+4]
	mov		eax,[esp+8]
	out		dx,eax
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void _io_in10( regs *ireg, regs *oreg )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_io_in10:
	pushad

	push	ebp
	mov		ebp,esp

	; ireg
	mov		esi,[esp+4]
	mov		esp,esi
	mov		dword[esi+8],ebp
	popad
	mov		esp,ebp

	jmp		$
	int		0x10

	; oreg
	mov		esi,[esp+8]
	add		esi,31
	mov		esp,esi
	pushad

	mov		esp,ebp
	pop		ebp
	popad

	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void _far_jmp( u32 selector, u32 offset )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_far_jmp:
	jmp		dword  far [esp+4]
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void memset( u8 dst, u8 code, u32 size )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_memset:
	push	ebp
	push	edi
	push	eax
	push	ecx
	
	mov		edi,[esp+4]
	mov		eax,[esp+8]
	mov		ecx,[esp+12]
	rep		stosb
	
	pop		ecx
	pop		eax
	pop		edi
	leave

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void _sys_call( u32 idx )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_sys_call:
	push	ebp
	mov		ebp,esp
	
	mov		eax,[esp+4]
	push	eax
	
	call	_inthandler30
	
	inc		ebp
	inc		esp
	
	leave
	ret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; u32 _get_eflag( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_get_eflag:
	pushfd
	pop		eax
;	mov		[esp+4],eax
	ret


;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
segment .data

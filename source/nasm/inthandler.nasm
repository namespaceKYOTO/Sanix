;--------------------------------------------------------------------
;
;    Copyright (C) 2014 t_sato
;
;    interrupt handler
;
;--------------------------------------------------------------------
%include "assm.inc"

[extern _isr_test]
[extern _isr_divide_error]
[extern _isr_nmi]
[extern _isr_break_point]
[extern _isr_overflow]
[extern _isr_bound_range_exceeded_exception]
[extern _ist_invalid_opcode]
[extern _isr_disable_device]
[extern _isr_double_fault]
[extern _isr_coprocessor_segment_overrun]
[extern _isr_invalid_tss]
[extern _isr_segment_not_present]
[extern _isr_stack_fault]
[extern _isr_general_protection]
[extern _isr_page_fault]
[extern _isr_floating_point_mf]
[extern _isr_floating_point_xf]
[extern _isr_timer]
[extern _isr_kerboard]
[extern _isr_ata_master]
[extern _isr_ata_slave]
[extern _isr_sys_call]


segment .text

[global _inthandler00]
[global _inthandler01]
[global _inthandler02]
[global _inthandler03]
[global _inthandler04]
[global _inthandler05]
[global _inthandler06]
[global _inthandler07]
[global _inthandler08]
[global _inthandler09]
[global _inthandler0a]
[global _inthandler0b]
[global _inthandler0c]
[global _inthandler0d]
[global _inthandler0e]
[global _inthandler0f]
[global _inthandler10]
[global _inthandler11]
[global _inthandler12]
[global _inthandler13]
[global _inthandler20]
[global _inthandler21]
[global _inthandler2e]
[global _inthandler2f]
[global _inthandler30]
[global _inthandler_ret]


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler00( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler00:
;	mov		eax,cs
;	push	eax
	call	_isr_divide_error
;	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler01( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler01:
	call	_isr_test
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler02( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler02:
	call	_isr_nmi
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler03( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler03:
	call	_isr_break_point
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler04( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler04:
	call	_isr_overflow
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler05( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler05:
	call	_isr_bound_range_exceeded_exception
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler06( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler06:
	call	_ist_invalid_opcode
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler07( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler07:
	call	_isr_disable_device
	
	mov		eax,cr0
	and		eax,0xFFFFFFF7	; TS 0
	mov		cr0,eax
	
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler08( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler08:
	call	_isr_double_fault
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler09( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler09:
	call	_isr_coprocessor_segment_overrun
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler0a( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler0a:
	call	_isr_invalid_tss
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler0b( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler0b:
	call	_isr_segment_not_present
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler0c( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler0c:
	call	_isr_stack_fault
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler0d( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler0d:
;	pop		eax		; get error code?
	call	_isr_general_protection
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler0e( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler0e:
	call	_isr_page_fault
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler0f( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler0f:
	call	_isr_test
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler10( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler10:
	call	_isr_floating_point_mf
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler11( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler11:
	call	_isr_test
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler12( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler12:
	call	_isr_test
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler13( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler13:
	call	_isr_floating_point_xf
	jmp		$
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler20( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler20:
	pushad
	
	mov		al,0x60
	out		PIC0_OCW2,al	; äÑÇËçûÇ›äÆóπÇPICÇ…í ím
	
	call	_isr_timer
	popad
_inthandler_ret:
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler21( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler21:
	push	eax
	
	mov		al,0x61
	out		PIC0_OCW2,al	; äÑÇËçûÇ›äÆóπÇPICÇ…í ím
	
	call	_isr_kerboard
	
	pop		eax
	iret


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler2e( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler2e:
;	call	_isr_ata_master
	call	_isr_test
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler2f( void )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler2f:
;	call	_isr_ata_slave
	call	_isr_test
	iret

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; void inthandler30( u32 idx )
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_inthandler30:
	push	ebp
	mov		ebp,esp
	
	mov		eax,[esp+4]
	push	eax
	call	_isr_sys_call
	
	inc		esp
	inc		ebp
	
	leave
	iret

segment .data

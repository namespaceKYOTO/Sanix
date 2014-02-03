;***************************************************************************
;
;    Copyright (C) 2014 t_sato
;
;    initialize floating point unit
;
;***************************************************************************
[BITS 16]

_init_fpu:
	mov		eax,0x01
	cpuid
	and		edx,0x00000001	; check FPU exists
	jz		_init_fpu_end

;_init_fpu:
	db		0x9b,0xdb,0xe3	; Initialize Floating Point Unit
	mov		eax,cr0
	and		eax,0xFFFFFFd3	; NE 0, EM 0, TS 0
	or		eax,0x00000002	; MP 1
;	or		eax,0x0000000a	; TS 1, MP 1
	mov		cr0,eax

_init_fpu_end:
	mov		si,not_exists_FPU
	call	prints
	ret


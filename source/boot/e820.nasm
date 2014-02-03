;***************************************************************************
;
;    Copyright (C) 2014 t_sato
;
;    Query system address map
;    use ACPI(Advanced Configuration and Power Interface)
;
;***************************************************************************
[BITS 16]

_e820_mem_map:
	xor		bp,bp				; entry cont in
	mov		ax,BS_PHYSICAL_ADD
	mov		es,ax				; [es:di]	buffer pointer
	mov		di,2064
	mov		eax,0x0000e820		;
	xor		ebx,ebx				; must contain zero
	mov		ecx,24				; buffer size
	mov		edx,0x534d4150		; 'SMAP'  signature
;	int		0x15
;	jc		_e820_failed_carry
;	mov		edx,0x534d4150		; verify correct to BIOS version
;	cmp		eax,edx
;	jne		_e820_failed_signature
;	jmp		_e820_preparation

_e820_rep:
;	mov		cx,20
;	add		di,cx
	int		0x15
	jc		_e820_end			; if( cf ) goto _e820_end;
	test	ebx,ebx
	jz		_e820_end			; if( !(ebx & ebx) ) goto _e820_end;
	mov		edx,0x534d4150
	cmp		eax,edx				; verify correct to BIOS version
	jne		_e820_end			; if( eax != 'SMAP') goto _e820_end;

_e820_preparation:
	mov		eax,0x0000e820
;	mov		edx,0x534d4150
	mov		ecx,24
	inc		bp
	add		di,24
	jmp		_e820_rep

_e820_end:
	mov		[_e820_num],bp
	ret

_e820_failed_carry:
	mov		si,e820_failed_on_carry
	call	prints
	jmp		$
_e820_failed_signature:
	mov		si,e820_failed_signature
	call	prints
	jmp		$

;****************************************************************************
;
;    Copyright (C) 2014 t_sato
;
;    Boot Sector
;        IPL, BPB
;
;****************************************************************************
%include "assm.inc"

[ORG  BS_PHYSICAL_ADD]
[BITS 16]

[global _boot_infomation]

jmp		_start_bs


%include "A20.nasm"
%include "video.nasm"
%include "fpu.nasm"
%include "e820.nasm"


_start_bs:
	xor		ax,ax
	mov		ss,ax
	mov		sp,BS_PHYSICAL_ADD
	mov		ax,cs
	mov		ds,ax		; コードセグメントとデータセグメントを同じにする


	; initialize fpu
	call	_init_fpu


	; query system memory map
	call	_e820_mem_map


	; setting video
	call	_set_video


_preparation_to_pm:

	; disabling interrupt in PIC
	mov		ax,0xff
	out		PIC0_IMR,ax
	nop
	out		PIC1_IMR,ax
	
	cli

	; A20 gate on
	call	_a20_gate_on

	; provisional GDT
	lgdt	[gdtr]


	; go to protected mode
	mov		eax,cr0
	and		eax,0x7fffffff		; paging ban
;	or		eax,0x80000000		; enable paging
	or		eax,0x00000001		; enable protected mode
	mov		cr0,eax
	jmp		_pre_setting		; CR0の代入でのProtectedMode切り替えの場合はすぐに
								; jmp命令を呼ぶ
	nop
	nop

_pre_setting:
	mov		ax,0x10
	mov		ds,eax
	mov		es,eax
	mov		fs,eax
	mov		gs,eax
	mov		ss,eax

	; stack pointer
	mov		ebx,SKERNEL_PHYSICAL_ADD
	mov		esp,ebx

	; initialize register
	xor		eax,eax
	xor		ebx,ebx
	xor		ecx,ecx
	xor		edx,edx
	xor		ebp,ebp
	xor		edi,edi

	; jump SIPL Kernel
	jmp		dword	0x08 : SKERNEL_PHYSICAL_ADD
;	jmp		dword	0x08 : _to_skernel
	nop
	nop

;--------------------------------------------------------------------
; print string
;--------------------------------------------------------------------
prints:
	lodsb
	
	and		al,al			; 0x00(文字列の終わり)かチェック
	jz		prints_end		; 終わり
	
	mov		ah,0x0e
	int		10h
	jmp		prints

prints_end:
	ret

;--------------------------------------------------------------------
; Message
;--------------------------------------------------------------------
not_exists_FPU db "Not Exists FPU",0x0a,0x00
e820_failed_on_carry db "E820 Failed On Carry",0x0a,0x00
e820_failed_signature db "E820 Failed Signature",0x0a,0x00


;--------------------------------------------------------------------
; Global Descriptor Table
;--------------------------------------------------------------------
gdtr:
	dw		gdt_end - gdt - 1		; GDT size
	dd		gdt
gdt:
	dd	0x00000000,0x00000000		; NULL
	dd	0x0000ffff,0x00cf9a00		; 4GB Code
	dd	0x0000ffff,0x00cf9200		; 4GB Data
gdt_end:



;--------------------------------------------------------------------
; to Kernel
;--------------------------------------------------------------------
[BITS 32]


times 0x0400 - ($-$$) db 0x00	; --- 1K


;times 448 - ($-$$) db 0x00
; 448 + 64 = 512
;--------------------------------------------------------------------
; boot infomation :: 64byte
;--------------------------------------------------------------------
times 0x0800 - ($-$$) db 0x00	; --- 2K
;
;--------------------------------------------------------------------
; boot infomation :: 
;--------------------------------------------------------------------
_boot_infomation:
	; video info
_video_width:		dw		0x0000		; width
_video_height:		dw		0x0000		; height
_video_vram:		dd		0x00000000	; vram

	; momory map
_e820_num:	dd		0x00000000 ; 12
_e820_buff:	dd		0x00000000 ; 16

	dd		0x00000000 ; 20
	dd		0x00000000 ; 24
	dd		0x00000000 ; 28
	dd		0x00000000 ; 32
	dd		0x00000000 ; 36
	dd		0x00000000 ; 40
	dd		0x00000000 ; 44
	dd		0x00000000 ; 48
	dd		0x00000000 ; 52
	dd		0x00000000 ; 56
	dd		0x00000000 ; 60
	dd		0x00000000 ; 64

times 0x1000 - ($-$$) db 0x00	; --- 4K

;; paging memeory
;_pde:
;	dd		0x00000000
times 0x2000 - ($-$$) db 0x00	; --- 8K
;
;_pte:
;times 0x3000 - ($-$$) db 0x00	; --- 12K

;****************************************************************************
;
;    Copyright (C) 2014 t_sato
;
;    Master Boot Record
;
;****************************************************************************
%include "assm.inc"

[ORG  MBR_PHYSICAL_ADD]
[BITS 16]

[global _start]


	jmp		_start

;--------------------------------------------------------------------
; void start( void )
;--------------------------------------------------------------------
_start:
	xor		ax,ax
	mov		ss,ax
	mov		sp,MBR_PHYSICAL_ADD
	mov		ds,ax						; コードセグメントとデータセグメントを同じにする

	mov		ax,0x0003
	int		10h

	; debug
;	mov		si,hello_MBR
;	call	prints


;--------------------------------------------------------------------
; Read
;--------------------------------------------------------------------
	mov		ax,BS_LOGICAL_LOAD_ADD		; [BS_LOGICAL_LOAD_ADD:0x00] load addres
	mov		es,ax
	mov		bx,0x00						; load address offset
	mov		ah,0x02						; Read
	mov		al,0x7e						; number of processing sector(about 64K byte)
	mov		dh,[_FIRST_PARTITION + 1]	; head
	mov		cl,[_FIRST_PARTITION + 2]	; cylinder(9-8bit)2bit  sector(0-5)6bit
	mov		ch,[_FIRST_PARTITION + 3]	; cylinder(7-0bit)8bit
	mov		dl,0x80						; drive number (bit 7 set for hard disk)?
	int		13h
	mov		si,partition_load_err
	jc		error


	mov		si,load_complete
	call	prints


	; go to boot sector
	jmp		BS_LOGICAL_LOAD_ADD: WORD 0x00


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
hello_MBR db "Hello My name is MBR",0x0a,0x00
partition_load_err db "Failed partition load",0x0a,0x00
load_err_msg db "Failed load kernel",0x0a,0x00
load_err_chs_msg db "Failed load Boot Sector",0x0a,0x00
load_success_chs_msg db "Successful load Boot Sector",0x0a,0x00
load_complete db "load complete",0x0a,0x00
err_msg db "Error",0x0a,0x00


;--------------------------------------------------------------------
; error  (Debug
;--------------------------------------------------------------------
error:
	mov		si,err_msg
	call	prints
	hlt
	jmp		$


;--------------------------------------------------------------------
; Global Descriptor Table during the boot
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
; Partition TBL
; Partition Type is 0x0B(FAT32)
; ### Description ###
	; Flag  (0x80 = bootable, 0x00 = not bootable)
	;	1Byte

	; Partition Start sector (CHS方式)
	;	3Byte

	; Partition Type (Fileシステム) File system
	;	1Byte

	; Partition End sector (CHS方式)
	;	3Byte

	; Partition Start sector (LBA方式)
	;	4Byte

	; Partition all sector num (LBA方式)
	;	4Byte
;--------------------------------------------------------------------
times FIRST_PARTITION - ($-$$) db 0x00
_FIRST_PARTITION:
	; bootable
	db		0x80
	; Partition start secotr (CHS)
	db		0x00		; head
	db		0x02		; cylinder(9-8bit)  sector(0-5)
	db		0x00		; cylinder(7-0bit)
	; Partition Type
	db		0x0B
	; Partition end sector (CHS)
	db		0x01		; head
	db		0x21		; cylinder(9-8bit)  sector(0-5)
	db		0x00		; cylinder(7-0bit)
	; Partition start sector (LBA)
	dd		0x01000000
	; Partition all num (LBA)
	dd		0x20000000

_SECOND_PARTITION:
	dd		0x00000000,0x00000000,0x00000000,0x00000000

_THIRD_PARTITION:
	dd		0x00000000,0x00000000,0x00000000,0x00000000

_FOURTH_PARTITION:
	dd		0x00000000,0x00000000,0x00000000,0x00000000

;--------------------------------------------------------------------
; MBR signature
;--------------------------------------------------------------------
	dw		0xAA55			; Intel CPU is little endian

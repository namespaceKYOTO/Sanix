;***************************************************************************
;
;    Copyright (C) 2012 t_sato
;
;    video setting
;
;***************************************************************************
%include "assm.inc"

[BITS 16]

[global _set_video]

;--------------------------------------------------------------------
; void _set_video( void )
;--------------------------------------------------------------------
_set_video:
	; Determine whether VBE are present
	mov		ax,0x10400
	mov		es,ax
	xor		di,di		; [es:di] buffer pointer
	mov		ax,0x4f00
	int		0x10
	cmp		ax,0x004f
	jne		_set_vga
	
	mov		ax,[es:di+4]	; VESA version number
	cmp		ax,0x0200		; ver 2.0
	jb		_set_vga		; if( ver < 2.0f ) go to _set_vga;
	
	; Examine the attribute of the specified video mode
;	mov		cx,0x105	; 105h   1024 x 768 x 8bit color
	mov		cx,0x103	; 103h    800 x 600 x 8bit color
;	mov		cx,0x101
	mov		ax,0x4f01
	int		0x10
	cmp		ax,0x004f
	jne		_set_vga
	
	mov		ah,0x08
	cmp		ah,byte[es:di+0x19] ; bits per pixel
	jne		_set_vga			; not equal, go to _set_vga
	
	mov		ah,0x04
	cmp		ah,byte[es:di+0x1b] ; packed pixel ?
	jne		_set_vga			; not equal, go to _set_vga
	
	mov		ax,word[es:di+0x00]
	and		ax,0x0080			; linear framebuffer mode supported ?
	jz		_set_vga			; not supported


_set_vbe:
; --- provisional ---------
;jmp	_set_vga
;--------------------------
	mov		ax,0x4f02	; VESA SuperVGA, 
;	mov		bx,0x4105	; 4000h(linear framebuffer enable) 
;						; + 105h(1024 x 768 x 8bit color)
	mov		bx,0x4103	; 4000h(linear framebuffer enable)
						; + 103h( 800 x 600 x 8bit color)
	int		0x10
	
	; write video info
	mov		ax,[es:di+0x12]
	mov		[BOOT_INFO_ADDR+0],ax	; screen width
	mov		ax,[es:di+0x14]
	mov		[BOOT_INFO_ADDR+2],ax	; screen height
	mov		eax,[es:di+0x28]
	mov		[BOOT_INFO_ADDR+4],eax	; vram(linear video buffer)
;	mov		eax,[es:di+0x2c]
;	mov		[BOOT_INFO_ADDR+8],eax	; offscreeen momory
;	mov		ax,[es:di+0x30]
;	mov		[BOOT_INFO_ADDR+12],ax	; KB of offscreen memory
	
	ret


_set_vga:
	mov		al,0x13		; VGA graphic, 320*200*8bit color, packed pixcel
	mov		ah,0x00		; 0x00
	int		0x10
	
	; write video info
	mov		ax,320
	mov		[BOOT_INFO_ADDR+0],ax	; screen width
	mov		ax,200
	mov		[BOOT_INFO_ADDR+2],ax	; screen height
	mov		eax,0x0a0000
	mov		[BOOT_INFO_ADDR+4],eax	; vram
	
	ret

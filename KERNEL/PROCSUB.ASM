;
; procsub.asm - プロセス管理
; Copyright (C) S.Ebisawa, 1995. All rights reserved.
;
	extrn	_sys_call	: near
	extrn	_KernelStack	: dword
	extrn	_Regs		: dword

	public	_x_start_proc
	public	_x_stop_proc
	public	_sys_entry
	public	_xreturn_sub

_DATA	segment public 'DATA'
_DATA	ends
_BSS	segment public 'BSS'
_BSS	ends

DGROUP	group	_DATA, _BSS

_TEXT	segment public 'CODE'
	assume	cs:_TEXT, ds:DGROUP

proc_list struc
r_sp	dw	?
r_ss	dw	?
proc_list ends

_x_start_proc proc far
	mov	word ptr [_KernelStack], sp
	mov	word ptr [_KernelStack+2], ss
	mov	bp, sp
	les	di, [bp+4]		; es:di = next
	mov	ss, es:[di].r_ss
	mov	sp, es:[di].r_sp

	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop	es
	pop	ds
	pop	si
	pop	di
	pop	bp
	iret
_x_start_proc endp


_x_stop_proc proc far
	mov	ss, word ptr [_KernelStack+2]
	mov	sp, word ptr [_KernelStack]
	ret
_x_stop_proc endp

;
; システムコールエントリ
;
_sys_entry proc far
	sti
	push	bp
	push	di
	push	si
	push	ds
	push	es
	push	dx
	push	cx
	push	bx
	push	ax

	mov	ax, DGROUP
	mov	ds, ax

	mov	word ptr [_Regs], sp
	mov	word ptr [_Regs+2], ss

	call	_sys_call

	mov	ss, dx
	mov	sp, ax

	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop	es
	pop	ds
	pop	si
	pop	di
	pop	bp
	iret
_sys_entry endp

_xreturn_sub proc far
	pop	ax
	pop	bx
	pushf
	push	bx
	push	ax

	push	bp
	push	di
	push	si
	push	ds
	push	es
	push	dx
	push	cx
	push	bx
	push	ax

	mov	bp, sp
	les	bx, [bp+28]
	mov	es:[bx], ss
	les	bx, [bp+32]
	mov	es:[bx], sp
	mov	ax, [bp+24]
	mov	bx, [bp+26]
	mov	ss, ax
	mov	sp, bx

	pop	ax
	pop	bx
	pop	cx
	pop	dx
	pop	es
	pop	ds
	pop	si
	pop	di
	pop	bp
	iret
_xreturn_sub endp


_TEXT	ends
	end


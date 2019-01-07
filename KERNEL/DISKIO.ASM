; 
; diskio.asm - ディスク入出力
; Copyright (C) S.Ebisawa, 1995. All rights reserved.
;
_TEXT	segment public 'CODE'
	assume	cs:_TEXT

	public	_create_file_dos
	public	_open_file_dos
	public	_close_file_dos
	public	_read_file_dos
	public	_write_file_dos
	public	_seek_file_dos

_create_file_dos proc near
	push	bp
	push	ds
	mov	bp, sp
	lds	dx, [bp+6]
	mov	cx, 20h
	mov	ah, 3ch
	int	21h
	jnb	cref_noerror
	mov	ax, 0
cref_noerror:
	pop	ds
	pop	bp
	ret
_create_file_dos endp

_open_file_dos	proc near
	push	bp
	push	ds
	mov	bp, sp
	lds	dx, [bp+6]
	mov	ax, [bp+10]
	mov	ah, 3dh
	int	21h
	jnb	of_noerror
	mov	ax, 0
of_noerror:
	pop	ds
	pop	bp
	ret
_open_file_dos	endp

_close_file_dos proc near
	push	bp
	mov	bp, sp
	mov	bx, [bp+4]
	mov	ah, 3eh
	int	21h
	jnb	cf_noerror
	mov	ax, 0
cf_noerror:
	pop	bp
	ret
_close_file_dos endp

_read_file_dos	proc near
	push	bp
	push	ds
	mov	bp, sp
	lds	dx, [bp+6]
	mov	cx, [bp+10]
	mov	bx, [bp+12]
	mov	ah, 3fh
	int	21h
	jnb	rf_noerror
	mov	ax, 0
rf_noerror:
	pop	ds
	pop	bp
	ret
_read_file_dos	endp

_write_file_dos	proc near
	push	bp
	push	ds
	mov	bp, sp
	lds	dx, [bp+6]
	mov	cx, [bp+10]
	mov	bx, [bp+12]
	mov	ah, 40h
	int	21h
	jnb	wf_noerror
	mov	ax, 0
wf_noerror:
	pop	ds
	pop	bp
	ret
_write_file_dos	endp

_seek_file_dos	proc near
	push	bp
	mov	bp, sp
	mov	bx, [bp+4]
	mov	dx, [bp+6]
	mov	cx, [bp+8]
	mov	ax, [bp+10]
	mov	ah, 42h
	int	21h
	jnb	sf_noerror
	mov	ax, 0
	mov	dx, 0
sf_noerror:
	pop	bp
	ret
_seek_file_dos	endp

_TEXT	ends
	end


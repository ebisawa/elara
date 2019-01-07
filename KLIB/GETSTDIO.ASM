_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getstdio

_getstdio proc	near
	push	bp
	push	ds
	mov	bp, sp
	mov	ax, getstdio
	mov	bx, [bp+6]
	lds	cx, [bp+8]
	les	dx, [bp+12]
	int	kernel_call
	pop	ds
	pop	bp
	ret
_getstdio endp

_TEXT	ends
	end


_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_write

_write	proc	near
	push	bp
	mov	bp, sp
	mov	ax, write
	les	bx, [bp+4]
	mov	cx, [bp+8]
	mov	dx, [bp+10]
	int	kernel_call
	pop	bp
	ret
_write	endp

_TEXT	ends
	end


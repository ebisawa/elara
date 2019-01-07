_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_open

_open	proc	near
	push	bp
	mov	bp, sp
	mov	ax, open
	les	bx, [bp+4]
	mov	cx, [bp+8]
	int	kernel_call
	pop	bp
	ret
_open	endp

_TEXT	ends
	end


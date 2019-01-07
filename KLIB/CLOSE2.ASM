_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_close2

_close2	proc	near
	push	bp
	mov	bp, sp
	mov	ax, close2
	mov	bx, [bp+4]
	mov	cx, [bp+6]
	int	kernel_call
	pop	bp
	ret
_close2	endp

_TEXT	ends
	end


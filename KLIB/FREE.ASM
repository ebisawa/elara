_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_free

_free	proc	near
	push	bp
	mov	bp, sp
	mov	ax, free
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_free	endp

_TEXT	ends
	end


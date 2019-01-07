_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_remove

_remove	proc	near
	push	bp
	mov	bp, sp
	mov	ax, remove
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_remove	endp

_TEXT	ends
	end


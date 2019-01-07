_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_xfree

_xfree	proc	near
	push	bp
	mov	bp, sp
	mov	ax, xfree
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_xfree	endp

_TEXT	ends
	end


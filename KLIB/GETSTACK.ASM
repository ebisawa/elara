_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getstack

_getstack proc	near
	push	bp
	mov	bp, sp
	mov	ax, getstack
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getstack endp

_TEXT	ends
	end


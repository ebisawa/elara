_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_xmalloc

_xmalloc proc	near
	push	bp
	mov	bp, sp
	mov	ax, xmalloc
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_xmalloc endp

_TEXT	ends
	end


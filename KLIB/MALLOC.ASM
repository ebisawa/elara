_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_malloc

_malloc proc	near
	push	bp
	mov	bp, sp
	mov	ax, malloc
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_malloc endp

_TEXT	ends
	end


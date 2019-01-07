_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getname

_getname proc	near
	push	bp
	mov	bp, sp
	mov	ax, getname
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getname endp

_TEXT	ends
	end


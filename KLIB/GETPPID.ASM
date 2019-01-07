_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getppid

_getppid proc	near
	push	bp
	mov	bp, sp
	mov	ax, getppid
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getppid endp

_TEXT	ends
	end


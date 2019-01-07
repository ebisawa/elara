_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_system

_system proc	near
	push	bp
	mov	bp, sp
	mov	ax, system
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_system endp

_TEXT	ends
	end


_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getprio

_getprio proc	near
	push	bp
	mov	bp, sp
	mov	ax, getprio
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getprio endp

_TEXT	ends
	end


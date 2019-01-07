_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_iolimit

_iolimit proc	near
	push	bp
	mov	bp, sp
	mov	ax, iolimit
	les	bx, [bp+4]
	mov	cx, [bp+8]
	int	kernel_call
	pop	bp
	ret
_iolimit endp

_TEXT	ends
	end


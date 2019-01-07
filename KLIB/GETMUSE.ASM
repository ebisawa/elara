_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getmuse

_getmuse proc	near
	push	bp
	mov	bp, sp
	mov	ax, getmuse
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getmuse endp

_TEXT	ends
	end


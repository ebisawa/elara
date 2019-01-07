_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_deldev

_deldev proc	near
	push	bp
	mov	bp, sp
	mov	ax, deldev
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_deldev endp

_TEXT	ends
	end


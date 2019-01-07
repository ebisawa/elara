_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getwait2

_getwait2 proc	near
	push	bp
	mov	bp, sp
	mov	ax, getwait2
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getwait2 endp

_TEXT	ends
	end


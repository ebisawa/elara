_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getwait1

_getwait1 proc	near
	push	bp
	mov	bp, sp
	mov	ax, getwait1
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getwait1 endp

_TEXT	ends
	end


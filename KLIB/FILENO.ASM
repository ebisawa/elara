_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_fileno

_fileno proc near
	push	bp
	mov	bp, sp
	mov	ax, fileno
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_fileno endp

_TEXT	ends
	end


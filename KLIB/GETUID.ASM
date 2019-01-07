_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getuid

_getuid proc	near
	push	bp
	mov	bp, sp
	mov	ax, getuid
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getuid endp

_TEXT	ends
	end


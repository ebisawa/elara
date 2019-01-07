_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_setuid

_setuid proc	near
	push	bp
	mov	bp, sp
	mov	ax, setuid
	mov	bx, [bp+4]
	les	cx, [bp+6]
	int	kernel_call
	pop	bp
	ret
_setuid endp

_TEXT	ends
	end


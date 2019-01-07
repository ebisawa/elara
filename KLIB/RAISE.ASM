_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_raise

_raise	proc	near
	push	bp
	mov	bp, sp
	mov	ax, raise
	mov	bx, [bp+4]
	mov	cx, [bp+6]
	int	kernel_call
	pop	bp
	ret
_raise	endp

_TEXT	ends
	end


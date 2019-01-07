_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_mkdir

_mkdir	proc	near
	push	bp
	mov	bp, sp
	mov	ax, mkdir
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_mkdir	endp

_TEXT	ends
	end


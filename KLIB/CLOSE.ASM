_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_close

_close	proc	near
	push	bp
	mov	bp, sp
	mov	ax, close
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_close	endp

_TEXT	ends
	end


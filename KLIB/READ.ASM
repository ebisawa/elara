_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_read

_read	proc	near
	push	bp
	mov	bp, sp
	mov	ax, read
	les	bx, [bp+4]
	mov	cx, [bp+8]
	mov	dx, [bp+10]
	int	kernel_call
	pop	bp
	ret
_read	endp

_TEXT	ends
	end


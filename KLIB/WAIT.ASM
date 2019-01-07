_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_wait

_wait	proc	near
	push	bp
	mov	bp, sp
	mov	ax, wait
	mov	bx, [bp+4]
	mov	cx, [bp+6]
	int	kernel_call
	pop	bp
	ret
_wait	endp

_TEXT	ends
	end


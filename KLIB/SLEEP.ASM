_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_sleep

_sleep	proc	near
	push	bp
	mov	bp, sp
	mov	ax, sleep
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_sleep	endp

_TEXT	ends
	end


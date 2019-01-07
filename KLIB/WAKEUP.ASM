_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_wakeup

_wakeup proc	near
	push	bp
	mov	bp, sp
	mov	ax, wakeup
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_wakeup endp

_TEXT	ends
	end


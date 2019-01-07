_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_setprio

_setprio proc	near
	push	bp
	mov	bp, sp
	mov	ax, setprio
	mov	bx, [bp+4]
	mov	cx, [bp+6]
	int	kernel_call
	pop	bp
	ret
_setprio endp

_TEXT	ends
	end


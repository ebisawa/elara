_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_procsw

_procsw	proc	near
	push	bp
	mov	bp, sp
	mov	ax, procsw
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_procsw	endp

_TEXT	ends
	end


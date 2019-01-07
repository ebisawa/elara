_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getstat

_getstat proc	near
	push	bp
	mov	bp, sp
	mov	ax, getstat
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getstat endp

_TEXT	ends
	end


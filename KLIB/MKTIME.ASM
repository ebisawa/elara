_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_mktime

_mktime proc near
	push	bp
	mov	bp, sp
	mov	ax, mktime
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_mktime endp

_TEXT	ends
	end


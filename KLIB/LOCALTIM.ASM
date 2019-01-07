_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_localtime

_localtime proc near
	push	bp
	mov	bp, sp
	mov	ax, localtime
	les	bx, [bp+4]
	mov	dx, [bp+8]
	mov	cx, [bp+10]
	int	kernel_call
	pop	bp
	ret
_localtime endp

_TEXT	ends
	end


_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_ctime

_ctime	proc near
	push	bp
	mov	bp, sp
	mov	ax, ctime
	mov	cx, [bp+4]
	mov	bx, [bp+6]
	int	kernel_call
	pop	bp
	ret
_ctime	endp

_TEXT	ends
	end


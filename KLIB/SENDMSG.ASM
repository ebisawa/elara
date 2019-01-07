_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_sendmsg

_sendmsg proc near
	push	bp
	push	si
	mov	bp, sp
	mov	ax, sendmsg
	mov	bx, [bp+6]
	mov	cx, [bp+10]
	mov	dx, [bp+8]
	les	si, [bp+12]
	int	kernel_call
	pop	si
	pop	bp
	ret
_sendmsg endp

_TEXT	ends
	end


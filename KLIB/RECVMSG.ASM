_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_recvmsg

_recvmsg proc near
	push	bp
	push	ds
	push	si
	mov	bp, sp
	mov	ax, recvmsg
	lds	bx, [bp+8]
	les	cx, [bp+12]
	mov	dx, [bp+16]
	mov	si, [bp+18]
	int	kernel_call
	pop	si
	pop	ds
	pop	bp
	ret
_recvmsg endp

_TEXT	ends
	end


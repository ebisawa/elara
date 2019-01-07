_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_ioctl

_ioctl	proc	near
	push	bp
	push	ds
	mov	bp, sp
	mov	ax, ioctl
	lds	bx, [bp+6]
	mov	cx, [bp+10]
	les	dx, [bp+12]
	int	kernel_call
	pop	ds
	pop	bp
	ret
_ioctl	endp

_TEXT	ends
	end


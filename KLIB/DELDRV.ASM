_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_deldrv

_deldrv proc	near
	push	bp
	mov	bp, sp
	mov	ax, deldrv
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_deldrv endp

_TEXT	ends
	end


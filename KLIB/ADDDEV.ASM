_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_adddev

_adddev proc	near
	push	bp
	push	ds
	mov	bp, sp
	mov	ax, adddev
	lds	bx, [bp+6]
	les	cx, [bp+10]
	int	kernel_call
	pop	ds
	pop	bp
	ret
_adddev endp

_TEXT	ends
	end


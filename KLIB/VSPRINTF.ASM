_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_vsprintf

_vsprintf proc	near
	push	bp
	push	ds
	push	si
	mov	bp, sp
	mov	ax, vsprintf
	lds	bx, [bp+8]
	les	cx, [bp+12]
	mov	dx, [bp+16]
	mov	si, [bp+18]
	int	kernel_call
	pop	si
	pop	ds
	pop	bp
	ret
_vsprintf endp

_TEXT	ends
	end


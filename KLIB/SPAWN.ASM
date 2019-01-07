_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_spawn

_spawn	proc	near
	push	bp
	push	ds
	mov	bp, sp
	mov	ax, spawn
	mov	bx, [bp+6]
	lds	cx, [bp+8]
	les	dx, [bp+12]
	int	kernel_call
	pop	ds
	pop	bp
	ret
_spawn	endp

_TEXT	ends
	end


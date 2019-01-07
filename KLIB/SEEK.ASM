_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_seek

_seek	proc	near
	push	bp
	push	si
	mov	bp, sp
	mov	ax, seek
	mov	bx, [bp+6]
	mov	dx, [bp+8]
	mov	cx, [bp+10]
	mov	si, [bp+12]
	int	kernel_call
	pop	si
	pop	bp
	ret
_seek	endp

_TEXT	ends
	end


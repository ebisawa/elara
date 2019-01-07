_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_rename

_rename proc near
	push	bp
	push	ds
	mov	bp, sp
	mov	ax, rename
	lds	bx, [bp+6]
	les	cx, [bp+10]
	int	kernel_call
	pop	ds
	pop	bp
	ret
_rename endp

_TEXT	ends
	end


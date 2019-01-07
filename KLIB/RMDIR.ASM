_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_rmdir

_rmdir	proc	near
	push	bp
	mov	bp, sp
	mov	ax, rmdir
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_rmdir	endp

_TEXT	ends
	end


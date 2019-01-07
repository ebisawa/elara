_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_disp

_disp	proc	near
	push	bp
	mov	bp, sp
	mov	ax, disp
	les	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_disp	endp

_TEXT	ends
	end


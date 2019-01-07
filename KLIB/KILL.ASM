_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_kill

_kill	proc	near
	push	bp
	mov	bp, sp
	mov	ax, kill
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_kill	endp

_TEXT	ends
	end


_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_exec

_exec	proc	near
	push	bp
	push	ds
	mov	bp, sp
	mov	ax, exec
	lds	bx, [bp+6]
	les	cx, [bp+10]
	int	kernel_call
	pop	ds
	pop	bp
_exec	endp

_TEXT	ends
	end


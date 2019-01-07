_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_setpflag

_setpflag proc near
	push	bp
	mov	bp, sp
	mov	ax, setpflag
	mov	bx, [bp+4]
	mov	cx, [bp+6]
	int	kernel_call
	pop	bp
	ret
_setpflag endp

_TEXT	ends
	end


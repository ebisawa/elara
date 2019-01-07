_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getpflag

_getpflag proc near
	push	bp
	mov	bp, sp
	mov	ax, getpflag
	mov	bx, [bp+4]
	int	kernel_call
	pop	bp
	ret
_getpflag endp

_TEXT	ends
	end


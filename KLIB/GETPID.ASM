_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getpid

_getpid proc	near
	mov	ax, getpid
	int	kernel_call
	ret
_getpid endp

_TEXT	ends
	end


_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_getargv

_getargv proc	near
	mov	ax, getargv
	int	kernel_call
	ret
_getargv endp

_TEXT	ends
	end


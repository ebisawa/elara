_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_firstpid

_firstpid proc	near
	mov	ax, firstpid
	int	kernel_call
	ret
_firstpid endp

_TEXT	ends
	end


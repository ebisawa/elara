_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_time

_time	proc	near
	mov	ax, time
	int	kernel_call
	ret
_time	endp

_TEXT	ends
	end


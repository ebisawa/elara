_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_relcpu

_relcpu proc	near
	mov	ax, relcpu
	int	kernel_call
	ret
_relcpu endp

_TEXT	ends
	end


_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_checkmsg

_checkmsg proc	near
	mov	ax, checkmsg
	int	kernel_call
	ret
_checkmsg endp

_TEXT	ends
	end


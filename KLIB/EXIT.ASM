_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_exit

; この関数からは帰ってこない
_exit	proc	near
	mov	ax, exit
	int	kernel_call
_exit	endp

_TEXT	ends
	end


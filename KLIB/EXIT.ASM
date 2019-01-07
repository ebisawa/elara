_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_exit

; ‚±‚ÌŠÖ”‚©‚ç‚Í‹A‚Á‚Ä‚±‚È‚¢
_exit	proc	near
	mov	ax, exit
	int	kernel_call
_exit	endp

_TEXT	ends
	end


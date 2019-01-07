_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	include kernel.inc

	public	_shutdown

; ‚±‚ÌŠÖ”‚©‚ç‚Í‹A‚Á‚Ä‚±‚È‚¢
_shutdown proc	near
	mov	bp, sp
	mov	ax, shutdown
	mov	bx, [bp+2]
	int	kernel_call
_shutdown endp

_TEXT	ends
	end


_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	public	_rstds
	extrn	dssave	: word

_rstds	proc near
	mov	ax, cs:dssave
	mov	ds, ax
	ret
_rstds	endp

_TEXT	ends
	end


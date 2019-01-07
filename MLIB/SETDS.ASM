_TEXT	segment public byte 'CODE'
	assume	cs:_TEXT

	public	_setds
	public	dssave

_setds	proc near
	mov	cs:dssave, ds
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	ret
_setds	endp

dssave	dw	0

_TEXT	ends
	end


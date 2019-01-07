_TEXT	segment public byte 'CODE'
_TEXT	ends

_DATA	segment public byte 'DATA'
_DATA	ends

_BSS	segment public byte 'BSS'
_BSS	ends

DGROUP	group	_TEXT, _DATA, _BSS

	end

_TEXT	segment public byte 'CODE'
_TEXT	ends

_DATA	segment public byte 'DATA'
_DATA	ends

_BSS	segment public byte 'BSS'
_BSS	ends

DGROUP	group	_TEXT, _DATA, _BSS

	extrn	_main	: near
	extrn	_exit	: near

_TEXT	segment public 'CODE'
	assume	cs:DGROUP, ds:DGROUP
	org	0h

startup:
	call	_main
	call	_exit

_TEXT	ends
	end	startup


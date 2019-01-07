;
; dosmem.asm - DOSƒƒ‚ƒŠ
; Copyright (C) S.Ebisawa, 1995. All rights reserved.
;
_TEXT	segment public 'CODE'
	assume	cs:_TEXT

; ƒƒ‚ƒŠŠm•Û
; unsigned near dosmem_alloc(unsigned parasize);
;
	public	_dosmem_alloc
	public	_x_dosmem_alloc

_dosmem_alloc	proc near
	push	bp
	mov	bp, sp
	call	strategy_high
	mov	ah, 48h
	mov	bx, [bp+4]		; BX <- parasize
	int	21h
	jnb	noerr
	mov	ax, 0
noerr:
	push	ax
	call	strategy_low
	pop	ax
	pop	bp
	ret
_dosmem_alloc	endp

_x_dosmem_alloc proc near
	push	bp
	mov	bp, sp
	mov	ah, 48h
	mov	bx, [bp+4]		; BX <- parasize
	int	21h
	jnb	noerr_x
	mov	ax, 0
noerr_x:
	pop	bp
	ret
_x_dosmem_alloc endp

; ƒƒ‚ƒŠ‰ğ•ú
; void near dosmem_free(unsigned segm);
;
	public	_dosmem_free

_dosmem_free	proc near
	push	bp
	mov	bp, sp
	mov	ax, [bp+4]		; AX <- segm
	mov	es, ax
	mov	ah, 49h
	int	21h
	pop	bp
	ret
_dosmem_free	endp


strategy_low	proc near
	mov	ax, 5801h
	mov	bx, 0
	int	21h
	ret
strategy_low	endp

strategy_high	proc near
	mov	ax, 5801h
	mov	bx, 2
	int	21h
	ret
strategy_high	endp

_TEXT	ends
	end


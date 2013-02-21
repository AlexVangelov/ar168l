;---------------------------------------------------------------------------
;   Math functions copy right information
;
;   Copyright (C) 2006-2008. Lin, Rongrong <woody@palmmicro.com>
;
;   This library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Lesser General Public
;   License as published by the Free Software Foundation; either
;   version 2.1 of the License, or (at your option) any later version.
;
;   This library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Lesser General Public License for more details.
;
;   You should have received a copy of the GNU Lesser General Public
;   License along with this library; if not, write to the Free Software
;   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
;
;   Part of code and remark lines are from: 
;   Learn TI-83 Plus Assembly In 28 Days v2.0
;---------------------------------------------------------------------------

	.module math
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _mul_8x8
	.globl _mul_16x8
	.globl _mul_32x8
	.globl _div_16x8
	.globl _mod_16x8
;	.globl _mod_32x8
	.globl _rr_32x8
	.globl _rl_32x8
	.globl _rlc_32x8
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE0

; ---------------------------------
; Function mul_8x8(UCHAR i8, UCHAR i8)
; 1.Shift the multiplier right to check the least-significant bit. 
; 2.If the carry flag is set, add the multiplicand to our running total. 
; 3.Regardless of whether there was an addition, shift the original multiplicand left. 
; 4.Repeat for each bit in the multiplier
; ---------------------------------
_mul_8x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	h,4(ix)
	ld	e,5(ix)
	ld	b,#8			; Eight bits to check
	xor	a,a
	ld	d,a
	ld	l,a			; Use HL to store the product
mul_8x8_loop:
	add	hl,hl			; Get most-significant bit of HL
	jr	nc,mul_8x8_skip
	add	hl,de
mul_8x8_skip:
	djnz	mul_8x8_loop
	pop	ix
	ret

; ---------------------------------
; Function mul_16x8(USHORT i16, UCHAR i8)
; ---------------------------------
_mul_16x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl,#-2
	add	hl,sp
	ld	sp,hl
; calculate P_HIBYTE(i16)*i8
	ld	a,6(ix)
	push	af
	inc	sp
	ld	a,5(ix)
	push	af
	inc	sp
	call	_mul_8x8
	pop	af
	ld	-2(ix),l
	ld	-1(ix),h
; calculate P_LOBYTE(i16)*i8
	ld	a,6(ix)
	push	af
	inc	sp
	ld	a,4(ix)
	push	af
	inc	sp
	call	_mul_8x8
	pop	af
; add	
	ld	a,-2(ix)
	add	a,h
	ld	h,a
	ld	a,-1(ix)
	adc	a,#0
	ld	e,a
	ld	a,#0
	adc	a,#0
	ld	d,a
	ld	sp,ix
	pop	ix
	ret

; ---------------------------------
; Function mul_32x8(ULONG l32, UCHAR i8)
; ---------------------------------
_mul_32x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	hl,#-2
	add	hl,sp
	ld	sp,hl
; calculate P_HIWORD(l32)*i8
	ld	a,8(ix)
	push	af
	inc	sp
	ld	l,6(ix)
	ld	h,7(ix)
	push	hl
	call	_mul_16x8
	pop	af
	inc	sp
	ld	-1(ix),h
	ld	-2(ix),l
; calculate P_LOWORD(l32)*i8
	ld	a,8(ix)
	push	af
	inc	sp
	ld	l,4(ix)
	ld	h,5(ix)
	push	hl
	call	_mul_16x8
	pop	af
	inc	sp
; add	
	ld	a,-2(ix)
	add	a,e
	ld	e,a
	ld	a,-1(ix)
	adc	a,d
	ld	d,a	
	ld	sp,ix
	pop	ix
	ret

; ---------------------------------
; Function div_16x8(USHORT i16, UCHAR i8)
; for dividing a number in n bits by a number in m bits, 
; 1.Shift the dividend left one bit. 
; 2.Shift the carry out into a temp area of size m+1 bits. 
; 3.See if the value of the temp area is greater than or equal to the divisor. 
; 4.If it is, subtract the divisor from the temp area and set the lsb of the dividend. 
; 5.Repeat n times. 
; ---------------------------------
_div_16x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
	ld	d,6(ix)
	xor	a,a			; Clear upper eight bits of AHL
	ld	b,#16			; Sixteen bits in dividend
div_16x8_loop:
	add	hl,hl			; Do a SLA HL
	rla				; This moves the upper bits of the dividend into A
	jr	c,div_16x8_overflow
	cp	d			; Check if we can subtract the divisor
	jr	c,div_16x8_skip   	; Carry means D > A
div_16x8_overflow:
	sub	d			; Do subtraction for real this time
	inc	l			; Set bit 0 of quotient
div_16x8_skip:
	djnz	div_16x8_loop
	pop	ix
	ret

; ---------------------------------
; Function mod_16x8(USHORT i16, UCHAR i8)
; same as div_16x8
; ---------------------------------
_mod_16x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
	ld	d,6(ix)
	xor	a,a			; Clear upper eight bits of AHL
	ld	b,#16			; Sixteen bits in dividend
mod_16x8_loop:
	add	hl,hl			; Do a SLA HL
	rla				; This moves the upper bits of the dividend into A
	jr	c,mod_16x8_overflow
	cp	d			; Check if we can subtract the divisor
	jr	c,mod_16x8_skip   	; Carry means D > A
mod_16x8_overflow:
	sub	d			; Do subtraction for real this time
	inc	l			; Set bit 0 of quotient
mod_16x8_skip:
	djnz	mod_16x8_loop
	ld	l,a
	pop	ix
	ret

.if 0
; ---------------------------------
; Function mod_32x8(ULONG l32, UCHAR i8)
; ---------------------------------
_mod_32x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	pop	ix
	ret
.endif

; ---------------------------------
; Function rr_32x8(ULONG l32, UCHAR i8)
; ---------------------------------
_rr_32x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	a,8(ix)
	ld	d,7(ix)
	ld	e,6(ix)
	ld	h,5(ix)
	ld	l,4(ix)
rr_32x8_loop1:
	or	a,a
	jr	z,rr_32x8_loop2
	rr	d
	rr	e
	rr	h
	rr	l
	dec	a
	jr	rr_32x8_loop1
rr_32x8_loop2:
	pop	ix
	ret

; ---------------------------------
; Function rl_32x8(ULONG l32, UCHAR i8)
; ---------------------------------
_rl_32x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	a,8(ix)
	ld	d,7(ix)
	ld	e,6(ix)
	ld	h,5(ix)
	ld	l,4(ix)
rl_32x8_loop1:
	or	a,a
	jr	z,rl_32x8_loop2
	rl	l
	rl	h
	rl	e
	rl	d
	dec	a
	jr	rl_32x8_loop1
rl_32x8_loop2:
	pop	ix
	ret

; ---------------------------------
; Function rlc_32x8(ULONG l32, UCHAR i8)
; ---------------------------------
_rlc_32x8:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	c,8(ix)
	ld	d,7(ix)
	ld	e,6(ix)
	ld	h,5(ix)
	ld	l,4(ix)
	ld	b,#0
rlc_32x8_loop1:
	ld	a,c
	or	a,a
	jr	z,rlc_32x8_loop2
	rl	l
	rl	h
	rl	e
	rl	d
	ld	a,b
	adc	a,l
	ld	l,a
	dec	c
	jr	rlc_32x8_loop1
rlc_32x8_loop2:
	pop	ix
	ret


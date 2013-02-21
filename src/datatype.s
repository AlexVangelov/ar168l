;---------------------------------------------------------------------------
;   Basic data type and string related functions copy right information
;
;   Copyright (C) 2006-2008. Lin, Rongrong <woody@palmmmicro.com>
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


	.module datatype
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl __sdcc_call_hl
	.globl _P_LOBYTE
	.globl _P_HIBYTE
	.globl _P_XCHGBYTE
	.globl _P_LOWORD
	.globl _P_HIWORD
	.globl _P_MAKEWORD
	.globl _P_MAKELONG
	.globl _PCHAR2USHORT
	.globl _PCHAR2USHORT_L
	.globl _USHORT2PCHAR
	.globl _USHORT2PCHAR_L
	.globl _PCHAR2ULONG
	.globl _PCHAR2ULONG_L
	.globl _ULONG2PCHAR
;	.globl _ULONG2PCHAR_L
	.globl _strchr
	.globl _strlen
	.globl _strcmp
	.globl _strcmp_lowercase
	.globl _strcpy
	.globl _strcat
	.globl _strcat_char
	.globl _memset
	.globl _memcmp
	.globl _memcmp_lowercase
	.globl _memcmp_str
	.globl _memcpy
	.globl _memcpy4
	.globl _memmove
;--------------------------------------------------------
; code
;--------------------------------------------------------
	
	.area _CODE0
	
; The Z80 has the jp (hl) instruction, which is perfect for implementing function pointers.

__sdcc_call_hl:
	jp	(hl)

	
; ---------------------------------
; Function P_LOBYTE(USHORT worddata)
; ---------------------------------
_P_LOBYTE:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	pop	ix
	ret

; ---------------------------------
; Function P_HIBYTE(USHORT worddata)
; ---------------------------------
_P_HIBYTE:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,5(ix)
	pop	ix
	ret

; ---------------------------------
; Function USHORT P_XCHGBYTE(USHORT worddata);
; ---------------------------------
_P_XCHGBYTE:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	h,4(ix)
	ld	l,5(ix)
	pop	ix
	ret

; ---------------------------------
; Function P_LOWORD(ULONG longdata)
; ---------------------------------
_P_LOWORD:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
	pop	ix
	ret

; ---------------------------------
; Function P_HIWORD(ULONG longdata)
; ---------------------------------
_P_HIWORD:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,6(ix)
	ld	h,7(ix)
	pop	ix
	ret

; ---------------------------------
; Function P_MAKEWORD(UCHAR lowbyte, UCHAR highbyte)
; ---------------------------------
_P_MAKEWORD:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
	pop	ix
	ret


; ---------------------------------
; Function P_MAKELONG(USHORT lowword, USHORT highword)
; ---------------------------------
_P_MAKELONG:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
	ld	e,6(ix)
	ld	d,7(ix)
	pop	ix
	ret

; ---------------------------------
; Function PCHAR2USHORT(PCHAR src)
; ---------------------------------
_PCHAR2USHORT:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	c,4(ix)
	ld	b,5(ix)
	ld	a,(bc)
	ld	h,a
	inc	bc
	ld	a,(bc)
	ld	l,a
	pop	ix
	ret

; ---------------------------------
; Function PCHAR2USHORT_L(PCHAR src)
; ---------------------------------
_PCHAR2USHORT_L:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	c,4(ix)
	ld	b,5(ix)
	ld	a,(bc)
	ld	l,a
	inc	bc
	ld	a,(bc)
	ld	h,a
	pop	ix
	ret

; ---------------------------------
; Function USHORT2PCHAR(USHORT src, PCHAR dst)
; ---------------------------------
_USHORT2PCHAR:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,6(ix)
	ld	d,7(ix)
	ld	a,5(ix)
	ld	(de),a
	inc	de
	ld	a,4(ix)
	ld	(de),a
	pop	ix
	ret

; ---------------------------------
; Function USHORT2PCHAR_L(USHORT src, PCHAR dst)
; ---------------------------------
_USHORT2PCHAR_L:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,6(ix)
	ld	d,7(ix)
.if 1
; none 'undocumented' instruction implementation
	ld	a,4(ix)
	ld	(de),a
	inc	de
	ld	a,5(ix)
	ld	(de),a
.else
	ld	bc,#0x0002
	ld	hl,#0x0004
	add	hl,sp
	ldir
.endif
	pop	ix
	ret

; ---------------------------------
; Function PCHAR2ULONG(PCHAR src)
; ---------------------------------
_PCHAR2ULONG:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	c,4(ix)
	ld	b,5(ix)
	ld	a,(bc)
	ld	d,a
	inc	bc
	ld	a,(bc)
	ld	e,a
	inc	bc
	ld	a,(bc)
	ld	h,a
	inc	bc
	ld	a,(bc)
	ld	l,a
	pop	ix
	ret

; ---------------------------------
; Function PCHAR2ULONG_L(PCHAR src)
; ---------------------------------
_PCHAR2ULONG_L:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	c,4(ix)
	ld	b,5(ix)
	ld	a,(bc)
	ld	l,a
	inc	bc
	ld	a,(bc)
	ld	h,a
	inc	bc
	ld	a,(bc)
	ld	e,a
	inc	bc
	ld	a,(bc)
	ld	d,a
	pop	ix
	ret

; ---------------------------------
; Function ULONG2PCHAR(ULONG src, PCHAR dst)
; ---------------------------------
_ULONG2PCHAR:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,8(ix)
	ld	d,9(ix)
	ld	a,7(ix)
	ld	(de),a
	inc	de
	ld	a,6(ix)
	ld	(de),a
	inc	de
	ld	a,5(ix)
	ld	(de),a
	inc	de
	ld	a,4(ix)
	ld	(de),a
	pop	ix
	ret

; ---------------------------------
; Function ULONG2PCHAR_L(ULONG src, PCHAR dst)
; ---------------------------------
.if 0
_ULONG2PCHAR_L:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,8(ix)
	ld	d,9(ix)
.if 0
; none 'undocumented' instruction implementation
	ld	a,4(ix)
	ld	(de),a
	inc	de
	ld	a,5(ix)
	ld	(de),a
	inc	de
	ld	a,6(ix)
	ld	(de),a
	inc	de
	ld	a,7(ix)
	ld	(de),a
.else
	ld	bc,#0x0004
	ld	hl,#0x0004
	add	hl,sp
	ldir
.endif
	pop	ix
	ret
.endif

; ---------------------------------
; Function strchr(PCHAR string, UCHAR ch)
; ---------------------------------
_strchr:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	l,4(ix)
	ld	h,5(ix)
.if 0
	ld	b,6(ix)
strchr_loop1:
	ld	a,(hl)
	or	a,a
	jr	z,strchr_fork1
	cp	b
	jr	z,strchr_fork2
	inc	hl
	jr	strchr_loop1
strchr_fork1:
.else
; find length first	
	xor	a,a
	ld	b,a
	ld	c,a
	cpir
	ld	h,a
	ld	l,a
	sbc	hl,bc
	ld	b,h
	ld	c,l
; find the char, using length in BC	
	ld	l,4(ix)
	ld	h,5(ix)
	ld	a,6(ix)
	cpir
	dec	hl
	jr	z,strchr_fork2
.endif
	ld	hl,#0x0000
strchr_fork2:
	pop	ix
	ret

; ---------------------------------
; Function strlen(PCHAR pStr)
; ---------------------------------
_strlen:
	push	ix
	ld	ix,#0
	add	ix,sp
.if 0
; none 'undocumented' instruction implementation
	ld	hl,#0x0000
	ld	e,4(ix)
	ld	d,5(ix)
strlen_loop1:
	ld	a,(de)
	inc	de
	or	a,a
	jr	z,strlen_loop2
	inc	hl
	jr	strlen_loop1
strlen_loop2:
.else
	ld	l,4(ix)
	ld	h,5(ix)
	xor	a,a		; zero is the value we are looking for	
	ld	b,a
	ld	c,a		; put 0 in BC to search 65, 536 bytes (the entire addressable memory space).
	cpir			; begin search for a byte equalling zero.
; BC has been decremented so that it holds -length. Now need to synthesize a NEG BC.
	ld	h,a             ; zero HL (basically set it to 65, 536) to get the
	ld	l,a             ; number of bytes
	sbc	hl,bc           ; find the size. CPIR doesn't affect carry.
	dec	hl              ; Compensate for null.	
.endif	
	pop	ix
	ret

; ---------------------------------
; Function strcmp(PCHAR dst, PCHAR src)
; ---------------------------------
_strcmp:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
strcmp_loop1:
	ld	a,(de)
	inc	de
	cpi
	jr	nz,strcmp_loop2
	or	a,a
	jr	nz,strcmp_loop1
	ld	l,#0
	jr	strcmp_loop3
strcmp_loop2:
	ld	l,#1
strcmp_loop3:	
;	ld	h,#0
	pop	ix
	ret

; ---------------------------------
; Function strcmp_lowercase(PCHAR dst, PCHAR src)
; ---------------------------------
_strcmp_lowercase:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
strcmp_lowercase_loop1:
	ld	a,(de)
	; if (iChar >= 'A' && iChar <= 'Z')
	cp	a,#0x41
	jr	C,strcmp_lowercase_fork1
	cp	a,#0x5B
	jr	NC,strcmp_lowercase_fork1
	; iChar += 0x20;
	add	a,#0x20
strcmp_lowercase_fork1:
	inc	de
	cpi
	jr	nz,strcmp_lowercase_loop2
	or	a,a
	jr	nz,strcmp_lowercase_loop1
	ld	l,#0
	jr	strcmp_lowercase_loop3
strcmp_lowercase_loop2:
	ld	l,#1
strcmp_lowercase_loop3:	
;	ld	h,#0
	pop	ix
	ret

; ---------------------------------
; Function strcpy(PCHAR dst, PCHAR src)
; ---------------------------------
_strcpy:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
.if 0
; none 'undocumented' instruction implementation
strcpy_loop:
	ld	a,(hl)
	inc	hl
	ld	(de),a
	inc	de
	or	a,a
	jr	nz,strcpy_loop
.else
strcpy_loop:
	ld	a,(hl)
	ldi
	or	a,a
	jr	nz,strcpy_loop
;	push hl
;	xor	a,a		; zero is the value we are looking for	
;	ld	b,a
;	ld	c,a		; put 0 in BC to search 65, 536 bytes (the entire addressable memory space).
;	cpir			; begin search for a byte equalling zero.
; BC has been decremented so that it holds -length. Now need to synthesize a NEG BC.
;	ld	h,a             ; zero HL (basically set it to 65, 536) to get the
;	ld	l,a             ; number of bytes
;	sbc	hl,bc           ; find the size. CPIR doesn't affect carry.
;	ld	b,h
;	ld	c,l
;	pop hl
;	ldir	
.endif
	pop	ix
	ret

; ---------------------------------
; Function strcat(PCHAR dst, PCHAR src)
; ---------------------------------
_strcat:
	push	ix
	ld	ix,#0
	add	ix,sp
.if 0
; none 'undocumented' instruction implementation
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
strcat_loop1:
	ld	a,(de)
	or	a,a
	jr	z,strcat_loop2
	inc	de
	jr	strcat_loop1
strcat_loop2:
	ld	a,(hl)
	inc	hl
	ld	(de),a
	inc	de
	or	a,a
	jr	nz,strcat_loop2
.else
; move to the end of dst first, store in DE
	ld	l,4(ix)
	ld	h,5(ix)
	xor	a,a
	ld	b,a
	ld	c,a
	cpir
	dec	hl
	ld	e,l
	ld	d,h

	ld	l,6(ix)
	ld	h,7(ix)
strcat_loop:
	ld	a,(hl)
	ldi
	or	a,a
	jr	nz,strcat_loop
.endif	
	pop	ix
	ret

; ---------------------------------
; Function strcat_char(PCHAR dst, UCHAR ch)
; ---------------------------------
_strcat_char:
	push	ix
	ld	ix,#0
	add	ix,sp

; move to the end of dst first, store in HL
	ld	l,4(ix)
	ld	h,5(ix)
	xor	a,a
	ld	b,a
	ld	c,a
	cpir
	dec	hl
	
	ld	a,6(ix)
	ld	(hl),a
	inc	hl
	xor	a,a
	ld	(hl),a

	pop	ix
	ret

; ---------------------------------
; Function memset(PCHAR dst, UCHAR ch, USHORT len)
; ---------------------------------
_memset:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	c,7(ix)
	ld	b,8(ix)
memset_loop:
	ld	a,l
	ld	(de),a
	inc	de
	dec	bc
	ld	a,b
	or	a,c
	jr	nz,memset_loop
	pop	ix
	ret

; ---------------------------------
; Function memcmp(PCHAR buf1, PCHAR buf2, USHORT len)
; ---------------------------------
_memcmp:
	push	ix
	ld	ix,#0
	add	ix,sp
.if 0
; none 'undocumented' instruction implementation
	ld	hl,#-1
	add	hl,sp
	ld	sp,hl
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
	ld	c,8(ix)
	ld	b,9(ix)
memcmp_loop1:
	ld	a,(hl)
	inc	hl
	ld	-1(ix),a
	ld	a,(de)
	inc	de
	sub	a,-1(ix)
	ld	-1(ix),a
	or	a,a
	jr	nz,memcmp_loop2
	dec	bc
	ld	a,b
	or	a,c
	jr	nz,memcmp_loop1
memcmp_loop2:
	ld	l,-1(ix)
;	ld	h,#0
	ld	sp,ix
.else
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
	ld	c,8(ix)
	ld	b,9(ix)
memcmp_loop1:
	ld	a,(de)
	inc	de
	cpi
	jr	nz,memcmp_loop2
	ld	a,b
	or	a,c
	jr	nz,memcmp_loop1
	ld	l,#0
	jr	memcmp_loop3
memcmp_loop2:
	ld	l,#1
memcmp_loop3:	
;	ld	h,#0
.endif	
	pop	ix
	ret


; ---------------------------------
; Function memcmp_lowercase(PCHAR buf1, PCHAR buf2)
; ---------------------------------
_memcmp_lowercase:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
memcmp_lowercase_loop1:
	ld	a,(de)
	; if (iChar >= 'A' && iChar <= 'Z')
	cp	a,#0x41
	jr	C,memcmp_lowercase_fork1
	cp	a,#0x5B
	jr	NC,memcmp_lowercase_fork1
	; iChar += 0x20;
	add	a,#0x20
memcmp_lowercase_fork1:
	inc	de
	cpi
	jr	nz,memcmp_lowercase_fork2
	or	a,a
	jr	nz,memcmp_lowercase_loop1
	jr	memcmp_lowercase_fork3
memcmp_lowercase_fork2:
	; check if buf2 ends
	dec hl
	ld	a,(hl)
	or	a,a
	jr	z,memcmp_lowercase_fork3
	ld	l,#1
	jr	memcmp_lowercase_fork4
memcmp_lowercase_fork3:	
	ld	l,#0
memcmp_lowercase_fork4:	
	pop	ix
	ret


; ---------------------------------
; Function memcmp_str(PCHAR buf1, PCHAR buf2)
; ---------------------------------
_memcmp_str:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,6(ix)
	ld	d,7(ix)
	ld	l,4(ix)
	ld	h,5(ix)
memcmp_str_loop1:
	ld	a,(de)
	inc	de
	cpi
	jr	nz,memcmp_str_fork1
	or	a,a
	jr	nz,memcmp_str_loop1
	jr	memcmp_str_fork2
memcmp_str_fork1:
	; check if buf2 ends
	or	a,a
	jr	z,memcmp_str_fork2
	ld	l,#1
	jr	memcmp_str_fork3
memcmp_str_fork2:	
	ld	l,#0
memcmp_str_fork3:	
	pop	ix
	ret


; ---------------------------------
; Function memcpy(PCHAR dst, PCHAR src, USHORT len)
; ---------------------------------
_memcpy:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
	ld	c,8(ix)
	ld	b,9(ix)
.if 0
; none 'undocumented' instruction implementation
memcpy_loop:
	ld	a,(hl)
	inc	hl
	ld	(de),a
	inc	de
	dec	bc
	ld	a,b
	or	a,c
	jr	nz,memcpy_loop
.else	
	ldir
.endif	
	pop	ix
	ret

; ---------------------------------
; Function memcpy4(PCHAR dst, PCHAR src)
; ---------------------------------
_memcpy4:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	e,4(ix)
	ld	d,5(ix)
	ld	l,6(ix)
	ld	h,7(ix)
	ld	bc,#0004
	ldir
	pop	ix
	ret

; ---------------------------------
; Function memmove(PCHAR dst, PCHAR src, USHORT len)
; ---------------------------------
_memmove:
	push	ix
	ld	ix,#0
	add	ix,sp
	ld	c,8(ix)
	ld	b,9(ix)

	ld	l,4(ix)
	ld	h,5(ix)
	add	hl,bc
	dec hl
	ld	d,h
	ld	e,l

	ld	l,6(ix)
	ld	h,7(ix)
	add hl,bc
	dec hl

	lddr
	pop	ix
	ret


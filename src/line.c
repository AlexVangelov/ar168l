/*-------------------------------------------------------------------------
   AR1688 Line function copy right information

   Copyright (c) 2006-2010. Tang, Li      <tangli@palmmicro.com>
                            Lin, Rongrong <woody@palmmicro.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Palmmicro nor the names of its contributors may be
	  used to endorse or promote products derived from this software without
	  specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL COPYRIGHT OWNER BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-------------------------------------------------------------------------*/

#include "type.h"
#include "core.h"
#include "apps.h"

USHORT _sLineLen;
USHORT _sLineLenBackup;
PCHAR _pLine;
PCHAR _pLineBackup;

void line_backup()
{
	_sLineLenBackup = _sLineLen;
	_pLineBackup = _pLine;
}

void line_restore()
{
	_sLineLen = _sLineLenBackup;
	_pLine = _pLineBackup;
}

void line_start(PCHAR pDst)
{
	_pLine = pDst;
	_sLineLen = 0;
}

USHORT line_get_len()
{
	return _sLineLen;
}

void line_update_len()
{
	_sLineLen += strlen((PCHAR)(_pLine + _sLineLen));
}

PCHAR line_get_buf()
{
	return (PCHAR)(_pLine + _sLineLen);
}

void line_add_data(PCHAR pData, USHORT sLen)
{
	memcpy((PCHAR)(_pLine + _sLineLen), pData, sLen);
	_sLineLen += sLen;
	_pLine[_sLineLen] = 0;
}

void line_add_str(PCHAR pStr)	__naked
{
/*	if (pStr)
	{
		strcpy((PCHAR)(_pLine + _sLineLen), pStr);
		_sLineLen += strlen(pStr);
	}
*/
	pStr;
	__asm
	push	ix
	ld	ix,#0
	add	ix,sp
	; if (pStr)
	ld	a,4(ix)
	or	a,5(ix)
	jr	Z,line_add_str_fork

	ld	hl,#__pLine
	ld	iy,#__sLineLen
	ld	a,0(iy)
	add	a,(hl)
	ld	e,a
	ld	a,1(iy)
	inc	hl
	adc	a,(hl)
	ld	d,a
	ld	l,4(ix)
	ld	h,5(ix)
	xor	a,a
	ld	b,a
	ld	c,a

line_add_str_loop:
	ld	a,(hl)
	ldi
	or	a,a
	jr	nz,line_add_str_loop

	inc	bc
	ld	a,0(iy)
	sub	a,c
	ld	0(iy),a
	ld	a,1(iy)
	sbc	a,b
	ld	1(iy),a
	
line_add_str_fork:
	pop	ix
	ret
	__endasm;
}

// if pStr is NULL, iChar is not added, not the same as {line_add_str(pStr); line_add_char(iChar);}
void line_add_str_with_char(PCHAR pStr, UCHAR iChar)	__naked
{
	pStr;
	iChar;
	__asm
	push	ix
	ld	ix,#0
	add	ix,sp
	; if (pStr)
	ld	a,4(ix)
	or	a,5(ix)
	jr	Z,line_add_str_with_char_fork

	ld	hl,#__pLine
	ld	iy,#__sLineLen
	ld	a,0(iy)
	add	a,(hl)
	ld	e,a
	ld	a,1(iy)
	inc	hl
	adc	a,(hl)
	ld	d,a
	ld	l,4(ix)
	ld	h,5(ix)
	xor	a,a
	ld	b,a
	ld	c,a

line_add_str_with_char_loop:
	ld	a,(hl)
	ldi
	or	a,a
	jr	nz,line_add_str_with_char_loop

	ld (de),a
	dec de
	ld a,6(ix) 
	ld (de),a
;	inc	bc

	ld	a,0(iy)
	sub	a,c
	ld	0(iy),a
	ld	a,1(iy)
	sbc	a,b
	ld	1(iy),a
	
line_add_str_with_char_fork:
	pop	ix
	ret
	__endasm;
}

// if pStr is NULL, crlf is not added, not the same as {line_add_str(pStr); line_add_crlf();}
void line_add_str_with_crlf(PCHAR pStr)	__naked
{
	pStr;
	__asm
	push	ix
	ld	ix,#0
	add	ix,sp
	; if (pStr)
	ld	a,4(ix)
	or	a,5(ix)
	jr	Z,line_add_str_with_crlf_fork

	ld	hl,#__pLine
	ld	iy,#__sLineLen
	ld	a,0(iy)
	add	a,(hl)
	ld	e,a
	ld	a,1(iy)
	inc	hl
	adc	a,(hl)
	ld	d,a
	ld	l,4(ix)
	ld	h,5(ix)
	xor	a,a
	ld	b,a
	ld	c,a

line_add_str_with_crlf_loop:
	ld	a,(hl)
	ldi
	or	a,a
	jr	nz,line_add_str_with_crlf_loop

	inc de
	ld (de),a
	dec de
	ld	a,#0x0A
	ld (de),a
	dec de
	ld	a,#0x0D
	ld (de),a
	dec	bc

	ld	a,0(iy)
	sub	a,c
	ld	0(iy),a
	ld	a,1(iy)
	sbc	a,b
	ld	1(iy),a
	
line_add_str_with_crlf_fork:
	pop	ix
	ret
	__endasm;
}

void line_add_short(USHORT sVal)
{
	itoa(sVal, line_get_buf(), 10);
	line_update_len();
}

void line_add_long(ULONG lVal)
{
	ltoa(lVal, line_get_buf(), 10);
	line_update_len();
}

void line_add_ip(PCHAR pIPAddr)
{
	ip2str(pIPAddr, line_get_buf());
	line_update_len();
}

void line_add_char(UCHAR iChar)
{
	_pLine[_sLineLen] = iChar;
	_sLineLen ++;
	_pLine[_sLineLen] = 0;
}

void line_add_crlf()
{
//	line_add_str("\r\n");
	_pLine[_sLineLen] = '\r';
	_sLineLen ++;
	_pLine[_sLineLen] = '\n';
	_sLineLen ++;
	_pLine[_sLineLen] = 0;
}

UCHAR is_endof_line(PCHAR pData)
{
	if (pData[0] == 0x0d)
	{
		return (pData[1] == 0x0a) ? 2 : 1;
	}
	else if (pData[0] == 0x0a)
	{
		return (pData[1] == 0x0d) ? 2 : 1;
	}
	return 0;
}

UCHAR count_space(PCHAR pStr)
{
	UCHAR iLen;

	iLen = 0;
	while (*pStr == (UCHAR)'\t' || *pStr == (UCHAR)' ')
	{
		iLen ++;
		pStr ++;
	}
	return iLen;
}

void line_add_options_str(USHORT sOffset, UCHAR iMaxLen)
{
	OptionsGetString(line_get_buf(), sOffset, iMaxLen);
	line_update_len();
}

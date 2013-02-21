/*-------------------------------------------------------------------------
   AR168M demo external controller MCS51
   LCD function copy right information

   Copyright (c) 2008-2010. Lin, Rongrong <woody@palmmicro.com>

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
#include "8051.h"
#include "sys.h"

#include "font.h"

#define LCD_RW	P2_5
#define LCD_RS	P2_6
#define LCD_E	P2_7

void _status_polling()
{
	DCHAR iVal;
	DSHORT sCount;

	LCD_RS = 0;
	LCD_RW = 1;
	LCD_E = 1;

	sCount = 0;
	do
	{
		P0 = 0xff;
		iVal = P0;
		if (!(iVal & 0x80))	break;
		sCount --;
	} while (sCount);

	LCD_E = 0;
	LCD_RW = 0;
}

void write_control_byte(UCHAR ch)
{
	P0 = ch;
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_E = 1;
	LCD_E = 0;

	_status_polling();
}

void write_data_byte(UCHAR ch)
{
	P0 = ch;
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_E = 1;
	LCD_E = 0;

	_status_polling();
}

UCHAR read_data_byte()
{
	DCHAR iVal;

	LCD_RS = 1;
	LCD_RW = 1;
	LCD_E = 1;

	P0 = 0xff;
	iVal = P0;

	LCD_E = 0;
	LCD_RW = 0;

	return iVal;
}

#define FONT_ARRAY_SIZE		8
#define FONT_ARRAY_NUM		8

#define LCD_STATUS_CHAR			0x1f
#define EXTENDED_CHAR_START		0xa0

DCHAR _pFontBuf[FONT_ARRAY_NUM];

void LcdInit()
{
	DCHAR i;

	for (i = 0; i < 3; i ++)
	{	
		write_control_byte(0x30);	// Set function mode
	}
	
	write_control_byte(0x38);	// set work mode, 8-bit 2-line 5*7
	write_control_byte(0x01);	// clear screen
	write_control_byte(0x06);	// set input method
	write_control_byte(0x0f);	// set display method, show cursor
	write_control_byte(0x14);	// set display method
	write_control_byte(0x80);

	memset_d(_pFontBuf, 0, FONT_ARRAY_NUM);
}

void _write_cgram(UCHAR iIndex, PCCHAR pFont)
{
	UCHAR i;

	iIndex <<= 3;
	write_control_byte(0x40 + iIndex);	// load CGRAM data
	for (i = 0; i < FONT_ARRAY_SIZE; i ++)
	{
		write_data_byte(pFont[i]);
	}
}

BOOLEAN _IsExtendedChar(UCHAR iVal)
{
	return (iVal >= EXTENDED_CHAR_START) ? TRUE : FALSE;
}

BOOLEAN _IsStatusChar(UCHAR iVal)
{
	return (iVal == LCD_STATUS_CHAR) ? TRUE : FALSE;
}

const CCHAR _cWriteControl[4] = {0x80, 0xc0, 0x90, 0xd0};		// line DDRAM address, max 4 lines supported

void LcdCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow)
{
	if (bShow)
	{
		write_control_byte(_cWriteControl[iLine] + iPos);
		write_control_byte(0x0f);
	}
	else
	{
		write_control_byte(0x0c);
	}
}

UCHAR _LoadFont(UCHAR iChar)
{
	DCHAR i, j, k;
	DSHORT sOffset;
	BOOLEAN bLoadFont;

	// check if font already exist in cgram
	for (i = 0; i < FONT_ARRAY_NUM; i ++)
	{
		if (_pFontBuf[i] == iChar)
		{
			return i;
		}
	}

	// check if any cgram font used
	bLoadFont = FALSE;
	for (i = 0; i < FONT_ARRAY_NUM; i ++)
	{
		bLoadFont = TRUE;
		for (j = 0; j < DISPLAY_MAX_LINE; j ++)
		{
			for (k = 0; k < DISPLAY_MAX_CHAR; k ++)
			{
				write_control_byte(_cWriteControl[j] + k);
				if (read_data_byte() == i)
				{
					bLoadFont = FALSE;
					break;
				}
			}
			if (!bLoadFont)	break;
		}
		if (bLoadFont)	break;
	}

	if (bLoadFont)
	{
		sOffset = iChar + 1 - EXTENDED_CHAR_START;
		sOffset *= FONT_ARRAY_SIZE;
		_write_cgram(i, (PCCHAR)(_cFont + sOffset));
		_pFontBuf[i] = iChar;
		return i;
	}

	return iChar;
}

void LcdDisplay(PDCHAR pBuf, UCHAR iLine)
{
	DCHAR i, iVal;
	BOOLEAN bStatus;

	write_control_byte(_cWriteControl[iLine]);
	for (i = 0; i < DISPLAY_MAX_CHAR; i ++)
	{
		iVal = pBuf[i];
		if (iVal)
		{
			bStatus = _IsStatusChar(iVal);
			if (bStatus)
			{
				iVal = EXTENDED_CHAR_START - 1;
			}
			if (_IsExtendedChar(iVal) || bStatus)
			{
				iVal = _LoadFont(iVal);
				write_control_byte(_cWriteControl[iLine] + i);
			}
			write_data_byte(iVal);
		}
		else
		{
			break;
		}
	}

	for (; i < DISPLAY_MAX_CHAR; i ++)
	{
		write_data_byte(' ');
	}
}

#ifdef SYS_DEBUG
void LcdDisplayC(PCCHAR pBuf, UCHAR iLine)
{
	DCHAR pDst[DISPLAY_MAX_CHAR];

	memcpy_c2d(pDst, pBuf, DISPLAY_MAX_CHAR);
	LcdDisplay(pDst, iLine);
}
#endif

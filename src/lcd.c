/*-------------------------------------------------------------------------
   AR1688 LCD function copy right information

   Copyright (C) 2006-2013. Lin, Rongrong <woody@palmmicro.com>
                            Lin, Guofeng <alanda@palmmicro.com>

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

#include "version.h"
#include "type.h"
#include "sfr.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"

#ifdef SYS_LCD

#ifdef SERIAL_UI

const UCHAR _pLcd[] = "LCD ";
const UCHAR _pLcdCursor[] = "LCDC";

void _SerialCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow)
{
	UCHAR pSerialBuf[8];

	strcpy(pSerialBuf, _pLcdCursor);
	pSerialBuf[4] = iLine + '0';
	pSerialBuf[5] = iPos + '0';
	pSerialBuf[6] = bShow + '0';
	pSerialBuf[7] = 0;
	TaskSerialSendString(pSerialBuf);
}

void _SerialDisplay(PCHAR pBuf, UCHAR iLine)
{
	UCHAR pSerialBuf[DISPLAY_MAX_CHAR + 6];

	strcpy(pSerialBuf, _pLcd);
	pSerialBuf[4] = iLine + '0';
	memcpy((PCHAR)(pSerialBuf + 5), pBuf, DISPLAY_MAX_CHAR);
	pSerialBuf[DISPLAY_MAX_CHAR + 5] = 0;
	TaskSerialSendString(pSerialBuf);
}

#endif

extern UCHAR Flash_iBaseBank;

#define ASCII_FONT_OFFSET		0x4001

UCHAR _pDisplayBuf[DISPLAY_MAX_LINE][DISPLAY_MAX_CHAR];

BOOLEAN _IsExtendedChar(UCHAR iVal)
{
	return (iVal >= EXTENDED_CHAR_START) ? TRUE : FALSE;
}

BOOLEAN _IsStatusChar(UCHAR iVal)
{
	return (iVal == LCD_STATUS_CHAR) ? TRUE : FALSE;
}

#if defined LCD_ST7565

void _status_polling()
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;
	while (p[0] & 0x80);
}

void write_control_byte(UCHAR ch)
{
	PCHAR p;
#ifdef LCD_ST7565_YEXINDA_SPI
	UCHAR i;
	p = (PCHAR)BANK_BASE;
	DI;
	rGPIO_A &= 0xfd;		// A1 LCD_CS
	for (i = 0; i < 8; i ++) p[0] = (ch << i);
	rGPIO_A |= 0x02;
	EI;
#else
	p = (PCHAR)BANK_BASE;
	_status_polling();
	p[0] = ch;
#endif
}

void write_data_byte(UCHAR ch)
{
	PCHAR p;
#ifdef LCD_ST7565_YEXINDA_SPI
	UCHAR i;
	p = (PCHAR)BANK_BASE;
	DI;
	SetExtPage(0x20);		// 00100000b, A20 high for data
	rGPIO_A &= 0xfd;		// A1 LCD_CS
	for (i = 0; i < 8; i ++) p[0] = (ch << i);
	rGPIO_A |= 0x02;
	SetExtPage(0x00);		// 00000000b, A20 low for read status
	EI;
#else
	p = (PCHAR)BANK_BASE;
	_status_polling();
	SetExtPage(0x20);		// 00100000b, A20 high for data
	p[0] = ch;
	SetExtPage(0x00);		// 00000000b, A20 low for read status
#endif
}

void _set_page(UCHAR iPage)
{
	iPage &= 0x0f;
	write_control_byte(iPage|0xb0);			// Page address set, 1011xxxx
}

void _set_column(UCHAR iColumn)
{
	UCHAR iVal;

	iVal = (iColumn >> 4)&0x0f;
	write_control_byte(iVal|0x10);			// Column address set upper bit, 0001xxxx
	write_control_byte(iColumn&0x0f);		// Column address set lower bit, 0000xxxx
}

#ifdef UI_4_LINES
#define LCD_PAGE_NUM	9
#else
#define LCD_PAGE_NUM	4
#endif

void lcd_clear_screen()
{
	UCHAR i, j, iOrg;

	EnableCE2();
	iOrg = GetExtPage();
	SetExtPage(0x00);		// 00000000b, A20 low for write control

	for (i = 0; i < LCD_PAGE_NUM; i ++)
	{
		_set_page(i);
		_set_column(0);
		for (j = 0; j < 132; j ++)
		{
			write_data_byte(0);
		}
	}
	SetExtPage(iOrg);
	DisableCEx();
}

void LcdLight(UCHAR iRow, UCHAR iData)
{
	UCHAR i, iOrg;

	EnableCE2();
	iOrg = GetExtPage();
	SetExtPage(0x00);		// 00000000b, A20 low for write control
	for (i = 0; i < LCD_PAGE_NUM; i ++)
	{
		_set_page(i);
		_set_column(iRow);
		write_data_byte(iData);
	}
	SetExtPage(iOrg);
	DisableCEx();
}

//#ifdef CALL_NONE
#if defined CALL_NONE || defined VER_AR168L
void _set_start_line(UCHAR iStartLine)
{
	iStartLine &= 0x3f;
	write_control_byte(iStartLine|0x40);		// Display start line set, 01xxxxxx
}

void LcdInit()
{
	UCHAR iOrg;

	EnableCE2();
	iOrg = GetExtPage();
	
	SetExtPage(0x00);		// 00000000b, A20 low for write control

	write_control_byte(0xe2);	// Reset

#ifdef VER_FWV2800
	write_control_byte(0xa3);	// LCD bias set, 10100010, 1/9 bias, 10100011, 1/7 bias
#else
	write_control_byte(0xa2);	// LCD bias set, 10100010, 1/9 bias, 10100011, 1/7 bias
#endif
#ifdef VER_BT2008N
	write_control_byte(0xa1);	// ADC select, 10100000, normal, 10100001, reverse
#else
	write_control_byte(0xa0);	// ADC select, 10100000, normal, 10100001, reverse
#endif
#if defined VER_AR168P || defined VER_GP1266 || defined VER_GP2266 || defined VER_FWV2800 || defined VER_DXDT || defined VER_AR168L // || defined VER_BT2008N
#ifndef LCD_ST756_YEXINDA_SPI
        write_control_byte(0xc0);       // Common output mode select, 11000***, normal, 11001***, reverse
#else
	write_control_byte(0xc8);	// Common output mode select, 11000***, normal, 11001***, reverse
#endif
#else
	write_control_byte(0xc0);	// Common output mode select, 11000***, normal, 11001***, reverse
#endif

	write_control_byte(0x2c);	// Power control set, 00101xxx
	write_control_byte(0x2e);	// Power control set, 00101xxx
	write_control_byte(0x2f);	// Power control set, 00101xxx

#if defined VER_AR168P || defined VER_GP1266 || defined VER_GP2266 || defined VER_FWV2800 || defined VER_DXDT || defined VER_AR168L
#ifndef LCD_ST7565_YEXINDA_SPI
	write_control_byte(0x25);	// V5 voltage regulator internal resistor ratio set, 00100xxx
#else
        write_control_byte(0x24);       // V5 voltage regulator internal resistor ratio set, 00100xxx
#endif
	write_control_byte(0x81);	// Electronic volume mode set, set the V5 output voltage
#else
	write_control_byte(0x24);	// V5 voltage regulator internal resistor ratio set, 00100xxx
	write_control_byte(0x81);	// Electronic volume mode set, set the V5 output voltage
#endif

#if defined VER_AR168P || defined VER_GP1266 || defined VER_GP2266 || defined VER_DXDT || defined AR168L
	write_control_byte(26);		// Electronic volume register, 00xxxxxx
#elif defined VER_FWV2800
	write_control_byte(32);		// Electronic volume register, 00xxxxxx
#else
	write_control_byte(38);		// Electronic volume register, 00xxxxxx
#endif

	write_control_byte(0xf8);	// Booster ratio set, 11111000
	write_control_byte(0);		// Booster ratio set step-up value, 00, 3x

	write_control_byte(0xa4);	// Display all points, 10100100, normal, 10100101, all points ON
	write_control_byte(0xa6);	// Display normal/reverse, 10100110, normal, 10100111, reverse
	write_control_byte(0xaf);	// Display ON/OFF, 10101111 ON, 10101110, OFF
//	write_control_byte(0xa5);	// Display all points, 10100100, normal, 10100101, all points ON

	_set_start_line(0);
	SetExtPage(iOrg);
	DisableCEx();
}

void LcdDisplayBMP(PCHAR pBuf, BOOLEAN bReverse)
{
	UCHAR i, j, iOrg;
	USHORT s;
	UCHAR pData[1024];
	PCHAR pCur;

	if (bReverse)
	{
		for (s = 0; s < 1024; s ++)
		{
			pData[s] = ~pBuf[s];
		}
	}
	else
	{
		memcpy(pData, pBuf, 1024);
	}
	pCur = pData;

	EnableCE2();
	iOrg = GetExtPage();

	SetExtPage(0x00);		// 00000000b, A20 low for write control
	for (j = 0; j < 8; j ++)
	{
		_set_page(j);
#if defined VER_AR168P || defined VER_GP1266 || defined VER_GP2266 || defined VER_DXDT
		_set_column(0);
#elif defined VER_FWV2800
		_set_column(2);
#else
		_set_column(4);
#endif
		for (i = 0; i < 128; i ++)
		{
			write_data_byte(*pCur++);
		}
	}

	SetExtPage(iOrg);
	DisableCEx();
}

#endif

#define ASCII_ARRAY_SIZE	16

#define FONT_ARRAY_SIZE		32
#define FONT_PAGE_SIZE		94

void _display_CJK(PCHAR pBuf, UCHAR iPos, UCHAR iLine)
{
	UCHAR i, j, iOrg, iBank;
	UCHAR pFont[FONT_ARRAY_SIZE];
	PCHAR pCur;
	USHORT sOffset;

#ifdef RES_CN
	sOffset = (pBuf[0] - 0xa1) * FONT_PAGE_SIZE;
//#elif defined RES_KR
#else
	sOffset = (pBuf[0] - 0xac) * (FONT_PAGE_SIZE + 1);
#endif
	sOffset += pBuf[1] - 0xa1;
	iBank = 0;
	while (sOffset > 1024)
	{
		sOffset -= 1024;
		iBank ++;
	}
	sOffset *= FONT_ARRAY_SIZE;

	iOrg = GetExtPage();

	// get chinese font
	SetExtPage((SYSTEM_FONT_PAGE<<1) + iBank);		// chinese starts from bank 2, second 32k
	memcpy(pFont, (PCHAR)(BANK_BASE + sOffset), FONT_ARRAY_SIZE);

#ifdef VER_FWV2800
	if (iLine < 2)
	{
		iLine += 2;
	}
	else
	{
		iLine -= 2;
	}
#endif
	iLine <<= 1;
	iPos <<= 3;
#if defined VER_AR168G || defined VER_BT2008N
	iPos += 4;
#elif defined VER_FWV2800
	iPos += 2;
#endif

	pCur = pFont;
	EnableCE2();
	SetExtPage(0x00);		// 00000000b, A20 low for write control
	for (j = 0; j < 2; j ++)
	{
		_set_page(iLine+j);
		_set_column(iPos);
		for (i = 0; i < 16; i ++)
		{
			write_data_byte(*pCur++);
		}
	}
	SetExtPage(iOrg);
	DisableCEx();
}

void _display_ascii(UCHAR iChar, UCHAR iPos, UCHAR iLine)
{	
	UCHAR i, j, iOrg;
	UCHAR pFont[ASCII_ARRAY_SIZE];
	PCHAR pCur;
	USHORT sOffset;

#ifdef RES_RU
	// CP1251 font has it all!
	sOffset = iChar - LCD_STATUS_CHAR;
#else
	sOffset = (iChar < EXTENDED_CHAR_START) ? iChar - LCD_STATUS_CHAR : iChar - LCD_STATUS_CHAR - 33;
#endif
	sOffset *= ASCII_ARRAY_SIZE;

	iOrg = GetExtPage();

	// get ascii font
//	SetExtPage(SYSTEM_FONT_PAGE<<1);		// ascii in font bank 1, first 32k, first 4096 bytes
	SetExtPage(Flash_iBaseBank);
	memcpy(pFont, (PCHAR)(BANK_BASE + ASCII_FONT_OFFSET + sOffset), ASCII_ARRAY_SIZE);
//	memmove(pFont, (PCHAR)(BANK_BASE + ASCII_FONT_OFFSET + sOffset), ASCII_ARRAY_SIZE);

#ifdef VER_FWV2800
	if (iLine < 2)
	{
		iLine += 2;
	}
	else
	{
		iLine -= 2;
	}
#endif
	iLine <<= 1;
	iPos <<= 3;
#if defined VER_AR168G || defined VER_BT2008N
	iPos += 4;
#elif defined VER_FWV2800
	iPos += 2;
#endif

	pCur = pFont;
	EnableCE2();
	SetExtPage(0x00);		// 00000000b, A20 low for write control
	for (j = 0; j < 2; j ++)
	{
		_set_page(iLine+j);
		_set_column(iPos);
		for (i = 0; i < 8; i ++)
		{
			write_data_byte(*pCur++);
		}
	}
	SetExtPage(iOrg);
	DisableCEx();
}

void LcdCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow)
{
	iLine;
	iPos;
	bShow;
#ifdef SERIAL_UI
	_SerialCursor(iLine, iPos, bShow);
#endif
/*	UCHAR iOrg;

	EnableCE2();
	iOrg = GetExtPage();
	SetExtPage(0x00);		// 00000000b, A20 low for write control
	if (bShow)
	{
		write_control_byte(0xad);			// Static indicator ON/OFF, 1010110x
		write_control_byte(0x01);
	}
	else
	{
		write_control_byte(0xac);			// Static indicator ON/OFF, 1010110x
	}
	SetExtPage(iOrg);
	DisableCEx();
*/
}

#elif defined LCD_HY1602 || defined LCD_HY2004

#ifdef SYS_CHIP_PLUS
void EnableLCD()
{
	// GPIO G0 as LCD_E pulse control
//	rGPIO_G_Enable |= 0x01;
	rGPIO_G &= 0xfe;
	rGPIO_G |= 0x01;
}
#endif

// Lcd functions

// LCD_RS - A20, H - data, L - control
// LCD_RW - A19, H - read, L - write

void _status_polling()
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;
	SetExtPage(0x10);		// 00010000b, A19 high for read

#ifdef SYS_CHIP_PLUS
	do
	{
		EnableLCD();
	} 
#endif
	while (p[0] & 0x80);
}

void write_control_byte(UCHAR ch)
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;

	_status_polling();

	SetExtPage(0x00);		// 00000000b
#ifdef SYS_CHIP_PLUS
	EnableLCD();
#endif
	p[0] = ch;
}

void write_data_byte(UCHAR ch)
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;

	_status_polling();

	SetExtPage(0x20);		// 00100000b, A20 high for data
#ifdef SYS_CHIP_PLUS
	EnableLCD();
#endif
	p[0] = ch;
}

/*
UCHAR read_data_byte()
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;

	_status_polling();

	SetExtPage(0x30);		// 00110000b, A19=A20 high for read data
	return p[0];
}
*/

// warning: global none-const var init have problem, maybe because init process is not executed because of data/code movement
#ifdef LCD_HY2004
const UCHAR _cWriteControl[DISPLAY_MAX_LINE] = {0x80, 0xc0, 0x94, 0xd4};		// line DDRAM address, 20x4
//const UCHAR _cWriteControl[DISPLAY_MAX_LINE] = {0x80, 0xc0, 0x90, 0xd0};		// line DDRAM address, 16x4
#elif defined LCD_HY1602
const UCHAR _cWriteControl[DISPLAY_MAX_LINE] = {0x80, 0xc0};		// line DDRAM address, 16x2
#endif

#define FONT_ARRAY_SIZE		8
#define FONT_ARRAY_NUM		8

UCHAR _pFontBuf[FONT_ARRAY_NUM];

#ifdef CALL_NONE

const UCHAR _cFontArray[FONT_ARRAY_NUM*FONT_ARRAY_SIZE] = {
0x08, 0x0F, 0x12, 0x0F, 0x0A, 0x1F, 0x02, 0x02,	// "年"代码=00H 
0x0F, 0x09, 0x0F, 0x09, 0x0F, 0x09, 0x11, 0x00,	// "月"代码=01H
0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F, 0x00,	// "日"代码=02H
0x11, 0x0A, 0x04, 0x1F, 0x04, 0x1F, 0x04, 0x00,	// "$"代码=03H
0x0E, 0x00, 0x1F, 0x0A, 0x0A, 0x0A, 0x13, 0x00,	// "元"代码=04H
0x18, 0x18, 0x07, 0x08, 0x08, 0x08, 0x07, 0x00,	// "℃"代码=05H
0x04, 0x0A, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00,	// "↑"代码=06H
0x17, 0x15, 0x15, 0x15, 0x15, 0x15, 0x17, 0x00,	// "10"代码=07H
};

void LcdInit()
{
	UCHAR i, iOrg;

	EnableCE2();
	iOrg = GetExtPage();

	for (i = 0; i < 3; i ++)
	{	
		write_control_byte(0x30);	// Set function mode
	}
	
	write_control_byte(0x38);	// set work mode, 8-bit 2-line 5*7
	write_control_byte(0x01);	// clear screen
	write_control_byte(0x02);	// cursor return home
	write_control_byte(0x06);	// set input method
	write_control_byte(0x0f);	// set display method, show cursor
	write_control_byte(0x14);	// set display method
	write_control_byte(0x80);

	SetExtPage(iOrg);
	DisableCEx();
}

void LcdTest4()
{
	UCHAR i, j, iOrg;
//	PCHAR p;

//	p = _cFontArray;

	EnableCE2();
	iOrg = GetExtPage();

	// load CGRAM data here
	write_control_byte(0x40);
	for (i = 0; i < FONT_ARRAY_NUM*FONT_ARRAY_SIZE; i ++)
	{
		write_data_byte(_cFontArray[i]); 
	}

	for (j = 0; j < DISPLAY_MAX_LINE; j ++)
	{
		write_control_byte(_cWriteControl[j]);
		for (i = 0; i < FONT_ARRAY_NUM; i ++)
		{
			write_data_byte(i);
			write_data_byte(i + 8);
		}
	}

	SetExtPage(iOrg);
	DisableCEx();
}

void LcdTest5()
{
	UCHAR i;
	UCHAR pBuf[DISPLAY_MAX_CHAR];

	for (i = 0; i < DISPLAY_MAX_CHAR; i ++)
	{
		pBuf[i] = i + 0xb0;
	}

	for (i = 0; i < DISPLAY_MAX_LINE; i ++)
	{
		LcdDisplay(pBuf, i);
	}
}

#endif

void LcdCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow)
{
	UCHAR iOrg;

	EnableCE2();
	iOrg = GetExtPage();

	if (bShow)
	{
		write_control_byte(_cWriteControl[iLine] + iPos);
		write_control_byte(0x0f);
	}
	else
	{
		write_control_byte(0x0c);
	}

	SetExtPage(iOrg);
	DisableCEx();

#ifdef SERIAL_UI
	_SerialCursor(iLine, iPos, bShow);
#endif
}

void _write_cgram(UCHAR iIndex, PCHAR pFont)
{
	UCHAR i;

	iIndex <<= 3;
	write_control_byte(0x40 + iIndex);	// load CGRAM data
	for (i = 0; i < FONT_ARRAY_SIZE; i ++)
	{
		write_data_byte(pFont[i]);
	}
}

void _read_font(UCHAR iChar, PCHAR pFont)
{
	USHORT sOffset;

	SetExtPage(Flash_iBaseBank);		// extended ascii font in bank0 0x4001
	sOffset = iChar - EXTENDED_CHAR_START;
	sOffset *= FONT_ARRAY_SIZE;
	memcpy(pFont, (PCHAR)(BANK_BASE + ASCII_FONT_OFFSET + sOffset), FONT_ARRAY_SIZE);
}

void lcd_clear_screen()
{
	UCHAR iOrg;
//	UCHAR i, iOrg, iChar;
//	UCHAR pFont[FONT_ARRAY_SIZE];

	iOrg = GetExtPage();

	EnableCE2();
	write_control_byte(0x01);	// clear screen
	DisableCEx();
/*
	// load default CGRAM font data
	for (i = 0; i < FONT_ARRAY_NUM; i ++)
	{
		iChar = EXTENDED_CHAR_START + i;
		_pFontBuf[i] = iChar;
		_read_font(iChar, pFont);

		EnableCE2();
		_write_cgram(i, pFont);
		DisableCEx();
	}
*/
	memset(_pFontBuf, 0, FONT_ARRAY_NUM);
	SetExtPage(iOrg);
}

void _display_ascii(UCHAR iChar, UCHAR iPos, UCHAR iLine)
{
	UCHAR i, j, k, iOrg;
	UCHAR pFont[FONT_ARRAY_SIZE];
	BOOLEAN bLoadFont, bStatus;

	iOrg = GetExtPage();

	bLoadFont = FALSE;
	bStatus = _IsStatusChar(iChar);
	if (_IsExtendedChar(iChar) || bStatus)
	{
		// check if font already exist in cgram
		for (i = 0; i < FONT_ARRAY_NUM; i ++)
		{
			if (_pFontBuf[i] == iChar)
			{
				iChar = i;
				break;
			}
		}

		if (iChar >= FONT_ARRAY_NUM)
		{	// check if any cgram font used
			for (i = 0; i < FONT_ARRAY_NUM; i ++)
			{
				bLoadFont = TRUE;
				for (j = 0; j < DISPLAY_MAX_LINE; j ++)
				{
					for (k = 0; k < DISPLAY_MAX_CHAR; k ++)
					{
						if (_pDisplayBuf[j][k] == _pFontBuf[i])
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
				if (bStatus)
				{
					memset(pFont, 0x1f, FONT_ARRAY_SIZE);
				}
				else
				{
					_read_font(iChar, pFont);
				}
				_pFontBuf[i] = iChar;
				iChar = i;
			}
		}
	}

	EnableCE2();

	if (bLoadFont)
	{
		_write_cgram(iChar, pFont);
	}
	write_control_byte(_cWriteControl[iLine] + iPos);
	write_data_byte(iChar);

	SetExtPage(iOrg);
	DisableCEx();
}

#else

void lcd_clear_screen()
{
}

void _display_ascii(UCHAR iChar, UCHAR iPos, UCHAR iLine)
{
	iChar;
	iPos;
	iLine;
}

void LcdInit()
{
}

void LcdCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow)
{
	iLine;
	iPos;
	bShow;
}

#endif

void LcdStart()
{
	UCHAR i;
	
	for (i = 0; i < DISPLAY_MAX_LINE; i ++)
	{
		memset(_pDisplayBuf[i], ' ', DISPLAY_MAX_CHAR);
	}

	lcd_clear_screen();
	LcdCursor(0, 0, FALSE);
//	LcdCursor(1, 1, TRUE);
}

void LcdDisplay(PCHAR pBuf, UCHAR iLine)
{
	UCHAR i, iVal;
#ifdef DISPLAY_DOT
#if defined RES_CN || defined RES_KR
	UCHAR iVal2;
#endif
#endif
	PCHAR pLine;

	pLine = _pDisplayBuf[iLine];

	i = 0;
	do
	{
		iVal = pBuf[i];
		if (iVal == 0)	break;

#ifdef DISPLAY_DOT
#if defined RES_CN || defined RES_KR
		iVal2 = pBuf[i+1];
		if (_IsExtendedChar(iVal) && _IsExtendedChar(iVal2))
		{
			if (iVal != pLine[i] || iVal2 != pLine[i+1])
			{
				_display_CJK((PCHAR)(pBuf+i), i, iLine);
				pLine[i] = iVal;
				pLine[i+1] = iVal2;
			}
			i += 2;
			continue;
		}
#endif
#endif
		if (iVal != pLine[i])
		{
			_display_ascii(iVal, i, iLine);
			pLine[i] = iVal;
		}
		i ++;
	} while (i < DISPLAY_MAX_CHAR);

	for (; i < DISPLAY_MAX_CHAR; i ++)
	{
		if (pLine[i] != (UCHAR)' ')
		{
			_display_ascii(' ', i, iLine);
			pLine[i] = ' ';
		}
	}

#ifndef CALL_NONE
#ifdef SYS_PROGRAMMER
	UdpDebugString(pBuf);
#endif
#endif

#ifdef SERIAL_UI
	_SerialDisplay(pBuf, iLine);
#endif
}

#else

void LcdInit()
{
}

void LcdStart()
{
}

void LcdCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow)
{
	iLine;
	iPos;
	bShow;
}

void LcdDisplay(PCHAR pBuf, UCHAR iLine)
{
	pBuf;
	iLine;
}

#endif	// SYS_LCD

#ifdef LCD_DEBUG

void LcdDebugVal(USHORT sVal, UCHAR iRadix, UCHAR iLine)
{
	UCHAR pBuf[DISPLAY_MAX_CHAR];

	itoa(sVal, pBuf, iRadix);
	LcdDisplay(pBuf, iLine);
}

void LcdDebugVals(PCHAR pVal, UCHAR iLen, UCHAR iLine)
{
	UCHAR i, iVal;
	UCHAR pBuf[DISPLAY_MAX_CHAR + 1];
	PCHAR pCur;

	if (iLen > DISPLAY_MAX_CHAR/2)	iLen = DISPLAY_MAX_CHAR/2;
	pCur = pBuf;
	for (i = 0; i < iLen; i ++)
	{
		iVal = pVal[i];
		if (iVal >= 0x10)
		{
			itoa(iVal, pCur, 16);
			pCur += 2;
		}
		else
		{
			*pCur ++ = '0';
			itoa(iVal, pCur, 16);
			pCur ++;
		}
	}
	LcdDisplay(pBuf, iLine);
}

#endif	// LCD_DEBUG

void LcdCenterDisplay(PCHAR pString, UCHAR iLine)
{
	UCHAR iPos, iLen;
	UCHAR pLine[DISPLAY_MAX_CHAR+2];

	iLen = strlen(pString);
	if (iLen >= DISPLAY_MAX_CHAR-1) 
	{
		LcdDisplay(pString, iLine);
	}
	else
	{
		iPos = (DISPLAY_MAX_CHAR - iLen) >> 1;
		memset(pLine, ' ', iPos);
		strcpy((PCHAR)(pLine + iPos), pString);
		LcdDisplay(pLine, iLine);
	}
}


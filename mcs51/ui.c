/*-------------------------------------------------------------------------
   AR168M demo external controller MCS51
   UI function copy right information

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

#define MODE_ZERO		0
#define MODE_COMMAND	1
#define MODE_DATA		2

#define COMMAND_LEN		4
#define MAX_DATA_LEN	24

DCHAR _iMode, _iCommandPos, _iDataPos;
DCHAR _pCommand[COMMAND_LEN];
DCHAR _pData[MAX_DATA_LEN + 1];

void UI_Init()
{
	_iMode = MODE_ZERO;
	_iCommandPos = _iDataPos = 0;
}

BOOLEAN _isprint(UCHAR iVal) 
{
	return (iVal >= 0x20 && iVal < 0x7f) ? TRUE : FALSE;
}

const CCHAR _pKey[] = "KEY ";
const CCHAR _pLcd[] = "LCD ";
const CCHAR _pLcdCursor[] = "LCDC";
const CCHAR _pLed[] = "LED ";
const CCHAR _pLoop[] = "LOOP";

void _Padding()
{
	DCHAR i;

	for (i = 0; i < 4; i ++)
	{
		UART_SendChar(0);
	}
}

void _SendCommand(PCCHAR pCommand)
{
	DCHAR i;

	for (i = 0; i < COMMAND_LEN; i ++)
	{
		UART_SendChar(pCommand[i]);
	}
}

void _SendData(PDCHAR pData)
{
	DCHAR i;

	for (i = 0; i < MAX_DATA_LEN; i ++)
	{
		UART_SendChar(pData[i]);
		if (!pData[i])
		{
			break;
		}
	}
}

BOOLEAN _CompareCommand(const PCCHAR p)
{
	return memcmp_c2d(_pCommand, p, COMMAND_LEN) ? FALSE : TRUE;
}

void _HandleData()
{
	DCHAR iVal;

	if (_CompareCommand(_pLcd))
	{
		LcdDisplay((PDCHAR)(_pData + 1), _pData[0] - '0');
	}
	else if (_CompareCommand(_pLcdCursor))
	{
		LcdCursor(_pData[0] - '0', _pData[1] - '0', (BOOLEAN)(_pData[2] - '0'));
	}
	else if (_CompareCommand(_pLed))
	{
		iVal = _pData[0] - '0';
		switch (_pData[1])
		{
		case '0':
			LED_On(iVal);
			break;

		case '1':
			LED_Off(iVal);
			break;

		case '2':
			LED_Blink(iVal);
			break;
		}
	}
	else if (_CompareCommand(_pLoop))
	{
		_SendCommand(_pLoop);
		_SendData(_pData);
		_Padding();
	}
}

void UI_HandleSerialChar(UCHAR iVal)
{
//	UART_SendChar(iVal);	// Simple loop back
	
	// Analyze it!
	if (_iMode == MODE_COMMAND)
	{
		if (_iCommandPos < COMMAND_LEN)
		{
			if (_isprint(iVal))
			{
				_pCommand[_iCommandPos] = iVal;
				_iCommandPos ++;
			}
			else
			{
				_iMode = MODE_ZERO;
			}
		}
		else
		{
			if (iVal)
			{
				_iMode = MODE_DATA;
				_pData[0] = iVal;
				_iDataPos = 1;
			}
			else
			{
				_iMode = MODE_ZERO;
			}
		}
	}
	else if (_iMode == MODE_DATA)
	{
		if (iVal)
		{
			if (_iDataPos < MAX_DATA_LEN)
			{
				_pData[_iDataPos] = iVal;
				_iDataPos ++;
			}
		}
		else
		{
			_pData[_iDataPos] = 0;
			_HandleData();
			_iMode = MODE_ZERO;
		}
	}
	else	// MODE_ZERO
	{
		if (_isprint(iVal))
		{
			_iMode = MODE_COMMAND;
			_pCommand[0] = iVal;
			_iCommandPos = 1;
		}
	}
}

void UI_HandleKey(UCHAR iKey)
{
	DCHAR pBuf[2];

	if (!iKey)	return;

	pBuf[0] = iKey;
	pBuf[1] = 0;
#ifdef SYS_DEBUG
	LcdDisplay(pBuf, 0);
#endif

	_SendCommand(_pKey);
	_SendData(pBuf);
	_Padding();
}

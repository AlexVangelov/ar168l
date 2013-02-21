/*-------------------------------------------------------------------------
   AR168M demo external controller MCS51
   UART function copy right information

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

#define UART_BUF_LEN	48

volatile DCHAR _iInputHead, _iInputTail;
volatile ICHAR _pInputBuf[UART_BUF_LEN];

void SerialInterrupt() __interrupt 4 __using 2
{
	DCHAR iVal;

	if (RI)
	{
		if (_iInputTail != _iInputHead)
		{
			_pInputBuf[_iInputHead] = SBUF;
			_iInputHead ++;
			if (_iInputHead == UART_BUF_LEN)
			{
				_iInputHead = 0;
			}
		}
		else
		{
			iVal = SBUF;	// drop it
		}
		RI = 0;
	}
}

void UART_SendChar(UCHAR iVal)
{
//	EA = 0;

	while (!TI);
	TI = 0;
	SBUF =  iVal;

//	EA = 1;
}

void UART_Init()
{
	_iInputHead = 0;
	_iInputTail = UART_BUF_LEN - 1;
}

void UART_Run()
{
	DCHAR iTail;

	iTail = _iInputTail + 1;
	if (iTail == UART_BUF_LEN)
	{
		iTail = 0;
	}

	while (iTail != _iInputHead)
	{
		UI_HandleSerialChar(_pInputBuf[iTail]);

		_iInputTail = iTail;
		iTail ++;
		if (iTail == UART_BUF_LEN)
		{
			iTail = 0;
		}
	}
}


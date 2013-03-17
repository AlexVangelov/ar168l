/*-------------------------------------------------------------------------
   AR1688 Serial function copy right information

   Copyright (c) 2007-2012. Lin, Rongrong <woody@palmmicro.com>

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
#include "ar168.h"
#include "core.h"
#include "apps.h"

#include "bank1.h"

#ifdef SYS_UART
volatile UCHAR Serial_iRecvHead, Serial_iRecvTail;
volatile UCHAR Serial_pRecv[SERIAL_BUF_SIZE_RX];

#ifdef SERIAL_UI

//#define SERIAL_STRING_SIZE	64		// max string length to handle
#define SERIAL_STRING_SIZE	48		// max string length to handle

UCHAR Serial_iSendHead, Serial_iSendTail;
UCHAR Serial_pSend[SERIAL_BUF_SIZE_TX];

PCHAR _pUnHandledData;
UCHAR _iUnHandledLen;


void SerialInit()
{
	Serial_iSendHead = 0;
	Serial_iSendTail = SERIAL_BUF_SIZE_TX - 1;
	Serial_iRecvHead = 0;
	Serial_iRecvTail = SERIAL_BUF_SIZE_RX - 1;

	_pUnHandledData = NULL;
	_iUnHandledLen = 0;

	UART_Init(78, FALSE);	// 19200 bps, 8 bit data, 1 bit stop, no parity
}

#define SERIAL_COMMAND_LEN	4

const UCHAR _pStat[] = "STAT";
const UCHAR _pText[] = "TEXT";

void SerialUI_Event(UCHAR iType)
{
	UCHAR pBuf[SERIAL_COMMAND_LEN + 4];

	strcpy(pBuf, _pStat);
	itoa(iType, (PCHAR)(pBuf + SERIAL_COMMAND_LEN), 10);
	SerialSendString(pBuf);
//	UdpDebugString(pBuf);
}

void SerialUI_DisplayMsg(PCHAR pMsg)
{
	if (!memcmp(pMsg, _pText, SERIAL_COMMAND_LEN))
	{
		UdpDebugString("Send Serial TEXT");
		SerialSendString(pMsg);
	}
}

void _SerialProcessString(PCHAR pStr)
{
	// Simply call UdpDebugString();
//	UdpDebugString(pStr);
	if (!memcmp_str(pStr, "LOOP"))
	{
		UdpDebugString(pStr);
	}
	else if (!memcmp_str(pStr, "KEY "))
	{
		UdpDebugString("Serial KEY");
		UI_HandleKeys(pStr[SERIAL_COMMAND_LEN]);
	}
	else if (!memcmp_str(pStr, _pText))
	{
		UdpDebugString("Receive Serial TEXT");
		TaskHandleEvent(UI_EVENT_TEXT, (USHORT)pStr);
	}
}

BOOLEAN _SerialHandleString(PCHAR pBuf, UCHAR iLen)
{
	UCHAR iTotal;
	PCHAR p;

	if (_iUnHandledLen)
	{	// Has old unhandled data, we do not support remalloc, so need to malloc a larger buffer again
		iTotal = _iUnHandledLen + iLen;
		if (iTotal >= SERIAL_STRING_SIZE)
		{	// must be something wrong
			free(_pUnHandledData);
			_iUnHandledLen = 0;
			return FALSE;
		}
		p = malloc(iTotal);
		if (p)	// fail silently when malloc failed
		{
			memcpy(p, _pUnHandledData, _iUnHandledLen);
			memcpy((PCHAR)(p + _iUnHandledLen), pBuf, iLen);
			free(_pUnHandledData);
			_iUnHandledLen = 0;

			_SerialProcessString(p);
			free(p);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if (iLen > 1)
		{
			_SerialProcessString(pBuf);
		}
	}
	return TRUE;
}

BOOLEAN _SerialSaveUnHandledData(PCHAR pBuf, UCHAR iLen)
{
	UCHAR iTotal;
	PCHAR p;

	if (_iUnHandledLen)
	{	// Has old unhandled data, we do not support remalloc, so need to malloc a larger buffer again
		iTotal = _iUnHandledLen + iLen;
		if (iTotal >= SERIAL_STRING_SIZE)
		{	// must be something wrong
			free(_pUnHandledData);
			_iUnHandledLen = 0;
			return FALSE;
		}
		p = malloc(iTotal);
		if (p)	// fail silently when malloc failed
		{
			memcpy(p, _pUnHandledData, _iUnHandledLen);
			memcpy((PCHAR)(p + _iUnHandledLen), pBuf, iLen);
			free(_pUnHandledData);
			_pUnHandledData = p;
			_iUnHandledLen = iTotal;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		_pUnHandledData = malloc(iLen);
		if (_pUnHandledData)	// fail silently when malloc failed
		{
			memcpy(_pUnHandledData, pBuf, iLen);
			_iUnHandledLen = iLen;
		}
		else
		{
			return FALSE;
		}
	}
//	UdpDebugVal(_iUnHandledLen, 10);
	return TRUE;
}

// Handle UART received data here, as an loop back example, we simply use UDP debug output
void _SerialHandleData(PCHAR pBuf, UCHAR iLen)	
{
	UCHAR i, iStrLen, iZeroNum;
	PCHAR pCur;

	iZeroNum = 0;

	// get "real" data by minus 1
	for (i = 0; i < iLen; i ++)
	{
//		pBuf[i] --;
		if (!pBuf[i])
		{
			iZeroNum ++;
		}
	}

	pCur = pBuf;
	for (i = 0; i < iZeroNum; i ++)
	{
		iStrLen = strlen(pCur) + 1;
		if (!_SerialHandleString(pCur, iStrLen))
		{
			UdpDebugString("UART data unhanded");
		}
		pCur += iStrLen;
		iLen -= iStrLen;
	}
	if (iLen)
	{
		if (!_SerialSaveUnHandledData(pCur, iLen))
		{
			UdpDebugString("UART data lost");
		}
	}
}

void _SerialSendData()
{
	UCHAR iTail;

//	DI;

	iTail = Serial_iSendTail + 1;
	if (iTail == SERIAL_BUF_SIZE_TX)
	{
		iTail = 0;
	}

	while (iTail != Serial_iSendHead)
	{
		if (UART_SendByte(Serial_pSend[iTail]))
		{
			Serial_iSendTail = iTail;
			iTail ++;
			if (iTail == SERIAL_BUF_SIZE_TX)
			{
				iTail = 0;
			}
		}
		else
		{
			break;
		}
	}

//	EI;
}

void SerialRun()
{
	UCHAR iLen;
	UCHAR pBuf[SERIAL_BUF_SIZE_RX];
	UCHAR iTail;

	_SerialSendData();

	iLen = 0;

	DI;

	iTail = Serial_iRecvTail + 1;
	if (iTail == SERIAL_BUF_SIZE_RX)
	{
		iTail = 0;
	}

	while (iTail != Serial_iRecvHead)
	{
		pBuf[iLen] = Serial_pRecv[iTail];
		iLen ++;
		Serial_iRecvTail = iTail;
		iTail ++;
		if (iTail == SERIAL_BUF_SIZE_RX)
		{
			iTail = 0;
		}
	}

	EI;

	if (iLen)
	{
		_SerialHandleData(pBuf, iLen);
	}

	_SerialSendData();
}

void SerialSend(PCHAR pBuf, UCHAR iLen)
{
	UCHAR i;

	_SerialSendData();
	for (i = 0; i < iLen; i ++)
	{
		if (Serial_iSendTail != Serial_iSendHead)
		{
			Serial_pSend[Serial_iSendHead] = pBuf[i];
			Serial_iSendHead ++;
			if (Serial_iSendHead == SERIAL_BUF_SIZE_TX)
			{
				Serial_iSendHead = 0;
			}
		}
	}
	_SerialSendData();
}

void SerialSendString(PCHAR pStr)
{
	UCHAR pZero[4];

	memset(pZero, 0, 4);
	SerialSend(pStr, strlen(pStr) + 1);
	SerialSend(pZero, 4);
}

#elif defined SERIAL_LOOP

// UART serial template

// ToDo: Add user globals here

//	Initialize serial communication (has to be called before/after[...])
void SerialInit()
{
	// ToDo: Insert user specific initiaization here
	Serial_iRecvHead = 0;
	Serial_iRecvTail = SERIAL_BUF_SIZE_RX - 1;

	UART_Init(78, FALSE);	// 19200 bps, 8 bit data, 1 bit stop, no parity
}

// Serial handler (main-loop)
void SerialRun()
{
	UCHAR i, iLen, iTail;
	UCHAR pBuf[SERIAL_BUF_SIZE_RX];

	iLen = 0;

	// read received data
	DI;

	iTail = Serial_iRecvTail + 1;
	if (iTail == SERIAL_BUF_SIZE_RX)
	{
		iTail = 0;
	}

	while (iTail != Serial_iRecvHead)
	{
		pBuf[iLen] = Serial_pRecv[iTail];
		iLen ++;
		Serial_iRecvTail = iTail;
		iTail ++;
		if (iTail == SERIAL_BUF_SIZE_RX)
		{
			iTail = 0;
		}
	}

	EI;

	// loop back
	for (i = 0; i < iLen; i ++)
	{
		if (!UART_SendByte(pBuf[i]))			// Check for transmit error
		{
			UdpDebugString("UART Send Error");		// Report error on Udp
		}
	}
}
#elif defined SERIAL_CARD_READER
volatile UCHAR cardBuf[32];
volatile USHORT cardLen;

void SerialInit()
{
        // ToDo: Insert user specific initiaization here
        Serial_iRecvHead = 0;
        Serial_iRecvTail = SERIAL_BUF_SIZE_RX - 1;

	UART_Init(156, FALSE);	//9600 8N1
	//UART_Init(39, FALSE);	// 38400

	strcpy(cardBuf,"");
	cardLen = 0;
}
void SerialRun() {
	UCHAR i, iLen, iTail;
	UCHAR pBuf[SERIAL_BUF_SIZE_RX];

	iLen = 0;

	// read received data
	DI;
	iTail = Serial_iRecvTail + 1;
	if (iTail == SERIAL_BUF_SIZE_RX) iTail = 0;

	while (iTail != Serial_iRecvHead)
	{
		pBuf[iLen] = Serial_pRecv[iTail];
		iLen ++;
		Serial_iRecvTail = iTail;
		iTail ++;
		if (iTail == SERIAL_BUF_SIZE_RX) iTail = 0;
	}
	EI;
	if (iLen > 0) {
		pBuf[iLen] = 0;
		for(i=0;i<iLen;i++) {
			cardBuf[sLen] = pBuf[i];
			cardLen++;
			if (pBuf[i] == 0x0d || pBuf[i] == 0x0a) {
				cardBuf[sLen-1] = 0;
				if (cardLen>1) {
					// Card 
					UdpDebugString(sBuf);
				}
				cardLen = 0;
			}
			if (cardLen >= sizeof(cardBuf)) cardLen=0;
		}
	}
}
#endif
#endif	// SYS_UART


/*-------------------------------------------------------------------------
   AR1688 Buf function copy right information

   Copyright (c) 2006-2010. Lin, Rongrong <woody@palmmicro.com>
                            Tang, Li      <tangli@palmmicro.com>

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
#include "ar168.h"
#include "core.h"

USHORT _sBufPos;

void BufCopyDM2PM()
{
	UCHAR i, j;
	UCHAR pBuf[256];
	PCHAR pCur;

	for (i = 0; i < 3; i ++)
	{
		pCur = (PCHAR)SRAM_BASE;
		for (j = 0; j < 63; j ++)
		{
			SetMemoryPage(_cMemoryPages[i+3]);
			memcpy(pBuf, pCur, 256);
			SetMemoryPage(_cMemoryPages[i]);
			memcpy(pCur, pBuf, 256);
			pCur += 256;
		}
	}
	SetMemoryPage(DEFAULT_MEMORY_PAGE);
}

void BufForward(USHORT sLen)
{
	_sBufPos += sLen;
}

void BufSeek(USHORT sOffset)
{
	_sBufPos = sOffset;
}

/* sLen can't exceed 0x2000 */
void _BufReadWrite(PCHAR pBuf, USHORT sLen, BOOLEAN bWrite)
{
	UCHAR iPageS, iPageE;
	USHORT sLen1, sLen2, sPageS;
	PCHAR pSram;

	if (!sLen)		return;

	iPageS = _sBufPos >> 14;
	iPageE = (_sBufPos + sLen) >> 14;

	if (iPageS == iPageE)
	{
		sLen1 = sLen;
		sLen2 = 0;
	}
	else
	{
		sLen1 = (iPageE << 14) - _sBufPos;
		sLen2 = sLen - sLen1;
	}

	SetMemoryPage(_cMemoryPages[iPageS]);
	sPageS = iPageS;
	pSram = (PCHAR)(SRAM_BASE + _sBufPos - (sPageS << 14)); 
	if (bWrite)
	{
		memcpy(pSram, pBuf, sLen1);
	}
	else
	{
		memcpy(pBuf, pSram, sLen1);
	}

	if (sLen2)
	{
		SetMemoryPage(_cMemoryPages[iPageE]);
		pBuf += sLen1;
		if (bWrite)
		{
			memcpy((PCHAR)SRAM_BASE, pBuf, sLen2);
		}
		else
		{
			memcpy(pBuf, (PCHAR)SRAM_BASE, sLen2);
		}
	}
	SetMemoryPage(DEFAULT_MEMORY_PAGE);
}

void BufWrite(PCHAR pBuf, USHORT sLen)
{
	_BufReadWrite(pBuf, sLen, TRUE);
}

void BufRead(PCHAR pBuf, USHORT sLen)
{
	_BufReadWrite(pBuf, sLen, FALSE);
}

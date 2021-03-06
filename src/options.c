/*-------------------------------------------------------------------------
   AR1688 Options function copy right information

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
#include "apps.h"

void OptionsGetString(PCHAR pStr, USHORT sOffset, UCHAR iMaxLen)
{
	UCHAR iLen;

	iLen = iMaxLen - 1;
	OptionsGetBuf(pStr, sOffset, iLen);
	pStr[iLen] = 0;
}

USHORT OptionsGetShort(USHORT sOffset)
{
	UCHAR pBuf[2];

	OptionsGetBuf(pBuf, sOffset, 2);
	return PCHAR2USHORT(pBuf);
}

void OptionsWriteBuf(PCHAR pBuf, USHORT sOffset, UCHAR iLen)
{
	UCHAR pData[255];
	if (iLen)
	{
		memcpy(pData, pBuf, iLen);
		BufSeek(sOffset);
		BufWrite(pData, iLen);
	}
}

void OptionsWriteByte(USHORT sOffset, UCHAR iVal)
{
	BufSeek(sOffset);
	BufWrite(&iVal, 1);
}

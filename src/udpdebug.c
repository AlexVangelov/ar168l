/*-------------------------------------------------------------------------
   AR1688 UDP debug function copy right information

   Copyright (C) 2006-2010. Lin, Rongrong <woody@palmmicro.com>

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
#include "tcpip.h"

/*
void UdpDebugString(PCHAR pStr)
{
	Syslog(SYSLOG_PRI_DEBUG, pStr);
}
*/

void UdpDebugVal(USHORT sVal, UCHAR iRadix)
{
	UCHAR pBuf[7];		// (2+4+1)
	PCHAR pCur;

	pCur = pBuf;
	if (iRadix == 16)
	{
		strcpy(pCur, "0x");	// 2
		pCur += 2;
	}
	itoa(sVal, pCur, iRadix);		// 4/5
	UdpDebugString(pBuf);
}

void UdpDebugVals(PCHAR pVals, USHORT sLen)
{
	USHORT s;
	UCHAR iVal;
	UCHAR p[48];	// (2+1)*15+2+1

	p[0] = 0;
	for (s = 0; s < sLen; s++)
	{
		iVal = pVals[s];
		if (iVal < 0x10)
		{
			strcat_char(p, '0');
		}
		itoa(iVal, p+strlen(p), 16);	// 2
		if ((s&0x0f) == 0x0f)
		{
			UdpDebugString(p);
			p[0] = 0;
		}
		else
		{
			strcat_char(p, ' ');				// 1
		}
	}
	UdpDebugString(p);
}

void UdpDebugIDS(UCHAR iIDS)
{
	UCHAR pBuf[21];		// Max(DISPLAY_MAX_CHAR) + 1

	TaskLoadString(iIDS, pBuf);
	UdpDebugString(pBuf);
}

void UdpDebugInfo(PCHAR pPrefix, USHORT sAddr, USHORT sLen)
{
	UCHAR pStr[MAX_UDP_DEBUG];

	strcpy(pStr, pPrefix);
	strcat(pStr, " addr: 0x");
	itoa(sAddr, pStr + strlen(pStr), 16);
	strcat(pStr, " len: ");
	itoa(sLen, pStr + strlen(pStr), 10);
	UdpDebugString(pStr);
}

void UdpDebugIpPort(PCHAR pPrefix, PCHAR pIp, USHORT sPort)
{
	UCHAR pStr[MAX_UDP_DEBUG];

	strcpy(pStr, pPrefix);
	ip2str(pIp, pStr + strlen(pStr));
	strcat(pStr, ":");
	itoa(sPort, pStr + strlen(pStr), 10);
	UdpDebugString(pStr);
}

void UdpDebugChar(UCHAR iChar)
{
	UCHAR pBuf[2];

	pBuf[0] = iChar;
	pBuf[1] = 0;
	UdpDebugString(pBuf);
}

void UdpDebugTcpHead(UCHAR iHeadLen)
{
	UCHAR pHead[24];

	memcpy(pHead, Adapter_pIpBuf, iHeadLen);
	UdpDebugVals(pHead, iHeadLen);
}


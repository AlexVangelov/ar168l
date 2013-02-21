/*-------------------------------------------------------------------------
   AR1688 Syslog function copy right information

   Copyright (c) 2007-2012. Tang, Li      <tangli@palmmicro.com>
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

// RFC 3164. The BSD syslog Protocol

#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"

#define SYSLOG_SERVER_PORT	514

#define SYSLOG_HEAD		42

UDP_SOCKET _pSyslogSocket;
UCHAR _pSyslogBackup[UDP_EXTRA + SYSLOG_HEAD + MAX_UDP_DEBUG];
BOOLEAN _bUseSyslog;

const UCHAR _cMonth[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

void SyslogOpen()
{
//	OptionsGetBuf(Sys_pLogIp, OPT_SYSLOG_IP, IP_ALEN);
	_pSyslogSocket = UdpListen(SYSLOG_SERVER_PORT, UDP_DEBUG_BANK_OFFSET, 0);
	if (!Sys_pLogIp[0] && !Sys_pLogIp[1])
	{
		_bUseSyslog = FALSE;
		UdpConnectTo(_pSyslogSocket, _pBroadcastIP, SYSLOG_SERVER_PORT);
	}
	else
	{
		_bUseSyslog = TRUE;
		UdpConnectTo(_pSyslogSocket, Sys_pLogIp, SYSLOG_SERVER_PORT);
	}
}

void SyslogClose()
{
	UdpClose(_pSyslogSocket);
}

UCHAR _BuildSyslogHead(PCHAR pCur, UCHAR iPri)
{
	UCHAR iLen;

	memcpy(pCur, "<160>", 5);
	pCur[3] += iPri;
	pCur += 5;

	memcpy(pCur, _cMonth[Sys_tmLocal.iMon - 1], 3);
	pCur += 3;
	*pCur++ = ' ';
	if (Sys_tmLocal.iMday < 10)
	{
		*pCur++ = ' ';
		*pCur++ = Sys_tmLocal.iMday + '0';
	}
	else
	{
		Fill2Digits(pCur, Sys_tmLocal.iMday);
		pCur += 2;
	}
	*pCur++ = ' ';
	Fill2Digits(pCur, Sys_tmLocal.iHour);
	pCur += 2;
	*pCur++ = ':';
	Fill2Digits(pCur, Sys_tmLocal.iMin);
	pCur += 2;
	*pCur++ = ':';
	Fill2Digits(pCur, Sys_tmLocal.iSec);
	pCur += 2;
	*pCur++ = ' ';

	ip2str(Sys_pIpAddress, pCur);
	iLen = (UCHAR)strlen(pCur);
	pCur += iLen;
	*pCur ++ = ' ';
	
	return iLen + 22;
}

void _SyslogSend(USHORT sLen)
{
	strcat_char((PCHAR)(Adapter_pUdpBuf + sLen), '\n');
	UdpSend(_pSyslogSocket, sLen + 2);
}

void Syslog(UCHAR iPri, PCHAR pStr)
{
	USHORT sLen, sBackupLen;
	UCHAR iLen;

	if (!_bUseSyslog)
	{
		if (iPri != SYSLOG_PRI_NOTICE)	return;
	}
	if (!pStr)			return;
	sLen = strlen(pStr);
	if (!sLen || sLen > MAX_UDP_DEBUG)	return;		// debug message too long, fail silently

	// backup
	sBackupLen = sLen + SYSLOG_HEAD + UDP_EXTRA;	// remember extra byte for checksum
	memcpy(_pSyslogBackup, Adapter_pPacketBuf, sBackupLen);

	iLen = _BuildSyslogHead(Adapter_pUdpBuf, iPri);
	strcpy((PCHAR)(Adapter_pUdpBuf + iLen), pStr);
	_SyslogSend(sLen + iLen);

	// restore
	memcpy(Adapter_pPacketBuf, _pSyslogBackup, sBackupLen);
}

void SyslogAllUdpData(USHORT sLen)
{
	UCHAR pHead[SYSLOG_HEAD];
	UCHAR iLen;

	iLen = _BuildSyslogHead(pHead, SYSLOG_PRI_DEBUG);
	memmove((PCHAR)(Adapter_pUdpBuf + iLen), Adapter_pUdpBuf, sLen);
	memcpy(Adapter_pUdpBuf, pHead, iLen);
	_SyslogSend(sLen + iLen);
}


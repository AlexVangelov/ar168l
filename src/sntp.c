/*-------------------------------------------------------------------------
   AR1688 SNTP function copy right information

   Copyright (c) 2006-2010. Tang, Li <tangli@palmmicro.com>

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
"time.windows.com",
"time.nist.gov",
"swisstime.ethz.ch",
"pool.ntp.org",
*/

#define SNTP_SERVER_PORT	123
#define SNTP_SRC_PORT		1029

/* Field offset of an NTP packet (following the UDP header) */
#define SNTP_FLAG		0
#define SNTP_STRATUM	1
#define SNTP_POLL		2
#define SNTP_PRECISION	3
#define SNTP_ROOTDL		4
#define SNTP_ROOTDP		8
#define SNTP_REFID		12
#define SNTP_REFRTIME	16
#define SNTP_ORIGTIME	24
#define SNTP_RECVTIME	32
#define SNTP_TRANTIME	40
#define SNTP_DATA		48

/* SNTP status */
#define SNS_FREE		0
#define SNS_PENDING		1
#define SNS_RESOLVED	2

#define SNTP_RETRY_TIMEOUT	3
#define SNTP_MAX_RETRY		3

void SntpQuery(PCHAR pServerIP);
void SntpRun(UDP_SOCKET pUcb);

UDP_SOCKET _pSntpSocket;
UCHAR _iSntpTimer, _iSntpRetry, _iSntpStatus;
BOOLEAN _bSntpStartup;

void _SntpSendQuery()
{
	PCHAR pCur;

	pCur = Adapter_pUdpBuf;
	memset(pCur, 0, SNTP_DATA);	//Preparing 48 byte NTP packet

	//Set Flags: (00) leap indicator no warning, (011) NTP version 3, (011) SNTP Client
	pCur[0] = 0x1b;

	UdpSend(_pSntpSocket, SNTP_DATA);
}

void SntpInit()
{
	_iSntpStatus = SNS_FREE;
	_pSntpSocket = NULL;
	_iSntpTimer = 0;
	_iSntpRetry = 0;
}

void SntpClose()
{
	if (_pSntpSocket)
	{
		UdpClose(_pSntpSocket);
	}
	SntpInit();
	if (_bSntpStartup)
	{
		_bSntpStartup = FALSE;
		TaskHandleEvent(UI_EVENT_LOGON, 0);
	}
}

void SntpQuery(PCHAR pServerIP)
{
	if (pServerIP)
	{
		if (!_pSntpSocket)
		{
			_pSntpSocket = UdpListen(SNTP_SRC_PORT, NET_BANK_OFFSET, (USHORT)SntpRun);
			UdpConnectTo(_pSntpSocket, pServerIP, SNTP_SERVER_PORT);
		}
		_SntpSendQuery();
		_iSntpTimer = 0;
		_iSntpRetry = 0;
		_iSntpStatus = SNS_PENDING;
	}
	else
	{
		SntpClose();
		TaskUIHandler(UI_SNTP_FAILED, 0);
	}
}

void SntpRun(UDP_SOCKET pUcb)
{
	PCHAR pBuf;

	if (pUcb != _pSntpSocket)	return;
	if (_iSntpStatus != SNS_PENDING || pUcb->sLen != SNTP_DATA)
	{
		return;
	}

	pBuf = Adapter_pUdpBuf;
	Sys_lCurrentTime = PCHAR2ULONG((PCHAR)(pBuf + SNTP_TRANTIME));
	TaskUIHandler(UI_SNTP_DONE, 0);

	SntpClose();
	_iSntpStatus = SNS_RESOLVED;
	srand((UCHAR)Sys_lCurrentTime);
}

void SntpStart(BOOLEAN bStartup)
{
	UCHAR pDns[MAX_DNS_LEN];
	UCHAR pDstIP[IP_ALEN];

	_bSntpStartup = bStartup;
	OptionsGetString(pDns, OPT_SNTP_DNS, MAX_DNS_LEN);
	if (strlen(pDns))
	{
		UdpDebugIDS(IDS_GET_DATETIME);
		if (str2ip(pDns, pDstIP))
		{
			SntpQuery(pDstIP);
		}
		else
		{
			TaskDnsQuery(pDns, DNS_TYPE_A, NET_BANK_OFFSET, (USHORT)SntpQuery);
		}
	}
	else
	{
		SntpQuery(NULL);
	}
}

void SntpTimer()
{
	if (_iSntpStatus == SNS_FREE)
	{
		_iSntpTimer ++;
		if (!_iSntpTimer)
		{	// try to get time every 4 minutes
			SntpStart(FALSE);
		}
	}
	else if (_iSntpStatus == SNS_PENDING)
	{
		_iSntpTimer ++;
		if (_iSntpTimer > (1 << _iSntpRetry))
		{
			_iSntpTimer = 0;
			_iSntpRetry ++;
			if (_iSntpRetry > SNTP_MAX_RETRY)
			{
				TaskUIHandler(UI_SNTP_FAILED, 0);
				SntpClose();
			}
			else
			{
				_SntpSendQuery();
			}
		}
	}
	else if (_iSntpStatus == SNS_RESOLVED)
	{
		_iSntpTimer ++;
		if (!_iSntpTimer)
		{
			_iSntpRetry ++;
			if (!_iSntpRetry)
			{
				SntpStart(FALSE);	// get time again after 18 hours
			}
		}
	}
}

/*-------------------------------------------------------------------------
   AR1688 HTTP client function copy right information

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

#include "version.h"
#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "bank3.h"

#ifndef OEM_INNOMEDIA

#define MIN_PROV_TTL		60	// 60 minutes

#define HTTPC_TASK_NONE				0
#define HTTPC_TASK_GET_SETTINGS		1
#define HTTPC_TASK_SETTINGS_DONE	2
#define HTTPC_TASK_GET_FIRMWARE		3
#define HTTPC_TASK_REBOOT			4

#define HCS_INIT			0
#define HCS_HEADER_BEGIN	1
#define HCS_HEADER_END		2
#define HCS_REQUEST_END		3

#define HTTPC_SETTINGS_CONFIG		0
#define HTTPC_DIGITMAP_BEGIN		1	//digit map
#define HTTPC_DIGITMAP_END			128
#define HTTPC_PHONE_BOOK			129

#define CONTENT_LENGTH_TOKEN_LEN	14

TCP_HANDLE _pHttpcTcb;
UCHAR _iHttpcState;
UCHAR _iHttpcTask;
UCHAR _iHttpcUpdatePage;
UCHAR _iHttpcCurPage;
USHORT _sHttpcPostLen;
ULONG _lHttpcContentLen;
UCHAR _iHttpcRecvLen;
PCHAR _pHttpcRecvBuf;
USHORT _sHttpcTimer;
UCHAR _iHttpcCurContent;

void HttpcWriteStr(PCHAR pString)
{
	TcpWriteStr(_pHttpcTcb, pString);
}

BOOLEAN HttpcGetFile()
{
	UCHAR pBuf[MAX_DNS_LEN];
	UCHAR pDns[MAX_DNS_LEN];
	PCHAR pTemp;
	UCHAR iLen;

	if (_iHttpcTask != HTTPC_TASK_GET_SETTINGS && _iHttpcTask != HTTPC_TASK_GET_FIRMWARE)
		return FALSE;

	// Fill first line
	HttpcWriteStr("GET /");
	OptionsGetString(pDns, OPT_PROVISION_SERVER, MAX_DNS_LEN);
	pTemp = SkipField(pDns, '/');
	if (pTemp)
	{
		iLen = strlen(pTemp);
		if (iLen)
		{
			if (pTemp[iLen-1] != (UCHAR)'/')
			{
				pTemp[iLen] = '/';
				pTemp[iLen+1] = 0;
			}
			HttpcWriteStr(pTemp);
		}
	}

	if (_iHttpcTask == HTTPC_TASK_GET_SETTINGS)
	{
		mac2str(Sys_pMacAddress, pBuf);
		HttpcWriteStr(pBuf);
		HttpcWriteStr(".txt");
	}
	else
	{
		OptionsGetString(pBuf, OPT_PROV_FILENAME, MAX_DNS_LEN);
		if (!strlen(pBuf))
		{
			return FALSE;
		}
		HttpcWriteStr(pBuf);
	}
	HttpcWriteStr(" HTTP/1.1\r\n");

	// Fill header
	HttpcWriteStr("Pragma: no-cache\r\nAccept: */*\r\nHost: ");
	HttpcWriteStr(pDns);
	HttpcWriteStr("\r\nConnection: close\r\n\r\n");
	return TRUE;
}

void HttpcSaveRecv(UCHAR iVal)
{
	if (_iHttpcRecvLen < HTTP_MAX_RECV_LEN-1)
	{
		_pHttpcRecvBuf[_iHttpcRecvLen] = iVal;
		_iHttpcRecvLen ++;
	}
}

BOOLEAN HttpcCheckFirmware()
{
	UCHAR pOldFile[MAX_DNS_LEN];
	UCHAR pNewFile[MAX_DNS_LEN];
	UCHAR pFlag[32];
	PCHAR pCur;

	OptionsGetString(pNewFile, OPT_PROV_FILENAME, MAX_DNS_LEN);
	pCur = SkipField(pNewFile, '_');
	if (pCur)
	{
		OptionsGetString(pFlag, OPT_FLAG_PRODUCT, MAX_FLAG_PRODUCT_LEN);
		if (!strcmp(pNewFile, pFlag))
		{
			OptionsGetString(pFlag, OPT_FLAG_PROTOCOL, MAX_FLAG_PROTOCOL);
			strcpy(pOldFile, pFlag);
			strcat_char(pOldFile, '_');
			OptionsGetString(pFlag, OPT_FLAG_COUNTRY, MAX_FLAG_COUNTRY);
			strcat(pOldFile, pFlag);
			strcat_char(pOldFile, '_');
			OptionsGetString(pFlag, OPT_FLAG_OEM, MAX_FLAG_OEM_LEN);
			if (strlen(pFlag))
			{
				strcat(pOldFile, pFlag);
				strcat_char(pOldFile, '_');
			}
			OptionsGetString(pFlag, OPT_FLAG_VERSION, MAX_FLAG_VERSION);
			strcat(pOldFile, pFlag);
			strcat(pOldFile, ".bin");

			if (strcmp(pCur, pOldFile))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void HttpcCheckSettings()
{
	UCHAR pNewCfgid[MAX_CONFIG_ID];
	UCHAR pOldCfgid[MAX_CONFIG_ID];

	BufSeek(OPT_CONFIG_ID);
	BufRead(pNewCfgid, MAX_CONFIG_ID);

	OptionsGetBuf(pOldCfgid, OPT_CONFIG_ID, MAX_CONFIG_ID);
	if (memcmp(pNewCfgid, pOldCfgid, MAX_CONFIG_ID))
	{
		// Clear the left entries in digitmap
		if (_iHttpcCurContent >= HTTPC_DIGITMAP_BEGIN)
		{
			_pHttpcRecvBuf[0] = 0;
			for (;_iHttpcCurContent <= HTTPC_DIGITMAP_END; _iHttpcCurContent ++)
			{
				_pHttpcRecvBuf[DIGITMAP_ENTRY_LEN] = _iHttpcCurContent-HTTPC_DIGITMAP_BEGIN;
				TaskHttpEvent(HTTP_WRITE_APDM, (USHORT)_pHttpcRecvBuf);
			}
		}
		// Do NOT change the sequence of the following two functions 
		// in order to keep the new config id
		TaskSystemHandler(SYS_MODIFY_PAGE, SYSTEM_SETTINGS_PAGE);
		OptionsWriteBuf(pNewCfgid, OPT_CONFIG_ID, MAX_CONFIG_ID);
		FlashWritePage(SYSTEM_SETTINGS_PAGE);
		_iHttpcTask = HTTPC_TASK_REBOOT;
	}
	else
	{
		_iHttpcTask = HTTPC_TASK_SETTINGS_DONE;
	}
}

void HttpcHandleSettings()
{
	if (_iHttpcRecvLen)
	{
		_pHttpcRecvBuf[_iHttpcRecvLen] = 0;
		if (!strcmp(_pHttpcRecvBuf, "[settings]"))
		{
			_iHttpcCurContent = HTTPC_SETTINGS_CONFIG;
		}
		else if (!strcmp(_pHttpcRecvBuf, "[digitmap]"))
		{
			_iHttpcCurContent = HTTPC_DIGITMAP_BEGIN;
		}
		else if (!strcmp(_pHttpcRecvBuf, "[phonebook]"))
		{
			_iHttpcCurContent = HTTPC_PHONE_BOOK;
		}
		else 
		{
			if (_iHttpcCurContent == HTTPC_SETTINGS_CONFIG)
			{
				TaskHttpEvent(HTTP_WRITE_SETTINGS, (USHORT)_pHttpcRecvBuf);
			}
			else if (_iHttpcCurContent >= HTTPC_DIGITMAP_BEGIN && _iHttpcCurContent <= HTTPC_DIGITMAP_END)
			{
				if (_iHttpcRecvLen < DIGITMAP_ENTRY_LEN)
				{
					_pHttpcRecvBuf[DIGITMAP_ENTRY_LEN] = _iHttpcCurContent-HTTPC_DIGITMAP_BEGIN;
					TaskHttpEvent(HTTP_WRITE_APDM, (USHORT)_pHttpcRecvBuf);
					_iHttpcCurContent ++;
				}
			}
			else if (_iHttpcCurContent == HTTPC_PHONE_BOOK)
			{
				TaskHttpEvent(HTTP_WRITE_PB, (USHORT)_pHttpcRecvBuf);
			}
		}
		_iHttpcRecvLen = 0;
	}
}

void HttpcHandleFirstline(PCHAR pBuf)
{
	USHORT sRespCode;
	PCHAR pCur;
	
	pCur = pBuf;
	if (!memcmp_str(pCur, "HTTP/1.1 "))
	{
		pCur += 9;
		sRespCode = atoi(pCur, 10);
		if (sRespCode != 200)
		{
			_iHttpcTask = HTTPC_TASK_NONE;
		}
		_iHttpcState = HCS_HEADER_BEGIN;
	}
}

void HttpcHandleHeader(PCHAR pBuf, UCHAR iLen)
{
	PCHAR pCur;

	if (iLen <= CONTENT_LENGTH_TOKEN_LEN)	return;

	if (!memcmp_lowercase(pBuf, "content-length"))
	{
		pCur = (PCHAR)(pBuf + CONTENT_LENGTH_TOKEN_LEN);
		while (*pCur == (UCHAR)':' || *pCur == (UCHAR)' ' || *pCur == (UCHAR)'\t')
		{
			pCur ++;
		}
		_lHttpcContentLen = atol(pCur, 10);
	}
}

void HttpcHandleLine(PCHAR pBuf, UCHAR iLen)
{
	pBuf[iLen] = 0;

	if (_iHttpcState == HCS_INIT)
	{
		HttpcHandleFirstline(pBuf);
	}
	else if (_iHttpcState == HCS_HEADER_BEGIN)
	{
		if (iLen)
		{
			HttpcHandleHeader(pBuf, iLen);
		}
		else 
		{
			if (_lHttpcContentLen)
			{
				_iHttpcState = HCS_HEADER_END;
				if (_iHttpcTask == HTTPC_TASK_GET_SETTINGS)
				{
					ReadFlashPage(SYSTEM_SETTINGS_PAGE);
				}
				else if (_iHttpcTask == HTTPC_TASK_GET_FIRMWARE)
				{
					_iHttpcUpdatePage = (Sys_iSystemPage == SYSTEM_1ST_PAGE) ? 	SYSTEM_2ND_PAGE : SYSTEM_1ST_PAGE;
					_sHttpcPostLen = 0;
					_iHttpcCurPage = _iHttpcUpdatePage;
					BufSeek(0);
				}
			}
			else
			{
				_iHttpcState = HCS_REQUEST_END;
				_iHttpcTask = HTTPC_TASK_NONE;
			}
		}
	}
}

void HttpcHandleContent(USHORT sLen, PCHAR pBuf)
{
	UCHAR iVal;
	USHORT sLeftLen, sLen2;

	if (_iHttpcTask == HTTPC_TASK_GET_SETTINGS)
	{
		while (sLen && _lHttpcContentLen)
		{
			iVal = *pBuf;
			if (iVal == 0x0d || iVal == 0x0a)
			{
				HttpcHandleSettings();
			}
			else
			{
				HttpcSaveRecv(iVal);
			}
			_lHttpcContentLen --;
			sLen --;
			pBuf ++;		
		}
		if (!_lHttpcContentLen)
		{
			HttpcHandleSettings();
			_iHttpcState = HCS_REQUEST_END;
			HttpcCheckSettings();
		}
		return;
	}

	if (_iHttpcTask == HTTPC_TASK_GET_FIRMWARE)
	{
		if (_iHttpcUpdatePage == SYSTEM_INVALID_PAGE)
			goto End;

		sLeftLen = FILE_FULL_PAGE_SIZE - _sHttpcPostLen;
		if (sLen < sLeftLen || !sLeftLen)
		{
			BufWrite(pBuf, sLen);
			BufForward(sLen);
			_sHttpcPostLen += sLen; 
		}
		else
		{
			BufWrite(pBuf, sLeftLen);
			BufForward(sLeftLen);
			_sHttpcPostLen = 0;
			TaskUIHandler(UI_UPDATE_PROGRESS, (USHORT)_iHttpcCurPage);
			if (FlashWritePage(_iHttpcCurPage))
			{
				_iHttpcUpdatePage = SYSTEM_INVALID_PAGE;
				_iHttpcTask = HTTPC_TASK_NONE;
				goto End;
			}
			_iHttpcCurPage ++;
			if ((_iHttpcCurPage - _iHttpcUpdatePage) >= SYSTEM_PROGRAM_PAGE_NUM)
			{
				TaskSystemHandler(SYS_UPDATE_FLAG, _iHttpcCurPage);
				_iHttpcTask = HTTPC_TASK_REBOOT;
			}
			else
			{
				sLen2 = (USHORT)(sLen - sLeftLen);
				BufSeek(0);
				BufWrite((PCHAR)(pBuf + sLeftLen), sLen2);
				BufForward(sLen2);
				_sHttpcPostLen = sLen2;
			}
		}
	}

End:
	if (_lHttpcContentLen > (ULONG)sLen)
	{
		_lHttpcContentLen -= (ULONG)sLen;
	}
	else
	{
		_lHttpcContentLen = 0;
		_iHttpcState = HCS_REQUEST_END;
	}
}

void HttpcRunData(USHORT sLen, PCHAR pBuf)
{
	UCHAR iVal, iNextVal;
	BOOLEAN bBrokenLine;

	while (sLen)
	{
		if (_iHttpcState < HCS_HEADER_END)
		{
			bBrokenLine = FALSE;
			if (_iHttpcRecvLen && _pHttpcRecvBuf[_iHttpcRecvLen-1] == 0x0d)
			{
				bBrokenLine = TRUE;
				_iHttpcRecvLen --;
			}

			iVal = *pBuf;
			pBuf ++;
			sLen --;
			if (iVal == 0x0d)
			{
				if (sLen)
				{
					iNextVal = *pBuf;
					pBuf ++;
					sLen --;
					if (iNextVal == 0x0a)
					{
						HttpcHandleLine(_pHttpcRecvBuf, _iHttpcRecvLen);
						_iHttpcRecvLen = 0;
					}
					else
					{
						HttpcSaveRecv(iVal);
						HttpcSaveRecv(iNextVal);
					}
					continue;
				}
			}
			else if (iVal == 0x0a)
			{
				if (bBrokenLine)
				{
					HttpcHandleLine(_pHttpcRecvBuf, _iHttpcRecvLen);
					_iHttpcRecvLen = 0;
					bBrokenLine = FALSE;
					continue;
				}
			}
			HttpcSaveRecv(iVal);
		}
		else 
		{
			if (_iHttpcState == HCS_HEADER_END)
			{
				HttpcHandleContent(sLen, pBuf);
			}
			break;
		}
	}
}
/*************************************************************************
**** Httpc API functions     *********************************************
**************************************************************************/

void HttpcInit()
{
	_pHttpcTcb = NULL;
	_iHttpcState = HCS_INIT;
	_iHttpcTask = HTTPC_TASK_NONE;
	_iHttpcUpdatePage = SYSTEM_INVALID_PAGE;
	_iHttpcCurPage = SYSTEM_INVALID_PAGE;
	_sHttpcPostLen = 0;
	_lHttpcContentLen = 0;
	_iHttpcRecvLen = 0;
	_pHttpcRecvBuf = NULL;
	_sHttpcTimer = 0;
	_iHttpcCurContent = HTTPC_SETTINGS_CONFIG;
}

void HttpcFree()
{
	if (_pHttpcTcb)
	{
		TcpFree(_pHttpcTcb);
		_pHttpcTcb = NULL;
	}
	if (_pHttpcRecvBuf)
	{
		free(_pHttpcRecvBuf);
	}
	HttpcInit();
}

void HttpcDone()
{
	HttpcFree();
	TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_STANDBY);
}

void HttpcStart()
{
	UCHAR pDns[MAX_DNS_LEN];
	UCHAR pDstIP[IP_ALEN];

	OptionsGetString(pDns, OPT_PROVISION_SERVER, MAX_DNS_LEN);
	SkipField(pDns, '/');

	if (strlen(pDns))
	{
		if (Sys_iCurMode == SYS_MODE_STANDBY)
		{
			TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_AUTOPROV);
			_iHttpcTask = HTTPC_TASK_GET_SETTINGS;
			_iHttpcCurContent = HTTPC_SETTINGS_CONFIG;
			if (str2ip(pDns, pDstIP))
			{
				HttpcStartStep2(pDstIP);
			}
			else
			{
				TaskDnsQuery(pDns, DNS_TYPE_A, HTTP_BANK_OFFSET, (USHORT)HttpcStartStep2);
			}
		}
	}
}

void HttpcStartStep2(PCHAR pServerIP)
{
	if (pServerIP)
	{
		_pHttpcRecvBuf = (PCHAR)malloc(HTTP_MAX_RECV_LEN + 1);
		if (_pHttpcRecvBuf)
		{
			_pHttpcTcb = TcpOpen(pServerIP, OptionsGetShort(OPT_PROVISION_PORT), 0, HTTP_BANK_OFFSET, (USHORT)HttpcRun);
			if (_pHttpcTcb)
			{
				TcpSetTimeout(_pHttpcTcb, HTTP_ACTIVE_TIMEOUT);
				TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_AUTOPROV);
				UdpDebugIDS((_iHttpcTask == HTTPC_TASK_GET_SETTINGS) ? IDS_GET_SETTINGS : IDS_GET_FIRMWARE);
				return;
			}
		}
	}

	HttpcDone();
}

void HttpcRun(TCP_HANDLE h, USHORT sLen, PCHAR pBuf)
{
	UCHAR iEvent;
	UCHAR pServerIP[IP_ALEN];

	if (h != _pHttpcTcb)
	{
		UdpDebugString("unkown tcp handle");
		TcpFree(h);
		return;
	}

	iEvent = TcpGetEvent(_pHttpcTcb);
	if (iEvent & (TCP_EVENT_RESET|TCP_EVENT_ABORT|TCP_EVENT_USERTO|TCP_EVENT_RETRYTO))
	{
		UdpDebugString("Http client reset/abort/timeout");
		if (iEvent & TCP_EVENT_ABORT)
		{
			TcpStartSend(_pHttpcTcb);
		}
		// should wait for some time before re-init this socket?
		if (_iHttpcTask == HTTPC_TASK_REBOOT)
		{
			UdpDebugString("rebooting ...");
			FlashReboot();
		}
		else
		{
			if (_iHttpcTask == HTTPC_TASK_SETTINGS_DONE)
			{
				if (HttpcCheckFirmware())
				{
					TcpGetRemoteIP(_pHttpcTcb, pServerIP);
					HttpcFree();
					_iHttpcTask = HTTPC_TASK_GET_FIRMWARE;
					HttpcStartStep2(pServerIP);
				}
				else
				{
					HttpcDone();
				}
			}
			else
			{
				HttpcDone();
			}
		}
		return;
	}
	if (iEvent & TCP_EVENT_CONNECT)
	{
		UdpDebugString("Http client connect");
		if (!HttpcGetFile())
		{
			TcpClose(_pHttpcTcb);
		}
	}
	if (iEvent & TCP_EVENT_DATA)
	{
		HttpcRunData(sLen, pBuf);
	}
	if (iEvent & TCP_EVENT_CLOSE)
	{
		UdpDebugString("Http client close");
		_iHttpcState = HCS_INIT;
		TcpClose(_pHttpcTcb);
	}

	TcpClearEvent(_pHttpcTcb);

	if (_iHttpcState == HCS_REQUEST_END)
	{
		TcpClose(_pHttpcTcb);
		_iHttpcState = HCS_INIT;
	}
	TcpStartSend(_pHttpcTcb);
}

void HttpcMinuteTimer()
{
	USHORT sProvTime;

	_sHttpcTimer ++;
	sProvTime = OptionsGetShort(OPT_PROVISION_TTL);
	if (sProvTime < MIN_PROV_TTL)
	{
		sProvTime = MIN_PROV_TTL;
	}
	if (_sHttpcTimer >= sProvTime)
	{
		_sHttpcTimer = 0;
		HttpcStart();
	}
}

#endif
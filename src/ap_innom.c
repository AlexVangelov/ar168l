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

#ifdef OEM_INNOMEDIA

#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "bank3.h"

#define MIN_PROV_TTL		60	// 60 minutes
#define PROV_MAX_RETRY		2

#define HTTPC_TASK_NONE			0
#define HTTPC_TASK_GET_PROV		1
#define HTTPC_TASK_GET_CFG		2
#define HTTPC_TASK_CFG_AUTH		3
#define HTTPC_TASK_GET_SW		4
#define HTTPC_TASK_PROV_FAIL	5
#define HTTPC_TASK_REBOOT		6
#define HTTPC_TASK_POST_CFGOK		7
#define HTTPC_TASK_POST_CFGFAIL		8
#define HTTPC_TASK_POST_SWOK		9
#define HTTPC_TASK_POST_SWFAIL		10

#define HCS_INIT			0
#define HCS_HEADER_BEGIN	1
#define HCS_HEADER_END		2
#define HCS_MESSAGE_END		3

PCHAR Sys_pProvUrl;

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
USHORT _sHttpcMinTimer;
PCHAR _pHttpcRecvAuth;
UCHAR _iHttpcRetry;
BOOLEAN _bHttpcEncrypt;
USHORT _sHttpcRespCode;

#define PROV_NONCE_SIZE		32
#define PROV_USER_SIZE		16
#define PROV_PASSWORD_SIZE	16
#define PROV_PUBKEY_SIZE	32
PCHAR _pProvNonce;
PCHAR _pProvUser;
PCHAR _pProvPassword;
PCHAR _pProvPubKey;

//UCHAR _pProvNonce[33];
//UCHAR _pProvUser[17];
//UCHAR _pProvPassword[17];
//UCHAR _pProvPubKey[33];
UCHAR _iEncryptCount;
PCHAR _pProvUrl;
PCHAR _pProvCfgUrl;
PCHAR _pProvSwUrl;

const UCHAR _cGet[] = "GET ";
const UCHAR _cPost[] = "POST ";
const UCHAR _cHttpVer[] = " HTTP/1.1\r\n";

UCHAR http_get_header_name(PCHAR pBuf, PCHAR pName, UCHAR iSeparator)
{
	PCHAR pCur;
	PCHAR pDst;
	UCHAR iLen;

	pCur = pBuf;
	pDst = pName;
	iLen = 0;
	while (*pCur && *pCur != (UCHAR)' ' && *pCur != iSeparator)
	{
		*pDst++ = *pCur++;
		iLen ++;
		if (iLen >= 32)		return 0;
	}
	*pDst = 0;

	while (*pCur == (UCHAR)' ' || *pCur == iSeparator)
	{
		pCur ++;
		iLen ++;
	}
	if (!strlen(pCur))	return 0;

	return iLen;
}

void HttpcWriteStr(PCHAR pString)
{
	TcpWriteStr(_pHttpcTcb, pString);
}

void GetProvAddr(PCHAR pAddr)
{
	PCHAR pSrc;
	PCHAR pDst;

	pSrc = _pProvUrl;
	pDst = pAddr;
	while (*pSrc && *pSrc != (UCHAR)'/' && *pSrc != (UCHAR)':')
	{
		*pDst ++ = *pSrc ++;
	}
	*pDst = 0;
}

USHORT GetProvPort()
{
	PCHAR pCur;

	pCur = SkipField(_pProvUrl, ':');
	if (pCur)
	{
		return atoi(pCur, 10);
	}

	return 0;
}

void HttpcWriteCommon()
{
	UCHAR pBuf[MAX_DNS_LEN];

	// Fill header
	HttpcWriteStr("User-Agent: MTA7308-SL/");
	TaskLoadString(IDS_VERSION, pBuf);
	HttpcWriteStr((PCHAR)(pBuf + 5));
	HttpcWriteStr("(innomedia)\r\nHost: ");
	GetProvAddr(pBuf);
	HttpcWriteStr(pBuf);
	HttpcWriteStr("\r\nConnection: Close\r\n\r\n");
}

void ChangeSeparator(PCHAR pString, UCHAR iOld, UCHAR iNew)
{
	while (*pString)
	{
		if (*pString == iOld)
		{
			*pString = iNew;
		}
		pString ++;
	}
}

void PadMac(PCHAR pDst)
{
	PCHAR pCur;

	pCur = pDst;
	strcpy(pCur, "hwid=01+");
	pCur += 8;
	mac2str(Sys_pMacAddress, pCur);
	ChangeSeparator(pCur, '-', '+');
}

void ComposeUri(PCHAR pUri)
{
	PCHAR pCur;
	
	pCur = strchr(_pProvUrl, '/');	
	strcpy(pUri, (pCur) ? pCur : (PCHAR)"/");
	strcat_char(pUri, '?');
	PadMac((PCHAR)(pUri + strlen(pUri)));
}

void GenNonce(PCHAR pNonce)
{
	UCHAR pVal[16];

	rand_array(pVal, 16);
	char2asc_str(pNonce, pVal, 16, FALSE);
}

void HttpcGetProv()
{
	UCHAR pUri[128];

	// Fill first line
	HttpcWriteStr(_cGet);
	ComposeUri(pUri);
	HttpcWriteStr(pUri);
	HttpcWriteStr("&nonce=");
	HttpcWriteStr(_pProvNonce);
	HttpcWriteStr(_cHttpVer);
	// Fill header
	HttpcWriteCommon();
}

void HttpcWriteAuth(PCHAR pUri)
{
	UCHAR pMethod[16];	
	UCHAR pAuth[255];

	strcpy(pMethod, "GET");
	TaskHttpDigest(pAuth, _pHttpcRecvAuth, pUri, _pProvUser, _pProvPassword, pMethod);
	HttpcWriteStr("Authorization: ");
	HttpcWriteStr(pAuth);
	HttpcWriteStr("\r\n");
}

void HttpcGetCfg()
{
	UCHAR pUri[128];

	// Fill first line
	HttpcWriteStr(_cGet);
	ComposeUri(pUri);
	HttpcWriteStr(pUri);
	HttpcWriteStr(_cHttpVer);
	if (_pHttpcRecvAuth)
	{
		HttpcWriteAuth(pUri);
	}
	// Fill header
	HttpcWriteCommon();
}

void HttpcGetSW()
{
	PCHAR pCur;

	HttpcWriteStr(_cGet);
	pCur = strchr(_pProvUrl, '/');	
	if (pCur)
	{
		HttpcWriteStr(pCur);
	}
	else
	{
		HttpcWriteStr("/");
		HttpcWriteStr(_pProvUrl);
	}
	HttpcWriteStr(_cHttpVer);
	HttpcWriteCommon();	
}

void HttpcPostMsg()
{
	UCHAR pBuf[128];
	UCHAR pLen[6];

	PadMac(pBuf);
	strcat(pBuf, "&facility=user&priority=notice&message=");
	strcat(pBuf, (_iHttpcTask == HTTPC_TASK_POST_SWOK || _iHttpcTask == HTTPC_TASK_POST_CFGOK) ? "ProvSucceed" : "ProvFail");
	HttpcWriteStr("POST /Provisioning/syslog.cgi");
	HttpcWriteStr(_cHttpVer);
	HttpcWriteStr("Content-Length:");
	itoa(strlen(pBuf), pLen, 10);
	HttpcWriteStr(pLen);
	HttpcWriteStr("\r\nContent-Type: application/x-www-form-urlencoded\r\n");
	HttpcWriteCommon();	
	HttpcWriteStr(pBuf);
}

BOOLEAN HttpcCheckSW()
{
	UCHAR pOldFile[MAX_DNS_LEN];
	UCHAR pNewFile[MAX_DNS_LEN];
	UCHAR pFlag[32];
	PCHAR pCur;
	PCHAR pImage;
	UCHAR iLen;

	iLen = strlen(_pProvSwUrl);
	if (!iLen)
	{
		UdpDebugString("No SW image");
		return FALSE;
	}
	pImage = (PCHAR)(_pProvSwUrl + iLen);
	while (pImage != _pProvSwUrl)
	{
		if (*pImage == (UCHAR)'/')
		{
			pImage ++;
			break;
		}
		else
		{
			pImage --;
		}
	};
	strcpy(pNewFile, pImage);
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

BOOLEAN HttpcCheckCfg()
{
	if (!strlen(Sys_pSipProxy))
	{
		return FALSE;
	}
	if (!strlen(Sys_pUserNumber))
	{
		return FALSE;
	}
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


void HttpcHandleSettings()
{
	UCHAR iHeaderLen;
	UCHAR pHeader[32];
	PCHAR pCur;

	if (_iHttpcRecvLen)
	{
		_pHttpcRecvBuf[_iHttpcRecvLen] = 0;
//		UdpDebugString(_pHttpcRecvBuf);

		iHeaderLen = http_get_header_name(_pHttpcRecvBuf, pHeader, '=');
		if (iHeaderLen)
		{
			pCur = (PCHAR)(_pHttpcRecvBuf + iHeaderLen);
			if (!strcmp_lowercase(pHeader, "retry_provision"))
			{
				_sHttpcTimer = atoi(pCur, 10);
			}
			else if (!strcmp_lowercase(pHeader, "prov_filename"))
			{
				if (!memcmp_lowercase(pCur, "http://"))
				{
					pCur += 7;
				}
				if (_pProvSwUrl)	free(_pProvSwUrl);
				_pProvSwUrl = heap_save_str(pCur);
			}
			else if (strcmp_lowercase(pHeader, "endofconfig"))
			{
				TaskHttpEvent(HTTP_WRITE_SETTINGS, (USHORT)_pHttpcRecvBuf);
			}
		}
		_iHttpcRecvLen = 0;
	}
}

void HandleProvParam()
{
	UCHAR iHeaderLen;
	UCHAR pHeader[32];
	PCHAR pCur;

	if (_iHttpcRecvLen)
	{
		_pHttpcRecvBuf[_iHttpcRecvLen] = 0;
//		UdpDebugString(_pHttpcRecvBuf);
		iHeaderLen = http_get_header_name(_pHttpcRecvBuf, pHeader, '=');
		if (iHeaderLen)
		{
			pCur = (PCHAR)(_pHttpcRecvBuf + iHeaderLen);
			if (!strcmp_lowercase(pHeader, "prov_url"))
			{
				if (!memcmp_lowercase(pCur, "http://"))
				{
					pCur += 7;
				}
				if (_pProvCfgUrl)	free(_pProvCfgUrl);
				_pProvCfgUrl = heap_save_str(pCur);
			}
			else if (!strcmp_lowercase(pHeader, "prov_user"))
			{
				strcpy(_pProvUser, pCur);
			}
			else if (!strcmp_lowercase(pHeader, "prov_password"))
			{
				strcpy(_pProvPassword, pCur);
			}
			else if (!strcmp_lowercase(pHeader, "prov_pub_key"))
			{
				strcpy(_pProvPubKey, pCur);
			}
			else if (!strcmp_lowercase(pHeader, "prov_key_form"))
			{
				_iEncryptCount = atoi(pCur, 10);
			}
		}
		_iHttpcRecvLen = 0;
	}
}

void HttpcInitKey()
{
	UCHAR psk[32];

//	UdpDebugString(_pProvPubKey);
//	UdpDebugString(_pProvNonce);
//	UdpDebugVal(_iEncryptCount, 10);
	TaskGenKey(_pProvPubKey, _pProvNonce, _iEncryptCount, psk, 32);
	TaskRC4Init(psk, 32);
	UdpDebugVals(psk, 32);
}

void HttpcHandleHeader(PCHAR pBuf)
{
	UCHAR iHeaderLen;
	UCHAR pHeader[32];
	PCHAR pCur;

	iHeaderLen = http_get_header_name(pBuf, pHeader, ':');
	if (!iHeaderLen)	return;

	pCur = (PCHAR)(pBuf + iHeaderLen);
	if (!strcmp_lowercase(pHeader, "content-length"))
	{
		_lHttpcContentLen = atol(pCur, 10);
	}
	else if (!strcmp_lowercase(pHeader, "www-authenticate"))
	{
		if (_pHttpcRecvAuth)
		{
			free(_pHttpcRecvAuth);
		}
		_pHttpcRecvAuth = heap_save_str(pCur);
	}
	else if (!strcmp_lowercase(pHeader, "encrypt-type"))
	{
		_bHttpcEncrypt = strcmp(pCur, "RC4") ? FALSE : TRUE;
	}
	else if (!strcmp_lowercase(pHeader, "nonce"))
	{
		strcpy(_pProvNonce, pCur);
	}
}

void HttpcNextTask()
{
	if (_iHttpcTask == HTTPC_TASK_GET_PROV)
	{
		if (_sHttpcRespCode == 200)
		{
			HandleProvParam();
			_iHttpcTask = HTTPC_TASK_GET_CFG;
		}
		else if (_sHttpcRespCode == 503)
		{
			_sHttpcTimer = 10;
			_iHttpcTask = HTTPC_TASK_PROV_FAIL;
		}
		else
		{
			_sHttpcTimer = 0;
			_iHttpcTask = HTTPC_TASK_PROV_FAIL;
		}
	}
	else if (_iHttpcTask == HTTPC_TASK_GET_CFG || _iHttpcTask == HTTPC_TASK_CFG_AUTH)
	{
		if (_sHttpcRespCode == 401)
		{
			if (_iHttpcTask == HTTPC_TASK_GET_CFG)
			{
				_iHttpcRetry = 0;
				_iHttpcTask = HTTPC_TASK_CFG_AUTH;
			}
		}
		else
		{
			if (_sHttpcRespCode == 200)
			{
				HttpcHandleSettings();
				TaskSystemHandler(SYS_MODIFY_PAGE, SYSTEM_SETTINGS_PAGE);
				FlashWritePage(SYSTEM_SETTINGS_PAGE);
				if (HttpcCheckCfg())
				{
					_iHttpcTask = HttpcCheckSW() ? HTTPC_TASK_GET_SW : HTTPC_TASK_POST_CFGOK;
				}
				else
				{
					_iHttpcTask = HTTPC_TASK_POST_CFGFAIL;
				}
			}
			else
			{
				_sHttpcTimer = 20;
				_iHttpcTask = HTTPC_TASK_POST_CFGFAIL;
			}
			_iHttpcRetry = 0;
			if (_pHttpcRecvAuth)
			{
				free(_pHttpcRecvAuth);
				_pHttpcRecvAuth = NULL;
			}
		}
	}
	else if (_iHttpcTask == HTTPC_TASK_GET_SW)
	{
		_iHttpcTask = HTTPC_TASK_POST_SWFAIL;
		_iHttpcRetry = 0;
	}
	else if (_iHttpcTask == HTTPC_TASK_REBOOT)
	{
		_iHttpcTask = HTTPC_TASK_POST_SWOK;
	}
	else if (_iHttpcTask == HTTPC_TASK_POST_SWOK)
	{
		_iHttpcTask = HTTPC_TASK_REBOOT;
	}
	else if (_iHttpcTask == HTTPC_TASK_POST_SWFAIL || _iHttpcTask == HTTPC_TASK_POST_CFGOK)
	{
		_iHttpcTask = HTTPC_TASK_NONE;
	}
	else if (_iHttpcTask == HTTPC_TASK_POST_CFGFAIL)
	{
		_iHttpcTask = HTTPC_TASK_PROV_FAIL;
	}
	_iHttpcState = HCS_MESSAGE_END;
	_bHttpcEncrypt = FALSE;
}

void HttpcHandleLine(PCHAR pBuf, UCHAR iLen)
{
	PCHAR pCur;
	
	pBuf[iLen] = 0;
	if (_iHttpcState == HCS_INIT)
	{
		pCur = pBuf;
		if (!memcmp_str(pCur, "HTTP/1.1 "))
		{
			pCur += 9;
			UdpDebugString(pCur);
			_sHttpcRespCode = atoi(pCur, 10);
			_iHttpcState = HCS_HEADER_BEGIN;
			_bHttpcEncrypt = FALSE;
		}
	}
	else if (_iHttpcState == HCS_HEADER_BEGIN)
	{
		if (iLen)
		{
			HttpcHandleHeader(pBuf);
		}
		else 
		{
			if (_lHttpcContentLen)
			{
				_iHttpcState = HCS_HEADER_END;
				if (_sHttpcRespCode == 200)
				{
					if (_iHttpcTask == HTTPC_TASK_GET_PROV)
					{
						// use MAC as public key
						mac2str(Sys_pMacAddress, _pProvPubKey);
						ChangeSeparator(_pProvPubKey, '-', ':');
						HttpcInitKey();
						_bHttpcEncrypt = TRUE;
					}
					else if (_iHttpcTask == HTTPC_TASK_GET_CFG || _iHttpcTask == HTTPC_TASK_CFG_AUTH)
					{
						if (_bHttpcEncrypt)
						{
							HttpcInitKey();
						}
						ReadFlashPage(SYSTEM_SETTINGS_PAGE);
					}
					else if (_iHttpcTask == HTTPC_TASK_GET_SW)
					{
						_iHttpcUpdatePage = (Sys_iSystemPage == SYSTEM_1ST_PAGE) ? SYSTEM_2ND_PAGE : SYSTEM_1ST_PAGE;
						_sHttpcPostLen = 0;
						_iHttpcCurPage = _iHttpcUpdatePage;
						BufSeek(0);
					}
				}
			}
			else
			{
				HttpcNextTask();
			}
		}
	}
}

void HttpcHandleContent(USHORT sLen, PCHAR pBuf)
{
	UCHAR iVal;
	USHORT sLeftLen, sLen2;

	if (_sHttpcRespCode != 200)
	{
		goto SkipContent;
	}
	if (_iHttpcTask == HTTPC_TASK_GET_PROV || _iHttpcTask == HTTPC_TASK_GET_CFG || _iHttpcTask == HTTPC_TASK_CFG_AUTH)
	{
		sLen2 = sLen;
		if (_bHttpcEncrypt)
		{
			TaskRC4Calc(pBuf, pBuf, sLen2);
		}
		while (sLen2 --)
		{
			iVal = *pBuf ++;
			if (iVal == 0x0d || iVal == 0x0a)
			{
				if (_iHttpcTask == HTTPC_TASK_GET_PROV)
				{
					HandleProvParam();
				}
				else
				{
					HttpcHandleSettings();
				}
			}
			else if (iVal != (UCHAR)'"')
			{
				HttpcSaveRecv(iVal);
			}
		}
		goto SkipContent;
	}

	if (_iHttpcTask == HTTPC_TASK_GET_SW)
	{
		if (_iHttpcUpdatePage == SYSTEM_INVALID_PAGE)
			goto SkipContent;

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
				goto SkipContent;
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

SkipContent:
	if (_lHttpcContentLen > (ULONG)sLen)
	{
		_lHttpcContentLen -= (ULONG)sLen;
	}
	else
	{
		_lHttpcContentLen = 0;
		HttpcNextTask();
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
void HttpcInitPtr()
{
	_pHttpcTcb = NULL;
	_pHttpcRecvBuf = NULL;
	_pHttpcRecvAuth = NULL;
	_pProvNonce = NULL;
	_pProvUser = NULL;
	_pProvPassword = NULL;
	_pProvPubKey = NULL;
	_pProvUrl = NULL;
	_pProvCfgUrl = NULL;
	_pProvSwUrl = NULL;
}

void HttpcInit()
{
	_iHttpcState = HCS_INIT;
	_sHttpcRespCode = 0;
	_iHttpcUpdatePage = SYSTEM_INVALID_PAGE;
	_iHttpcCurPage = SYSTEM_INVALID_PAGE;
	_sHttpcPostLen = 0;
	_lHttpcContentLen = 0;
	_iHttpcRecvLen = 0;
	_iHttpcRetry = 0;
	_sHttpcTimer = 0;
	_sHttpcMinTimer = 0;
	_bHttpcEncrypt = FALSE;
	_iEncryptCount = 1;
	_iHttpcTask = HTTPC_TASK_GET_PROV;
	HttpcInitPtr();
}

void HttpcFree()
{
	if (_pHttpcTcb)			TcpFree(_pHttpcTcb);
	if (_pHttpcRecvBuf)		free(_pHttpcRecvBuf);
	if (_pHttpcRecvAuth)	free(_pHttpcRecvAuth);
	if (_pProvNonce)		free(_pProvNonce);
	if (_pProvUser)			free(_pProvUser);
	if (_pProvPassword)		free(_pProvPassword);
	if (_pProvPubKey)		free(_pProvPubKey);
	if (_pProvCfgUrl)		free(_pProvCfgUrl);
	if (_pProvSwUrl)		free(_pProvSwUrl);

	HttpcInitPtr();
}

void HttpcDone(BOOLEAN bSucceed)
{
	_iHttpcRetry = 0;
	_iHttpcTask = HTTPC_TASK_NONE;
	HttpcFree();
	if (bSucceed)
	{
		_sHttpcMinTimer = OptionsGetShort(OPT_PROVISION_TTL);
		TaskUIHandler(UI_PROV_DONE, 1);
	}
	else
	{
		_sHttpcTimer = 60;
		TaskUIHandler(UI_PROV_DONE, 0);
	}
}

void HttpcFailed()
{
	if (_iHttpcTask == HTTPC_TASK_GET_PROV ||_iHttpcTask == HTTPC_TASK_GET_CFG || _iHttpcTask == HTTPC_TASK_CFG_AUTH || _iHttpcTask == HTTPC_TASK_POST_CFGFAIL)
	{
		HttpcDone(0);
	}
	else if (_iHttpcTask == HTTPC_TASK_GET_SW || _iHttpcTask == HTTPC_TASK_POST_SWFAIL || _iHttpcTask == HTTPC_TASK_POST_CFGOK)
	{
		HttpcDone(1);
	}
	else if (_iHttpcTask == HTTPC_TASK_POST_SWOK)
	{
		UdpDebugString("rebooting ...");
		FlashReboot();
	}
}

BOOLEAN HttpcStartProv()
{
	UCHAR pDns[MAX_DNS_LEN];
	UCHAR pDstIP[IP_ALEN];

	if (_pProvUrl)
	{
		GetProvAddr(pDns);
		if (strlen(pDns))
		{
			if (str2ip(pDns, pDstIP))
			{
				HttpcStartStep2(pDstIP);
			}
			else
			{
				TaskDnsQuery(pDns, DNS_TYPE_A, HTTP_BANK_OFFSET, (USHORT)HttpcStartStep2);
			}
			return TRUE;
		}
	}

	UdpDebugString("Prov Url error");
	return FALSE;
}

void HttpcStart()
{
	if (!Sys_pProvUrl || !strlen(Sys_pProvUrl))
	{
		return;
	}
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_AUTOPROV);
		HttpcInit();
		_pProvUrl = Sys_pProvUrl;
		_pHttpcRecvBuf = (PCHAR)malloc(HTTP_MAX_RECV_LEN + 1);
		_pProvNonce = (PCHAR)malloc(PROV_NONCE_SIZE + 1);
		_pProvUser = (PCHAR)malloc(PROV_USER_SIZE + 1);
		_pProvPassword = (PCHAR)malloc(PROV_PASSWORD_SIZE + 1);
		_pProvPubKey = (PCHAR)malloc(PROV_PUBKEY_SIZE + 1);
		if (_pHttpcRecvBuf && _pProvNonce && _pProvUser && _pProvPassword && _pProvPubKey)
		{
			GenNonce(_pProvNonce);
			if (!HttpcStartProv())
			{
				HttpcFree();
				TaskUIHandler(UI_PROV_DONE, 0);
				return;
			}
		}
		else
		{
			UdpDebugString("Prov res fail");
			HttpcFree();
			_sHttpcTimer = 60;
			TaskUIHandler(UI_PROV_DONE, 0);
		}
	}
	else
	{
		UdpDebugString("Can't Prov now");
		_sHttpcTimer = 60;
	}
}

BOOLEAN HttpcOpen(PCHAR pServerIP)
{
	_pHttpcTcb = TcpOpen(pServerIP, GetProvPort(), 0, HTTP_BANK_OFFSET, (USHORT)HttpcRun);
	if (_pHttpcTcb)
	{
		TcpSetTimeout(_pHttpcTcb, HTTP_ACTIVE_TIMEOUT);
		return TRUE;
	}
	else
	{
		UdpDebugString("Can't open Prov TCP");
		return FALSE;
	}
}

void HttpcStartStep2(PCHAR pServerIP)
{
	if (pServerIP)
	{
		if (HttpcOpen(pServerIP))
		{
			return;
		}
	}

	UdpDebugString("Invalid Prov IP");
	HttpcFailed();
}

void HttpcRetry()
{
	TcpFree(_pHttpcTcb);
	_pHttpcTcb = NULL;
	if (_iHttpcTask == HTTPC_TASK_GET_PROV)
	{
		_pProvUrl = Sys_pProvUrl;
	}
	else if (_iHttpcTask == HTTPC_TASK_GET_CFG || _iHttpcTask == HTTPC_TASK_CFG_AUTH || _iHttpcTask == HTTPC_TASK_POST_CFGOK || _iHttpcTask == HTTPC_TASK_POST_CFGFAIL)
	{
		_pProvUrl = _pProvCfgUrl;
	}
	else
	{
		_pProvUrl = _pProvSwUrl;
	}
	if (!HttpcStartProv())
	{
		HttpcFailed();
	}
}

void HttpcRun(TCP_HANDLE h, USHORT sLen, PCHAR pBuf)
{
	UCHAR iEvent;

	if (h != _pHttpcTcb || Sys_iCurMode != SYS_MODE_AUTOPROV)
	{
		UdpDebugString("unknown tcp handle");
		TcpFree(h);
		return;
	}

	iEvent = TcpGetEvent(_pHttpcTcb);
	if (iEvent & (TCP_EVENT_RESET|TCP_EVENT_ABORT|TCP_EVENT_USERTO|TCP_EVENT_RETRYTO))
	{
		UdpDebugString("AutoProv client abort");
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
		else if (_iHttpcTask == HTTPC_TASK_PROV_FAIL)
		{
			UdpDebugString("Prov failed ...");
			HttpcDone(0);
		}
		else if (_iHttpcTask == HTTPC_TASK_NONE)
		{
			HttpcDone(1);
		}
		else 
		{
			if (_iHttpcRetry >= PROV_MAX_RETRY)
			{
				if (_iHttpcTask == HTTPC_TASK_CFG_AUTH)
				{
					_iHttpcRetry = 0;
					_iHttpcTask = HTTPC_TASK_POST_CFGFAIL;
					HttpcRetry();
				}
				else
				{
					HttpcFailed();
				}
			}
			else
			{
				_iHttpcRetry ++;
				HttpcRetry();
			}
		}
		return;
	}
	if (iEvent & TCP_EVENT_CONNECT)
	{
		UdpDebugString("AutoProv client connect");
		if (_iHttpcTask == HTTPC_TASK_GET_PROV)
		{
			UdpDebugString("Prov stage 2");
			HttpcGetProv();
		}
		else if (_iHttpcTask == HTTPC_TASK_GET_CFG || _iHttpcTask == HTTPC_TASK_CFG_AUTH)
		{
			UdpDebugIDS(IDS_GET_SETTINGS);
			HttpcGetCfg();
		}
		else if (_iHttpcTask == HTTPC_TASK_GET_SW)
		{
			UdpDebugIDS(IDS_GET_FIRMWARE);
			HttpcGetSW();
		}
		else if (_iHttpcTask > HTTPC_TASK_REBOOT)
		{
			UdpDebugString("Post message");
			HttpcPostMsg();
		}
	}
	if (iEvent & TCP_EVENT_DATA)
	{
		HttpcRunData(sLen, pBuf);
	}
	if (iEvent & TCP_EVENT_CLOSE)
	{
		UdpDebugString("AutoProv client close");
		_iHttpcState = HCS_INIT;
		TcpClose(_pHttpcTcb);
	}

	TcpClearEvent(_pHttpcTcb);

	if (_iHttpcState == HCS_MESSAGE_END)
	{
		TcpClose(_pHttpcTcb);
		_iHttpcState = HCS_INIT;
	}
	TcpStartSend(_pHttpcTcb);
}

void HttpcTimer()
{
	if (_sHttpcTimer)
	{
		_sHttpcTimer --;
		if (!_sHttpcTimer)
		{
			HttpcStart();
		}
	}
}

void HttpcMinuteTimer()
{
	if (_sHttpcMinTimer)
	{
		_sHttpcMinTimer --;
		if (!_sHttpcMinTimer)
		{
			HttpcStart();
		}
	}
}

#endif


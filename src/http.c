/*-------------------------------------------------------------------------
   AR1688 HTTP Server function copy right information

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

#define MAX_SEND_LEN		384		// should be less than TCP_SND_MSS, the space left is because I don't know exactly how much will be written each time

// Http control block
#define HTTP_CB_TSIZE		2

// HTTP control block structure
struct http_cb 
{
	UCHAR iState;		// state of this HTTP control block
	TCP_HANDLE pTcb;	// TCP handle of this HTTP control block
	UCHAR iGetTask;		// GET task to be fullfilled
	UCHAR iPostTask;	// POST task to be fullfilled
	PCHAR pRecvBuf;		// buffer to store incomplete message from last received packet 
	UCHAR iRecvLen;		// length of the incomplete message

	USHORT sMsgAddr;	// Address of the web page
	USHORT sMsgLen;		// length of a web page
	USHORT sSendLen;	// length of data that has been sent

	ULONG lRecvLen;		// total length of data to receive
	USHORT sPostLen;	// Length of data posted
	UCHAR iUpdatePage;	// Page to be upgraded
	UCHAR iUpdateState;
	UCHAR iCurPage;
};
typedef struct http_cb * HTTP_HANDLE;

#define HTTP_CB_LENGTH		(sizeof(struct http_cb))

// HTTP control block state
#define HS_FREE				0
#define HS_INIT				1
#define HS_HEADER_BEGIN		2
#define HS_HEADER_END		3
#define HS_REQUEST_END		4
#define HS_SEND_BEGIN		5
#define HS_SEND_END			6

// HTTP task
#define HTTP_TASK_NONE					0
#define HTTP_TASK_GET_LOGIN				1
#define	HTTP_TASK_GET_SETTINGS			2
#define	HTTP_TASK_GET_PHONEBOOK			3
#define	HTTP_TASK_GET_PHONEBOOK_BODY	4
#define	HTTP_TASK_GET_PHONEBOOK_TAIL	5
#define	HTTP_TASK_GET_UPGRADE			6
#define	HTTP_TASK_GET_SUCCESS			7
#define	HTTP_TASK_GET_FAILURE			8
#define HTTP_TASK_GET_ABORT				9
#define	HTTP_TASK_GET_IMAGE				10
#define HTTP_TASK_GET_DIGITMAP			11
#define HTTP_TASK_GET_DIGITMAP_BODY		12
#define HTTP_TASK_GET_DIGITMAP_TAIL		14
#define HTTP_TASK_POST_LOGIN			15
#define HTTP_TASK_POST_SETTINGS			16
#define HTTP_TASK_POST_PHONEBOOK		17
#define HTTP_TASK_POST_DIGITMAP			18
#define HTTP_TASK_POST_UPGRADE			19
#define HTTP_TASK_UNKNOWN				255

// WEB PAGE index
#define HTTP_LOGIN_PAGE			0
#define HTTP_SETTINGS_PAGE		4
#define HTTP_PBHEAD_PAGE		8
#define HTTP_PBTAIL_PAGE		12
#define HTTP_UPGRADE_PAGE		16
#define HTTP_SUCCESS_PAGE		20
#define HTTP_FAILURE_PAGE		24
#define HTTP_ABORT_PAGE			28
#define HTTP_IMAGE_PAGE			32
#define HTTP_DMHEAD_PAGE		36
#define HTTP_DMTAIL_PAGE		40

// Content type
#define CTYPE_TEXT_HTML		0
#define CTYPE_IMAGE_JPEG	1

#define UPDATE_STATE_PART1_HEAD		0
#define UPDATE_STATE_PART1_BODY		1
#define UPDATE_STATE_PART2_HEAD		2
#define UPDATE_STATE_PART2_BODY		3
#define UPDATE_STATE_END			4

// local variables
struct http_cb _pHcbData[HTTP_CB_TSIZE];
HTTP_HANDLE _pCurHcb;
TCP_HANDLE _pCurTcb;
TCP_HANDLE _pHttpdTcb;
BOOLEAN _bNeedReboot;
#ifdef OEM_IP20
BOOLEAN Write ;
USHORT sIndexPhn;
#endif
UCHAR _iRebootTimer;

/****************************************************************************
**** HTTP common functions                                                 **
****************************************************************************/

// This data block may cross 0x8000 boundary
void http_read_page(USHORT sDstAddr, USHORT sSrcAddr, USHORT sLen)
{
	USHORT sTemp;
	UCHAR iPage;

	iPage = Sys_iSystemPage + SYSTEM_HTTP_PAGE_OFFSET;

	sTemp = sSrcAddr + sLen;
	if (sSrcAddr < BANK_BASE && sTemp > BANK_BASE)
	{
		sTemp = BANK_BASE - sSrcAddr;
		ReadFlashData(sDstAddr, sSrcAddr, sTemp, iPage);
		ReadFlashData((USHORT)(sDstAddr + sTemp), BANK_BASE, (USHORT)(sLen - sTemp), iPage);
	}
	else
	{
		ReadFlashData(sDstAddr, sSrcAddr, sLen, iPage);
	}
}

/****************************************************************************
**** HTTP control block functions                                          **
****************************************************************************/
void http_cb_free(HTTP_HANDLE pHcb)
{
	if (pHcb->pRecvBuf)
	{
		free(pHcb->pRecvBuf);
	}
	memset((PCHAR)pHcb, 0, HTTP_CB_LENGTH);
	pHcb->iUpdatePage = SYSTEM_INVALID_PAGE;
}

HTTP_HANDLE http_cb_match(TCP_HANDLE pTcb)
{
	UCHAR i;
	HTTP_HANDLE pHcb;

	pHcb = _pHcbData;
	for (i = 0; i < HTTP_CB_TSIZE ; i ++)
	{
		if ((pHcb->iState != HS_FREE) && ((USHORT)pHcb->pTcb == (USHORT)pTcb))
		{
			return pHcb;
		}
		pHcb ++;
	}

	return NULL;
}

BOOLEAN http_cb_busy()
{
	UCHAR i;
	HTTP_HANDLE pHcb;

	pHcb = _pHcbData;
	for (i = 0; i < HTTP_CB_TSIZE ; i ++)
	{
		if (pHcb->iPostTask >= HTTP_TASK_POST_SETTINGS && pHcb->iPostTask <= HTTP_TASK_POST_UPGRADE)
		{
			return TRUE;
		}
		pHcb ++;
	}

	return FALSE;
}

/****************************************************************************
**** Send functions                                                        **
****************************************************************************/
void HttpSendError()
{
	TcpWriteStr(_pCurTcb, "HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n");
}

void HttpSendOK(USHORT sLen, UCHAR iType)
{
	UCHAR pStr[16];

	// fill header
	TcpWriteStr(_pCurTcb, "HTTP/1.1 200 OK\r\nContent-Length: ");
	itoa(sLen, pStr, 10);
	TcpWriteStr(_pCurTcb, pStr);
	TcpWriteStr(_pCurTcb, "\r\n");
	if (iType == CTYPE_IMAGE_JPEG)
	{
		TcpWriteStr(_pCurTcb, "Cache-Control: max-age=86400\r\n");
		TcpWriteStr(_pCurTcb, "Content-Type: image/jpeg");
	}
	else
	{
		TcpWriteStr(_pCurTcb, "Content-Type: text/html");
	}
	TcpWriteStr(_pCurTcb, "\r\nConnection: close\r\n\r\n");
}


/* each item is 4 bytes, first 2 bytes is address, next 2 bytes is length */
void http_prepare_send()
{
	USHORT sLen;
	UCHAR iType, iIndex, iTask;
	UCHAR pHead[64];

	http_read_page((USHORT)pHead, 0, 64);
	iType = CTYPE_TEXT_HTML;
	iTask = _pCurHcb->iGetTask;
	switch (iTask)
	{
	case HTTP_TASK_GET_LOGIN:
		iIndex = HTTP_LOGIN_PAGE;
		break;

	case HTTP_TASK_GET_SETTINGS:
		iIndex = HTTP_SETTINGS_PAGE;
		break;

	case HTTP_TASK_GET_PHONEBOOK:
		iIndex = HTTP_PBHEAD_PAGE;
		break;

	case HTTP_TASK_GET_DIGITMAP:
		iIndex = HTTP_DMHEAD_PAGE;
		break;

	case HTTP_TASK_GET_UPGRADE:
		iIndex = HTTP_UPGRADE_PAGE;
		break;
		
	case HTTP_TASK_GET_SUCCESS:
#ifdef OEM_IP20
		iIndex = HTTP_LOGIN_PAGE;
#else
		iIndex = HTTP_SUCCESS_PAGE;
#endif
		break;

	case HTTP_TASK_GET_FAILURE:
		iIndex = HTTP_FAILURE_PAGE;
		break;

	case HTTP_TASK_GET_ABORT:
#ifdef OEM_IP20
		iIndex = HTTP_UPGRADE_PAGE;
#else
		iIndex = HTTP_ABORT_PAGE;
#endif
		break;

	case HTTP_TASK_GET_IMAGE:
		iIndex = HTTP_IMAGE_PAGE;
		iType = CTYPE_IMAGE_JPEG;
		break;

	default:
		return;
	}
		
	_pCurHcb->sMsgAddr = PCHAR2USHORT((PCHAR)(pHead + iIndex));
	_pCurHcb->sMsgLen = PCHAR2USHORT((PCHAR)(pHead + iIndex + 2));
	_pCurHcb->sSendLen = 0;
	sLen = _pCurHcb->sMsgLen;
	if (iTask == HTTP_TASK_GET_PHONEBOOK)
	{
		sLen += PCHAR2USHORT((PCHAR)(pHead + HTTP_PBTAIL_PAGE + 2));
		sLen += TaskHttpEvent(HTTP_GET_PB_LEN, 0);
	}
	else if (iTask == HTTP_TASK_GET_DIGITMAP)
	{
		sLen += PCHAR2USHORT((PCHAR)(pHead + HTTP_DMTAIL_PAGE + 2));
		sLen += TaskHttpEvent(HTTP_GET_DM_LEN, 0);
	}
#ifdef OEM_IP20
	else if (iTask == HTTP_TASK_GET_SETTINGS || iTask == HTTP_TASK_GET_SUCCESS)
	{
		sLen += TaskHttpEvent(HTTP_GET_SETTINGS_LEN, 0);
	}
#else
	else if (iTask == HTTP_TASK_GET_SETTINGS)
	{
		sLen += TaskHttpEvent(HTTP_GET_SETTINGS_LEN, 0);
	}
#endif
	HttpSendOK(sLen, iType);
}

USHORT http_send_settings(PCHAR pMsg, USHORT sMsgLen)
{
	PCHAR pLeft;
	PCHAR pRight;
	PCHAR pLast;
	USHORT sLen, sIndex;
	PCHAR pInput;

	pLast = pMsg;
	sIndex = 0;
	pInput = (PCHAR)(pMsg + MAX_SEND_LEN + MAX_SEND_LEN);
	while(sIndex < sMsgLen)
	{
		pLeft = strchr(pLast, '<');
		if (pLeft)
		{
			sLen = (USHORT)pLeft - (USHORT)pLast;
			if (sLen)
			{
				TcpWriteData(_pCurTcb, pLast, sLen);
				sIndex += sLen;
			}

			pRight = strchr(pLeft, '>');
			if (pRight)
			{
				sLen = (USHORT)pRight - (USHORT)pLeft + 1;
				memcpy(pInput, pLeft, sLen);
				pInput[sLen] = 0;
				TaskHttpEvent(HTTP_UPDATE_SETTINGS, (USHORT)pInput);
				TcpWriteStr(_pCurTcb, pInput);
				sIndex += sLen;
				pLast = pRight;
				pLast ++;
			}
			else
			{
				break;
			}
		}
		else
		{
			sLen = sMsgLen - ((USHORT)pLast - (USHORT)pMsg);
			if (sLen)
			{
				TcpWriteData(_pCurTcb, pLast, sLen);
				sIndex += sLen;
			}
			break;
		}
	}

	return sIndex;
}

#ifdef OEM_IP20
void http_send_webpage()
{
	PCHAR pMsg;
	USHORT sWebPage;
	USHORT sLeftLen, sMsgLen;
	BOOLEAN bEnd;
	UCHAR pHead[64];
	

	pMsg = Adapter_pIpBuf;
	if (_pCurHcb->iGetTask == HTTP_TASK_GET_PHONEBOOK_BODY)
	{
		pMsg[0] = _pCurHcb->sSendLen;
		TaskHttpEvent(HTTP_UPDATE_PB, (USHORT)pMsg);
		TcpWriteStr(_pCurTcb, pMsg);
		_pCurHcb->sSendLen ++;
		if (_pCurHcb->sSendLen >= 128)		//PHN
		{
			http_read_page((USHORT)pHead, 0, 64);
			_pCurHcb->iGetTask = HTTP_TASK_GET_PHONEBOOK_TAIL;
			_pCurHcb->sMsgAddr = PCHAR2USHORT((PCHAR)(pHead + HTTP_PBTAIL_PAGE));
			_pCurHcb->sMsgLen = PCHAR2USHORT((PCHAR)(pHead + HTTP_PBTAIL_PAGE + 2));
			//_pCurHcb->sMsgAddr = PCHAR2USHORT((PCHAR)(pHead + HTTP_PBHEAD_PAGE));
			//_pCurHcb->sMsgLen = PCHAR2USHORT((PCHAR)(pHead + HTTP_PBHEAD_PAGE + 2));
			_pCurHcb->sSendLen = 0;
		}
	}
	
	else if (_pCurHcb->iGetTask == HTTP_TASK_GET_DIGITMAP_BODY)
	{
		
		
				
		if(_pCurHcb->sSendLen == 0){
		pMsg[0] = 7;
		}
		else if(_pCurHcb->sSendLen == 1){
			
			pMsg[0] = 119;
		}
		else if(_pCurHcb->sSendLen == 2){
			
			pMsg[0] = 8;
		}
		else if(_pCurHcb->sSendLen == 3){
			
			pMsg[0] = 0;
		}
		else if(_pCurHcb->sSendLen == 4){
			
			pMsg[0] = 9;
		}
		else if(_pCurHcb->sSendLen == 5){
			
			pMsg[0] = 1;
		}
		else if(_pCurHcb->sSendLen == 6){
			
			pMsg[0] = 10;
		}
		else if(_pCurHcb->sSendLen == 7){
			
			pMsg[0] = 2;
		}
		else if(_pCurHcb->sSendLen == 8){
			
			pMsg[0] = 11;
		}
		else if(_pCurHcb->sSendLen == 9){
			
			pMsg[0] = 3;
		}
		else if(_pCurHcb->sSendLen == 10){
			
			pMsg[0] = 12;
		}
		else if(_pCurHcb->sSendLen == 11){
			
			pMsg[0] = 4;
		}
		else if(_pCurHcb->sSendLen == 12){
			
			pMsg[0] = 13;
		}
		else if(_pCurHcb->sSendLen == 13){
			
			pMsg[0] = 5;
		}
		else if(_pCurHcb->sSendLen == 14){
			
			pMsg[0] = 14;
		}
		else if(_pCurHcb->sSendLen == 15){
			pMsg[0] = 6;			
		}
		
		//pMsg[0] = _pCurHcb->sSendLen;				
		TaskHttpEvent(HTTP_UPDATE_DM, (USHORT)pMsg);
		TcpWriteStr(_pCurTcb, pMsg);
		//UdpDebugVal(_pCurHcb->sSendLen , 10) ;		
		_pCurHcb->sSendLen ++;
				
		if (_pCurHcb->sSendLen >= 16)	//PHN
		{
			http_read_page((USHORT)pHead, 0, 64);
			_pCurHcb->iGetTask = HTTP_TASK_GET_DIGITMAP_TAIL;	//Change
			_pCurHcb->sMsgAddr = PCHAR2USHORT((PCHAR)(pHead + HTTP_DMTAIL_PAGE));
			_pCurHcb->sMsgLen = PCHAR2USHORT((PCHAR)(pHead + HTTP_DMTAIL_PAGE + 2));
			_pCurHcb->sSendLen = 0;
		}
	}
	else
	{
		bEnd = FALSE;
		sLeftLen = _pCurHcb->sMsgLen - _pCurHcb->sSendLen;
		sWebPage = _pCurHcb->sMsgAddr + _pCurHcb->sSendLen;
		// fill body
		if (sLeftLen > MAX_SEND_LEN)
		{
			sMsgLen = MAX_SEND_LEN;
		}
		else
		{
			sMsgLen = sLeftLen;
			bEnd = TRUE;
		}

		http_read_page((USHORT)pMsg, sWebPage, sMsgLen);

		if (_pCurHcb->iGetTask == HTTP_TASK_GET_SETTINGS || _pCurHcb->iGetTask == HTTP_TASK_GET_SUCCESS)
		{
			pMsg[sMsgLen] = 0;
			_pCurHcb->sSendLen += http_send_settings(pMsg, sMsgLen);
		}
		else
		{
			TcpWriteData(_pCurTcb, pMsg, sMsgLen);
			_pCurHcb->sSendLen += sMsgLen;
		}
		if (bEnd)
		{
			if (_pCurHcb->iGetTask == HTTP_TASK_GET_PHONEBOOK)
			{
				_pCurHcb->iGetTask = HTTP_TASK_GET_PHONEBOOK_BODY;
				_pCurHcb->sSendLen = 0;
			}
			
			else if (_pCurHcb->iGetTask == HTTP_TASK_GET_DIGITMAP)
			{
				_pCurHcb->iGetTask = HTTP_TASK_GET_DIGITMAP_BODY;
				_pCurHcb->sSendLen = 0;
			}
			else
			{
				_pCurHcb->iState = HS_SEND_END;
			}
		}
	}
}
#else

void http_send_webpage()
{
	PCHAR pMsg;
	USHORT sWebPage;
	USHORT sLeftLen, sMsgLen;
	BOOLEAN bEnd;
	UCHAR pHead[64];

	pMsg = Adapter_pIpBuf;
	if (_pCurHcb->iGetTask == HTTP_TASK_GET_PHONEBOOK_BODY)
	{
		pMsg[0] = _pCurHcb->sSendLen;
		TaskHttpEvent(HTTP_UPDATE_PB, (USHORT)pMsg);
		TcpWriteStr(_pCurTcb, pMsg);
		_pCurHcb->sSendLen ++;
		if (_pCurHcb->sSendLen >= PHONEBOOK_ENTRY_NUM)
		{
			http_read_page((USHORT)pHead, 0, 64);
			_pCurHcb->iGetTask = HTTP_TASK_GET_PHONEBOOK_TAIL;
			_pCurHcb->sMsgAddr = PCHAR2USHORT((PCHAR)(pHead + HTTP_PBTAIL_PAGE));
			_pCurHcb->sMsgLen = PCHAR2USHORT((PCHAR)(pHead + HTTP_PBTAIL_PAGE + 2));
			_pCurHcb->sSendLen = 0;
		}
	}
	else if (_pCurHcb->iGetTask == HTTP_TASK_GET_DIGITMAP_BODY)
	{
		pMsg[0] = _pCurHcb->sSendLen;
		TaskHttpEvent(HTTP_UPDATE_DM, (USHORT)pMsg);
		TcpWriteStr(_pCurTcb, pMsg);
		_pCurHcb->sSendLen +=2;
		if (_pCurHcb->sSendLen >= DIGITMAP_ENTRY_NUM)
		{
			http_read_page((USHORT)pHead, 0, 64);
			_pCurHcb->iGetTask = HTTP_TASK_GET_DIGITMAP_TAIL;
			_pCurHcb->sMsgAddr = PCHAR2USHORT((PCHAR)(pHead + HTTP_DMTAIL_PAGE));
			_pCurHcb->sMsgLen = PCHAR2USHORT((PCHAR)(pHead + HTTP_DMTAIL_PAGE + 2));
			_pCurHcb->sSendLen = 0;
		}
	}
	else
	{
		bEnd = FALSE;
		sLeftLen = _pCurHcb->sMsgLen - _pCurHcb->sSendLen;
		sWebPage = _pCurHcb->sMsgAddr + _pCurHcb->sSendLen;
		// fill body
		if (sLeftLen > MAX_SEND_LEN)
		{
			sMsgLen = MAX_SEND_LEN;
		}
		else
		{
			sMsgLen = sLeftLen;
			bEnd = TRUE;
		}

		http_read_page((USHORT)pMsg, sWebPage, sMsgLen);

		if (_pCurHcb->iGetTask == HTTP_TASK_GET_SETTINGS)
		{
			pMsg[sMsgLen] = 0;
			_pCurHcb->sSendLen += http_send_settings(pMsg, sMsgLen);
		}
		else
		{
			TcpWriteData(_pCurTcb, pMsg, sMsgLen);
			_pCurHcb->sSendLen += sMsgLen;
		}
		if (bEnd)
		{
			if (_pCurHcb->iGetTask == HTTP_TASK_GET_PHONEBOOK)
			{
				_pCurHcb->iGetTask = HTTP_TASK_GET_PHONEBOOK_BODY;
				_pCurHcb->sSendLen = 0;
			}
			else if (_pCurHcb->iGetTask == HTTP_TASK_GET_DIGITMAP)
			{
				_pCurHcb->iGetTask = HTTP_TASK_GET_DIGITMAP_BODY;
				_pCurHcb->sSendLen = 0;
			}
			else
			{
				_pCurHcb->iState = HS_SEND_END;
			}
		}
	}
}

#endif
/****************************************************************************
**** Receive functions                                                        **
****************************************************************************/
void http_write_flash()
{
	TaskSystemHandler(SYS_MODIFY_PAGE, SYSTEM_SETTINGS_PAGE);
	FlashWritePage(SYSTEM_SETTINGS_PAGE);
}

void http_handle_login_data(PCHAR pBuf)
{
	PCHAR pCur;
	UCHAR pAdmin[MAX_DNS_LEN];
	UCHAR iGetTask;

	pCur = pBuf;
	if (!memcmp_str(pCur, "auth="))
	{
		pCur += 5;
		OptionsGetString(pAdmin, OPT_ADMIN_PIN, MAX_ADMIN_PIN_LEN);
		if (strcmp(pCur, pAdmin))
		{
			_pCurHcb->iGetTask = HTTP_TASK_GET_FAILURE;
		}
	}
#ifdef OEM_INNOMEDIA
	else if (!memcmp_str(pCur, "login_user="))
	{
		pCur += 11;
		OptionsGetString(pAdmin, OPT_ADMIN_USER, MAX_ADMIN_USER_LEN);
		if (strcmp(pCur, pAdmin))
		{
			_pCurHcb->iGetTask = HTTP_TASK_GET_FAILURE;
		}
	}
#endif
	else if (!memcmp_str(pCur, "nextpage="))
	{
		pCur += 9;
		iGetTask = HTTP_TASK_GET_LOGIN;
		if (!strcmp(pCur, "settings"))
		{
			iGetTask = HTTP_TASK_GET_SETTINGS;
		}
		else if (!strcmp(pCur, "phonebook"))
		{
			iGetTask = HTTP_TASK_GET_PHONEBOOK;
#ifdef OEM_IP20
			Write = FALSE;
#endif
		}
		else if (!strcmp(pCur, "digitmap"))
		{
			iGetTask = HTTP_TASK_GET_DIGITMAP;
		}
		else if (!strcmp(pCur, "upgrade"))
		{
			iGetTask = HTTP_TASK_GET_UPGRADE;
		}
		_pCurHcb->iGetTask = iGetTask;
	}
}

void http_handle_settings_data(PCHAR pBuf)
{
	if (!memcmp_str(pBuf, "update="))
	{
		http_write_flash();
		// End of settings list
		_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		_pCurHcb->iUpdatePage = SYSTEM_SETTINGS_PAGE;
	}
#ifdef OEM_IP20
	else if (!memcmp_str(pBuf, "reset="))
	{	
		_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		_pCurHcb->iUpdatePage = SYSTEM_SETTINGS_PAGE;
	}
#endif
	else
	{
		TaskHttpEvent(HTTP_WRITE_SETTINGS, (USHORT)pBuf);
	}

}
// n001 name
// p001 number
void http_handle_pb_data(PCHAR pBuf)
{
#ifdef OEM_IP20
	sIndexPhn = atoi((PCHAR)(pBuf + 1), 10) - 1;	
#endif

	
	if (!memcmp_str(pBuf, "write="))
	{
#ifdef OEM_IP20
		Write = TRUE;
#else
		http_write_flash();
		// End of phonebook list
		_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		_pCurHcb->iUpdatePage = SYSTEM_SETTINGS_PAGE;
#endif		
	}
#ifdef OEM_IP20
	else if(!memcmp_str(pBuf, "reset="))
	{
		_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		_pCurHcb->iUpdatePage = SYSTEM_SETTINGS_PAGE;
	}
#endif
	else
	{
		if (pBuf[0] == (UCHAR)'n' || pBuf[0] == (UCHAR)'p')
		{
			TaskHttpEvent(HTTP_WRITE_PB, (USHORT)pBuf);
		}
	}
#ifdef OEM_IP20
		if (Write == 1 && sIndexPhn > 126)
		{
		http_write_flash();
		_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		_pCurHcb->iUpdatePage = SYSTEM_SETTINGS_PAGE;
		}
#endif

}

// m001 digtmap
void http_handle_dm_data(PCHAR pBuf)
{
	if (!memcmp_str(pBuf, "write="))
	{
		http_write_flash();
		// End of phonebook list
		_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		_pCurHcb->iUpdatePage = SYSTEM_SETTINGS_PAGE;
	}
#ifdef OEM_IP20
	else if (!memcmp_str(pBuf, "reset="))
	{
		_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		_pCurHcb->iUpdatePage = SYSTEM_SETTINGS_PAGE;
	}
#endif
		
	else
	{
#ifdef OEM_IP20
		if (pBuf[0] == (UCHAR)'n' || pBuf[0] == (UCHAR)'p')
		{
			TaskHttpEvent(HTTP_WRITE_PB, (USHORT)pBuf);
		}
#else
		if (pBuf[0] == (UCHAR)'m')
		{
			TaskHttpEvent(HTTP_WRITE_DM, (USHORT)pBuf);
		}
#endif		
	}
}

void http_handle_postdata(UCHAR iRecvLen, PCHAR pBuf)
{
	UCHAR i, iChar, iLen;
	UCHAR pTemp[3];

	if (!iRecvLen)	return;

	iLen = 0;
	pTemp[2] = 0;
	for (i = 0; i < iRecvLen; i ++)
	{
		iChar = pBuf[i];
		if (iChar == (UCHAR)'+')
		{
			iChar = ' ';
		}
		else if (iChar == (UCHAR)'%')
		{
			memcpy(pTemp, (PCHAR)(pBuf+i+1), 2);
			iChar = (UCHAR)atoi(pTemp, 16);
			i += 2;
		}
		pBuf[iLen] = iChar;
		iLen ++;
	}
	pBuf[iLen] = 0;

	switch (_pCurHcb->iPostTask)
	{
	case HTTP_TASK_POST_LOGIN:
		http_handle_login_data(pBuf);
		break;

	case HTTP_TASK_POST_SETTINGS:
		http_handle_settings_data(pBuf);
		break;

	case HTTP_TASK_POST_PHONEBOOK:
		http_handle_pb_data(pBuf);
		break;

	case HTTP_TASK_POST_DIGITMAP:
		http_handle_dm_data(pBuf);
		break;
	}
}

void http_handle_firstline(PCHAR pBuf)
{
	PCHAR pCur;
//	PCHAR pTemp;
	UCHAR iGetTask, iPostTask;

	pCur = pBuf;
	_pCurHcb->iState = HS_HEADER_BEGIN;
	iGetTask = HTTP_TASK_UNKNOWN;
	iPostTask = HTTP_TASK_UNKNOWN;

	// Format of this message is "Method SP URL SP HTTP/version CRLF"
	if (!memcmp_str(pCur, "GET "))
	{
		pCur += 4;
		SkipField(pCur, ' ');
		if (!strcmp(pCur, "/"))
		{
			iGetTask = HTTP_TASK_GET_LOGIN;
		}
		else if (!strcmp(pCur, "/image.jpg"))
		{
			iGetTask = HTTP_TASK_GET_IMAGE;
		}
	}
	else if (!memcmp_str(pCur, "POST "))
	{
		pCur += 5;
		SkipField(pCur, ' ');
		if (!strcmp(pCur, "/login"))
		{
			// retrieve login password
			iPostTask = HTTP_TASK_POST_LOGIN;
		}
		else if (!strcmp(pCur, "/settings"))
		{
			// retrieve settings
			iPostTask = HTTP_TASK_POST_SETTINGS;
		}
		else if (!strcmp(pCur, "/phonebook"))
		{
			// retrieve phone book
			iPostTask = HTTP_TASK_POST_PHONEBOOK;
		}
		else if (!strcmp(pCur, "/digitmap"))
		{
			// retrieve digit map
			iPostTask = HTTP_TASK_POST_DIGITMAP;
		}
		else if (!strcmp(pCur, "/upgrade"))
		{
			iPostTask = HTTP_TASK_POST_UPGRADE;
		}
	}

	_pCurHcb->iGetTask = iGetTask;
	_pCurHcb->iPostTask = iPostTask;
}

void http_get_info(PCHAR pBuf)
{
	UCHAR iLen;
	PCHAR pCur;
	UCHAR pHeader[24];

	iLen = 0;
	pCur = pBuf;
	while (*pCur && *pCur != (UCHAR)' ' && *pCur != (UCHAR)':')
	{
		pHeader[iLen] = *pCur;
		iLen ++;
		pCur ++;
		if (iLen >= 24)
		{
			return;
		}
	}
	pHeader[iLen] = 0;

	while (*pCur == (UCHAR)' ' || *pCur == (UCHAR)':')
	{
		pCur ++;
	}

	if (!strcmp_lowercase(pHeader, "content-length"))
	{
		_pCurHcb->lRecvLen = atol(pCur, 10);
	}
}

void http_handle_header(PCHAR pBuf, UCHAR iLen)
{
//	PCHAR pTemp;

	pBuf[iLen] = 0;

	if (_pCurHcb->iState == HS_INIT)
	{
		http_handle_firstline(pBuf);
	}
	else if (_pCurHcb->iState == HS_HEADER_BEGIN)
	{
		if (iLen)
		{
			http_get_info(pBuf);
		}
		else 
		{
			_pCurHcb->iState = (_pCurHcb->lRecvLen) ? HS_HEADER_END : HS_REQUEST_END;
			if (_pCurHcb->iPostTask == HTTP_TASK_POST_SETTINGS || _pCurHcb->iPostTask == HTTP_TASK_POST_PHONEBOOK || _pCurHcb->iPostTask == HTTP_TASK_POST_DIGITMAP)
			{
				ReadFlashPage(SYSTEM_SETTINGS_PAGE);
			}
		}
	}
}

void http_handle_upgrade_header(PCHAR pBuf, UCHAR iLen)
{
	UCHAR iVal, iPage;

	switch (_pCurHcb->iUpdateState)
	{
	case UPDATE_STATE_PART1_HEAD:
		if (!iLen)
		{
			UdpDebugString("Part1 Body");
			_pCurHcb->iUpdateState = UPDATE_STATE_PART1_BODY;
		}
		break;

	case UPDATE_STATE_PART1_BODY:
		iVal = (UCHAR)atoi(pBuf, 10);
		BufSeek(0);
		if (!iVal)	
		{
			iPage = (Sys_iSystemPage == SYSTEM_1ST_PAGE) ? 	SYSTEM_2ND_PAGE : SYSTEM_1ST_PAGE;
		}
		else if (iVal == 1)
		{
			ReadFlashPage(SYSTEM_SETTINGS_PAGE);
			iPage = SYSTEM_SETTINGS_PAGE;
			_pCurHcb->sSendLen = SETTINGS_FILE_SIZE;
		}
		else if (iVal == 2)
		{
			// phonebook
			ReadFlashPage(SYSTEM_SETTINGS_PAGE);
			iPage = SYSTEM_SETTINGS_PAGE;
			_pCurHcb->sSendLen = PHONEBOOK_FILE_SIZE;
			BufSeek(OPT_PHONE_BOOK);
		}
		else if (iVal == 3)
		{
			iPage = SYSTEM_RINGTONE_PAGE;
		}
		else if (iVal == 4)
		{
			iPage = SYSTEM_HOLDMUSIC_PAGE;
		}
		else
		{
			iPage = SYSTEM_INVALID_PAGE;
			UdpDebugString("Invalid page number");
		}
		_pCurHcb->iUpdatePage = iPage;
		_pCurHcb->iCurPage = iPage;
		_pCurHcb->iUpdateState = UPDATE_STATE_PART2_HEAD;
		break;

	case UPDATE_STATE_PART2_HEAD:
		if (!iLen)
		{
			UdpDebugString("Part2 Body");
			_pCurHcb->iUpdateState = UPDATE_STATE_PART2_BODY;
			_pCurHcb->sPostLen = 0;
		}
		break;
	}
}

void http_handle_upgrade_body(USHORT sLen, PCHAR pBuf)
{
	USHORT sLeftLen, sLen2;
	UCHAR iPageNum;

	if (_pCurHcb->iUpdatePage == SYSTEM_INVALID_PAGE)		return;

	sLeftLen = (_pCurHcb->iUpdatePage == SYSTEM_SETTINGS_PAGE) ? _pCurHcb->sSendLen - _pCurHcb->sPostLen : FILE_FULL_PAGE_SIZE - _pCurHcb->sPostLen;

	if (sLen < sLeftLen || !sLeftLen)
	{
		BufWrite(pBuf, sLen);
		BufForward(sLen);
		_pCurHcb->sPostLen += sLen; 
	}
	else
	{
		BufWrite(pBuf, sLeftLen);
		BufForward(sLeftLen);
		_pCurHcb->sPostLen = 0;
		if (_pCurHcb->iUpdatePage == SYSTEM_SETTINGS_PAGE)
		{
			http_write_flash();
			_pCurHcb->iUpdateState = UPDATE_STATE_END;
			return;
		}
		else
		{
			if (_pCurHcb->iUpdatePage == SYSTEM_RINGTONE_PAGE)
			{
				iPageNum = SYSTEM_RINGTONE_PAGE_NUM;
			}
			else if (_pCurHcb->iUpdatePage == SYSTEM_HOLDMUSIC_PAGE)
			{
				iPageNum = SYSTEM_HOLDMUSIC_PAGE_NUM;
			}
			else
			{
				iPageNum = SYSTEM_PROGRAM_PAGE_NUM;
			}
			TaskUIHandler(UI_UPDATE_PROGRESS, (USHORT)_pCurHcb->iCurPage);
			if (FlashWritePage(_pCurHcb->iCurPage))
			{
				_pCurHcb->iUpdatePage = SYSTEM_INVALID_PAGE;
				_pCurHcb->iUpdateState = UPDATE_STATE_END;
				return;
			}
			_pCurHcb->iCurPage ++;
			if ((_pCurHcb->iCurPage - _pCurHcb->iUpdatePage) >= iPageNum)
			{
				if (_pCurHcb->iUpdatePage == SYSTEM_1ST_PAGE || _pCurHcb->iUpdatePage == SYSTEM_2ND_PAGE)
				{
					TaskSystemHandler(SYS_UPDATE_FLAG, _pCurHcb->iCurPage);
				}
				_pCurHcb->iUpdateState = UPDATE_STATE_END;
				return;
			}
			else
			{
				sLen2 = (USHORT)(sLen - sLeftLen);
				BufSeek(0);
				BufWrite((PCHAR)(pBuf + sLeftLen), sLen2);
				BufForward(sLen2);
				_pCurHcb->sPostLen = sLen2;
			}
		}
	}
}

void http_save_recv(UCHAR iVal)
{
	if (_pCurHcb->iRecvLen < HTTP_MAX_RECV_LEN-1)
	{
		_pCurHcb->pRecvBuf[_pCurHcb->iRecvLen] = iVal;
		_pCurHcb->iRecvLen ++;
	}
}

void http_handle_content(USHORT sLen, PCHAR pBuf)
{
	ULONG lContentLen;
	UCHAR iVal, iNextVal, iTask;
	BOOLEAN bBrokenLine;

	lContentLen = _pCurHcb->lRecvLen;
	iTask = _pCurHcb->iPostTask;

	if (iTask == HTTP_TASK_POST_LOGIN || iTask == HTTP_TASK_POST_SETTINGS || iTask == HTTP_TASK_POST_PHONEBOOK || iTask == HTTP_TASK_POST_DIGITMAP)
	{
		while (lContentLen && sLen)
		{
			iVal = *pBuf;
			if (iVal == (UCHAR)'&')
			{
				http_handle_postdata(_pCurHcb->iRecvLen, _pCurHcb->pRecvBuf);
				_pCurHcb->iRecvLen = 0;
			}
			else
			{
				http_save_recv(iVal);
			}
			lContentLen --;
			sLen --;
			pBuf ++;		
		}
		if (!lContentLen)
		{
			if (_pCurHcb->iRecvLen)
			{
				http_handle_postdata(_pCurHcb->iRecvLen, _pCurHcb->pRecvBuf);
				_pCurHcb->iRecvLen = 0;
			}
			_pCurHcb->iState = HS_REQUEST_END;
		}

		_pCurHcb->lRecvLen = lContentLen;
		return;
	}

	if (iTask == HTTP_TASK_POST_UPGRADE)
	{
		while (lContentLen && sLen)
		{
			if (_pCurHcb->iUpdateState < UPDATE_STATE_PART2_BODY)
			{
				bBrokenLine = FALSE;
				if (_pCurHcb->iRecvLen && _pCurHcb->pRecvBuf[_pCurHcb->iRecvLen-1] == 0x0d)
				{
					bBrokenLine = TRUE;
					_pCurHcb->iRecvLen --;
				}
				iVal = *pBuf;
				pBuf ++;
				sLen --;
				lContentLen --;
				if (iVal == 0x0d)
				{
					if (sLen)
					{
						iNextVal = *pBuf;
						pBuf ++;
						sLen --;
						lContentLen --;
						if (iNextVal == 0x0a)
						{
							http_handle_upgrade_header(_pCurHcb->pRecvBuf, _pCurHcb->iRecvLen);
							_pCurHcb->iRecvLen = 0;
						}
						else
						{
							http_save_recv(iVal);
							http_save_recv(iNextVal);
						}
						continue;
					}
				}
				else if (iVal == 0x0a)
				{
					if (bBrokenLine)
					{
						http_handle_upgrade_header(_pCurHcb->pRecvBuf, _pCurHcb->iRecvLen);
						_pCurHcb->iRecvLen = 0;
						bBrokenLine = FALSE;
						continue;
					}
				}
				http_save_recv(iVal);
			}
			else
			{
				if (_pCurHcb->iUpdateState == UPDATE_STATE_PART2_BODY)
				{
					http_handle_upgrade_body(sLen, pBuf);
				}
				break;
			}
		}
	}

	if (lContentLen > (ULONG)sLen)
	{
		lContentLen -= (ULONG)sLen;
	}
	else
	{
		UdpDebugString("upgrade data end");
		lContentLen = 0;
		_pCurHcb->iState = HS_REQUEST_END;
		if (_pCurHcb->iUpdatePage == SYSTEM_INVALID_PAGE || _pCurHcb->iUpdateState != UPDATE_STATE_END)
		{
			_pCurHcb->iGetTask = HTTP_TASK_GET_ABORT;
		}
		else
		{
			_pCurHcb->iGetTask = HTTP_TASK_GET_SUCCESS;
		}
	}

End:
	_pCurHcb->lRecvLen = lContentLen;
}

void http_run_data(USHORT sLen, PCHAR pBuf)
{
	UCHAR iVal, iNextVal;
	BOOLEAN bBrokenLine;
	
	if (!_pCurHcb->pRecvBuf)	return;

	while (sLen)
	{
		if (_pCurHcb->iState < HS_HEADER_END)
		{
			bBrokenLine = FALSE;
			if (_pCurHcb->iRecvLen && _pCurHcb->pRecvBuf[_pCurHcb->iRecvLen-1] == 0x0d)
			{
				bBrokenLine = TRUE;
				_pCurHcb->iRecvLen --;
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
						http_handle_header(_pCurHcb->pRecvBuf, _pCurHcb->iRecvLen);
						_pCurHcb->iRecvLen = 0;
					}
					else
					{
						http_save_recv(iVal);
						http_save_recv(iNextVal);
					}
					continue;
				}
			}
			else if (iVal == 0x0a)
			{
				if (bBrokenLine)
				{
					http_handle_header(_pCurHcb->pRecvBuf, _pCurHcb->iRecvLen);
					_pCurHcb->iRecvLen = 0;
					bBrokenLine = FALSE;
					continue;
				}
			}
			http_save_recv(iVal);
		}
		else 
		{
			if (_pCurHcb->iState == HS_HEADER_END)
			{
				http_handle_content(sLen, pBuf);
			}
			break;
		}
	}
}

HTTP_HANDLE http_alloc()
{
	UCHAR i;
	HTTP_HANDLE pHcb;

	pHcb = _pHcbData;
	for (i = 0; i < HTTP_CB_TSIZE; i ++, pHcb ++)
	{
		if (pHcb->iState == HS_FREE)
		{
			pHcb->pRecvBuf = (PCHAR)malloc(HTTP_MAX_RECV_LEN + 1);
			if (pHcb->pRecvBuf)
			{
				return pHcb;
			}
			else
			{
				UdpDebugString("No more space for HTTP");
				heap_debug();
				return NULL;
			}
		}
	}
	UdpDebugString("No HTTP control block");
	return NULL;
}

void HttpdInit()
{
	UCHAR i;
	HTTP_HANDLE pHcb;

	pHcb = _pHcbData;
	for (i = 0; i < HTTP_CB_TSIZE; i ++)
	{
		memset((PCHAR)pHcb, 0, HTTP_CB_LENGTH);
		pHcb->iUpdatePage = SYSTEM_INVALID_PAGE;
		pHcb ++;
	}
	
	_pHttpdTcb = TcpListen(HTTP_SERVER_PORT, HTTP_BANK_OFFSET, (USHORT)HttpdRun);
	_bNeedReboot = FALSE;
	_iRebootTimer = 0;
}

void HttpdRun(TCP_HANDLE h, USHORT sLen, PCHAR pBuf)
{
	UCHAR iEvent;

	iEvent = TcpGetEvent(h);
	if (iEvent & TCP_EVENT_ACCEPT)
	{
		UdpDebugString("Http accept");
#ifdef VER_AR168R
		if (Sys_iCurMode == SYS_MODE_CALL)
		{
			TaskHandleEvent(UI_EVENT_HANDSET_DOWN, 0);
			Sys_iCurMode = SYS_MODE_STANDBY;
		}
#endif
		if ((Sys_iCurMode != SYS_MODE_STANDBY && Sys_iCurMode != SYS_MODE_HTTPD) || http_cb_busy())
		{
			UdpDebugString("HTTP blocked");
			TcpFree(h);
		}
		else
		{
			_pCurHcb = http_alloc();
			if (!_pCurHcb)
			{
				TcpFree(h);
			}
			else
			{
				TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_HTTPD);
				_pCurHcb->iState = HS_INIT;
				_pCurHcb->pTcb = h;
				TcpSetTimeout(h, HTTP_ACTIVE_TIMEOUT);
				TcpClearEvent(h);
				TcpStartSend(h);
			}
		}
		return;
	}

	_pCurHcb = http_cb_match(h);
	if (!_pCurHcb)
	{
		UdpDebugString("unkown Httpd handle");
		TcpFree(h);
		return;
	}
	if (iEvent & (TCP_EVENT_RESET|TCP_EVENT_ABORT|TCP_EVENT_USERTO|TCP_EVENT_RETRYTO))
	{
		UdpDebugString("Http reset/abort/timeout");
		if (iEvent & TCP_EVENT_ABORT)
		{
			TcpStartSend(h);
		}
		TcpFree(h);
		// should wait for some time before re-init this socket?
		if (_pCurHcb->iUpdatePage != SYSTEM_INVALID_PAGE)
		{
			_bNeedReboot = TRUE;
			_iRebootTimer = 0;
		}
		else
		{
			TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_STANDBY);
		}
		http_cb_free(_pCurHcb);
		return;
	}

	_pCurTcb = h;
	if (iEvent & TCP_EVENT_CONNECT)
	{
		UdpDebugString("Http connect");
	}
	if (iEvent & TCP_EVENT_DATA)
	{
//		UdpDebugString("Http data");
		http_run_data(sLen, pBuf);
	}
	if (iEvent & TCP_EVENT_CLOSE)
	{
		UdpDebugString("Http close");
		if (_pCurHcb->iState < HS_REQUEST_END)
		{
			_pCurHcb->iState = HS_REQUEST_END;
		}
		TcpClose(_pCurTcb);
	}

	TcpClearEvent(_pCurTcb);

	while (TcpCanWrite(_pCurTcb, MAX_SEND_LEN))
	{
		if (_pCurHcb->iState == HS_REQUEST_END)
		{
			if (_pCurHcb->iGetTask == HTTP_TASK_UNKNOWN)
			{
				HttpSendError();
				TcpClose(_pCurTcb);
				_pCurHcb->iState = HS_INIT;
				break;
			}
			else
			{
				http_prepare_send();
				_pCurHcb->iState = HS_SEND_BEGIN;
			}
		}
		else if (_pCurHcb->iState == HS_SEND_BEGIN)
		{
			http_send_webpage();
			if (_pCurHcb->iState == HS_SEND_END)
			{
				TcpClose(_pCurTcb);
				_pCurHcb->iState = HS_INIT;
				break;
			}
		}
		else
		{
			break;
		}
	}

	TcpStartSend(_pCurTcb);
}

void HttpdTimer()
{
	if (_bNeedReboot)
	{
		_iRebootTimer ++;
		if (_iRebootTimer > HTTP_REBOOT_TIMEOUT)
		{
			UdpDebugString("rebooting ...");
			FlashReboot();
		}
	}
}

void HttpInit()
{
	HttpdInit();
	HttpcInit();
}

/*
void HttpTimer()
{
	HttpdTimer();
	HttpcTimer();
}
*/

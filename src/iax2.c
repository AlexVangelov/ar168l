/*-------------------------------------------------------------------------
   AR1688 IAX2 function copy right information

   Copyright (c) 2006-2012. Tang, Li <tangli@palmmicro.com>

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

#ifdef CALL_IAX2

#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "iax2.h"
#include "bank5.h"

#define IAX2_SERVER_PORT	4569

const ULONG _cAstFormat[VOICE_CODER_NUM] = {
	AST_FORMAT_ULAW, AST_FORMAT_ALAW, AST_FORMAT_G726, AST_FORMAT_GSM, AST_FORMAT_ILBC, AST_FORMAT_SPEEX, AST_FORMAT_G729A
};

const UCHAR _cIax2Control[IAX_CONTROL_NUMBER][15] = {
"HANGUP",
"RESERVED0",
"RINGING",
"ANSWER",
"BUSY",
"RESERVED1",
"RESERVED2",
"CONGESTION",
"FLASH",
"RESERVED",
"OPTION",
"KEYRADIO",
"UNKEYRADIO",
"CALLPROGRESS",
"CALLPROCEEDING",
"HOLD",
"UNHOLD"
};

const UCHAR _cIax2Command[IAX_COMMAND_NUMBER][10] = {
"NEW", 
"PING", 
"PONG", 
"ACK", 
"HANGUP", 
"REJECT", 
"ACCEPT", 
"AUTHREQ", 
"AUTHREP",
"INVAL", 
"LAGRQ", 
"LAGRP", 
"REGREQ", 
"REGAUTH", 
"REGACK", 
"REGREJ", 
"REGREL", 
"VNAK", 
"DPREQ", 
"DPREP", 
"DIAL", 
"TXREQ", 
"TXCNT", 
"TXACC", 
"TXREADY", 
"TXREL", 
"TXREJ",
"QUELCH", 
"UNQUELCH", 
"POKE", 
"RESERVED0", 
"MWI", 
"UNSUPPORT", 
"TRANSFER", 
"PROVISION", 
"FWDOWNL", 
"FWDATA"
};

const UCHAR _cTBD[] = "TBD";

USHORT Sys_sIax2Port;

// Ie variables
USHORT _sIeLen;
PCHAR _pIeData;

struct iax2_frame_cb _pIax2FCB[FCB_TSIZE];
struct iax2_session_cb _pIax2SCB[SCB_TSIZE];
struct iax2_line_cb _pIax2LCB[LCB_TSIZE];

IAX2_SCB_HANDLE _pCurScb;		// current active session control block
IAX2_LCB_HANDLE _pActiveLcb;	// Current active line control block
// Registration variables
USHORT _sRegTimer, _sRegTimeOut;
UCHAR _pRegServerIP[IP_ALEN];
UCHAR _iRegState;
BOOLEAN _bDigitDialing;

// other 
USHORT _sCalledPort;
USHORT _sSrcCallNo;
UDP_SOCKET _pIax2Socket;

UCHAR Iax2_pTimeStamp[4];
UCHAR Iax2_iISeqNo;

//Information Elements
//BOOLEAN Iax2_pIeExist[IAX_IE_NUM];
UCHAR Iax2_pIeExist[IAX_IE_NUM];
PCHAR Iax2_pIeCallingName;
PCHAR Iax2_pIeCallingNumber;
ULONG Iax2_lIeFormat;
ULONG Iax2_lIeCapability;
USHORT Iax2_sIeAuthMethods;
PCHAR Iax2_pChallenge;
UCHAR Iax2_iCauseCode;
USHORT Iax2_sIeRefresh;
UCHAR Iax2_pIeMsgCount[2];
UCHAR Iax2_pIeDateTime[4];
UCHAR Iax2_pIeApparentAddr[SOCKADDR_DATA];
UCHAR Iax2_pIeTransferId[IAX_XFID_LEN];
USHORT Iax2_sIeCallNo;
PCHAR Iax2_pIeCalledNumber;
USHORT Iax2_sIeDPStatus;

void Iax2SendKey();
void Iax2CallStep1(PCHAR pCalledUri);
/**********************************************************************
 ** Misc common functions                                            **
 **********************************************************************
 */
PCHAR iax2_get_cause(UCHAR iCauseCode)
{
	switch (iCauseCode)
	{
	case IAX_CAUSE_NORMAL_CLEARING:
		return (PCHAR)("Normal Call Clearing");

	case IAX_CAUSE_USER_BUSY:
		return (PCHAR)("User Busy");

	case IAX_CAUSE_NO_USER_RESPONSE:
		return (PCHAR)("No User Response");

	case IAX_CAUSE_NO_ANSWER:
		return (PCHAR)("No Answer");

	case IAX_CAUSE_CALL_REJECTED:
		return (PCHAR)("Call Rejected");

	case IAX_CAUSE_INCOMPATIBEL:
		return (PCHAR)("Incompatible Destination");

	default:
		return (PCHAR)("Normal, unspecified");
	}
}

void iax2_debug(PCHAR pStrInfo, UCHAR iFrameType, UCHAR iCommand)
{
	UCHAR pDebug[32];

	strcpy(pDebug, pStrInfo);
	strcat(pDebug, ": ");

	switch (iFrameType)
	{
	case AST_FRAME_DTMF:
		strcat(pDebug, "DTMF");
		break;

	case AST_FRAME_VOICE:
		strcat(pDebug, "VOICE");
		break;

	case AST_FRAME_VIDEO:
		strcat(pDebug, "VIDEO");
		break;

	case AST_FRAME_NULL:
		strcat(pDebug, "NULL");
		break;

	case AST_FRAME_TEXT:
		strcat(pDebug, "TEXT");
		break;

	case AST_FRAME_IMAGE:
		strcat(pDebug, "IMAGE");
		break;

	case AST_FRAME_CFTNOISE:
		strcat(pDebug, "CFTNOISE");
		break;

	case AST_FRAME_IAX:
		if (iCommand && iCommand <= IAX_COMMAND_NUMBER)
		{
			strcat(pDebug, _cIax2Command[iCommand-1]);
		}
		break;

	case AST_FRAME_CONTROL:
		if (iCommand && iCommand <= IAX_CONTROL_NUMBER)
		{
			strcat(pDebug, _cIax2Control[iCommand-1]);
		}
		break;
	}
	UdpDebugString(pDebug);
}

ULONG iax2_get_format(ULONG lCapability)
{
	UCHAR i, iVoiceCoder;
	ULONG lFormat;

	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		iVoiceCoder = Sys_pVoiceCoders[i];
		if (iVoiceCoder > VOICE_CODER_NONE && iVoiceCoder <= VOICE_CODER_NUM)
		{
			lFormat = _cAstFormat[iVoiceCoder-1];
			if (lFormat & lCapability)
			{
				return lFormat;
			}
		}
	}
	return 0;
}

UCHAR iax2_get_coder(ULONG lFormat)
{
	UCHAR i, iVoiceCoder;

	iVoiceCoder = VOICE_CODER_NONE;
	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		if (lFormat == _cAstFormat[i])
		{
			iVoiceCoder = i + 1;
			break;
		}
	}

	if (iVoiceCoder != VOICE_CODER_NONE)
	{
		for (i = 0; i < VOICE_CODER_NUM; i ++)
		{
			if (iVoiceCoder == Sys_pVoiceCoders[i])
			{
				return iVoiceCoder;
			}
		}
	}
	return VOICE_CODER_NONE;
}

ULONG iax2_get_local_format()
{
	UCHAR i, iVoiceCoder;

	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		iVoiceCoder = Sys_pVoiceCoders[i];
		if (iVoiceCoder != VOICE_CODER_NONE && iVoiceCoder <= VOICE_CODER_NUM)
		{
			return _cAstFormat[iVoiceCoder-1];
		}
	}

	return VOICE_CODER_NONE;
}

ULONG iax2_get_local_cap()
{
	UCHAR i, iVoiceCoder;
	ULONG lCapability;

	lCapability = 0;
	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		iVoiceCoder = Sys_pVoiceCoders[i];
		if (iVoiceCoder != VOICE_CODER_NONE && iVoiceCoder <= VOICE_CODER_NUM)
		{
			lCapability |= _cAstFormat[iVoiceCoder-1];
		}
	}	
	return lCapability;
}

/**********************************************************************
 ** Register functions                                               **
 **********************************************************************
 */
void iax2_reg_init()
{
	_sRegTimeOut = Sys_sRegisterTtl;
	_sRegTimer = 0;
}

void iax2_reg_ok()
{
	switch (_iRegState)
	{
	case RS_REGISTERING:
		TaskUIHandler(UI_LOGON_OK, 0);
		_iRegState = RS_REGISTERED;
		break;

	case RS_REFRESHING:
		UdpDebugString("Logon Refreshed");
		_iRegState = RS_REGISTERED;
		break;
	}
}

void iax2_reg_failed()
{
	TaskUIHandler(UI_LOGON_FAILED, 0);
	iax2_reg_init();
	_iRegState = RS_FREE;
}

/**********************************************************************
 ** IE functions                                                     **
 **********************************************************************
 */
void iax2_ie_start()
{
	_sIeLen = 0;
	_pIeData = (PCHAR)(Adapter_pUdpBuf + IAX_FULLFR_DATA);
}

void iax2_ie_add_raw(PCHAR pData, UCHAR iLen)
{
	if (iLen)
	{
		memcpy((PCHAR)(_pIeData + _sIeLen), pData, iLen);
		_sIeLen += iLen;
	}
}

void iax2_ie_add_data(UCHAR iType, PCHAR pData, UCHAR iLen)
{
	_pIeData[_sIeLen] = iType;
	_sIeLen ++;
	_pIeData[_sIeLen] = iLen;
	_sIeLen ++;
	iax2_ie_add_raw(pData, iLen);
}

void iax2_ie_add_byte(UCHAR iType, UCHAR iData)
{
	_pIeData[_sIeLen] = iType;
	_sIeLen ++;
	_pIeData[_sIeLen] = 1;
	_sIeLen ++;
	_pIeData[_sIeLen] = iData;
	_sIeLen ++;
}

void iax2_ie_add_short(UCHAR iType, USHORT sData)
{
	_pIeData[_sIeLen] = iType;
	_sIeLen ++;
	_pIeData[_sIeLen] = 2;
	_sIeLen ++;
	USHORT2PCHAR(sData, (PCHAR)(_pIeData + _sIeLen));
	_sIeLen += 2;
}

void iax2_ie_add_long(UCHAR iType, ULONG lData)
{
	_pIeData[_sIeLen] = iType;
	_sIeLen ++;
	_pIeData[_sIeLen] = 4;
	_sIeLen ++;
	ULONG2PCHAR(lData, (PCHAR)(_pIeData + _sIeLen));
	_sIeLen += 4;
}

void iax2_ie_add_str(UCHAR iType, PCHAR pStr)
{
	iax2_ie_add_data(iType, pStr, strlen(pStr));
}

USHORT iax2_ie_len()
{
	return _sIeLen;
}

void iax2_add_auth()
{
	PCHAR pAuthBuf;
	UCHAR pValue[MD5_SIGNATURE_SIZE];
	UCHAR pAuthStr[MD5_SIGNATURE_SIZE*2+1];

	if ((Iax2_sIeAuthMethods & IAX_AUTH_MD5) && Iax2_pChallenge)
	{
		// generate MD5
		pAuthBuf = (char *)malloc(strlen(Iax2_pChallenge) + MAX_AUTH_PIN_LEN);
		if (pAuthBuf)
		{
			strcpy(pAuthBuf, Iax2_pChallenge);
			strcat(pAuthBuf, Sys_pAuthPin);
			TaskMD5GenValue(pValue, pAuthBuf, strlen(pAuthBuf));
			char2asc_str(pAuthStr, pValue, MD5_SIGNATURE_SIZE, FALSE);
			iax2_ie_add_str(IAX_IE_MD5_RESULT, pAuthStr);
			free(pAuthBuf);
			return;
		}
	}

	iax2_ie_add_str(IAX_IE_PASSWORD, Sys_pAuthPin);
}

void iax2_ie_init()
{
	UCHAR i;

	for (i = 0; i < IAX_IE_NUM; i ++)
	{
		Iax2_pIeExist[i] = FALSE;
	}
	Iax2_pIeCallingName = NULL;;
	Iax2_pIeCallingNumber = NULL;
	Iax2_lIeFormat = 0;
	Iax2_lIeCapability = 0;
	Iax2_sIeAuthMethods = 0;
	Iax2_pChallenge = NULL;
	Iax2_iCauseCode = 0;
	Iax2_sIeRefresh = 0;
	memset(Iax2_pIeMsgCount, 0, 2);
	memset(Iax2_pIeDateTime, 0, 4);
	memset(Iax2_pIeApparentAddr, 0, SOCKADDR_DATA);
	memset(Iax2_pIeTransferId, 0, IAX_XFID_LEN);
	Iax2_sIeCallNo = 0;
	Iax2_pIeCalledNumber = NULL;
	Iax2_sIeDPStatus = 0;;
}

void iax2_ie_free()
{
	if (Iax2_pIeCallingName)	free(Iax2_pIeCallingName);
	if (Iax2_pIeCallingNumber)	free(Iax2_pIeCallingNumber);
	if (Iax2_pChallenge)		free(Iax2_pChallenge);
	if (Iax2_pIeCalledNumber)	free(Iax2_pIeCalledNumber);
	iax2_ie_init();
}

PCHAR iax2_ie_save_str(PCHAR pData, UCHAR iLen)
{
	PCHAR pBuf;

	pBuf = (char *)malloc(iLen + 1);
	if (pBuf)
	{
		memcpy(pBuf, pData, iLen);
		pBuf[iLen] = 0;
	}
	return pBuf;
}


void iax2_ie_parse(PCHAR pData, USHORT sLen)
{
	PCHAR pCur;
	UCHAR iType, iLen;

	pCur = pData;
	while ((USHORT)(pCur - pData) < sLen)
	{
		iType = *pCur ++;
		iLen = *pCur ++;
		if (iType < IAX_IE_NUM && iLen)	
		{
			Iax2_pIeExist[iType] = TRUE;
			switch (iType)
			{
			case IAX_IE_CALLING_NAME:
				Iax2_pIeCallingName = iax2_ie_save_str(pCur, iLen);
				break;
			
			case IAX_IE_CALLING_NUMBER:
				Iax2_pIeCallingNumber = iax2_ie_save_str(pCur, iLen);
				break;

			case IAX_IE_FORMAT:
				Iax2_lIeFormat = PCHAR2ULONG(pCur);
				break;

			case IAX_IE_CAPABILITY:
				Iax2_lIeCapability = PCHAR2ULONG(pCur);
				break;

			case IAX_IE_AUTHMETHODS:
				Iax2_sIeAuthMethods = PCHAR2USHORT(pCur);
				break;

			case IAX_IE_CHALLENGE:
/*				if (Iax2_pChallenge)
				{
					UdpDebugString("IAX2 warning: duplicated IAX_IE_CHALLENGE");
					free(Iax2_pChallenge);
				}
*/
				Iax2_pChallenge = iax2_ie_save_str(pCur, iLen);
				break;

			case IAX_IE_CAUSECODE:
				Iax2_iCauseCode = *pCur;
				break;

			case IAX_IE_REFRESH:
				Iax2_sIeRefresh = PCHAR2USHORT(pCur);
				break;

			case IAX_IE_MSGCOUNT:
				Iax2_pIeMsgCount[0] = *pCur;
				Iax2_pIeMsgCount[1] = pCur[1];
				break;

			case IAX_IE_DATETIME:
				memcpy4(Iax2_pIeDateTime, pCur);
				break;

			case IAX_IE_APPARENT_ADDR:
				memcpy(Iax2_pIeApparentAddr, pCur, SOCKADDR_DATA);
				break;

			case IAX_IE_TRANSFERID:
				memcpy4(Iax2_pIeTransferId, pCur);
				break;

			case IAX_IE_CALLNO:
				Iax2_sIeCallNo = PCHAR2USHORT(pCur);
				break;

			case IAX_IE_CALLED_NUMBER:
				Iax2_pIeCalledNumber = iax2_ie_save_str(pCur, iLen);
				break;

			case IAX_IE_DPSTATUS:
				Iax2_sIeDPStatus = PCHAR2USHORT(pCur);
				break;
			}
		}
		pCur += iLen;
	}
}


/**********************************************************************
 ** Line functions                                                **
 **********************************************************************
 */
void iax2_ln_init()
{
	memset((PCHAR)_pIax2LCB, 0, LCB_LENGTH*LCB_TSIZE);
}

void iax2_ln_free(IAX2_LCB_HANDLE pLcb)
{
	IAX2_SCB_HANDLE pScb;

	if (pLcb)
	{
		pScb = pLcb->pScb;
		if (pLcb)
		{
			memset((PCHAR)pLcb, 0, LCB_LENGTH);
		}
		if (pScb)	pScb->pLcb = NULL;
	}
}

IAX2_LCB_HANDLE iax2_ln_alt()
{
	UCHAR i;
	IAX2_LCB_HANDLE pLcb;

	pLcb = _pIax2LCB;
	for (i = 0; i < LCB_TSIZE; i ++, pLcb ++)
	{
		if (pLcb != _pActiveLcb)
		{
			return pLcb;
		}
	}
	return NULL;
}

void iax2_hangup(UCHAR iCauseCode)
{
	iax2_send_hangup(IAX_COMMAND_HANGUP, iCauseCode);
	_pCurScb->iState = SSS_FINALACK;
}

void iax2_show_cid(PCHAR pName, PCHAR pNumber, BOOLEAN bShowLine0)
{
#ifdef UI_4_LINES
	bShowLine0;
	TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pName);
	if (strcmp(pName, pNumber))
	{
		TaskUIHandler(UI_CALL_DISPLAY_LINE2, (USHORT)pNumber);
	}
#else
	if (bShowLine0 && strcmp(pName, pNumber))
	{
		TaskUIHandler(UI_CALL_DISPLAY_LINE0, (USHORT)pName);
	}
	TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pNumber);
#endif
}

void iax2_check_altline()
{
	_pActiveLcb = iax2_ln_alt();
	if (_pActiveLcb->iState == CALL_STATE_RINGING)
	{
		TaskUIHandler(UI_CALL_RINGING, _pActiveLcb->iVoiceCoder);
		iax2_show_cid(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, TRUE);
	}
	else if (_pActiveLcb->iState == CALL_STATE_TALKING)
	{
		TaskUIHandler(UI_CALL_HOLD, (USHORT)(&_pActiveLcb->iVoiceCoder));
		iax2_show_cid(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, FALSE);
	}
	else
	{
		TaskUIHandler(UI_CALL_FINISHED, 0);
	}
}

void iax2_call_failed(UCHAR iCause)
{
	UCHAR iState;

	iState = _pActiveLcb->iState;
	if (iState == CALL_STATE_DIALING || iState == CALL_STATE_CALLING || iState == CALL_STATE_TALKING)
	{
		_pActiveLcb->iState = CALL_STATE_HANGUP;
		TaskUIHandler(UI_CALL_ENDED, iCause);
	}
	else if (iState == CALL_STATE_RINGING)
	{
		TaskCallLogAdd(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, CALL_LOG_TYPE_MISSED);
		iax2_ln_free(_pActiveLcb);
		iax2_check_altline();
	}
}

/**********************************************************************
 ** frame functions, handle retransmission                           **
 **********************************************************************
 */

void iax2_fr_init()
{
	memset((PCHAR)_pIax2FCB, 0, FCB_LENGTH*FCB_TSIZE);
}

IAX2_FCB_HANDLE iax2_fr_new()
{
	UCHAR i;
	IAX2_FCB_HANDLE pFcb;

	pFcb = _pIax2FCB;
	for (i = 0; i < FCB_TSIZE; i ++, pFcb ++)
	{
		if (pFcb->iState == FRS_FREE)
		{
			return pFcb;
		}
	}

	UdpDebugString("All frame busy now!");
	return NULL;
}

void iax2_fr_free(IAX2_FCB_HANDLE pFcb)
{
	if (pFcb)
	{
		if (pFcb->pData)
		{
			free((void *)pFcb->pData);
		}
		memset((PCHAR)pFcb, 0, FCB_LENGTH);
	}
}

void iax2_fr_close(IAX2_SCB_HANDLE pScb)
{
	UCHAR i;
	IAX2_FCB_HANDLE pFcb;

	pFcb = _pIax2FCB;
	for (i = 0; i < FCB_TSIZE; i ++, pFcb ++)
	{
		if (pFcb->iState == FRS_OPEN && pFcb->pScb == pScb)
		{
			iax2_fr_free(pFcb);
		}
	}
}

void iax2_fr_acked(IAX2_SCB_HANDLE pScb, UCHAR iOSeqNo)
{
	UCHAR i;
	IAX2_FCB_HANDLE pFcb;

	pFcb = _pIax2FCB;
	for (i = 0; i < FCB_TSIZE; i ++, pFcb ++)
	{
		if (pFcb->iState == FRS_FREE)	continue;
		if (pFcb->pScb == pScb && pFcb->pData[IAX_FULLFR_OSEQNO] == iOSeqNo)
		{
			iax2_fr_free(pFcb);
		}
	}
}

void iax2_fr_txacked(IAX2_SCB_HANDLE pScb)
{
	UCHAR i;
	IAX2_FCB_HANDLE pFcb;

	pFcb = _pIax2FCB;
	for (i = 0; i < FCB_TSIZE; i ++, pFcb ++)
	{
		if (pFcb->iState == FRS_FREE)	continue;
		if (pFcb->pScb == pScb && (pFcb->pData[IAX_FULLFR_TYPE] == AST_FRAME_IAX && pFcb->pData[IAX_FULLFR_CSUB] == IAX_COMMAND_TXCNT))
		{
			iax2_fr_free(pFcb);
		}
	}
}

void iax2_fr_save(IAX2_SCB_HANDLE pScb, PCHAR pData, USHORT sLen)
{
	IAX2_FCB_HANDLE pFcb;
	PCHAR pBuf;

	pFcb = iax2_fr_new();
	if (!pFcb)	return;

	pBuf = heap_save_data(pData, sLen);
	if (!pBuf)	return;

	pBuf[IAX_FULLFR_DCALLNO] |= (UCHAR)(IAX_FLAG_RETRANS >> 8);
	pFcb->iState = FRS_OPEN;
	pFcb->pScb = pScb;
	pFcb->sDataLen = sLen;
	pFcb->pData = pBuf;
}

void iax2_fr_timer()
{
	UCHAR i, iType, iCommand;
	IAX2_FCB_HANDLE pFcb;
	IAX2_SCB_HANDLE pScb;
	PCHAR pData;
	PCHAR pDst;
	USHORT sLen;

	pFcb = _pIax2FCB;
	pDst = Adapter_pUdpBuf;
	for (i = 0; i < FCB_TSIZE; i ++, pFcb ++)
	{
		if (pFcb->iState == FRS_FREE)	continue;
		
		pFcb->iTimer ++;
		if (pFcb->iTimer >= FR_RETRY_TIMEOUT)
		{
			pFcb->iTimer = 0;
			pFcb->iRetry ++;
			pData = pFcb->pData;
			pScb = pFcb->pScb;
			iType = pData[IAX_FULLFR_TYPE];
			iCommand = pData[IAX_FULLFR_CSUB];
			if (pFcb->iRetry >= FR_MAX_RETRY)
			{
				iax2_debug("retry timeout", iType, iCommand);
				// inform session here
				iax2_ss_to(pScb, iType, iCommand);
				// retransmission time out
				iax2_fr_free(pFcb);
			}
			else
			{
				sLen = pFcb->sDataLen;
				iax2_debug("retry", iType, iCommand);
				memcpy(pDst, pData, sLen);
				if ( iType == AST_FRAME_IAX && iCommand == IAX_COMMAND_TXCNT)
				{
					UdpSendTo(_pIax2Socket, sLen, pScb->pXferDstIP, pScb->sXferDstPort);
				}
				else
				{
					pDst[IAX_FULLFR_ISEQNO] = pScb->iISeqNo;
					UdpSendTo(_pIax2Socket, sLen, pScb->pDstIP, pScb->sDstPort);
				}
			}
		}
	}
}
/**********************************************************************
 ** Session functions                                                **
 **********************************************************************
 */
void iax2_ss_init()
{
	UCHAR i;
	IAX2_SCB_HANDLE pScb;

	pScb = _pIax2SCB;
	for (i = 0; i < SCB_TSIZE; i ++)
	{
		memset((PCHAR)pScb, 0, SCB_LENGTH);
		pScb ++;
	}
}

IAX2_SCB_HANDLE iax2_ss_new()
{
	UCHAR i;
	IAX2_SCB_HANDLE pScb;

	pScb = &_pIax2SCB[1];
	for (i = 1; i < SCB_TSIZE; i ++)
	{
		if (pScb->iState == SSS_FREE)
		{
			UdpDebugString("New session");
			return pScb;
		}
		pScb ++;
	}

	UdpDebugString("All Session busy now!");
	return NULL;
}

void iax2_ss_sync(IAX2_SCB_HANDLE pScb)
{
	pScb->iState = SSS_OPEN;
	pScb->lOffset = Sys_lTicker;
	pScb->sSrcCallNo = _sSrcCallNo;
	_sSrcCallNo ++;
	if (_sSrcCallNo > 32767)
	{
		_sSrcCallNo = 1;
	}
}

void iax2_ss_fill_cm(IAX2_SCB_HANDLE pScb, UCHAR iFrameType, UCHAR iSubClass)
{
	PCHAR pHeader;

	iax2_debug("Send", iFrameType, iSubClass);
	pHeader = Adapter_pUdpBuf;
	USHORT2PCHAR((pScb->sSrcCallNo | IAX_FLAG_FULL), (PCHAR)(pHeader + IAX_FULLFR_SCALLNO));
	USHORT2PCHAR(pScb->sDstCallNo, (PCHAR)(pHeader + IAX_FULLFR_DCALLNO));
	pHeader[IAX_FULLFR_ISEQNO] = pScb->iISeqNo;
	pHeader[IAX_FULLFR_OSEQNO] = pScb->iOSeqNo;
	pHeader[IAX_FULLFR_TYPE] = iFrameType;
	pHeader[IAX_FULLFR_CSUB] = iSubClass;
}

void iax2_ss_fill_ts(IAX2_SCB_HANDLE pScb)
{
	ULONG2PCHAR((ULONG)(Sys_lTicker - pScb->lOffset), (PCHAR)(Adapter_pUdpBuf + IAX_FULLFR_TSTAMP));
}

void iax2_ss_send(IAX2_SCB_HANDLE pScb, USHORT sDataLen)
{
	USHORT sLen;

	sLen = IAX_FULLFR_DATA;
	sLen += sDataLen;
	// save frame for retransmission
	iax2_fr_save(pScb, Adapter_pUdpBuf, sLen);
	// will do retransmission later
	UdpSendTo(_pIax2Socket, sLen, pScb->pDstIP, pScb->sDstPort);
	pScb->iOSeqNo ++;
}

IAX2_SCB_HANDLE iax2_ss_find(USHORT sSrcCallNo, USHORT sDstCallNo, BOOLEAN bMakeNew)
{
	UCHAR i;
	IAX2_SCB_HANDLE pScb;
	PCHAR pDstIP;
	USHORT sDstPort;

	pDstIP = UdpGetDstIP(_pIax2Socket);
	sDstPort = UdpGetDstPort(_pIax2Socket);
	pScb = _pIax2SCB;
	for (i = 0; i < SCB_TSIZE; i ++, pScb ++)
	{
		if (pScb->iState == SSS_FREE)	continue;
		if ((!memcmp(pDstIP, pScb->pDstIP, IP_ALEN)) && (sDstPort == pScb->sDstPort))
		{
			if (pScb->sDstCallNo == sSrcCallNo || ((pScb->sSrcCallNo == sDstCallNo) && (!pScb->sDstCallNo)))
			{
				/* That's us.  Be sure we keep track of the peer call number */
				pScb->sDstCallNo = sSrcCallNo;
				return pScb;
			}
		}
		// here we doesn't compare port because of NAT
		if (pScb->iXferState && !memcmp(pDstIP, pScb->pXferDstIP, IP_ALEN))
		{
			if (pScb->sSrcCallNo == sDstCallNo)
			{
				return pScb;
			}
		}
	}

	if (!sDstCallNo && bMakeNew)
	{
		pScb = iax2_ss_new();
		if (pScb)
		{
			memcpy4(pScb->pDstIP, pDstIP);
			pScb->sDstPort = sDstPort;
			pScb->sDstCallNo = sSrcCallNo;
			iax2_ss_sync(pScb);
			return pScb;
		}
	}

//	UdpDebugString("No Session?");
	return NULL;
}

void iax2_ss_free(IAX2_SCB_HANDLE pScb)
{
	UdpDebugString("Free session");
	// free any pending retransmission
	iax2_fr_close(pScb);
	// free this session
	memset((PCHAR)pScb, 0, SCB_LENGTH);
}

void iax2_ss_acked(IAX2_SCB_HANDLE pScb)
{
	if (pScb->iState == SSS_FINALACK)
	{
		pScb->iState = SSS_TIMEWAIT;
	}
}

void iax2_ss_clear_xfer(IAX2_SCB_HANDLE pScb)
{
	memset(pScb->pXferDstIP, 0, IP_ALEN);
	pScb->sXferDstPort = 0;
	pScb->iXferState = TRANSFER_NONE;
	memset(pScb->pXferID, 0, IAX_XFID_LEN);
	pScb->sXferCallNo = 0;
}

void iax2_ss_to(IAX2_SCB_HANDLE pScb, UCHAR iType, UCHAR iCommand)
{
	IAX2_LCB_HANDLE pLcb;

	_pCurScb = pScb;
	pLcb = _pCurScb->pLcb;
	if (iType == AST_FRAME_IAX)
	{
		switch (iCommand)
		{
		case IAX_COMMAND_REGREQ:
		case IAX_COMMAND_REGREL:
			iax2_reg_failed();
			iax2_ss_free(pScb);
			break;

		case IAX_COMMAND_NEW:
		case IAX_COMMAND_DPREQ:
		case IAX_COMMAND_DIAL:
			if (pLcb == _pActiveLcb)
			{
				iax2_call_failed(CALL_END_MSG_TIMEOUT);
			}
			else
			{
				iax2_ln_free(pLcb);
			}
			iax2_ss_free(pScb);
			break;

		case IAX_COMMAND_REJECT:
		case IAX_COMMAND_HANGUP:
			iax2_ss_free(pScb);
			break;

		case IAX_COMMAND_TRANSFER:
			if (pLcb == _pActiveLcb)
			{
				iax2_call_failed(CALL_END_XFER_FAILED);
			}
			else
			{
				iax2_ln_free(pLcb);
			}
			iax2_ss_free(pScb);
			break;

		case IAX_COMMAND_TXCNT:
			iax2_send_noie(IAX_COMMAND_TXREJ);
			iax2_ss_clear_xfer(pScb);
			break;
		}
	}
}

void iax2_ss_timer()
{
	UCHAR i;
	IAX2_SCB_HANDLE pScb;

	pScb = _pIax2SCB;
	for (i = 0; i < SCB_TSIZE; i ++)
	{
		if (pScb->iState == SSS_TIMEWAIT || pScb->iState == SSS_FINALACK)
		{
			pScb->iTimer ++;
			if (pScb->iTimer > SS_WAIT_TIME)
			{
				iax2_ss_free(pScb);
			}
		}
		pScb ++;
	}
}
/**********************************************************************
 ** Send packet functions                                            **
 **********************************************************************
 */
void iax2_send_dtmf(UCHAR iDtmf)
{
	iax2_ss_fill_cm(_pCurScb, AST_FRAME_DTMF, iDtmf);
	iax2_ss_fill_ts(_pCurScb);
	//send packet
	iax2_ss_send(_pCurScb, 0);
}

// Control frames, including RINGING, ANSWER
void iax2_send_ctrl(UCHAR iControl)
{
	iax2_ss_fill_cm(_pCurScb, AST_FRAME_CONTROL, iControl);
	iax2_ss_fill_ts(_pCurScb);
	//send packet
	iax2_ss_send(_pCurScb, 0);
}

void iax2_send_iax(UCHAR iCommand, USHORT sIeLen)
{
	// fill header
	iax2_ss_fill_cm(_pCurScb, AST_FRAME_IAX, iCommand);
	iax2_ss_fill_ts(_pCurScb);
	//send packet
	iax2_ss_send(_pCurScb, sIeLen);
}

// Commands that don't have ie field, including HANGUP, REJECT, TXREJ
void iax2_send_noie(UCHAR iCommand)
{
	iax2_send_iax(iCommand, 0);
}

// REGREQ(with/without authentication)
void iax2_send_regcmd(BOOLEAN bAuth)
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_str(IAX_IE_USERNAME, Sys_pAuthID);
	iax2_ie_add_short(IAX_IE_REFRESH, _sRegTimeOut);
	if (bAuth)
	{
		iax2_add_auth();
	}
	//send packet
	iax2_send_iax(IAX_COMMAND_REGREQ, iax2_ie_len());
}

void iax2_send_lagrp()
{
	// fill header
	iax2_ss_fill_cm(_pCurScb, AST_FRAME_IAX, IAX_COMMAND_LAGRP);
	memcpy4((PCHAR)(Adapter_pUdpBuf + IAX_FULLFR_TSTAMP), Iax2_pTimeStamp);
	//send packet
	iax2_ss_send(_pCurScb, 0);
}

void iax2_send_pong()
{
	// fill header
	iax2_ss_fill_cm(_pCurScb, AST_FRAME_IAX, IAX_COMMAND_PONG);
	memcpy4((PCHAR)(Adapter_pUdpBuf + IAX_FULLFR_TSTAMP), Iax2_pTimeStamp);
	//send packet
	iax2_ss_send(_pCurScb, 0);
}

void iax2_send_ack()
{
	PCHAR pHeader;

	// fill header
	iax2_ss_fill_cm(_pCurScb, AST_FRAME_IAX, IAX_COMMAND_ACK);
	pHeader = Adapter_pUdpBuf;
	memcpy4((PCHAR)(pHeader + IAX_FULLFR_TSTAMP), Iax2_pTimeStamp);
	pHeader[IAX_FULLFR_OSEQNO] = Iax2_iISeqNo;
	// send packet
	UdpSendTo(_pIax2Socket, IAX_FULLFR_DATA, _pCurScb->pDstIP, _pCurScb->sDstPort);
}

void iax2_send_vnak()
{
	PCHAR pHeader;

	// fill header
	iax2_ss_fill_cm(_pCurScb, AST_FRAME_IAX, IAX_COMMAND_VNAK);
	iax2_ss_fill_ts(_pCurScb);
	pHeader = Adapter_pUdpBuf;
	pHeader[IAX_FULLFR_OSEQNO] = _pCurScb->iOSeqNo;
	// send packet
	UdpSendTo(_pIax2Socket, IAX_FULLFR_DATA, _pCurScb->pDstIP, _pCurScb->sDstPort);
}

void iax2_send_new()
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_short(IAX_IE_VERSION, IAX_PROTO_VERSION);
	if (!strlen(_pActiveLcb->pPeerNumber))
	{
		iax2_ie_add_str(IAX_IE_CALLED_NUMBER, "s");
		strcpy(_pActiveLcb->pPeerNumber, _pActiveLcb->pPeerName);
	}
	else
	{
		iax2_ie_add_str(IAX_IE_CALLED_NUMBER, _pActiveLcb->pPeerNumber);
	}
	if (strlen(Sys_pUserNumber))
	{
		iax2_ie_add_str(IAX_IE_CALLING_NUMBER, Sys_pUserNumber);
	}
	if (strlen(Sys_pUserName))
	{
		iax2_ie_add_str(IAX_IE_CALLING_NAME, Sys_pUserName);
	}
	if (strlen(Sys_pAuthID))
	{
		iax2_ie_add_str(IAX_IE_USERNAME, Sys_pAuthID);
	}
	iax2_ie_add_long(IAX_IE_FORMAT, iax2_get_local_format());
	iax2_ie_add_long(IAX_IE_CAPABILITY, iax2_get_local_cap());
	
	//send packet
	iax2_send_iax(IAX_COMMAND_NEW, iax2_ie_len());
}

void iax2_send_dpreq()
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_str(IAX_IE_CALLED_NUMBER, _pActiveLcb->pPeerNumber);
	//send packet
	iax2_send_iax(IAX_COMMAND_DPREQ, iax2_ie_len());
}

void iax2_send_dial()
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_str(IAX_IE_CALLED_NUMBER, _pActiveLcb->pPeerNumber);
	//send packet
	iax2_send_iax(IAX_COMMAND_DIAL, iax2_ie_len());
}

void iax2_send_accept(ULONG lFormat)
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_long(IAX_IE_FORMAT, lFormat);
	//send packet
	iax2_send_iax(IAX_COMMAND_ACCEPT, iax2_ie_len());
}

void iax2_send_quelch()
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_str(IAX_IE_MUSICONHOLD, "");
	//send packet
	iax2_send_iax(IAX_COMMAND_QUELCH, iax2_ie_len());
}

void iax2_send_authrep()
{
	// fill ie
	iax2_ie_start();
	iax2_add_auth();
	//send packet
	iax2_send_iax(IAX_COMMAND_AUTHREP, iax2_ie_len());
}

void iax2_send_hangup(UCHAR iCommand, UCHAR iCauseCode)
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_byte(IAX_IE_CAUSECODE, iCauseCode);
	iax2_ie_add_str(IAX_IE_CAUSE, iax2_get_cause(iCauseCode));
	//send packet
	iax2_send_iax(iCommand, iax2_ie_len());
}

void iax2_send_xfer(PCHAR pNumber)
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_str(IAX_IE_CALLED_NUMBER, pNumber);
	//send packet
	iax2_send_iax(IAX_COMMAND_TRANSFER, iax2_ie_len());
}

// TXCNT, TXACC has the same format
void iax2_send_txcmd(UCHAR iCommand)
{
	PCHAR pHeader;
	UCHAR sLen;

	iax2_debug("Send", AST_FRAME_IAX, iCommand);
	// fill header
	pHeader = Adapter_pUdpBuf;
	USHORT2PCHAR((_pCurScb->sSrcCallNo | IAX_FLAG_FULL), (PCHAR)(pHeader + IAX_FULLFR_SCALLNO));
	USHORT2PCHAR(_pCurScb->sXferCallNo, (PCHAR)(pHeader + IAX_FULLFR_DCALLNO));
	pHeader[IAX_FULLFR_ISEQNO] = 0;
	pHeader[IAX_FULLFR_OSEQNO] = 0;
	pHeader[IAX_FULLFR_TYPE] = AST_FRAME_IAX;
	pHeader[IAX_FULLFR_CSUB] = iCommand;
	ULONG2PCHAR((ULONG)(Sys_lTicker - _pCurScb->lOffset), (PCHAR)(pHeader + IAX_FULLFR_TSTAMP));
	// fill ie
	iax2_ie_start();
	iax2_ie_add_data(IAX_IE_TRANSFERID, _pCurScb->pXferID, IAX_XFID_LEN);
	//send packet
	sLen = IAX_FULLFR_DATA + iax2_ie_len();
	// save frame for retransmission
	if (iCommand == IAX_COMMAND_TXCNT)
	{
		iax2_fr_save(_pCurScb, pHeader, sLen);
	}
	// will do retransmission later
	UdpSendTo(_pIax2Socket, sLen, _pCurScb->pXferDstIP, _pCurScb->sXferDstPort);
}

void iax2_send_txready()
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_data(IAX_IE_TRANSFERID, _pCurScb->pXferID, IAX_XFID_LEN);
	//send packet
	iax2_send_iax(IAX_COMMAND_TXREADY, iax2_ie_len());
}

void iax2_send_txreq()
{
	UCHAR pSockAddr[SOCKADDR_DATA];

	// fill ie
	iax2_ie_start();
	iax2_ie_add_data(IAX_IE_TRANSFERID, _pCurScb->pXferID, IAX_XFID_LEN);

	memset(pSockAddr, 0, SOCKADDR_DATA);
	pSockAddr[0] = 2;
	USHORT2PCHAR(_pCurScb->sXferDstPort, (PCHAR)(pSockAddr + SOCKADDR_PORT));
	memcpy4((PCHAR)(pSockAddr + SOCKADDR_IP), _pCurScb->pXferDstIP);
	iax2_ie_add_data(IAX_IE_APPARENT_ADDR, pSockAddr, SOCKADDR_DATA);

	iax2_ie_add_short(IAX_IE_CALLNO, _pCurScb->sXferCallNo);
	//send packet
	iax2_send_iax(IAX_COMMAND_TXREQ, iax2_ie_len());
}

void iax2_send_txrel()
{
	// fill ie
	iax2_ie_start();
	iax2_ie_add_short(IAX_IE_CALLNO, _pCurScb->sXferCallNo);
	//send packet
	iax2_send_iax(IAX_COMMAND_TXREL, iax2_ie_len());
}
/**********************************************************************
 ** Voice functions                                                  **
 **********************************************************************
 */

UCHAR iax2_compress_subclass(ULONG lSubClass)
{
	UCHAR i;

	/* If it's 128 or smaller, just return it */
	if (lSubClass < IAX_FLAG_SC_LOG)
		return (UCHAR)lSubClass;

	/* Otherwise find its power */
	for (i = 0; i < IAX_MAX_SHIFT; i ++) 
	{
		if (lSubClass == (ULONG)(1 << i)) 
		{
			break;
		}
	}
	return (UCHAR)(i | IAX_FLAG_SC_LOG);
}

ULONG iax2_uncompress_subclass(UCHAR iSubClass)
{
	/* If the SC_LOG flag is set, return 2^csub otherwise csub */
	if (iSubClass & IAX_FLAG_SC_LOG)
		return (ULONG)(1 << (iSubClass & ~IAX_FLAG_SC_LOG & IAX_MAX_SHIFT));
	else
		return (ULONG)iSubClass;
}

USHORT iax2_fill_voice(PCHAR pSendBuf, UCHAR iFrame, PCHAR pBuf)
{
	UCHAR i, iLen;
	USHORT sLen;
	PCHAR pSrc;

	sLen = 0;
	pSrc = pBuf;
	for (i = 0; i < iFrame; i ++)
	{
		iLen = *pSrc;
		if (!iLen)	break;
		pSrc ++;
		memcpy((PCHAR)(pSendBuf + sLen), pSrc, iLen);
		pSrc += iLen;
		sLen += iLen;
	}

	return sLen;
}

void Iax2FrameTimer()
{
	if (_pActiveLcb->bSendKey)
	{
		_pActiveLcb->sSendKeyLen += GetFrameTime(_pActiveLcb->iVoiceCoder);
		if (_pActiveLcb->sSendKeyLen >= 240)
		{
			Iax2SendKey();
		}
	}
}

void Iax2SendVoice(UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2)
{
	ULONG lFormat;
	USHORT sLen;
	ULONG lTimeStamp;
	PCHAR pSend;

	if (_pActiveLcb->iState != CALL_STATE_TALKING && _pActiveLcb->iState != CALL_STATE_RINGING)		return;

	if (!iFrame1 && !iFrame2)	return;

	_pCurScb = _pActiveLcb->pScb;
	pSend = Adapter_pUdpBuf;
	lTimeStamp = Sys_lTicker - _pCurScb->lOffset;
	if (_pCurScb->lLastSent && ((lTimeStamp & 0xFFFF0000) == (_pCurScb->lLastSent & 0xFFFF0000)))
	{
		USHORT2PCHAR(_pCurScb->sSrcCallNo, (PCHAR)(pSend + IAX_MINIFR_SCALLNO));
		USHORT2PCHAR((USHORT)lTimeStamp, (PCHAR)(pSend + IAX_MINIFR_TSTAMP));
		sLen = iax2_fill_voice((PCHAR)(pSend + IAX_MINIFR_DATA), iFrame1, pBuf1);
		sLen += iax2_fill_voice((PCHAR)(pSend + IAX_MINIFR_DATA + sLen), iFrame2, pBuf2);
		if (sLen)
		{
			UdpSendTo(_pIax2Socket, sLen + IAX_MINIFR_DATA, _pCurScb->pDstIP, _pCurScb->sDstPort);
			_pCurScb->lLastSent = lTimeStamp;
		}
	}
	else
	{
		lFormat = _cAstFormat[_pActiveLcb->iVoiceCoder-1];
		iax2_ss_fill_cm(_pCurScb, AST_FRAME_VOICE, iax2_compress_subclass(lFormat));
		ULONG2PCHAR(lTimeStamp, (PCHAR)(pSend + IAX_FULLFR_TSTAMP));
		// fill data
		sLen = iax2_fill_voice((PCHAR)(pSend + IAX_FULLFR_DATA), iFrame1, pBuf1);
		sLen += iax2_fill_voice((PCHAR)(pSend + IAX_FULLFR_DATA + sLen), iFrame2, pBuf2);
		//send packet
		if (sLen)
		{
			iax2_ss_send(_pCurScb, sLen);
			_pCurScb->lLastSent = lTimeStamp;
		}
	}
}

void iax2_handle_voice(USHORT sLen, PCHAR pBuf)
{
	UCHAR iState;
//	ULONG lTimeStamp;

	// Only process voice if the source call is the active voice session
	if (_pCurScb->pLcb != _pActiveLcb)	return;
	if (_pActiveLcb->bQuelch)	return;

	iState = _pActiveLcb->iState;
	if (iState != CALL_STATE_CALLING && iState != CALL_STATE_TALKING)
	{
		return;
	}
	if (_pActiveLcb->bLocalRb)
	{
		_pActiveLcb->bLocalRb = FALSE;
		TaskUIHandler(UI_CALL_RINGBACK_REMOTE, _pActiveLcb->iVoiceCoder);
	}
//	lTimeStamp = rl_32x8(PCHAR2ULONG(Iax2_pTimeStamp), 3);
	TaskIncomingData(sLen, pBuf);
}

/**********************************************************************
 ** Receive packet functions                                            **
 **********************************************************************
 */
void iax2_check_msg()
{
	if (Iax2_pIeExist[IAX_IE_MSGCOUNT])
	{
		TaskUIHandler(UI_VOICE_MESSAGE, (USHORT)Iax2_pIeMsgCount);
	}
}

void iax2_check_datetime()
{
	if (Iax2_pIeExist[IAX_IE_DATETIME])
	{
		TaskUIHandler(UI_IAX2_TIME, (USHORT)Iax2_pIeDateTime);
	}
}

void iax2_recv_regack()
{
	if (Iax2_pIeExist[IAX_IE_REFRESH])
	{
		_sRegTimeOut = Iax2_sIeRefresh;
	}
	_pCurScb->iState = SSS_TIMEWAIT;
	iax2_reg_ok();
	iax2_check_msg();
	iax2_check_datetime();
}

void iax2_recv_regrej()
{
	_pCurScb->iState = SSS_TIMEWAIT;
	iax2_reg_failed();
}

void iax2_recv_accept()
{
	UCHAR iVoiceCoder;
	IAX2_LCB_HANDLE pLcb;

	pLcb = _pCurScb->pLcb;
	iVoiceCoder = iax2_get_coder(Iax2_lIeFormat);
	if (iVoiceCoder == VOICE_CODER_NONE)
	{
		iax2_hangup(IAX_CAUSE_INCOMPATIBEL);
		if (pLcb == _pActiveLcb)
		{
			iax2_call_failed(CALL_END_CODER_MISMATCH);
		}
	}
	else
	{
		pLcb->iVoiceCoder = iVoiceCoder;
	}
}

void iax2_recv_ringing()
{
	IAX2_LCB_HANDLE pLcb;

	pLcb = _pCurScb->pLcb;
	if (_pCurScb->pLcb == _pActiveLcb && (pLcb->iState == CALL_STATE_CALLING || pLcb->iState == CALL_STATE_TALKING))
	{
		TaskUIHandler(UI_CALL_RINGBACK, _pActiveLcb->iVoiceCoder);
		_pActiveLcb->bLocalRb = TRUE;
	}
}

void iax2_recv_answer()
{
	IAX2_LCB_HANDLE pLcb;

	pLcb = _pCurScb->pLcb;
	if (pLcb)
	{
		if (pLcb == _pActiveLcb)
		{
			TaskUIHandler(UI_CALL_CONNECTED, (USHORT)(&_pActiveLcb->iVoiceCoder));
			iax2_show_cid(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, FALSE);
		}
		pLcb->iState = CALL_STATE_TALKING;
	}
}

void iax2_recv_busy()
{
	iax2_hangup(IAX_CAUSE_CALL_REJECTED);
	if (_pCurScb->pLcb == _pActiveLcb)
	{
		iax2_call_failed(CALL_END_PEER_BUSY);
	}
	else
	{
		iax2_ln_free(_pCurScb->pLcb);
	}
}

void iax2_recv_dtmf(UCHAR iDtmf)
{
	if (_pCurScb->pLcb != _pActiveLcb)	return;
	if (_pActiveLcb->iState != CALL_STATE_TALKING)	return;

	TaskUIHandler(UI_CALL_INCOMING_DTMF, (USHORT)iDtmf);
}

void iax2_recv_voice(PCHAR pData, USHORT sLen, UCHAR iSubClass)
{
	ULONG lFormat;
	UCHAR iVoiceCoder;
	IAX2_LCB_HANDLE pLcb;

	if (!sLen)	return;
	
	pLcb = _pCurScb->pLcb;
	lFormat = iax2_uncompress_subclass(iSubClass);
	iVoiceCoder = iax2_get_coder(lFormat);
	if (iVoiceCoder == VOICE_CODER_NONE)
	{
		iax2_hangup(IAX_CAUSE_INCOMPATIBEL);
		if (pLcb == _pActiveLcb)
		{
			iax2_call_failed(CALL_END_CODER_MISMATCH);
		}
	}
	else 
	{
		if (iVoiceCoder != pLcb->iVoiceCoder)
		{
			pLcb->iVoiceCoder = iVoiceCoder;
			if (pLcb == _pActiveLcb && !pLcb->bQuelch)
			{
				TaskUIHandler(UI_CALL_UPDATE_CODEC, (USHORT)(&_pActiveLcb->iVoiceCoder));
			}
		}
		iax2_handle_voice(sLen, pData);
	}
}

void iax2_recv_hangup()
{
	UCHAR iEndCause;

	_pCurScb->iState = SSS_TIMEWAIT;

	if (_pCurScb->pLcb == _pActiveLcb)
	{
		iEndCause = CALL_END_UNKNOWN;
		if (Iax2_pIeExist[IAX_IE_CAUSECODE])
		{
			switch (Iax2_iCauseCode)
			{
			case IAX_CAUSE_NORMAL_CLEARING:
				iEndCause = CALL_END_PEER_HANGUP;
				break;

			case IAX_CAUSE_USER_BUSY:
				iEndCause = CALL_END_PEER_BUSY;
				break;

			case IAX_CAUSE_NO_USER_RESPONSE:
			case IAX_CAUSE_NO_ANSWER:
				iEndCause = CALL_END_NO_ANSWER;
				break;

			case IAX_CAUSE_CALL_REJECTED:
				iEndCause = CALL_END_PEER_REJECT;
				break;

			case IAX_CAUSE_INCOMPATIBEL:
				iEndCause = CALL_END_CODER_MISMATCH;
				break;

			}
		}
		iax2_call_failed(iEndCause);
	}
	else
	{
		iax2_ln_free(_pCurScb->pLcb);
	}
}

USHORT iax2_ie_get_addr(PCHAR pDstIP)
{
	if (PCHAR2USHORT(Iax2_pIeApparentAddr) == ADDR_FAMILY_INET)
	{
		memcpy4(pDstIP, (PCHAR)(Iax2_pIeApparentAddr + SOCKADDR_IP));
		return PCHAR2USHORT((PCHAR)(Iax2_pIeApparentAddr + SOCKADDR_PORT));
	}
	else
	{
		memset(pDstIP, 0, IP_ALEN);
		return 0;
	}
}

void iax2_recv_txreq()
{
	if (_pCurScb->iXferState != TRANSFER_NONE)
	{
		iax2_send_noie(IAX_COMMAND_TXREJ);
		return;
	}

	// Sets up a transfer session
	_pCurScb->sXferDstPort = iax2_ie_get_addr(_pCurScb->pXferDstIP);
/*	
	check if the transfer IP and port is valid
	if (!_pCurScb->sXferDstPort || !IsValidIP(_pCurScb->pXferDstIP))	
	{
		iax2_send_noie(IAX_COMMAND_TXREJ);
		return;
	}
*/
	_pCurScb->iXferState = TRANSFER_BEGIN;
	memcpy4(_pCurScb->pXferID, Iax2_pIeTransferId);
	_pCurScb->sXferCallNo = Iax2_sIeCallNo; 
	iax2_send_txcmd(IAX_COMMAND_TXCNT);
}

void iax2_recv_txacc()
{
	if (_pCurScb->iXferState == TRANSFER_NONE)
	{
		UdpDebugString("Ignoring txacc: No Corresponding Transferer.");
		return;
	}

	// Ensure TransferId match
	if (memcmp(Iax2_pIeTransferId, _pCurScb->pXferID, IAX_XFID_LEN))
	{
		UdpDebugString("Ignoring txacc: TransferId doesn't match.");
		return;
	}

	iax2_fr_txacked(_pCurScb);
	_pCurScb->iXferState = TRANSFER_READY;
	// Things are o.k., so send a TXREADY to the original and set our status as ready
	iax2_send_txready();
}

void iax2_recv_txcnt()
{
	if (_pCurScb->iXferState == TRANSFER_NONE)
	{
		UdpDebugString("Ignoring txcnt: No Corresponding Transferer.");
		return;
	}

	// Ensure TransferId match
	if (memcmp(Iax2_pIeTransferId, _pCurScb->pXferID, IAX_XFID_LEN))
	{
		UdpDebugString("Ignoring txcnt: TransferId doesn't match.");
		return;
	}

	memcpy4(_pCurScb->pXferDstIP, UdpGetDstIP(_pIax2Socket));
	_pCurScb->sXferDstPort = UdpGetDstPort(_pIax2Socket);
	iax2_send_txcmd(IAX_COMMAND_TXACC);
}

void iax2_recv_txrel()
{
	if (_pCurScb->iXferState != TRANSFER_READY)
	{
		UdpDebugString("Ignoring txrel: Transfer is not ready.");
		return;
	}

	// Switch current session to transfer session
	memcpy4(_pCurScb->pDstIP, _pCurScb->pXferDstIP);
	_pCurScb->sDstPort = _pCurScb->sXferDstPort;
	_pCurScb->sDstCallNo = _pCurScb->sXferCallNo;
	_pCurScb->iOSeqNo = 0;
	_pCurScb->iISeqNo = 0;
	_pCurScb->iRSeqNo = 0;
	_pCurScb->lLastSent = 0;
	_pCurScb->lOffset = Sys_lTicker;
	_pCurScb->pLcb->bQuelch = FALSE;

	iax2_ss_clear_xfer(_pCurScb);
}

void iax2_recv_txrej()
{
	if (_pCurScb->iXferState == TRANSFER_NONE)
	{
		UdpDebugString("Ignoring txrej: No transfer going on.");
		return;
	}

	iax2_ss_clear_xfer(_pCurScb);
}

void iax2_recv_txready()
{
/*	IAX2_LCB_HANDLE pAltLcb;

	_pActiveLcb = _pCurScb->pLcb;	//?
	pAltLcb = iax2_ln_alt();

	if (pAltLcb->pScb->iXferState == TRANSFER_READY)
	{
		iax2_send_txrel();
		_pCurScb->iState = SSS_FINALACK;
		iax2_ln_free(_pCurScb->pLcb);

		_pCurScb = pAltLcb->pScb;
		iax2_send_txrel();
		_pCurScb->iState = SSS_FINALACK;
		iax2_ln_free(_pCurScb->pLcb);

		TaskUIHandler(UI_CALL_FINISHED, 0);
	}
	else
	{
		_pCurScb->iXferState = TRANSFER_READY;
	}
*/
}

void iax2_recv_xfer()
{
	IAX2_LCB_HANDLE pLcb;

	iax2_hangup(IAX_CAUSE_NORMAL_CLEARING);
	pLcb = _pCurScb->pLcb;
	iax2_ln_free(_pCurScb->pLcb);

	if (pLcb == _pActiveLcb)
	{
		TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)Iax2_pIeCalledNumber);
		Iax2CallStep1(Iax2_pIeCalledNumber);
	}
}

void iax2_recv_dprep()
{
	if (_pCurScb->pLcb != _pActiveLcb)	return;

	if (Iax2_sIeDPStatus & IAX_DPSTATUS_NONEXISTANT)
	{
		iax2_hangup(IAX_CAUSE_NORMAL_CLEARING);
		iax2_call_failed(CALL_END_WRONG_NUMBER);
		return;
	}
	if ((Iax2_sIeDPStatus & IAX_DPSTATUS_EXISTS) || (Iax2_sIeDPStatus & IAX_DPSTATUS_CANEXIST))
	{
		if (Iax2_sIeDPStatus & (USHORT)IAX_DPSTATUS_IGNOREPAT)
		{
			UdpDebugString("Ignore pat");
			return;
		}
		if (Iax2_sIeDPStatus & (USHORT)IAX_DPSTATUS_MATCHMORE)
		{
			return;
		}
		strcpy(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber);
		TaskUIHandler(UI_CALL_CALLING, (USHORT)_pActiveLcb->pPeerNumber);
		_pActiveLcb->iState = CALL_STATE_CALLING;
		iax2_send_dial();
	}
}

void iax2_get_cid(PCHAR pDst, PCHAR pSrc, UCHAR iMaxLen)
{
	UCHAR iLen;

	if (pSrc)
	{
		iLen = strlen(pSrc);
		if (iLen)
		{
			if (iLen < iMaxLen)
			{
				strcpy(pDst, pSrc);
			}
			else
			{
				memcpy(pDst, pSrc, iMaxLen-2);
				pDst[iMaxLen-2] = '_';
				pDst[iMaxLen-1] = 0;
			}
		}
	}
}

void iax2_fwd()
{
	UCHAR pNumber[MAX_USER_NUMBER_LEN];

	iax2_send_ctrl(AST_CONTROL_ANSWER);
	OptionsGetString(pNumber, OPT_FWD_NUMBER, MAX_USER_NUMBER_LEN);
	UdpDebugString("forwarding to");
	UdpDebugString(pNumber);
	iax2_send_xfer(pNumber);
	_pCurScb->iState = SSS_FINALACK;
}

void iax2_recv_new()
{
	UCHAR iVoiceCoder, iCause;
	ULONG lFormat;
	UCHAR pName[MAX_USER_NAME_LEN];
	UCHAR pNumber[MAX_USER_NUMBER_LEN];
	IAX2_LCB_HANDLE pLcb;
	IAX2_LCB_HANDLE pAltLcb;
	BOOLEAN bAutoAnswer;

	// display caller id
	pName[0] = 0;
	pNumber[0] = 0;
	iax2_get_cid(pNumber, Iax2_pIeCallingNumber, MAX_USER_NUMBER_LEN);
	iax2_get_cid(pName, Iax2_pIeCallingName, MAX_USER_NAME_LEN);
	if (!strlen(pNumber))
	{
		if (Sys_bRegister)
		{
			strcpy(pNumber, strlen(pName) ? pName : (PCHAR)"[No number]");
		}
		else
		{
			ip2str(UdpGetDstIP(_pIax2Socket), pNumber);
		}
	}
	if (!strlen(pName))
	{
		strcpy(pName, "[No name]");
	}

	// check if we are able to accept call
	if (Sys_bDnd ||(Sys_iCurMode != SYS_MODE_STANDBY && Sys_iCurMode != SYS_MODE_CALL))
	{
		iCause = IAX_CAUSE_USER_BUSY;
		goto Reject;
	}

	// check audio type
	lFormat = Iax2_lIeFormat;
	iVoiceCoder = iax2_get_coder(lFormat);
	if (iVoiceCoder == VOICE_CODER_NONE)
	{
		lFormat = iax2_get_format(Iax2_lIeCapability);
		if (!lFormat)
		{
			iCause = IAX_CAUSE_INCOMPATIBEL;
			goto Reject;
		}
		iVoiceCoder = iax2_get_coder(lFormat);
	}

	// check if should forward this call
	iax2_send_accept(lFormat);
	if (Sys_bFwdAlways)
	{
		goto Forward;
	}

	// check if we are busy
	bAutoAnswer = FALSE;
	if (_pActiveLcb->iState == CALL_STATE_FREE)
	{
		// Start Codec
		pLcb = _pActiveLcb;
		if (Iax2_pIeExist[IAX_IE_AUTOANSWER] || (Sys_bAutoAnswer && !Sys_iNoanswerTimeout))
		{
			bAutoAnswer = TRUE;
		}
		else
		{
			TaskUIHandler(UI_CALL_RINGING, iVoiceCoder);
			iax2_show_cid(pName, pNumber, TRUE);
		}
	}
	else
	{
		pAltLcb = iax2_ln_alt();
		if (Sys_bCallWaiting && pAltLcb->iState == CALL_STATE_FREE)
		{
			TaskUIHandler(UI_CALL_WAITING, 0);
			iax2_show_cid(pName, pNumber, FALSE);
			pLcb = pAltLcb;
		}
		else
		{
			if (Sys_bFwdBusy)
			{
				goto Forward;
			}
			else
			{
				iCause = IAX_CAUSE_USER_BUSY;
				goto Reject;
			}
		}
	}

	// save line information
	pLcb->pScb = _pCurScb;
	pLcb->iState = CALL_STATE_RINGING;
	pLcb->bCallee = TRUE;
	pLcb->bQuelch = FALSE;
	pLcb->bLocalRb = FALSE;
	pLcb->iVoiceCoder = iVoiceCoder;
	pLcb->iTimer = 0;
	CallTimeZero(&pLcb->ct);
	_pCurScb->pLcb = pLcb;
	strcpy(pLcb->pPeerName, pName);
	strcpy(pLcb->pPeerNumber, pNumber);
	if (bAutoAnswer)
	{
		TaskUIHandler(UI_CALL_AUTOANSWER, 0); 
		iax2_answer();
	}
	else
	{
		iax2_send_ctrl(AST_CONTROL_RINGING);
	}
	return;

Reject:
	iax2_hangup(iCause);
	TaskCallLogAdd(pName, pNumber, CALL_LOG_TYPE_MISSED);
	return;

Forward:
	iax2_send_ctrl(AST_CONTROL_RINGING);
	iax2_fwd();
	TaskCallLogAdd(pName, pNumber, CALL_LOG_TYPE_MISSED);
}

BOOLEAN iax2_cmp_addr()
{
	return (!memcmp(UdpGetDstIP(_pIax2Socket), _pCurScb->pDstIP, IP_ALEN) && (UdpGetDstPort(_pIax2Socket) == _pCurScb->sDstPort)) ? TRUE : FALSE;
}

USHORT _get_line(PCHAR pBuf, PSHORT pLineLen)
{
	PCHAR pr;
	PCHAR pn;
	USHORT sLen, sLen1, sLen2;

	pr = strchr(pBuf, '\r');
	pn = strchr(pBuf, '\n');

	if (pr && pn)
	{
		sLen1 = (USHORT)pr - (USHORT)pBuf;
		sLen2 = (USHORT)pn - (USHORT)pBuf;
		if (sLen1 < sLen2)
		{
			sLen = sLen1;
			*pLineLen = (sLen2 == sLen1 + 1) ? sLen2 + 1 : sLen1 + 1;
		}
		else
		{
			sLen = sLen2;
			*pLineLen = (sLen1 == sLen2 + 1) ? sLen1 + 1 : sLen2 + 1;
		}
	}
	else
	{
		if (pr)
		{
			sLen = (USHORT)pr - (USHORT)pBuf;
			*pLineLen = sLen + 1;
		}
		else if (pn)
		{
			sLen = (USHORT)pn - (USHORT)pBuf;
			*pLineLen = sLen + 1;
		}
		else
		{
			sLen = strlen(pBuf);
			*pLineLen = sLen;
		}
	}

	pBuf[sLen] = 0;
	return sLen;
}

void iax2_recv_text(PCHAR pData, USHORT sLen)
{
	UCHAR i, iRecvLine;
	USHORT sIndex, sActualLen, sLineLen;
	PCHAR pLine;
	PCHAR pRecvLine[DISPLAY_MAX_LINE];

	sIndex = 0;
	iRecvLine = 0;
	for (i = 0; i < DISPLAY_MAX_LINE; i ++)
	{
		pRecvLine[i] = NULL;
	}
	while (sIndex < sLen)
	{
		pLine = (PCHAR)(pData + sIndex);
		sActualLen = _get_line(pLine, &sLineLen);
		sIndex += sLineLen;
		if (sActualLen)
		{
			if (iRecvLine < DISPLAY_MAX_LINE)
			{
				pRecvLine[iRecvLine] = heap_save_str(pLine);
				iRecvLine ++;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	if (iRecvLine)
	{
		TaskUIHandler(UI_CALL_DISPLAY_MSG, (USHORT)pRecvLine);
		for (i = 0; i < iRecvLine; i ++)
		{
			free(pRecvLine[i]);
		}
	}
}

void iax2_recv_packet(PCHAR pData, USHORT sLen, UCHAR iType, UCHAR iSubClass, UCHAR iISeqNo, UCHAR iOSeqNo)
{
	UCHAR iTemp;
	BOOLEAN bAck;
	BOOLEAN bXfer;
	BOOLEAN bInval;

	bAck = (iType == AST_FRAME_IAX) && (iSubClass == IAX_COMMAND_ACK || iSubClass == IAX_COMMAND_VNAK);
	bInval = (iType == AST_FRAME_IAX) && (iSubClass == IAX_COMMAND_INVAL);
	bXfer = (iType == AST_FRAME_IAX) && (iSubClass == IAX_COMMAND_TXACC || iSubClass == IAX_COMMAND_TXCNT);

	/* Handle implicit ACKing unless this is an INVAL, and only if this is from the real peer, not the transfer peer */
	if (iax2_cmp_addr() && !bInval) 
	{
		/* XXX This code is not very efficient.  Surely there is a better way which still properly handles boundary conditions? XXX */
		/* First we have to qualify that the ACKed value is within our window */
		for (iTemp = _pCurScb->iRSeqNo; iTemp != _pCurScb->iOSeqNo; iTemp ++)
		{
			if (iTemp == iISeqNo)	break;
		}
		if (iTemp != _pCurScb->iOSeqNo || _pCurScb->iOSeqNo == iISeqNo) 
		{
			/* The acknowledgement is within our window.  Time to acknowledge everything that it says to */
			for (iTemp = _pCurScb->iRSeqNo; iTemp != iISeqNo; iTemp ++) 
			{
				/* Ack the packet with the given timestamp */
				iax2_fr_acked(_pCurScb, iTemp);
			}
			/* Note how much we've received acknowledgement for */
			_pCurScb->iRSeqNo = iISeqNo;
		} 
/*		else
		{
			// Received iseqno not within window
		}
*/
	}

	if (!bXfer)
	{
		if (_pCurScb->iISeqNo != iOSeqNo) 
		{
			if (!bAck && !bInval) 
			{
				/* If it's not an ACK packet, it's out of order. */
				if (_pCurScb->iISeqNo > iOSeqNo) 
				{
					/* If we've already seen it, ack it XXX There's a border condition here XXX */
					/* XXX Maybe we should handle its ack to us, but then again, it's probably outdated anyway, and if
					   we have anything to send, we'll retransmit and get an ACK back anyway XXX */
					iax2_send_ack();
				} 
				else 
				{
					/* Send a VNAK requesting retransmission */
					iax2_send_vnak();
				}
			}
			return;
		} 
		else 
		{
			/* Increment unless it's an ACK or VNAK */
			if (!bAck&& !bInval)	_pCurScb->iISeqNo ++;
		}
	}

	iax2_debug("Got", iType, iSubClass);
	switch(iType) 
	{
	case AST_FRAME_DTMF:
		iax2_recv_dtmf(iSubClass);
		iax2_send_ack();
		break;

	case AST_FRAME_VOICE:
		iax2_recv_voice(pData, sLen, iSubClass);
		iax2_send_ack();
		break;

	case AST_FRAME_IAX:
		iax2_ie_parse(pData, sLen);
		switch(iSubClass) 
		{
		case IAX_COMMAND_POKE:
			iax2_send_pong();
			_pCurScb->iState = SSS_FINALACK;
			break;

		case IAX_COMMAND_NEW:
			iax2_recv_new();
			break;

		case IAX_COMMAND_AUTHREQ:
			/* This is a request for a call */
			iax2_send_authrep();
			break;

		case IAX_COMMAND_INVAL:
			iax2_send_ack();
			break;

		case IAX_COMMAND_HANGUP:
		case IAX_COMMAND_REJECT:
			iax2_send_ack();
			iax2_recv_hangup();
			break;

		case IAX_COMMAND_ACK:
			iax2_ss_acked(_pCurScb);
			break;

		case IAX_COMMAND_LAGRQ:
			iax2_send_lagrp();
			break;

		case IAX_COMMAND_PING:
			/* PINGS and PONGS don't get scheduled; */
			iax2_send_pong();
			break;

		case IAX_COMMAND_ACCEPT:
			iax2_send_ack();
			iax2_recv_accept();
			break;

		case IAX_COMMAND_REGACK:
			iax2_send_ack();
			iax2_recv_regack();
			break;

		case IAX_COMMAND_REGAUTH:
			iax2_send_regcmd(TRUE);
			break;

		case IAX_COMMAND_REGREJ:
			iax2_send_ack();
			iax2_recv_regrej();
			break;

		case IAX_COMMAND_QUELCH:
			iax2_send_ack();
//			_pCurScb->pLcb->bQuelch = TRUE;
			break;

		case IAX_COMMAND_UNQUELCH:
			iax2_send_ack();
//			_pCurScb->pLcb->bQuelch = FALSE;
			break;

		case IAX_COMMAND_TXREQ:
			iax2_send_ack();
			iax2_recv_txreq();
			break;

		case IAX_COMMAND_TXCNT:
			// TXCNT gets ack'd with a TXACC
			iax2_recv_txcnt();
			break;

		case IAX_COMMAND_TXACC:
			iax2_recv_txacc();
			break;

		case IAX_COMMAND_TXREL:
			iax2_send_ack();
			iax2_recv_txrel();
			break;

		case IAX_COMMAND_TXREJ:
			iax2_send_ack();
			iax2_recv_txrej();
			break;

		case IAX_COMMAND_TXREADY:
			iax2_recv_txready();
			break;

		case IAX_COMMAND_MWI:
			iax2_send_ack();
			iax2_check_msg();
			break;

		case IAX_COMMAND_TRANSFER:
			iax2_send_ack();
			iax2_recv_xfer();
			break;

		case IAX_COMMAND_DPREP:
			iax2_send_ack();
			iax2_recv_dprep();
			break;

		default:
			iax2_send_ack();
			break;
		}
		iax2_ie_free();
		break;

	case AST_FRAME_CONTROL:
		iax2_send_ack();
		switch(iSubClass) 
		{
		case AST_CONTROL_ANSWER:
			iax2_recv_answer();
			break;

		case AST_CONTROL_RINGING:
			iax2_recv_ringing();
			break;

		case AST_CONTROL_BUSY:
		case AST_CONTROL_CONGESTION:
			iax2_recv_busy();
			break;

//		case AST_CONTROL_HOLD:
//			_pCurScb->pLcb->bQuelch = TRUE;
//			break;

//		case AST_CONTROL_UNHOLD:
//			_pCurScb->pLcb->bQuelch = FALSE;
//			break;
		}
		break;

	case AST_FRAME_TEXT:
		iax2_recv_text(pData, sLen);
		iax2_send_ack();
		break;

	default:
		iax2_send_ack();
		break;
	}
}

/**********************************************************************
 ** API functions                                                    **
 **********************************************************************
 */
void Iax2Init()
{
	_pActiveLcb = _pIax2LCB;
	_sSrcCallNo = 1;
	_iRegState = RS_FREE;
	_bDigitDialing = FALSE;
	_pIax2Socket = NULL;
	_sRegTimeOut = 0;
	_sRegTimer = 0;

	iax2_ie_init();
	iax2_ss_init();
	iax2_fr_init();
	iax2_ln_init();

	Sys_sIax2Port = OptionsGetShort(OPT_IAX2_PORT);
}

void Iax2Startup()
{
	_sSrcCallNo = P_MAKEWORD(rand(), rand()&0x7f);
	if (!_sSrcCallNo)	_sSrcCallNo = 1;

	_bDigitDialing = ((OptionsGetByte(OPT_DIGIT_DIALING) == 1) && Sys_bRegister)? TRUE : FALSE;
	_iRegState = RS_FREE;
	if (_pIax2Socket)
	{
		UdpClose(_pIax2Socket);
	}
	_pIax2Socket = UdpListen(Sys_sLocalPort, PROTOCOL_BANK_OFFSET, (USHORT)Iax2Run);
}

void iax2_reg()
{
	iax2_ss_free(_pIax2SCB);
	_pCurScb = _pIax2SCB;
	iax2_reg_init();
	memcpy4(_pCurScb->pDstIP, _pRegServerIP);
	_pCurScb->sDstPort = Sys_sIax2Port;
	iax2_ss_sync(_pCurScb);
	iax2_send_regcmd(FALSE);
}

void Iax2Logon()
{
	UCHAR pDns[MAX_DNS_LEN];
	UCHAR pDstIP[IP_ALEN];

	if (Sys_bRegister)
	{
		UdpDebugString("Logon Step1");
		_iRegState = RS_REGISTERING;
		OptionsGetString(pDns, OPT_IAX2_SERVER, MAX_DNS_LEN);
		if (strlen(pDns))
		{
			if (str2ip(pDns, pDstIP))
			{
				Iax2LogonStep2(pDstIP);
			}
			else
			{
				TaskDnsQuery(pDns, DNS_TYPE_A, PROTOCOL_BANK_OFFSET, (USHORT)Iax2LogonStep2);
			}
		}
		else
		{
			iax2_reg_failed();
		}
	}
	else
	{
		_iRegState = RS_REGISTERED;
		TaskUIHandler(UI_LOGON_OK, 0);
	}
}

void Iax2LogonStep2(PCHAR pServerIP)
{
	if (pServerIP)
	{
		UdpDebugString("Logon Step2");
		memcpy4(_pRegServerIP, pServerIP);
		iax2_reg();
	}
	else
	{
		iax2_reg_failed();
	}
}

void Iax2DPCall()
{
	if (!Sys_bRegister)
	{
		iax2_call_failed(CALL_END_NOT_LOGON);
		return;
	}

	strcpy(_pActiveLcb->pPeerNumber, _cTBD);
	_sCalledPort = Sys_sIax2Port;
	Iax2CallStep2(_pRegServerIP);
	memset(_pActiveLcb->pPeerNumber, 0, MAX_USER_NUMBER_LEN);
}

void Iax2CallStep1(PCHAR pCalledUri)
{
	PCHAR p;
	UCHAR iLen, iState;
	UCHAR pDstIP[IP_ALEN];

	TaskUIHandler(UI_CALL_CALLING, (USHORT)pCalledUri);
	iState = _pActiveLcb->iState;
	_pActiveLcb->iState = CALL_STATE_CALLING;
	_pActiveLcb->iTimer = 0;

	iLen = strlen(pCalledUri);
	if (!iLen || iLen >= MAX_USER_NUMBER_LEN)
	{
		iax2_call_failed(CALL_END_EMPTY_NUMBER);
		return;
	}

	strcpy(_pActiveLcb->pPeerName, pCalledUri);
	strcpy(_pActiveLcb->pPeerNumber, pCalledUri);
	if (iState == CALL_STATE_DIALING && _bDigitDialing)
	{
		iax2_send_dial();
		return;
	}

	if (Sys_bRegister && _iRegState != RS_FREE)
	{
		_sCalledPort = Sys_sIax2Port;
		Iax2CallStep2(_pRegServerIP);
	}
	else
	{
		// Get destination port
		_sCalledPort = IAX2_SERVER_PORT;
		p = SkipField(pCalledUri, ':');
		if (p)
		{
			_sCalledPort = atoi(p, 10);
		}

		// get called number
		p = SkipField(pCalledUri, '@');
		if (!p)
		{
			p = pCalledUri;
		}
		if (strlen(p))
		{
			if (str2ip(p, pDstIP))
			{
				Iax2CallStep2(pDstIP);
			}
			else
			{
				TaskDnsQuery(p, DNS_TYPE_A, PROTOCOL_BANK_OFFSET, (USHORT)Iax2CallStep2);
			}
		}
		else
		{
			iax2_call_failed(CALL_END_WRONG_NUMBER);
		}
	}
}

void Iax2CallStep2(PCHAR pServerIP)
{
	if (!pServerIP)	
	{
		iax2_call_failed(CALL_END_BAD_SERVERIP);
		return;
	}

	_pCurScb = iax2_ss_new();
	if (!_pCurScb)
	{
		iax2_call_failed(CALL_END_RES_LIMITED);
		return;
	}

	// set Line control block
	_pActiveLcb->pScb = _pCurScb;
	_pActiveLcb->bCallee = FALSE;
	_pActiveLcb->bQuelch = FALSE;
	_pActiveLcb->iVoiceCoder = VOICE_CODER_NONE;
	_pActiveLcb->iTimer = 0;
	CallTimeZero(&_pActiveLcb->ct);
	_pActiveLcb->bLocalRb = TRUE;
	// set session control block 
	_pCurScb->pLcb = _pActiveLcb;
	memcpy4(_pCurScb->pDstIP, pServerIP);
	_pCurScb->sDstPort = _sCalledPort;
	iax2_ss_sync(_pCurScb);
	// send packet
	iax2_send_new();
}

void iax2_answer()
{
	iax2_send_ctrl(AST_CONTROL_ANSWER);
	_pActiveLcb->iState = CALL_STATE_TALKING;
	TaskUIHandler(UI_CALL_CONNECTED, (USHORT)(&_pActiveLcb->iVoiceCoder));
	iax2_show_cid(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, FALSE);

	TaskCallLogAdd(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, CALL_LOG_TYPE_RECVED);
}

void iax2_unhold()
{
	iax2_send_noie(IAX_COMMAND_UNQUELCH);
	_pActiveLcb->bQuelch = FALSE;
	TaskUIHandler(UI_CALL_CONNECTED, (USHORT)(&_pActiveLcb->iVoiceCoder));
	iax2_show_cid(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, FALSE);
}

void iax2_hold()
{
	iax2_send_quelch();
	_pActiveLcb->bQuelch = TRUE;
	TaskUIHandler(UI_CALL_HOLD, (USHORT)(&_pActiveLcb->iVoiceCoder));
	iax2_show_cid(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, FALSE);
}

void iax2_dial()
{
	_pActiveLcb->iState = CALL_STATE_DIALING;
	TaskUIHandler(UI_CALL_DIAL, 0);
	if (_bDigitDialing)
	{
		Iax2DPCall();
	}
}

void Iax2BlindTransfer(PCHAR pNumber)
{
	IAX2_LCB_HANDLE pAltLcb;

	pAltLcb = iax2_ln_alt();
	if (strlen(pNumber))
	{
		if (pAltLcb->iState == CALL_STATE_TALKING)
		{
			UdpDebugString("Blind transfer");
			TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_CALL_XFER);
			iax2_ln_free(_pActiveLcb);
			_pActiveLcb = pAltLcb;
			_pCurScb = _pActiveLcb->pScb;
			iax2_send_xfer(pNumber);
			_pActiveLcb->bXferor = FALSE;
		}
		else
		{
			TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_XFER_FAILED);
		}
	}
}

void Iax2Call(PCHAR pNumber)
{
	UCHAR pBuf[128];

	strcpy(pBuf, pNumber);
	if (_pActiveLcb->bXferor)
	{
		Iax2BlindTransfer(pBuf);
	}
	else
	{
		_pCurScb = _pActiveLcb->pScb;
		Iax2CallStep1(pBuf);
	}
}

void Iax2CollectKey(UCHAR iKey)
{
	UCHAR iLen;
	PCHAR pNumber;

	pNumber = _pActiveLcb->pPeerNumber;
	_pActiveLcb->iTimer = 0;
	iLen = strlen(pNumber);
	if (iLen < (MAX_USER_NUMBER_LEN-1))
	{
		if (Sys_bUseDigitmap)
		{
			pNumber[iLen] = iKey;
			pNumber[iLen + 1] = 0;
			if (TaskMapMatch(pNumber))
			{
				if (iKey == (UCHAR)'T')
				{
					pNumber[iLen] = 0;
				}
			}
			else
			{
				return;
			}
		}
		else 
		{
			if (iKey != (UCHAR)'T')
			{
				pNumber[iLen] = iKey;
				pNumber[iLen + 1] = 0;
				if (_bDigitDialing && !_pActiveLcb->bXferor)
				{
					iax2_send_dpreq();
				}			
				return;
			}
		}
	}

	Iax2Call(pNumber);
}

void Iax2KeyCanceled()
{
	UCHAR iLen;
	PCHAR pNumber;

	_pActiveLcb->iTimer = 0;
	pNumber = _pActiveLcb->pPeerNumber;
	iLen = strlen(pNumber);
	if (iLen)
	{
		pNumber[iLen-1] = 0;
	}
}

void Iax2HandsetUp(BOOLEAN bMemCall)
{
	if (_pActiveLcb->iState == CALL_STATE_RINGING)
	{
		_pCurScb = _pActiveLcb->pScb;
		iax2_answer();
	}
	else if (_pActiveLcb->iState == CALL_STATE_FREE)
	{
		iax2_dial();
		if (bMemCall)
		{
			_pActiveLcb->bMemCall = TRUE;
		}
		else
		{
			OptionsGetString(_pActiveLcb->pPeerNumber, OPT_HOTLINE_NUMBER, MAX_USER_NUMBER_LEN);
			if (strlen(_pActiveLcb->pPeerNumber))
			{
				TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)_pActiveLcb->pPeerNumber);
				_pActiveLcb->bMemCall = TRUE;
			}
		}
	}
	else if (_pActiveLcb->iState == CALL_STATE_TALKING)
	{
		_pCurScb = _pActiveLcb->pScb;
		iax2_unhold();
	}
}

void Iax2HandsetDown()
{
	UCHAR iState;

	iState = _pActiveLcb->iState;
	if (iState == CALL_STATE_RINGING)
	{
		return;
	}

	_pCurScb = _pActiveLcb->pScb;
	if (iState == CALL_STATE_TALKING)
	{
		if (_pActiveLcb->bQuelch)
		{
			return;
		}
	}

	if (iState != CALL_STATE_FREE)
	{
		if (_pCurScb && iState != CALL_STATE_HANGUP)
		{
			iax2_hangup(IAX_CAUSE_NORMAL_CLEARING);
		}
		iax2_ln_free(_pActiveLcb);
	}

	iax2_check_altline();
}

void Iax2Hold()
{
	if (_pActiveLcb->iState == CALL_STATE_TALKING)
	{
		_pCurScb = _pActiveLcb->pScb;
		if (_pActiveLcb->bQuelch)
		{
			iax2_unhold();
		}
		else
		{
			iax2_hold();
		}
	}
}

void Iax2Unhold()
{
	if (_pActiveLcb->iState == CALL_STATE_TALKING)
	{
		_pCurScb = _pActiveLcb->pScb;
		if (_pActiveLcb->bQuelch)
		{
			iax2_unhold();
		}
	}
}

void Iax2Flash()
{
	UCHAR iState;

	iState = _pActiveLcb->iState;
	if (iState == CALL_STATE_FREE || iState == CALL_STATE_RINGING)	return;

	_pCurScb = _pActiveLcb->pScb;
	// First hold the current active call session
	if (iState == CALL_STATE_TALKING)
	{
		if (!_pActiveLcb->bQuelch)
		{
			iax2_send_quelch();
			_pActiveLcb->bQuelch = TRUE;
		}
	}
	else
	{
		if ((iState == CALL_STATE_DIALING && _bDigitDialing) || iState == CALL_STATE_CALLING)
		{
			iax2_hangup(IAX_CAUSE_NORMAL_CLEARING);
		}
		iax2_ln_free(_pActiveLcb);
	}
	
	// Active another call session
	_pActiveLcb = iax2_ln_alt();
	iState = _pActiveLcb->iState;
	_pCurScb = _pActiveLcb->pScb;
	if (iState == CALL_STATE_FREE)
	{
		// Give user dial tone to start another call
		iax2_dial();
	}
	else if (iState == CALL_STATE_TALKING)
	{
		iax2_unhold();
	}
	else if (iState == CALL_STATE_RINGING)
	{
		iax2_answer();
	}
}

void Iax2Transfer()
{
	IAX2_LCB_HANDLE pAltLcb;
	IAX2_SCB_HANDLE pAltScb;
	UCHAR pXferID[IAX_XFID_LEN];

	if (_pActiveLcb->iState != CALL_STATE_TALKING)	return;

	pAltLcb = iax2_ln_alt();
	if (pAltLcb->iState == CALL_STATE_TALKING)
	{
		// Attended transfer
		rand_array(pXferID, IAX_XFID_LEN);

		_pCurScb = _pActiveLcb->pScb;
		pAltScb = pAltLcb->pScb;
		memcpy4(_pCurScb->pXferID, pXferID);
		_pCurScb->iXferState = TRANSFER_BEGIN;
		memcpy4(_pCurScb->pXferDstIP, pAltScb->pDstIP);
		_pCurScb->sXferDstPort = pAltScb->sDstPort;
		_pCurScb->sXferCallNo = pAltScb->sDstCallNo;
		iax2_send_txreq();

		_pCurScb = pAltLcb->pScb;
		pAltScb = _pActiveLcb->pScb;
		memcpy4(_pCurScb->pXferID, pXferID);
		_pCurScb->iXferState = TRANSFER_BEGIN;
		memcpy4(_pCurScb->pXferDstIP, pAltScb->pDstIP);
		_pCurScb->sXferDstPort = pAltScb->sDstPort;
		_pCurScb->sXferCallNo = pAltScb->sDstCallNo;
		iax2_send_txreq();
	}
	else if (pAltLcb->iState == CALL_STATE_FREE)
	{
		// First hold the current active call session
		if (!_pActiveLcb->bQuelch)
		{
			_pCurScb = _pActiveLcb->pScb;
			iax2_send_quelch();
			_pActiveLcb->bQuelch = TRUE;
		}

		_pActiveLcb = pAltLcb;
		_pActiveLcb->bXferor = TRUE;
		_pActiveLcb->iState = CALL_STATE_DIALING;
		TaskUIHandler(UI_CALL_DIAL, 0);
	}
}

void Iax2Dnd()
{
	_pCurScb = _pActiveLcb->pScb;
	iax2_hangup(IAX_CAUSE_NO_ANSWER);
	TaskCallLogAdd(_pActiveLcb->pPeerName, _pActiveLcb->pPeerNumber, CALL_LOG_TYPE_MISSED);
	iax2_ln_free(_pActiveLcb);
	iax2_check_altline();
}

void Iax2SendKey()
{
	UCHAR iKey, iDtmfCode;

	iKey = _pActiveLcb->pKeyOut[_pActiveLcb->iKeyIndex];
	if (iKey && iKey != (UCHAR)'T')
	{
		iDtmfCode = ascii2keyid(iKey);
		if (iDtmfCode == MAP_KEY_UNKNOWN)	
		{
			return;
		}
		_pCurScb = _pActiveLcb->pScb;
		TaskUIHandler(UI_CALL_PLAYKEY, (USHORT)iKey);
		iax2_send_dtmf(iKey);
		_pActiveLcb->bSendKey = TRUE;
		_pActiveLcb->sSendKeyLen = 0;
		_pActiveLcb->iKeyIndex ++;
	}
}

void Iax2HandleEvent(UCHAR iEvent, USHORT sParam)
{
	PCHAR p;
	UCHAR iState;

	iState = _pActiveLcb->iState;
	switch (iEvent)
	{
	case UI_EVENT_LOGON:
		Iax2Startup();
		Iax2Logon();
		break;

	case UI_EVENT_HANDSET_UP:
		Iax2HandsetUp((BOOLEAN)sParam);
		break;

	case UI_EVENT_HANDSET_DOWN:
		Iax2HandsetDown();
		break;

	case UI_EVENT_FLASH:
		Iax2Flash();
		break;

	case UI_EVENT_HOLD:
		if (sParam)
		{
			Iax2Unhold();
		}
		else
		{
			Iax2Hold();
		}
		break;

	case UI_EVENT_CALL:
		if (iState == CALL_STATE_DIALING)
		{
			Iax2Call((PCHAR)sParam);
		}
		break;

	case UI_EVENT_MEM_CALL:
		if (iState == CALL_STATE_DIALING)
		{
			p = strchr((PCHAR)sParam, 'T');
			if (p)
			{
				strcpy(_pActiveLcb->pKeyOut, p);
				_pActiveLcb->iKeyIndex = 0;
				*p = 0;
			}			
			if (!_pActiveLcb->bMemCall)
			{
				TaskUIHandler(UI_CALL_PLAYKEYS, sParam);
				Iax2Call((PCHAR)sParam);
			}
			else
			{
				strcpy(_pActiveLcb->pPeerNumber, (PCHAR)sParam);
			}
		}
		else if (iState == CALL_STATE_TALKING)
		{
			strcpy(_pActiveLcb->pKeyOut, (PCHAR)sParam);
			_pActiveLcb->iKeyIndex = 0;
			Iax2SendKey();
		}
		break;

	case UI_EVENT_TRANSFER:
		Iax2Transfer();
		break;

	case UI_EVENT_DIGIT:
		if (iState == CALL_STATE_TALKING)
		{
			_pActiveLcb->pKeyOut[0] = (UCHAR)sParam;
			_pActiveLcb->pKeyOut[1] = 0;
			_pActiveLcb->iKeyIndex = 0;
			Iax2SendKey();
		}
		else 
		{
			TaskUIHandler(UI_CALL_PLAYKEY, (UCHAR)sParam);
			if (iState == CALL_STATE_DIALING)
			{
				Iax2CollectKey((UCHAR)sParam);
			}
		}
		break;

	case UI_EVENT_CANCEL:
		if (iState == CALL_STATE_DIALING)
		{
			Iax2KeyCanceled();
		}
		else if (iState == CALL_STATE_RINGING)
		{
			Iax2Dnd();
		}
		break;

	case UI_EVENT_MUTE_DND:
		if (iState == CALL_STATE_RINGING)
		{
			Iax2Dnd();
		}
		break;

	}
}

USHORT Iax2GetCallInfo(UCHAR iType)
{
	switch (iType)
	{
	case CALL_INFO_STATE:
		return _pActiveLcb->iState;

	case CALL_INFO_IS_CALLEE:
		return _pActiveLcb->bCallee;

	case CALL_INFO_PEER_NAME:
		return (USHORT)(_pActiveLcb->pPeerName);

	case CALL_INFO_PEER_NUMBER:
		return (USHORT)(_pActiveLcb->pPeerNumber);

	case CALL_INFO_CALL_TIME:
		return (USHORT)(&_pActiveLcb->ct);
	}
	return 0;
}

void Iax2Run(UDP_SOCKET pUcb)
{
	USHORT sDCallNo, sSCallNo;
	UCHAR iCommand, iType;
	PCHAR pBuf;

	if (pUcb != _pIax2Socket)		return;

//	Sys_sDebug ++;

	pBuf = Adapter_pUdpBuf;

	sSCallNo = PCHAR2USHORT((PCHAR)(pBuf + IAX_FULLFR_SCALLNO));
	if (sSCallNo & IAX_FLAG_FULL)
	{
		if (pUcb->sLen < IAX_FULLFR_DATA)
		{
			UdpDebugString("Packet too short");
			return;
		}
		sDCallNo = PCHAR2USHORT((PCHAR)(pBuf + IAX_FULLFR_DCALLNO));
		sDCallNo &= ~IAX_FLAG_RETRANS;
		iType = pBuf[IAX_FULLFR_TYPE];
		iCommand = pBuf[IAX_FULLFR_CSUB];
		if (!sDCallNo)
		{
			if (iType != AST_FRAME_IAX || (iCommand != IAX_COMMAND_POKE && iCommand != IAX_COMMAND_NEW))
			{
				UdpDebugString("Unsupported command");
				return;
			}
		}
		sSCallNo &= ~IAX_FLAG_FULL;
		_pCurScb = iax2_ss_find(sSCallNo, sDCallNo, TRUE);
		if (_pCurScb)
		{
			memcpy4(Iax2_pTimeStamp, (PCHAR)(pBuf + IAX_FULLFR_TSTAMP));
			Iax2_iISeqNo = pBuf[IAX_FULLFR_ISEQNO];
			iax2_recv_packet((PCHAR)(pBuf + IAX_FULLFR_DATA), (USHORT)(pUcb->sLen - IAX_FULLFR_DATA), iType, iCommand, Iax2_iISeqNo, pBuf[IAX_FULLFR_OSEQNO]);
		}
	}
	else
	{
		_pCurScb = iax2_ss_find(sSCallNo, 0, FALSE);
		if (_pCurScb)
		{
			if (pUcb->sLen > IAX_MINIFR_DATA)
			{
				Iax2_pTimeStamp[2] = pBuf[IAX_MINIFR_TSTAMP];
				Iax2_pTimeStamp[3] = pBuf[IAX_MINIFR_TSTAMP+1];
				pUcb->sLen -= IAX_MINIFR_DATA;
				pBuf += IAX_MINIFR_DATA;
				iax2_handle_voice(pUcb->sLen, pBuf);
			}
		}
	}
}

void Iax2Timer()
{
	IAX2_LCB_HANDLE pLcb;
	IAX2_LCB_HANDLE pAltLcb;
	UCHAR i, iState, iKey, iDialTO;

	iax2_ss_timer();
	iax2_fr_timer();

	if (Sys_bRegister)
	{
		if (_iRegState == RS_FREE || _iRegState == RS_REFRESHING || _iRegState == RS_REGISTERING)
		{
			_sRegTimer ++;
			if(_sRegTimer >= IAX_DEFAULT_REG_EXPIRE)
			{
				_sRegTimer = 0;
				Iax2Logon();
			}
		}
		else if (_iRegState == RS_REGISTERED)
		{
			_sRegTimer ++;
			if(_sRegTimer >= _sRegTimeOut)
			{
				_iRegState = RS_REFRESHING;
				iax2_reg();
			}
		}
	}

	pLcb = _pIax2LCB;
	for (i = 0; i < LCB_TSIZE; i ++, pLcb ++)
	{
		iState = pLcb->iState;
		if (iState == CALL_STATE_RINGING)
		{
			pLcb->iTimer ++;
			if (Sys_iNoanswerTimeout && (pLcb->iTimer > Sys_iNoanswerTimeout))
			{
				pLcb->iTimer = 0;
				_pCurScb = pLcb->pScb;
				if (Sys_bAutoAnswer)
				{
					if (pLcb == _pActiveLcb)
					{
						TaskUIHandler(UI_CALL_AUTOANSWER, 0); 
						iax2_answer();
					}
				}
				else
				{
					if (Sys_bFwdNoanswer)
					{
						iax2_fwd();
					}
					else
					{
						iax2_hangup(IAX_CAUSE_NO_ANSWER);
					}
					TaskCallLogAdd(pLcb->pPeerName, pLcb->pPeerNumber, CALL_LOG_TYPE_MISSED);
					iax2_ln_free(pLcb);
					if (pLcb == _pActiveLcb)
					{
						iax2_check_altline();
					}
				}
			}
		}
		else if (iState == CALL_STATE_TALKING)
		{
			CallTimeTick(&pLcb->ct);
			if (pLcb == _pActiveLcb)
			{
				pAltLcb = iax2_ln_alt();
				if (!pLcb->bQuelch)
				{
					if (pAltLcb->iState != CALL_STATE_RINGING)
					{
						TaskUIHandler(UI_CALL_TIMER, (USHORT)&pLcb->ct);
					}
					else
					{
						if (mod_16x8(pAltLcb->iTimer, 10) == 9)
						{
							TaskUIHandler(UI_CALL_WAITING, 0);
						}
					}
				}
				iKey = pLcb->pKeyOut[pLcb->iKeyIndex];
				if (iKey)
				{
					if (iKey == (UCHAR)'T')
					{
						pLcb->iTimer ++;
						if (pLcb->iTimer >= 4)
						{
							pLcb->iTimer = 0;
							pLcb->iKeyIndex ++;
							Iax2SendKey();
						}
					}
				}
			}
		}
		else if (iState == CALL_STATE_DIALING)
		{
			if (pLcb == _pActiveLcb)
			{
				pLcb->iTimer ++;
				if (pLcb->bMemCall)
				{
					if (pLcb->iTimer >= 2)
					{
						pLcb->iTimer = 0;
						pLcb->bMemCall = FALSE;
						TaskUIHandler(UI_CALL_PLAYKEYS, (USHORT)pLcb->pPeerNumber);
						Iax2Call(pLcb->pPeerNumber);
					}
				}
				else
				{
					if (strlen(pLcb->pPeerNumber))
					{
						iDialTO = Sys_bUseDigitmap ? Sys_iDigitmapTimeout : DIAL_TIMEOUT_END;
						if (pLcb->iTimer > iDialTO)
						{
							Iax2CollectKey('T');
						}
					}
					else
					{
						if (pLcb->iTimer >= DIAL_TIMEOUT_START)
						{
							pLcb->iTimer = 0;
							iax2_call_failed(CALL_END_EMPTY_NUMBER);
						}
					}
				}
			}
		}
		else if (iState == CALL_STATE_HANGUP)
		{
			pLcb->iTimer ++;
			if (pLcb->iTimer >= HANGUP_TIMEOUT)
			{
				iax2_ln_free(pLcb);
				if (pLcb == _pActiveLcb)
				{
					TaskUIHandler(UI_CALL_HANGUP, 0);
				}
			}
		}
	}
}

#endif


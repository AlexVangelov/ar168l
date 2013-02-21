/*-------------------------------------------------------------------------
   AR1688 SIP function copy right information

   Copyright (C) 2006-2012. Tang, Li      <tangli@palmmicro.com>
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

#include "version.h"
#include "type.h"

#ifdef CALL_SIP

#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "sip.h"
#include "sips.h"
#include "bank2.h"
#include "bank5.h"

#define ALPHANUM_NUM	62
const UCHAR _cAlphaNum[ALPHANUM_NUM] = {
'0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 
'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 
'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
'U', 'V', 'W', 'X', 'Y', 'Z'
};

const UCHAR _cVia[] = "Via: ";
const UCHAR _cFrom[] = "From: ";
const UCHAR _cTo[] = "To: ";
const UCHAR _cCSeq[] = "CSeq: ";
const UCHAR _cCall_ID[] = "Call-ID: ";
const UCHAR _cContact[] = "Contact: ";
const UCHAR _cContent_Length[] = "Content-Length: ";
const UCHAR _cContent_Type[] = "Content-Type: ";
const UCHAR _cExpires[] = "Expires: ";
const UCHAR _cRoute[] = "Route: ";
const UCHAR _cRecord_Route[] = "Record-Route: ";
const UCHAR _cAuthorization[] = "Authorization: ";
const UCHAR _cProxy_Authorization[] = "Proxy-Authorization: ";
const UCHAR _cRAck[] = "RAck: ";
const UCHAR _cSupported[] = "Supported: ";
const UCHAR _cRequire[] = "Require: ";
const UCHAR _cRSeq[] = "RSeq: ";
const UCHAR _cAllow[] = "Allow: ";
const UCHAR _cAccept[] = "Accept: ";
const UCHAR _cReferTo[] = "Refer-To: ";
const UCHAR _cReferredBy[] = "Referred-By: ";
const UCHAR _cReplaces[] = "Replaces: ";
const UCHAR _cSession_Expires[] = "Session-Expires: ";
const UCHAR _cMin_SE[] = "Min-SE: ";
const UCHAR _cEvent[] = "Event: ";

const UCHAR _cTokenMagic[] = "z9hG4bK";

const UCHAR _cAppSdp[] = "application/sdp";
const UCHAR _cAppDtmf[] = "application/dtmf-relay";

const UCHAR _cSend[] = "Send";
const UCHAR _cRtpmap[] = "a=rtpmap:";

#define RESERVED_CHAR_NUM	10
const UCHAR _cReserved[RESERVED_CHAR_NUM] = {';', '/', '?', ':', '@', '&', '=', '+', '$', ','};

const UCHAR _cSipUdp[] = "_sip._udp.";

void SipRun(UDP_SOCKET pUcb); // function in sipr.c

void SipSendResp(USHORT sResp);
void sip_send_notify(USHORT sResp);
void sip_send_bye();
void SipUnhold();
BOOLEAN SipXfereeDone(USHORT sResp);

UCHAR Sys_iNatTraversal;
UCHAR Sys_iKeepAlive;
BOOLEAN Sys_bDnsSrv;
BOOLEAN Sys_bSubscribeMwi;

#ifdef OEM_IP20
extern UCHAR hotline;

extern UCHAR TransferFlag ;
extern UCHAR TransFlagResp ;
extern UCHAR voice_message ;

extern UDP_SOCKET _pSyslogSocket;
#endif

#define MAX_SIP_SERVER	3		//was 2 fv
UCHAR Sip_iServerIndex;
UCHAR Sip_iServerNum;
T_DNSSRV_ANS Sip_tServerInfo[MAX_SIP_SERVER];

/**********************************************************************
 ** message compose functions                                        **
 **********************************************************************
 */
BOOLEAN sip_is_reserved(UCHAR iVal)
{
	UCHAR i;

	for (i = 0; i < RESERVED_CHAR_NUM; i ++)
	{
		if (iVal == _cReserved[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}

void sip_escape_str(PCHAR pDstStr, PCHAR pSrcStr)
{
	PCHAR pDst;
	PCHAR pSrc;
	UCHAR iVal;
	UCHAR pEscaped[4];

	pSrc = pSrcStr;
	pDst = pDstStr;
	pEscaped[0] = '%';
	pEscaped[3] = 0;
	while (*pSrc)
	{
		iVal = *pSrc;
		if (sip_is_reserved(iVal))
		{
			pEscaped[1] = char2asc((UCHAR)(iVal >> 4), FALSE);
			pEscaped[2] = char2asc(iVal, FALSE);
			strcpy(pDst, pEscaped);
			pDst += 3;
		}
		else
		{
			*pDst = iVal;
			pDst ++;
		}
		pSrc ++;
	}
	*pDst = 0;
}


PCHAR sip_get_respcode(USHORT sResp)
{
	switch (sResp)
	{
	case 180:
		return (PCHAR)("Ringing");
	case 200:
		return (PCHAR)("OK");
	case 202:
		return (PCHAR)("Accepted");
	case 301:
		return (PCHAR)("Moved Permanently");
	case 302:
		return (PCHAR)("Moved Temporarily");
	case 400:
		return (PCHAR)("Bad Request");
	case 410:
		return (PCHAR)("Gone");
	case 415:
		return (PCHAR)("Unsupported Media Type");
	case 420:
		return (PCHAR)("Bad Extensions");
	case 480:
		return (PCHAR)("Temporarily Unavailable");
	case 481:
		return (PCHAR)("Call Doesn't Exist");
	case 482:
		return (PCHAR)("Loop Detected");
	case 486:
		return (PCHAR)("Busy Here");
	case 487:
		return (PCHAR)("Request Terminated");
	case 488:
		return (PCHAR)("Not Acceptable");
	case 603:
		return (PCHAR)("Declined");
	default:
		return (PCHAR)" ";
	}
}

void sip_new_token(PCHAR pDst, UCHAR iLen)
{
	UCHAR i, index;

	for (i = 0; i < iLen; i ++)
	{
		index = mod_16x8((USHORT)rand(), ALPHANUM_NUM);
		*pDst = _cAlphaNum[index];
		pDst ++;
	}
	*pDst = 0;
}

void sip_new_branch(PCHAR pBranch)
{
	strcpy(pBranch, _cTokenMagic);
	sip_new_token((PCHAR)(pBranch + 7), (UCHAR)(MAX_BRANCH_LEN - 7));
}

void sip_new_callid(PCHAR pCallId)
{
	sip_new_token(pCallId, 16);
	pCallId[16] = '@';
	ip2str(Sys_pIpAddress, (PCHAR)(pCallId + 17));
}

void sip_add_req_line(UCHAR iMethod, PCHAR pUri)
{
	line_start(Adapter_pUdpBuf);
	line_add_str_with_char(_cRequest[iMethod], ' ');
	line_add_str_with_char(pUri, ' ');
	line_add_str_with_crlf(_cTokenSipVer);
}

void sip_add_status_line(USHORT sResp)
{
	line_add_str_with_char(_cTokenSipVer, ' ');
	line_add_short(sResp);
	line_add_char(' ');
	line_add_str_with_crlf(sip_get_respcode(sResp));
}

void sip_add_token(PCHAR pToken, PCHAR pValue)
{
	if (pValue)
	{
		line_add_char(';');
		line_add_str(pToken);
		line_add_str(pValue);
	}
}

void sip_add_via(PCHAR pBranch)
{
	line_add_str(_cVia);
	line_add_str(_cTokenSipVer);
	line_add_str("/UDP ");
	line_add_ip(Sip_pNatIP);
	line_add_char(':');
	line_add_short(Sip_sNatPort);
	sip_add_token(_cTokenBranch, pBranch);
	line_add_str_with_crlf(";rport");
}

void sip_add_local_uri(BOOLEAN bIP, BOOLEAN bPort)
{
	if (strlen(Sys_pUserName))
	{
		line_add_char('"');
		line_add_str(Sys_pUserName);
		line_add_str("\" ");
	}
	line_add_char('<');
	line_add_str(_cTokenSip);
	if (strlen(Sys_pUserNumber))
	{
		line_add_str_with_char(Sys_pUserNumber, '@');
	}
	if (bIP)
	{
		line_add_ip(Sip_pNatIP);
	}
	else
	{
		line_add_str(Sys_pSipDomain);
	}
	if (bPort)
	{
		line_add_char(':');
		line_add_short(Sip_sNatPort);
	}
	line_add_char('>');
}

void sip_add_to(PCHAR pUri, PCHAR pTag)
{
	line_add_str_with_char(_cTo, '<');
	line_add_str_with_char(pUri, '>');
	sip_add_token(_cTokenTag, pTag);
	line_add_crlf();
}

void sip_add_call_id(PCHAR pCallId)
{
	line_add_str(_cCall_ID);
	line_add_str_with_crlf(pCallId);
}

void sip_add_contact()
{
	line_add_str_with_char(_cContact, '<');
	line_add_str(_cTokenSip);
	if (strlen(Sys_pUserNumber))
	{
		line_add_str_with_char(Sys_pUserNumber, '@');
	}
	line_add_ip(Sip_pNatIP);
	line_add_char(':');
	line_add_short(Sip_sNatPort);
	line_add_char('>');
	line_add_crlf();
}

void sip_add_fwdcontact()
{
	line_add_str_with_char(_cContact, '<');
	line_add_str(_cTokenSip);
	line_add_options_str(OPT_FWD_NUMBER, MAX_USER_NUMBER_LEN);
	if (Sys_bRegister)
	{
		line_add_char('@');
		line_add_str(Sys_pSipDomain);
	}
	line_add_char('>');
	line_add_crlf();
}

void sip_add_referredby()
{
	line_add_str(_cReferredBy);
	sip_add_local_uri(!Sys_bRegister, FALSE);
	line_add_crlf();
}

void sip_add_cseq(ULONG lSeq, UCHAR iMethod)
{
	line_add_str(_cCSeq);
	line_add_long(lSeq);
	line_add_char(' ');
	line_add_str_with_crlf(_cRequest[iMethod]);
}

void sip_add_content_length(USHORT sLength)
{
	line_add_str(_cContent_Length);
	line_add_short(sLength);
	line_add_crlf();
}

void sip_add_content_type(PCHAR pType)
{
	line_add_str(_cContent_Type);
	line_add_str_with_crlf(pType);
}

void sip_add_supported()
{
	line_add_str(_cSupported);
	if (Sys_bPrack)
	{
		line_add_str_with_char(_c100rel, ',');
	}
	line_add_str_with_crlf("replaces,timer");
}

void sip_add_allow()
{
	UCHAR i;

	line_add_str(_cAllow);
	for (i = 0; i < SIP_REQ_NUMBER-1; i ++)
	{
		line_add_str_with_char(_cRequest[i], ',');
	}
	line_add_str_with_crlf(_cRequest[i]);
}

void sip_add_accept()
{
	line_add_str(_cAccept);
	line_add_str_with_char(_cAppSdp, ',');
	line_add_str_with_char(_cMsgFrag, ',');
	line_add_str_with_char(_cAppDtmf, ',');
	line_add_str_with_crlf(_cTextPlain);
}

void sip_add_require(PCHAR pTag)
{
	line_add_str(_cRequire);
	line_add_str_with_crlf(pTag);
}

void sip_add_rseq(ULONG lSeq)
{
	line_add_str(_cRSeq);
	line_add_long(lSeq);
	line_add_crlf();
}

void sip_add_max_forwards()
{
	line_add_str_with_crlf("Max-Forwards: 70");
}

void sip_add_proxy_require()
{
	if (strlen(Sys_pProxyRequire))
	{
		line_add_str("Proxy-Require: ");
		line_add_str_with_crlf(Sys_pProxyRequire);
	}
}

void sip_add_user_agent()
{
#ifndef OEM_IP20
	UCHAR pBuf[32];
#endif

	line_add_str("User-Agent: ");
#ifdef OEM_IP20
	line_add_str("IP20_AAT");
#else
	TaskLoadString(IDS_DISPLAY_MODEL, pBuf);
	line_add_str_with_char(pBuf, ' ');
	TaskLoadString(IDS_VERSION, pBuf);
	line_add_str((PCHAR)(pBuf+5));
#endif
	line_add_crlf();
}

void sip_add_expires(USHORT sVal)
{
	line_add_str(_cExpires);
	line_add_short(sVal);
	line_add_crlf();
}

void sip_add_session_expires(SIP_LCB_HANDLE pLcb)
{
	if (pLcb->lSessionExpires)
	{
		line_add_str(_cSession_Expires);
		line_add_long(pLcb->lSessionExpires);
		if (pLcb->iRefresher)
		{
			line_add_str(";refresher=");
			line_add_str((pLcb->iRefresher == SIP_REFRESHER_UAC) ? "uac" : "uas");
		}
		line_add_crlf();
	}
	if (pLcb->lMinSE)
	{
		line_add_str(_cMin_SE);
		line_add_long(pLcb->lMinSE);
		line_add_crlf();
	}
}

void sip_add_header(PCHAR pName, PCHAR pHeapBuf)
{
	if (pHeapBuf)
	{
		line_add_str(pName);
		line_add_str_with_crlf(pHeapBuf);
	}
}

void sip_new_random(PCHAR pBuf, UCHAR iLen)
{
	UCHAR i;
	PCHAR pDst;

	pDst = pBuf;
	for (i = 0; i < iLen; i ++)
	{
		*pDst = mod_16x8((USHORT)rand(), 10) + '0';
		pDst ++;
	}
	*pDst = 0;
}

/**********************************************************************
 ** SDP functions                                                 **
 **********************************************************************
 */
// Section 6 Payload Type Definitions from rfc3551, for the encoding names. The names are to be sent in capitals. In order to better interoperate, your implementation may choose to accepting either case.
const UCHAR Rtp_cCoderName[VOICE_CODER_NUM][8] = {"PCMU", "PCMA", "G726-32", "GSM", "iLBC", "speex", "G729"};	
const UCHAR _cAudioTxMode[4] = {AUDIO_MODE_INACTIVE, AUDIO_MODE_SENDONLY, AUDIO_MODE_RECVONLY, AUDIO_MODE_SENDRECV};

void sip_add_fmtp(PCHAR pInfo, UCHAR iPayload)
{
	line_add_str("a=fmtp:");
	line_add_short(iPayload);
	line_add_char(' ');
	line_add_str_with_crlf(pInfo);
}

void sip_add_rtpmap(PCHAR pInfo, UCHAR iPayload)
{
	line_add_str(_cRtpmap);
	line_add_short(iPayload);
	line_add_char(' ');
	line_add_str(pInfo);
	line_add_str_with_crlf("/8000");
}

// This should be called before adding data to UdpSendBuf
USHORT sip_add_sdp(SIP_LCB_HANDLE pLcb)
{
	UCHAR i, iDtmfPayload, iVoiceCoder, iMode, iTemp;
	BOOLEAN bVad;
	USHORT sRtpPort;
	UCHAR pPayload[VOICE_CODER_NUM];
	UCHAR pCoder[VOICE_CODER_NUM];
	UCHAR pBuf[9];
	UCHAR iCoderNum;
	USHORT sLen;

	if (pLcb)
	{
		sRtpPort = pLcb->sRtpPort;
		bVad = pLcb->bVad;
		if (pLcb->iSdpType == SDP_TYPE_OFFER)
		{
			iVoiceCoder = VOICE_CODER_NONE;
			iMode = pLcb->iLocalMode;
		}
		else if (pLcb->iSdpType == SDP_TYPE_REINVITE)
		{
			iVoiceCoder = pLcb->iVoiceCoder;
			iMode = pLcb->iLocalMode;
		}
		else
		{
			iVoiceCoder = pLcb->iVoiceCoder;
			iMode = _cAudioTxMode[pLcb->iRemoteMode];
		}
	}
	else
	{
		iVoiceCoder = VOICE_CODER_NONE;
		iMode = AUDIO_MODE_SENDRECV;
		sRtpPort = Sip_sRtpPort;
		bVad = Sys_bOptVad;
	}
	iDtmfPayload = (Sys_iDtmfType == DTMF_TYPE_RFC2833) ? Sys_iDtmfPayload : 0;

	sLen = line_get_len();
	line_add_str("v=0\r\no=- ");

	if (pLcb)
	{
		line_add_str_with_char(pLcb->pSdpSessionId, ' ');
		line_add_long(pLcb->lSdpVersion);
		pLcb->lSdpVersion ++;
	}
	else
	{
		sip_new_random(pBuf, 8);
		line_add_str_with_char(pBuf, ' ');
		sip_new_random(pBuf, 8);
		line_add_str(pBuf);
	}
	line_add_str(" IN IP4 ");
	line_add_ip(Sip_pNatIP);

	line_add_str("\r\ns=SIP CALL\r\nc=IN IP4 ");
#ifdef OEM_BT2008
	// Siemens's switch doesn't support the old "0.0.0.0" hold scheme
	line_add_ip(Sip_pNatIP);
#else
	if (iMode & AUDIO_MODE_RECVONLY)
	{
		line_add_ip(Sip_pNatIP);
	}
	else
	{
		line_add_str("0.0.0.0");
	}
#endif

	line_add_str("\r\nt=0 0\r\nm=audio ");
	line_add_short(sRtpPort);
	line_add_str(" RTP/AVP");

	if (iVoiceCoder != VOICE_CODER_NONE)
	{
		pPayload[0] = RtpGetPayload(iVoiceCoder, pLcb);
		pCoder[0] = iVoiceCoder;
		iCoderNum = 1;
	}
	else
	{
		iCoderNum = 0;
		// this is a SDP offer
		for (i = 0; i < VOICE_CODER_NUM; i ++)
		{
			iTemp = Sys_pVoiceCoders[i];
			if (iTemp != VOICE_CODER_NONE && iTemp <= VOICE_CODER_NUM)
			{
				pPayload[iCoderNum] = RtpGetPayload(iTemp, pLcb);
				pCoder[iCoderNum] = iTemp;
				iCoderNum ++;
			}
		}
	}
	for (i = 0; i < iCoderNum; i ++)
	{
		line_add_char(' ');
		line_add_short(pPayload[i]);
	}
	if (iDtmfPayload)
	{
		line_add_char(' ');
		line_add_short(iDtmfPayload);
	}
	line_add_crlf();

	for (i = 0; i < iCoderNum; i ++)
	{
		iVoiceCoder = pCoder[i];
		iTemp = pPayload[i];
		sip_add_rtpmap(Rtp_cCoderName[iVoiceCoder-1], iTemp);
		if (iVoiceCoder == VOICE_CODER_G729)
		{
			if (!bVad)
			{
				sip_add_fmtp("annexb=no", iTemp);
			}
		}
		else if (iVoiceCoder == VOICE_CODER_ILBC)
		{
			sip_add_fmtp((Sys_iIlbcFrame) ? "mode=30" : "mode=20", iTemp);
		}
	}
	if (iDtmfPayload)
	{
		sip_add_rtpmap("telephone-event", iDtmfPayload);
		sip_add_fmtp("0-15", iDtmfPayload);
	}
	
	line_add_str("a=");
	line_add_str_with_crlf(_cAudioMode[iMode]);

	return line_get_len() - sLen;
}

/**********************************************************************
 ** transaction layer send functions                                 **
 **********************************************************************
 */
// transaction timer
#define TR_TIMER_A		2	// retransmit interval, for UDP only
#define TR_TIMER_B		11	// transaction timeout timer
#ifdef OEM_IP20
#define TR_TIMER_D		0
#else
#define TR_TIMER_D		4	// Wait time for message retransmits
#endif

void sip_tr_init()
{
	UCHAR i;
	SIP_TCB_HANDLE pTcb;

	pTcb = Sip_pReqTCB;
	for (i = 0; i < TR_TSIZE; i ++)
	{
		memset((PCHAR)pTcb, 0, SIP_TCB_LENGTH);
		pTcb ++;
	}
	pTcb = Sip_pRespTCB;
	for (i = 0; i < TR_TSIZE; i ++)
	{
		memset((PCHAR)pTcb, 0, SIP_TCB_LENGTH);
		pTcb ++;
	}
}

SIP_TCB_HANDLE sip_tr_new_req()
{
	UCHAR i;
	SIP_TCB_HANDLE pTcb;

	pTcb = Sip_pReqTCB;
	for (i = 0; i < TR_TSIZE; i ++)
	{
		if (pTcb->iState == TRS_FREE)
		{
			return pTcb;
		}
		pTcb ++;
	}

	UdpDebugString("No Free Req Transactions");
	return NULL;
}

void sip_tr_free_lcb(SIP_LCB_HANDLE pLcb)
{
	UCHAR i;
	SIP_TCB_HANDLE pTcb;
	
	pTcb = Sip_pReqTCB;
	for (i = 0; i < TR_TSIZE; i ++)
	{
		if (pTcb->iState != TRS_FREE && pTcb->pLcb == pLcb)
		{
			pTcb->pLcb = NULL;
		}
		pTcb ++;
	}
	pTcb = Sip_pRespTCB;
	for (i = 0; i < TR_TSIZE; i ++)
	{
		if (pTcb->iState != TRS_FREE && pTcb->pLcb == pLcb)
		{
			pTcb->pLcb = NULL;
		}
		pTcb ++;
	}
}

void sip_tr_send(PCHAR pDstIP, USHORT sDstPort, BOOLEAN bSave)
{
	USHORT sLen;
#ifdef OEM_IP20
	PCHAR pTempSys;
#endif

	memcpy4(Sip_pCurTcb->pDstIP, pDstIP);
	Sip_pCurTcb->sDstPort = sDstPort;

	sLen = line_get_len();
	if (bSave)
	{
		if (Sip_pCurTcb->pData)	
		{
			free(Sip_pCurTcb->pData);
			Sip_pCurTcb->pData = NULL;
		}
		Sip_pCurTcb->pData = heap_save_str(Adapter_pUdpBuf);
	}
	// send out packet
	UdpSendTo(Sip_pSocket, sLen, Sip_pCurTcb->pDstIP, Sip_pCurTcb->sDstPort);
//	SyslogAllUdpData(sLen);

#ifdef OEM_IP20
	pTempSys = heap_save_str(Adapter_pUdpBuf);		//added for syslog sip messaging
	line_start(Adapter_pUdpBuf);
	line_add_str("PPP");
	line_add_short(111);
	line_add_char(' ');
	line_add_str(pTempSys);		
	
	UdpSend(_pSyslogSocket, strlen(Adapter_pUdpBuf)+1);
	
	free(pTempSys);
	pTempSys = NULL;
#endif
}

void sip_tr_send_req(PCHAR pDstIP, USHORT sDstPort, PCHAR pBranch, UCHAR iMethod, SIP_LCB_HANDLE pLcb)
{
	Sip_pCurTcb = sip_tr_new_req();
	if (Sip_pCurTcb)
	{
		Sip_pCurTcb->iState = TRS_TRYING;
		Sip_pCurTcb->iMethod = iMethod;
		Sip_pCurTcb->pLcb = pLcb;
		Sip_pCurTcb->pBranch = heap_save_str(pBranch);

		sip_tr_send(pDstIP, sDstPort, TRUE);
	}
	else
	{
		UdpSendTo(Sip_pSocket, line_get_len(), pDstIP, sDstPort);
	}
}

void sip_tr_send_resp(PCHAR pDstIP, USHORT sDstPort, USHORT sResp)
{
	BOOLEAN bSave;
	UCHAR iMethod;

	bSave = TRUE;
	iMethod = Sip_pCurTcb->iMethod;
	if (iMethod == SIP_REQ_INVITE)
	{
		if (sResp >= 200)
		{
			Sip_pCurTcb->iState = TRS_PROCEEDING;
			Sip_pCurTcb->iTimer0 = 0;
		}
	}
	else
	{
		if (sResp < 200)	return;
		if (iMethod == SIP_REQ_OPTIONS)
		{
			bSave = FALSE;
		}
		Sip_pCurTcb->iState = TRS_COMPLETED;
	}

	sip_tr_send(pDstIP, sDstPort, bSave);
}

void sip_tr_retry(SIP_TCB_HANDLE pTcb)
{
	PCHAR pMsg;

	sip_debug("sip_tr_retry", pTcb->iMethod);
	pMsg = pTcb->pData;
	if (pMsg)
	{
		strcpy(Adapter_pUdpBuf, pMsg);
		UdpSendTo(Sip_pSocket, strlen(pMsg), pTcb->pDstIP, pTcb->sDstPort);
	}
}

void sip_tr_timer()
{
	UCHAR i, iState;
	SIP_TCB_HANDLE pTcb;

	pTcb = Sip_pReqTCB;
	for (i = 0; i < TR_TSIZE; i ++, pTcb ++)
	{
		switch (pTcb->iState)
		{
		case TRS_TRYING:
			pTcb->iTimer1 ++;
			if (pTcb->iTimer1 > TR_TIMER_B)
			{
				// Timer B fires, inform TU of transmission error ...
				sip_handle_error(pTcb->pLcb, pTcb->iMethod);
				sip_tr_free(pTcb);
				break;
			}

			pTcb->iTimer0 ++;
			if (pTcb->iTimer0 > TR_TIMER_A)
			{
				if (pTcb->iMethod == SIP_REQ_INVITE && !pTcb->pLcb)
				{
					sip_tr_free(pTcb);
				}
				else
				{
					sip_tr_retry(pTcb);
					pTcb->iTimer0 = 0;
				}
			}
			break;

		case TRS_COMPLETED:
			pTcb->iTimer2 ++;
			if (pTcb->iTimer2 > TR_TIMER_D)
			{
				sip_tr_free(pTcb);
			}
			break;
		}
	}

	pTcb = Sip_pRespTCB;
	for (i = 0; i < TR_TSIZE; i ++, pTcb ++)
	{
		iState = pTcb->iState;
		if (iState == TRS_FREE)	continue;
		if (iState == TRS_COMPLETED)
		{
			pTcb->iTimer2 ++;
			if (pTcb->iTimer2 > TR_TIMER_D)
			{
				sip_tr_free(pTcb);
			}
		}
		else
		{
			if (pTcb->iMethod == SIP_REQ_INVITE)
			{
				if (iState == TRS_TRYING)
				{
					if (pTcb->b100rel)
					{
						pTcb->iTimer0 ++;
						if (pTcb->iTimer0 > TR_TIMER_A)
						{
							sip_tr_retry(pTcb);
							pTcb->iTimer0 = 0;
						}
					}
				}
				else
				{
					pTcb->iTimer1 ++;
					if (pTcb->iTimer1 > TR_TIMER_B)
					{
						sip_tr_free(pTcb);
						break;
					}

					pTcb->iTimer0 ++;
					if (pTcb->iTimer0 > TR_TIMER_A)
					{
						sip_tr_retry(pTcb);
						pTcb->iTimer0 = 0;
					}
				}
			}
		}
	}
}

/**********************************************************************
 ** Subscribe functions                                                **
 **********************************************************************/

void sip_sub_init()
{
	Sip_bSubscribe = FALSE;
	Sip_pSubReqUri = NULL;
	Sip_pSubCallId = NULL;
	Sip_pSubFromTag = NULL;
	Sip_pSubToTag = NULL;
	Sip_lSubSeq = 1;
	Sip_sSubTimer = 0;
	Sip_sSubTimeout = SIP_DEFAULT_SUB_EXPIRE;
	Sip_iSubAuthRetry = 0;
	Sip_pSubWWWAuth = NULL;
	Sip_pSubProxyAuth = NULL;
}

void sip_sub_free()
{
	if (Sip_pSubReqUri)		free(Sip_pSubReqUri);
	if (Sip_pSubCallId)		free(Sip_pSubCallId);
	if (Sip_pSubFromTag)	free(Sip_pSubFromTag);
	if (Sip_pSubToTag)		free(Sip_pSubToTag);
	if (Sip_pSubWWWAuth)	free(Sip_pSubWWWAuth);
	if (Sip_pSubProxyAuth)	free(Sip_pSubProxyAuth);
}

void sip_sub_failed()
{
	sip_sub_free();
	sip_sub_init();
	Sip_bSubscribe = Sys_bSubscribeMwi;
}

void sip_add_from_to(PCHAR pHeader, PCHAR pTag)
{
	line_add_str(pHeader);
	sip_add_local_uri(FALSE, FALSE);
	sip_add_token(_cTokenTag, pTag);
	line_add_crlf();
}

void sip_send_subscribe()
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	sip_debug(_cSend, SIP_REQ_SUBSCRIBE);

	sip_add_req_line(SIP_REQ_SUBSCRIBE, Sip_pSubReqUri);

	sip_new_branch(pBranch);
	sip_add_via(pBranch);

	sip_add_from_to(_cFrom, Sip_pSubFromTag);
	sip_add_from_to(_cTo, Sip_pSubToTag);
	
	sip_add_call_id(Sip_pSubCallId);
	sip_add_cseq(Sip_lSubSeq, SIP_REQ_SUBSCRIBE);
	sip_add_contact();
	sip_add_expires(Sip_sSubTimeout);
	sip_add_max_forwards();
	sip_add_user_agent();
	sip_add_proxy_require();
	sip_add_header(_cAuthorization, Sip_pSubWWWAuth);
	sip_add_header(_cProxy_Authorization, Sip_pSubProxyAuth);
	sip_add_header(_cEvent, "message-summary");
	sip_add_content_length(0);
	line_add_crlf();

	sip_tr_send_req(Sip_pServerIP, Sip_sServerPort, pBranch, SIP_REQ_SUBSCRIBE, NULL);
	Sip_lSubSeq ++;
}

void sip_sub_start()
{
	UCHAR pBuf[MAX_DNS_LEN+MAX_DNS_LEN];

	Sip_sSubTimer = 0;
	Sip_sSubTimeout = SIP_DEFAULT_SUB_EXPIRE;
	if (!Sip_pSubReqUri)
	{
		line_start(pBuf);
		line_add_str(_cTokenSip);
		line_add_str_with_char(Sys_pUserNumber, '@');
		line_add_str(Sys_pSipDomain);
		Sip_pSubReqUri = heap_save_str(pBuf);
	}

	if (!Sip_pSubCallId)
	{
		Sip_pSubCallId = (char *)malloc(MAX_CALL_ID_LEN+1);
		sip_new_callid(Sip_pSubCallId);
	}
	if (!Sip_pSubFromTag)
	{
		Sip_pSubFromTag = (char *)malloc(MAX_TAG_LEN+1);
		sip_new_token(Sip_pSubFromTag, MAX_TAG_LEN);
	}
	sip_send_subscribe();
}

void sip_sub_timer()
{
	if (Sip_bSubscribe)
	{
		Sip_sSubTimer ++;
		if (Sip_sSubTimer >= Sip_sSubTimeout)
		{
			sip_sub_start();
		}
	}
}

/**********************************************************************
 ** LOGIN functions                                                  **
 **********************************************************************/

void sip_logon_init()
{
	Sip_iRegState = RS_FREE;
	Sip_sRegTimer = 0;
	Sip_sRegTimeout = 0;
	Sip_pRegWWWAuth = NULL;
	Sip_pRegProxyAuth = NULL;
	Sip_iRegAuthRetry = 0;
	Sip_iNatTimer = 0;
	Sip_lRegSeq = 0;
	Sip_pRegCallId[0] = 0;
	Sip_pRegFromTag[0] = 0;
}

void sip_logon_free()
{
	if (Sip_pRegWWWAuth)
	{
		free(Sip_pRegWWWAuth);
		Sip_pRegWWWAuth = NULL;
	}
	if (Sip_pRegProxyAuth)
	{
		free(Sip_pRegProxyAuth);
		Sip_pRegProxyAuth = NULL;
	}
}

void sip_logon_failed()
{
/*
//fv
	UCHAR i;

        UdpDebugString("sip_logon_failed()");
	for (i = 0; i < Sip_iServerNum; i ++)
	{
		UdpDebugString(Sip_tServerInfo[i].pcTarget);
	}
//fv
	UdpDebugVal(Sip_iServerNum, 10);
	UdpDebugVal(Sip_iServerIndex, 10);
	UdpDebugString(Sip_tServerInfo[Sip_iServerIndex].pcTarget);
*/
	if (Sip_iServerNum && Sip_iServerIndex < Sip_iServerNum)
	{
		Sip_sServerPort = Sip_tServerInfo[Sip_iServerIndex].sPort;
		TaskDnsQuery(Sip_tServerInfo[Sip_iServerIndex].pcTarget, DNS_TYPE_A, PROTOCOL_BANK_OFFSET, (USHORT)SipLogonStep2);
		Sip_iServerIndex ++;
	}
	else
	{
		sip_logon_free();
		Sip_sRegTimer = 0;
		Sip_iRegAuthRetry = 0;
		Sip_iRegState = RS_FREE;
		TaskUIHandler(UI_LOGON_FAILED, 0);
	}
}

void sip_logon_ok()
{
	sip_logon_free();
	Sip_iRegState = RS_REGISTERED;
	Sip_sRegTimer = 0;
	Sip_iRegAuthRetry = 0;
	TaskUIHandler(UI_LOGON_OK, 0);
}

void sip_send_register()
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];

	sip_debug(_cSend, SIP_REQ_REGISTER);

	line_start(Adapter_pUdpBuf);
	line_add_str("REGISTER sip:");
	line_add_str_with_char(Sys_pSipDomain, ' ');
	line_add_str_with_crlf(_cTokenSipVer);

	sip_new_branch(pBranch);
	sip_add_via(pBranch);

	sip_add_from_to(_cFrom, Sip_pRegFromTag);
	sip_add_from_to(_cTo, NULL);

	sip_add_call_id(Sip_pRegCallId);
	sip_add_cseq(Sip_lRegSeq, SIP_REQ_REGISTER);
	sip_add_contact();
	sip_add_expires(Sip_sRegTimeout);
	sip_add_max_forwards();
	sip_add_user_agent();
	sip_add_proxy_require();
	sip_add_header(_cAuthorization, Sip_pRegWWWAuth);
	sip_add_header(_cProxy_Authorization, Sip_pRegProxyAuth);
	sip_add_content_length(0);
	line_add_crlf();

	sip_tr_send_req(Sip_pServerIP, Sip_sServerPort, pBranch, SIP_REQ_REGISTER, NULL);
	Sip_lRegSeq ++;
}

void sip_logon_timer()
{
	USHORT sRegTTL;

	if (Sip_iRegState == RS_FREE)
	{
		Sip_sRegTimer ++;
		if(Sip_sRegTimer >= SIP_DEFAULT_REG_EXPIRE)
		{
			Sip_sRegTimer = 0;
			SipLogon();
		}
	}
	else if (Sip_iRegState == RS_REGISTERED)
	{
		sRegTTL = (Sip_sRegTimeout >> 2) + (Sip_sRegTimeout >> 1);
		Sip_sRegTimer ++;
		if(Sip_sRegTimer >= sRegTTL)
		{
			Sip_iRegState = RS_REFRESHING;
			Sip_sRegTimer = 0;
			Sip_sRegTimeout = Sys_sRegisterTtl;
			sip_send_register();
		}

		if (Sys_iKeepAlive)
		{
			Sip_iNatTimer ++;
			if (Sip_iNatTimer >= Sys_iKeepAlive)
			{
				strcpy(Adapter_pUdpBuf, "\r\n\r\n");
				UdpSendTo(Sip_pSocket, 4, Sip_pServerIP, Sip_sServerPort);
				Sip_iNatTimer = 0;
			}
		}
	}
	else if(Sip_iRegState == RS_REFRESHING)// Timeout for RS_REFRESHING state
	{
		Sip_sRegTimer++;
		if(Sip_sRegTimer >= SIP_RS_REFRESH_TIMEOUT)
		{
			Sip_iRegState = RS_FREE;
			Sip_sRegTimer = SIP_DEFAULT_REG_EXPIRE;// We want to do it immediately!
		}
	}
}

/**********************************************************************
 ** Line functions                                                **
 **********************************************************************/

void sip_ln_init()
{
	UCHAR i;
	SIP_LCB_HANDLE pLcb;

	pLcb = Sip_pLCB;
	for (i = 0; i < LCB_TSIZE; i ++)
	{
		memset((PCHAR)pLcb, 0, SIP_LCB_LENGTH);
		pLcb ++;
	}
}

void sip_ln_free()
{
	if (!Sip_pCurLcb)	return;
	
	UdpDebugString("sip_ln_free");
	sip_tr_free_lcb(Sip_pCurLcb);
	UdpClose(Sip_pCurLcb->pRtpSocket);

	if (Sip_pCurLcb->pInvUri)	free(Sip_pCurLcb->pInvUri);
	if (Sip_pCurLcb->pReqUri)	free(Sip_pCurLcb->pReqUri);
	if (Sip_pCurLcb->pBranch)	free(Sip_pCurLcb->pBranch);
	if (Sip_pCurLcb->pCallId)	free(Sip_pCurLcb->pCallId);
	if (Sip_pCurLcb->pFrom)		free(Sip_pCurLcb->pFrom);
	if (Sip_pCurLcb->pTo)		free(Sip_pCurLcb->pTo);
	if (Sip_pCurLcb->pFromTag)	free(Sip_pCurLcb->pFromTag);
	if (Sip_pCurLcb->pToTag)	free(Sip_pCurLcb->pToTag);
	if (Sip_pCurLcb->pRoute)	free(Sip_pCurLcb->pRoute);
	if (Sip_pCurLcb->pProxyAuth)	free(Sip_pCurLcb->pProxyAuth);
	if (Sip_pCurLcb->pWWWAuth)		free(Sip_pCurLcb->pWWWAuth);
	if (Sip_pCurLcb->pReferredBy)	free(Sip_pCurLcb->pReferredBy);
	if (Sip_pCurLcb->pReplaces)		free(Sip_pCurLcb->pReplaces);
	if (Sip_pCurLcb->pInvHeader)	free(Sip_pCurLcb->pInvHeader);

	memset((PCHAR)Sip_pCurLcb, 0, SIP_LCB_LENGTH);
}

SIP_LCB_HANDLE sip_ln_alt()
{
	UCHAR i;
	SIP_LCB_HANDLE pLcb;

	pLcb = Sip_pLCB;
	for (i = 0; i < LCB_TSIZE; i ++, pLcb ++)
	{
		if (pLcb != Sip_pActiveLcb)
		{
			return pLcb;
		}
	}
	return NULL;
}

void sip_show_curcid(SIP_LCB_HANDLE pLcb, BOOLEAN bShowLine0)
{
#ifdef OEM_IP20
	UCHAR pNumName[64];
#endif
	
#ifdef UI_4_LINES
	bShowLine0;
#ifdef OEM_IP20
	strcpy(pNumName, pLcb->pPeerNumber);
	if (strcmp(pLcb->pPeerName,""))		strcat_char(pNumName, '<');
	strcat(pNumName, pLcb->pPeerName);
	if (strcmp(pLcb->pPeerName,""))		strcat_char(pNumName, '>');
	TaskUIHandler(UI_CALL_DISPLAY_LINE2, (USHORT)pNumName);
#else
	TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pLcb->pPeerName);
	if (strcmp(pLcb->pPeerName, pLcb->pPeerNumber))
	{
		TaskUIHandler(UI_CALL_DISPLAY_LINE2, (USHORT)pLcb->pPeerNumber);
	}
#endif	
#else
	if (bShowLine0 && strcmp(pLcb->pPeerName, pLcb->pPeerNumber))
	{
		TaskUIHandler(UI_CALL_DISPLAY_LINE0, (USHORT)pLcb->pPeerName);
	}
	TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pLcb->pPeerNumber);
#endif
}

void sip_check_altline()
{
	Sip_pActiveLcb = sip_ln_alt();
	if (Sip_pActiveLcb->iState == CALL_STATE_RINGING)
	{
		TaskUIHandler(UI_CALL_RINGING, Sip_pActiveLcb->iVoiceCoder);
		sip_show_curcid(Sip_pActiveLcb, TRUE);
	}
	else if (Sip_pActiveLcb->iState == CALL_STATE_TALKING)
	{
		if (Sip_pActiveLcb->iXferPart == XFER_PART_TRANSFEROR)
		{
			Sip_pActiveLcb->iState = CALL_STATE_HANGUP;
		}
		else
		{
			TaskUIHandler(UI_CALL_HOLD, (USHORT)(&Sip_pActiveLcb->iVoiceCoder));
			sip_show_curcid(Sip_pActiveLcb, FALSE);
		}
	}
	else
	{
		TaskUIHandler(UI_CALL_FINISHED, 0);
	}
}

void sip_caller_failed(UCHAR iCause)
{
	if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEREE_NEW)
	{
		if (SipXfereeDone(503))
		{
			return;
		}
	}

	Sip_pCurLcb->iState = CALL_STATE_HANGUP;
	TaskUIHandler(UI_CALL_ENDED, iCause);
}

/**********************************************************************
 ** Misc functions                                                  **
 **********************************************************************/

void sip_handle_error(SIP_LCB_HANDLE pLcb, UCHAR iMethod)
{
	if (iMethod == SIP_REQ_REGISTER)
	{
		sip_logon_failed();
		return;
	}

	if (iMethod == SIP_REQ_SUBSCRIBE)
	{
		sip_sub_failed();
		return;
	}
	if (!pLcb)
	{
		return;
	}
	Sip_pCurLcb = pLcb;
	if (iMethod == SIP_REQ_INVITE)
	{
		if (Sip_pCurLcb == Sip_pActiveLcb)
		{
			if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEREE_NEW)
			{
				if (SipXfereeDone(480))
				{
					return;
				}
			}
			Sip_pCurLcb->iState = CALL_STATE_HANGUP;
			TaskUIHandler(UI_CALL_ENDED, CALL_END_MSG_TIMEOUT);
		}
		else
		{
			sip_ln_free();
		}
		return;
	}

	if (iMethod == SIP_REQ_CANCEL || iMethod == SIP_REQ_BYE || iMethod == SIP_REQ_NOTIFY)
	{
		if (Sip_pCurLcb == Sip_pActiveLcb)
		{
			Sip_pCurLcb->iState = CALL_STATE_HANGUP;
			TaskUIHandler(UI_CALL_ENDED, CALL_END_MSG_TIMEOUT);
		}
		else
		{
			sip_ln_free();
		}
	}
	else
	{
		if (iMethod == SIP_REQ_REFER)
		{
			if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEROR)
			{
				Sip_pCurLcb->iXferPart = XFER_PART_NONE;
				if (Sip_pCurLcb == Sip_pActiveLcb)
				{
					SipUnhold();
				}
			}
		}
	}
}

void sip_add_auth(PCHAR pUri, UCHAR iMethod)
{
	if (Sip_pCurLcb->pProxyAuth)
	{
		line_add_str(_cProxy_Authorization);
		TaskHttpDigest(line_get_buf(), Sip_pCurLcb->pProxyAuth, pUri, Sys_pAuthID, Sys_pAuthPin, _cRequest[iMethod]);
		line_update_len();
		line_add_crlf();
	}
	if (Sip_pCurLcb->pWWWAuth)
	{
		line_add_str(_cAuthorization);
		TaskHttpDigest(line_get_buf(), Sip_pCurLcb->pWWWAuth, pUri, Sys_pAuthID, Sys_pAuthPin, _cRequest[iMethod]);
		line_update_len();
		line_add_crlf();
	}
}

void sip_add_common()
{
	sip_add_header(_cRoute, Sip_pCurLcb->pRoute);
	sip_add_call_id(Sip_pCurLcb->pCallId);
	sip_add_contact();
	sip_add_max_forwards();
	sip_add_user_agent();
	sip_add_proxy_require();
	sip_add_header(_cFrom, Sip_pCurLcb->pFrom);
}

void sip_add_content_sdp(SIP_LCB_HANDLE pLcb)
{
	UCHAR iLen;
	USHORT sContentLen;
	UCHAR pBuf[6];
	PCHAR pLen;

	sip_add_content_type(_cAppSdp);
	line_add_str(_cContent_Length);
	line_add_str("     ");
	pLen = line_get_buf();
	line_add_crlf();
	line_add_crlf();
	// add SDP body
	sContentLen = sip_add_sdp(pLcb);
	itoa(sContentLen, pBuf, 10);
	iLen = strlen(pBuf);
	pLen -= iLen;
	memcpy(pLen, pBuf, iLen);
}

void sip_send_invite()
{
	PCHAR pUri;
	PCHAR pBranch;

	sip_debug(_cSend, SIP_REQ_INVITE);

	pUri = Sip_pCurLcb->pInvUri;
	sip_add_req_line(SIP_REQ_INVITE, pUri);

	if (Sip_pCurLcb->pBranch)	free(Sip_pCurLcb->pBranch);
	pBranch = (PCHAR)malloc((USHORT)(MAX_BRANCH_LEN+1));
	Sip_pCurLcb->pBranch = pBranch;
	sip_new_branch(pBranch);
	sip_add_via(pBranch);

	sip_add_common();
	if (Sip_pCurLcb->pTo)
	{
		sip_add_header(_cTo, Sip_pCurLcb->pTo);
	}
	else
	{
		sip_add_to(pUri, NULL);
	}
	sip_add_cseq(Sip_pCurLcb->lCurSeq, SIP_REQ_INVITE);
	Sip_pCurLcb->lInvSeq = Sip_pCurLcb->lCurSeq;
	Sip_pCurLcb->lCurSeq ++;

	if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEREE_NEW)
	{
		sip_add_header(_cReferredBy, Sip_pCurLcb->pReferredBy);
		sip_add_header(_cReplaces, Sip_pCurLcb->pReplaces);
	}
	sip_add_auth(pUri, SIP_REQ_INVITE);
	sip_add_supported();
	sip_add_allow();
	sip_add_session_expires(Sip_pCurLcb);
	sip_add_content_sdp(Sip_pCurLcb);

	// test INVITE without SDP
//	sip_add_content_length(0);
//	line_add_crlf();

	sip_tr_send_req(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, pBranch, SIP_REQ_INVITE, Sip_pCurLcb);
	Sip_pCurLcb->pInvTcb = Sip_pCurTcb;
}

void sip_send_reinvite()
{
	if (Sip_pCurLcb->pInvUri)	free(Sip_pCurLcb->pInvUri);
	Sip_pCurLcb->pInvUri = heap_save_str(Sip_pCurLcb->pReqUri);
	Sip_pCurLcb->iSdpType = SDP_TYPE_REINVITE;
	sip_send_invite();
}

void sip_send_ack()
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	PCHAR pUri;

	sip_debug(_cSend, SIP_REQ_ACK);

	if (Sip_sRecvResp < 300)
	{
		pUri = Sip_pCurLcb->pReqUri;
		sip_add_req_line(SIP_REQ_ACK, pUri);
		sip_new_branch(pBranch);
		sip_add_via(pBranch);
	}
	else
	{
		pUri = Sip_pCurLcb->pInvUri;
		sip_add_req_line(SIP_REQ_ACK, pUri);
		sip_add_via(Sip_pCurLcb->pBranch);
	}
	sip_add_common();
	sip_add_header(_cTo, Sip_pCurLcb->pTo);
	sip_add_cseq(Sip_pCurLcb->lInvSeq, SIP_REQ_ACK);
	sip_add_auth(pUri, SIP_REQ_ACK);
	sip_add_content_length(0);
	line_add_crlf();
/*	
	//test ACK with SDP
	if (Sip_sRecvResp < 300)
	{
		sip_add_content_sdp(Sip_pCurLcb);
	}
*/
	sip_tr_send(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, TRUE);
}

void sip_send_bye()
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	PCHAR pUri;

	sip_debug(_cSend, SIP_REQ_BYE);

	pUri = Sip_pCurLcb->pReqUri;
	sip_add_req_line(SIP_REQ_BYE, pUri);
	sip_new_branch(pBranch);
	sip_add_via(pBranch);
	sip_add_common();
	sip_add_header(_cTo, Sip_pCurLcb->pTo);
	sip_add_cseq(Sip_pCurLcb->lCurSeq, SIP_REQ_BYE);
	Sip_pCurLcb->lCurSeq ++;
	sip_add_content_length(0);
	sip_add_auth(pUri, SIP_REQ_BYE);
	line_add_crlf();

	sip_tr_send_req(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, pBranch, SIP_REQ_BYE, Sip_pCurLcb);
}

void sip_send_cancel()
{
	PCHAR pBranch;
	PCHAR pUri;

	sip_debug(_cSend, SIP_REQ_CANCEL);

	pUri = Sip_pCurLcb->pInvUri;
	sip_add_req_line(SIP_REQ_CANCEL, pUri);
	pBranch = Sip_pCurLcb->pBranch;
	sip_add_via(pBranch);
	sip_add_common();
	sip_add_to(pUri, NULL);
	sip_add_cseq(Sip_pCurLcb->lInvSeq, SIP_REQ_CANCEL);
	sip_add_content_length(0);
	sip_add_auth(pUri, SIP_REQ_CANCEL);
	line_add_crlf();

	sip_tr_send_req(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, pBranch, SIP_REQ_CANCEL, Sip_pCurLcb);
}

void sip_send_info(UCHAR iKey)
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	UCHAR pBuf[32];
	PCHAR pUri;
	USHORT sLen;

	sip_debug(_cSend, SIP_REQ_INFO);
	// compose INFO body
	line_start(pBuf);
	line_add_str_with_char("Signal=", iKey);
	line_add_crlf();
	line_add_str_with_crlf("Duration=200");

	sLen = line_get_len();
	// compose INFO msg
	pUri = Sip_pCurLcb->pReqUri;
	sip_add_req_line(SIP_REQ_INFO, pUri);
	sip_new_branch(pBranch);
	sip_add_via(pBranch);
	sip_add_common();
	sip_add_header(_cTo, Sip_pCurLcb->pTo);
	sip_add_cseq(Sip_pCurLcb->lCurSeq, SIP_REQ_INFO);
	Sip_pCurLcb->lCurSeq ++;
	sip_add_auth(pUri, SIP_REQ_INFO);
	sip_add_content_type(_cAppDtmf);
	sip_add_content_length(sLen);
	line_add_crlf();
	line_add_str(pBuf);

	sip_tr_send_req(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, pBranch, SIP_REQ_INFO, Sip_pCurLcb);
}

void sip_send_notify(USHORT sResp)
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	UCHAR pBuf[64];
	PCHAR pUri;
	USHORT sLen;

	sip_debug(_cSend, SIP_REQ_NOTIFY);
	// compose NOTIFY body
	line_start(pBuf);
	sip_add_status_line(sResp);
	sLen = line_get_len();
	// compose NOTIFY msg
	pUri = Sip_pCurLcb->pReqUri;
	sip_add_req_line(SIP_REQ_NOTIFY, pUri);
	sip_new_branch(pBranch);
	sip_add_via(pBranch);
	sip_add_common();
	sip_add_header(_cTo, Sip_pCurLcb->pTo);
	sip_add_cseq(Sip_pCurLcb->lCurSeq, SIP_REQ_NOTIFY);
	Sip_pCurLcb->lCurSeq ++;
	sip_add_auth(pUri, SIP_REQ_NOTIFY);
	sip_add_header(_cEvent, "refer");
	sip_add_content_type(_cMsgFrag);
	sip_add_content_length(sLen);
	line_add_crlf();
	line_add_str(pBuf);

	sip_tr_send_req(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, pBranch, SIP_REQ_NOTIFY, Sip_pCurLcb);
}

void sip_send_prack()
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	PCHAR pUri;
	UCHAR pSeq[128];

	sip_debug(_cSend, SIP_REQ_PRACK);

	strcpy(pSeq, Sip_pRecvCSeq);

	pUri = Sip_pCurLcb->pReqUri;
	sip_add_req_line(SIP_REQ_PRACK, pUri);
	sip_new_branch(pBranch);
	sip_add_via(pBranch);
	sip_add_common();
	sip_add_header(_cTo, Sip_pCurLcb->pTo);
	sip_add_cseq(Sip_pCurLcb->lCurSeq, SIP_REQ_PRACK);
	Sip_pCurLcb->lCurSeq ++;
	sip_add_content_length(0);
	sip_add_auth(pUri, SIP_REQ_PRACK);

	// add RACK
	line_add_str(_cRAck);
	line_add_long(Sip_pCurLcb->lRseq);
	line_add_char(' ');
	line_add_str_with_crlf(pSeq);
	line_add_crlf();

	sip_tr_send_req(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, pBranch, SIP_REQ_PRACK, Sip_pCurLcb);
}

void sip_send_refer(PCHAR pReferTo)
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	PCHAR pUri;

	sip_debug(_cSend, SIP_REQ_REFER);
	pUri = Sip_pCurLcb->pReqUri;
	sip_add_req_line(SIP_REQ_REFER, pUri);
	sip_new_branch(pBranch);
	sip_add_via(pBranch);
	sip_add_common();
	sip_add_header(_cTo, Sip_pCurLcb->pTo);
	sip_add_cseq(Sip_pCurLcb->lCurSeq, SIP_REQ_REFER);
	Sip_pCurLcb->lCurSeq ++;
	// add refer-to
	line_add_str_with_char(_cReferTo, '<');
	line_add_str_with_char(pReferTo, '>');
	line_add_crlf();
	sip_add_referredby();
	sip_add_content_length(0);
	sip_add_auth(pUri, SIP_REQ_REFER);
	line_add_crlf();

	sip_tr_send_req(Sip_pCurLcb->pDstIP, Sip_pCurLcb->sDstPort, pBranch, SIP_REQ_REFER, Sip_pCurLcb);
}

void sip_send_invresp(USHORT sResp)
{ 
	sip_debug(_cSend, sResp);
	Sip_pCurTcb = Sip_pCurLcb->pInvTcb;

	line_start(Adapter_pUdpBuf);
	sip_add_status_line(sResp);
	
	line_add_str(Sip_pCurLcb->pInvHeader);
	free(Sip_pCurLcb->pInvHeader);
	Sip_pCurLcb->pInvHeader = NULL;

	sip_add_header(_cFrom, Sip_pCurLcb->pTo);
	sip_add_header(_cTo, Sip_pCurLcb->pFrom);
	sip_add_call_id(Sip_pCurLcb->pCallId);
	sip_add_max_forwards();
	sip_add_user_agent();
	if (sResp == 302)
	{
		sip_add_fwdcontact();
	}
	else
	{
		sip_add_contact();
	}
	if (sResp == 200)
	{
		sip_add_session_expires(Sip_pCurLcb);
		sip_add_supported();
		sip_add_allow();
		sip_add_content_sdp(Sip_pCurLcb);
	}
	else
	{
		sip_add_content_length(0);
		line_add_crlf();
	}
	sip_tr_send_resp(Sip_pCurLcb->pRespDstIP, Sip_pCurLcb->sRespDstPort, sResp);
}

/**********************************************************************
 ** API functions                                                    **
 **********************************************************************/

void SipInit()
{
	Sip_pActiveLcb = Sip_pLCB;
	Sip_pCurLcb = Sip_pActiveLcb;

	sip_logon_init();
	sip_sub_init();
	sip_tr_init();
	sip_ln_init();

	memset(Sip_pNatIP, 0, IP_ALEN);
	Sip_sNatPort = 0;
	memset(Sip_pServerIP, 0, IP_ALEN);
	Sip_sServerPort = 0;
	Sip_pSocket = NULL;
	Sip_sRtpPort = 0;

	Rtp_pData = (PCHAR)(Adapter_pUdpBuf + RTP_DATA);

	Sys_iKeepAlive = OptionsGetByte(OPT_KEEP_ALIVE);
	Sys_iNatTraversal = OptionsGetByte(OPT_NAT_TRAVERSAL);
	Sys_bDnsSrv = OptionsGetByte(OPT_DNS_SRV) ? TRUE : FALSE;
	Sys_bSubscribeMwi = OptionsGetByte(OPT_SUBSCRIBE_MWI) ? TRUE : FALSE;
}

void SipStartup()
{
	if (Sys_iNatTraversal == NAT_TRAVERSAL_NATIP)
	{
		OptionsGetBuf(Sip_pNatIP, OPT_NAT_IP, IP_ALEN);
	}
	else
	{
		memcpy4(Sip_pNatIP, Sys_pIpAddress);
	}
	Sip_sNatPort = Sys_sLocalPort;
	memset(Sip_pServerIP, 0, IP_ALEN);
	Sip_sServerPort = OptionsGetShort(OPT_SIP_PORT);
	if (Sip_pSocket)
	{
		UdpClose(Sip_pSocket);
	}
	Sip_pSocket = UdpListen(Sys_sLocalPort, SIPR_BANK_OFFSET, (USHORT)SipRun);
	Sip_sRtpPort = Sys_sRtpPort;

	Sip_bSubscribe = Sys_bSubscribeMwi;
	Sip_iRegState = RS_FREE;
	Sip_lRegSeq = rand();
	sip_new_callid(Sip_pRegCallId);
	sip_new_token(Sip_pRegFromTag, MAX_TAG_LEN);
}

void SipLogon()
{
	Sip_iServerNum = 0;
	Sip_iServerIndex = 0;
	Sip_sRegTimer = 0;
	Sip_sRegTimeout = Sys_sRegisterTtl;

	if (Sys_iNatTraversal == NAT_TRAVERSAL_STUN)
	{
//		UdpDebugString("Stun before logon...");
		TaskStunMap(STUN_TASK_LOGON, Sip_pSocket);
	}
	else
	{
		SipLogonStep1();
	}
}

void SipLogonStep1()
{
	UCHAR pSrvDns[MAX_DNS_LEN+10];
	PCHAR pDns;
	UCHAR pDstIpPort[IP_ALEN+2];

	pDns = (PCHAR)(pSrvDns + 10);
//fv	pDns = (PCHAR)(pSrvDns);	//fv
	if (Sys_bRegister)
	{
		UdpDebugString("Logon Step1");
		Sip_iRegState = RS_REGISTERING;
		strcpy(pDns, Sys_pSipProxy);
		if (strlen(pDns))
		{
			if (str2ip(pDns, pDstIpPort))
			{
				USHORT2PCHAR(0, (PCHAR)(pDstIpPort + IP_ALEN));
				SipLogonStep2(pDstIpPort);
			}
			else
			{
				if (Sys_bDnsSrv)
				{
					memcpy(pSrvDns, _cSipUdp, 10);
					TaskDnsQuery(pSrvDns, DNS_TYPE_SRV, PROTOCOL_BANK_OFFSET, (USHORT)SipLogonSrvStep2);
				}
				else
				{
					TaskDnsQuery(pDns, DNS_TYPE_A, PROTOCOL_BANK_OFFSET, (USHORT)SipLogonStep2);
				}
			}
		}
		else
		{
			sip_logon_failed();
		}
	}
	else
	{
		sip_logon_ok();
	}
}

void swapRecord(UCHAR cnt){
	char tpcTarget[MAX_DNS_LEN];
	USHORT tsPort;
	USHORT tsPriority;

	memcpy(tpcTarget,Sip_tServerInfo[cnt+1].pcTarget,MAX_DNS_LEN);
	tsPort = Sip_tServerInfo[cnt+1].sPort;
	tsPriority = Sip_tServerInfo[cnt+1].sPriority;

	memcpy(Sip_tServerInfo[cnt+1].pcTarget,Sip_tServerInfo[cnt].pcTarget,MAX_DNS_LEN);
	Sip_tServerInfo[cnt+1].sPort = Sip_tServerInfo[cnt].sPort;
	Sip_tServerInfo[cnt+1].sPriority = Sip_tServerInfo[cnt].sPriority;

	memcpy(Sip_tServerInfo[cnt].pcTarget,tpcTarget,MAX_DNS_LEN);
	Sip_tServerInfo[cnt].sPort = tsPort;
	Sip_tServerInfo[cnt].sPriority = tsPriority;

}

void sortPriority(UCHAR numb){		//fv add this routine to sort the sip server as pre there priorities
	UCHAR cnt, test;				//the Weight factor is not used

	do{
		test = 0;
		for (cnt=0;cnt<numb; cnt++){
			if(Sip_tServerInfo[cnt].sPriority > Sip_tServerInfo[cnt+1].sPriority){
				swapRecord(cnt);
				test=1;
			}
		}
//		UdpDebugString("loop");
	}while(test);
}

void SipLogonSrvStep2(PCHAR pAnswer)
{
//	UCHAR i;

	if (!pAnswer)
	{
		sip_logon_failed();
		return;
	}

	Sip_iServerNum = pAnswer[0];
//	UdpDebugString("SIP dns srv result: number of server");
//	UdpDebugVal(Sip_iServerNum, 10);
	if (Sip_iServerNum > MAX_SIP_SERVER)
	{
		Sip_iServerNum = MAX_SIP_SERVER;
	}
	memcpy((PCHAR)&Sip_tServerInfo[0], (PCHAR)(pAnswer + 1), Sip_iServerNum * sizeof(T_DNSSRV_ANS));

	sortPriority(Sip_iServerNum-1);		//fv
/*
//fv
	for (i = 0; i < Sip_iServerNum; i ++)
	{
		UdpDebugString(Sip_tServerInfo[i].pcTarget);
		UdpDebugVal(Sip_tServerInfo[i].sPriority,10);
	}
//fv
*/
	Sip_iServerIndex = 1;
	Sip_sServerPort = Sip_tServerInfo[0].sPort;
	TaskDnsQuery(Sip_tServerInfo[0].pcTarget, DNS_TYPE_A, PROTOCOL_BANK_OFFSET, (USHORT)SipLogonStep2);
}

void SipLogonStep2(PCHAR pServerIP)
{
	if (pServerIP)
	{
		UdpDebugString("Logon Step2");
		memcpy4(Sip_pServerIP, pServerIP);
		sip_send_register();
		if (Sip_bSubscribe)
		{
			sip_sub_start();
		}
	}
	else
	{
		sip_logon_failed();
	}
}

void sip_new_nameuri(PCHAR pDst, PCHAR pName)
{
	strcpy(pDst, _cTokenSip);
	strcat(pDst, pName);
	strcat_char(pDst, '@');
	strcpy((PCHAR)(pDst + strlen(pDst)), Sys_pSipDomain);
}

void sip_star2dot(PCHAR pBuf)
{
	PCHAR p;

	p = pBuf;
	while (*p)
	{
		if (*p == (UCHAR)'*')	*p = '.';
		p ++;
	}
}

void SipCallStep1(PCHAR pCalledUri)
{
	PCHAR p;
	PCHAR pDomain;
	PCHAR pName;
	UCHAR iLen;
	UCHAR pDstIpPort[IP_ALEN+2];	
	UCHAR pBuf[128];
	USHORT sDstPort;
	BOOLEAN bThroughProxy;

	Sip_pCurLcb->iState = CALL_STATE_CALLING;

	iLen = strlen(pCalledUri);
	if (!iLen)
	{
		sip_caller_failed(CALL_END_EMPTY_NUMBER);
		return;
	}

	sDstPort = SIP_SERVER_PORT;
	p = SkipField(pCalledUri, ':');
	if (p)
	{
		sDstPort = atoi(p, 10);
	}

	p = SkipField(pCalledUri, '@');
	if (p)
	{
		pDomain = p;
		pName = pCalledUri;
	}
	else
	{
		if (str2ip(pCalledUri, pDstIpPort) && IsValidIP(pDstIpPort))
		{
			pDomain = pCalledUri;
			sip_star2dot(pDomain);
			pName = NULL;
		}
		else
		{
			pName = pCalledUri;
			pDomain = NULL;
		}
	}

	p = (pName) ? pName : pDomain;
	strcpy(Sip_pCurLcb->pPeerName, (strlen(p) < MAX_USER_NAME_LEN) ? p : (PCHAR)"NameTooLong");
	strcpy(Sip_pCurLcb->pPeerNumber, Sip_pCurLcb->pPeerName);

	line_start(pBuf);
	line_add_str(_cTokenSip);
	if (pName)
	{
		line_add_str_with_char(pName, '@');
	}

	bThroughProxy = FALSE;
	if (pDomain)
	{
		if (!strcmp(pDomain, Sys_pSipProxy))
		{
			bThroughProxy = TRUE;
		}
		if (!strcmp(pDomain, Sys_pSipDomain))
		{
			bThroughProxy = TRUE;
		}
		line_add_str(pDomain);
	}
	else
	{
		if (strlen(Sys_pSipProxy) && strlen(Sys_pSipDomain))
		{
			bThroughProxy = TRUE;
			line_add_str(Sys_pSipDomain);
			pDomain = Sys_pSipProxy;
		}
		else
		{
			sip_caller_failed(CALL_END_WRONG_NUMBER);
			return;
		}
	}
// code compiled correctly 
	if (Sip_pCurLcb->pReqUri)	free(Sip_pCurLcb->pReqUri);
	if (Sip_pCurLcb->pInvUri)	free(Sip_pCurLcb->pInvUri);
	Sip_pCurLcb->pReqUri = heap_save_str(pBuf);
	Sip_pCurLcb->pInvUri = heap_save_str(pBuf);
//	if (Sip_pCurLcb->pReqUri)	free(Sip_pCurLcb->pReqUri);
//	Sip_pCurLcb->pReqUri = heap_save_str(pBuf);
//	if (Sip_pCurLcb->pInvUri)	free(Sip_pCurLcb->pInvUri);
//	Sip_pCurLcb->pInvUri = heap_save_str(pBuf);

	Sip_pCurLcb->sDstPort = (bThroughProxy) ? Sip_sServerPort : sDstPort;
	USHORT2PCHAR(0, (PCHAR)(pDstIpPort + IP_ALEN));
	if (str2ip(pDomain, pDstIpPort))
	{
		SipCallStep2(pDstIpPort);
	}
	else
	{
		if (bThroughProxy && Sys_bRegister && Sip_iRegState != RS_FREE)
		{
			memcpy4(pDstIpPort, Sip_pServerIP);
			SipCallStep2(pDstIpPort);
		}
		else
		{
			TaskDnsQuery(pDomain, DNS_TYPE_A, PROTOCOL_BANK_OFFSET, (USHORT)SipCallStep2);
		}
	}
}

void sip_new_from()
{
	UCHAR pBuf[128];
	UCHAR pTag[MAX_TAG_LEN+1];

	line_start(pBuf);
	sip_add_local_uri(!Sys_bRegister, FALSE);
	sip_new_token(pTag, MAX_TAG_LEN);
	sip_add_token(_cTokenTag, pTag);
	free(Sip_pCurLcb->pFrom);
	Sip_pCurLcb->pFrom = heap_save_str(pBuf);
	free(Sip_pCurLcb->pFromTag);
	Sip_pCurLcb->pFromTag = heap_save_str(pTag);
}

void sip_modify_rtpport()
{
	if (Sys_iNatTraversal != NAT_TRAVERSAL_NATIP)
	{
		if (Sip_sRtpPort >= 65534)
		{
			Sip_sRtpPort = Sys_sRtpPort;
		}
		else
		{
			Sip_sRtpPort += 2;
		}
	}
}

void RtpRun(UDP_SOCKET pUcb);	// functions in SIPR_BANK_OFFSET rtpr.c

void SipCallStep3()
{
	Sip_pCurLcb->bCallee = FALSE;
	Sip_pCurLcb->lCurSeq = rand();
	Sip_pCurLcb->lSessionExpires = SIP_DEFAULT_SESSION_EXPIRE;
	Sip_pCurLcb->lMinSE = SIP_DEFAULT_MIN_SE;
	Sip_pCurLcb->iRefresher = SIP_REFRESHER_UAC;
	Sip_pCurLcb->pCallId = malloc((USHORT)(MAX_CALL_ID_LEN + 1));
	sip_new_callid(Sip_pCurLcb->pCallId);
	sip_new_from();

	sip_new_random(Sip_pCurLcb->pSdpSessionId, 8);
	Sip_pCurLcb->lSdpVersion = Sys_lCurrentTime; 
	Sip_pCurLcb->iLocalMode = AUDIO_MODE_SENDRECV;
	Sip_pCurLcb->iRemoteMode = AUDIO_MODE_INACTIVE;
	Sip_pCurLcb->iVoiceCoder = VOICE_CODER_NONE;
	Sip_pCurLcb->bVad = Sys_bOptVad;
	Sip_pCurLcb->iCoderPayload = 255;
	Sip_pCurLcb->iDtmfPayload = 255;
	rand_array(Sip_pCurLcb->pRtpSSRC, RTP_SSRC_LEN);
	Sip_pCurLcb->sRtpSeq = P_MAKEWORD(rand(), rand());
	Sip_pCurLcb->lRtpTimeStamp = 0;
	Sip_pCurLcb->lRtpOffset = Sys_lTicker;
	Sip_pCurLcb->bVoiceStart = TRUE;
	Sip_pCurLcb->iSdpType = SDP_TYPE_OFFER;
	Sip_pCurLcb->sRtpPort = Sip_sRtpPort;
	sip_modify_rtpport();
	Sip_pCurLcb->pRtpSocket = UdpListen(Sip_pCurLcb->sRtpPort, SIPR_BANK_OFFSET, (USHORT)RtpRun);

	if (Sys_iNatTraversal == NAT_TRAVERSAL_STUN)
	{
//		UdpDebugString("Stun before call...");
		TaskStunMap(STUN_TASK_CALL, Sip_pCurLcb->pRtpSocket);
	}
	else
	{
		sip_send_invite();
	}
}

void SipCallStep2(PCHAR pServerIP)
{
	USHORT sPort;

	if (Sip_pActiveLcb->iState != CALL_STATE_CALLING)	return;
	Sip_pCurLcb = Sip_pActiveLcb;

	if (!pServerIP)	
	{
		sip_caller_failed(CALL_END_BAD_SERVERIP);
		return;
	}
	sPort = PCHAR2USHORT((PCHAR)(pServerIP + IP_ALEN));
	if (sPort)
	{
		Sip_pCurLcb->sDstPort = sPort;
	}
	memcpy4(Sip_pCurLcb->pDstIP, pServerIP);
	Sip_pCurLcb->iState = CALL_STATE_CALLING2;
	SipCallStep3();
}

// Answer after 180 ringing has already been sent out
void SipAnswer()
{
	Sip_pCurLcb = Sip_pActiveLcb;
#ifdef OEM_IP20
#ifdef __ADSL
	Sip_pCurLcb->brtpIpPrivate = IsPrivateIP(Sip_pCurLcb->pRtpDstIP);	
#endif					
#endif
	Sip_pCurLcb->iState = CALL_STATE_TALKING;
	TaskUIHandler(UI_CALL_CONNECTED, (USHORT)(&Sip_pCurLcb->iVoiceCoder));
	sip_show_curcid(Sip_pCurLcb, FALSE);
	TaskCallLogAdd(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, CALL_LOG_TYPE_RECVED);

	if (Sys_iNatTraversal == NAT_TRAVERSAL_STUN)
	{
//		UdpDebugString("Stun before answer...");
		TaskStunMap(STUN_TASK_ANSWER, Sip_pCurLcb->pRtpSocket);
	}
	else
	{
		sip_send_invresp(200);
	}
}

void SipDial()
{
	Sip_pActiveLcb->iState = CALL_STATE_DIALING;
	TaskUIHandler(UI_CALL_DIAL, 0);
}

void sip_unhold_call()
{
	Sip_pCurLcb->bHolding = TRUE;
	Sip_pCurLcb->iLocalMode |= AUDIO_MODE_RECVONLY;
	sip_send_reinvite();
//	TaskUIHandler(UI_CALL_CONNECTED, (USHORT)(&Sip_pCurLcb->iVoiceCoder));
//	sip_show_curcid(Sip_pCurLcb, FALSE);
}

void sip_hold_call()
{
	Sip_pCurLcb->bHolding = TRUE;
	Sip_pCurLcb->iLocalMode &= ~AUDIO_MODE_RECVONLY;
	sip_send_reinvite();
	TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_CALL_ONHOLD);
//	TaskUIHandler(UI_CALL_HOLD, (USHORT)(&Sip_pCurLcb->iVoiceCoder));
//	sip_show_curcid(Sip_pCurLcb, FALSE);
}

void SipHold()
{
	if (Sip_pActiveLcb->iState == CALL_STATE_TALKING && !Sip_pActiveLcb->bHolding)
	{
		Sip_pCurLcb = Sip_pActiveLcb;
		if (Sip_pCurLcb->iLocalMode & AUDIO_MODE_RECVONLY)
		{
			sip_hold_call();
		}
		else
		{
			sip_unhold_call();
		}
	}
}

void SipUnhold()
{
	if (Sip_pActiveLcb->iState == CALL_STATE_TALKING)
	{
		Sip_pCurLcb = Sip_pActiveLcb;
		if (!(Sip_pCurLcb->iLocalMode & AUDIO_MODE_RECVONLY))
		{
			sip_unhold_call();
		}
	}
}

void SipCallCanceled()
{
	UCHAR iState;

	iState = Sip_pCurLcb->iState;
	if (iState == CALL_STATE_RINGING)
	{
		sip_send_invresp(487);
		Sip_pCurLcb->iState = CALL_STATE_HANGUP;
		TaskCallLogAdd(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, CALL_LOG_TYPE_MISSED);
#ifdef OEM_IP20
		voice_message = 0;
#endif
		if (Sip_pCurLcb == Sip_pActiveLcb)
		{
			sip_check_altline();
		}
		else
		{
			TaskUIHandler(UI_CALL_TIMER, (USHORT)&Sip_pActiveLcb->ct);
			sip_show_curcid(Sip_pActiveLcb, FALSE);
		}
	}
	else if (iState == CALL_STATE_TALKING)
	{
		if (Sip_pCurLcb == Sip_pActiveLcb)
		{
			Sip_pCurLcb->iState = CALL_STATE_HANGUP;
			Sip_pCurLcb->iTimer = 0;
			TaskUIHandler(UI_CALL_ENDED, CALL_END_PEER_HANGUP);
		}
		else
		{
			sip_ln_free();
		}
	}
	else if (iState == CALL_STATE_HANGUP && Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEROR)
	{
		sip_ln_free();
	}
}

void SipHandsetUp(BOOLEAN bMemCall)
{
	UCHAR iState;

	iState = Sip_pActiveLcb->iState;
	Sip_pCurLcb = Sip_pActiveLcb;

	if (iState == CALL_STATE_RINGING)
	{
		SipAnswer();
	}
	else if (iState == CALL_STATE_FREE)
	{
		SipDial();
		if (bMemCall)
		{
			Sip_pActiveLcb->bMemCall = TRUE;
		}
#ifdef OEM_IP20
		else if (hotline)
		{
			OptionsGetString(Sip_pActiveLcb->pPeerNumber, OPT_HOTLINE_NUMBER, MAX_USER_NUMBER_LEN);
			if (strlen(Sip_pActiveLcb->pPeerNumber))
			{
				TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)Sip_pActiveLcb->pPeerNumber);
				Sip_pActiveLcb->bMemCall = TRUE;
			}
		}
#else
		else
		{
			OptionsGetString(Sip_pActiveLcb->pPeerNumber, OPT_HOTLINE_NUMBER, MAX_USER_NUMBER_LEN);
			if (strlen(Sip_pActiveLcb->pPeerNumber))
			{
				TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)Sip_pActiveLcb->pPeerNumber);
				Sip_pActiveLcb->bMemCall = TRUE;
			}
		}
#endif
	}
	else if (iState == CALL_STATE_TALKING)
	{
		sip_unhold_call();
	}
}

void SipHandsetDown()
{
	UCHAR iState;

	iState = Sip_pActiveLcb->iState;
	if (iState == CALL_STATE_RINGING)
	{
		return;
	}

	Sip_pCurLcb = Sip_pActiveLcb;
	if (iState == CALL_STATE_TALKING)
	{
		if (Sip_pCurLcb->iLocalMode & AUDIO_MODE_RECVONLY)
		{
			sip_send_bye();
			Sip_pCurLcb->iState = CALL_STATE_HANGUP;
		}
		else
		{
			return;
		}
	}
#ifdef OEM_IP20
	else if (iState == CALL_STATE_CALLING2 && TransFlagResp == 0)
	{
		sip_send_cancel();
		Sip_pCurLcb->iState = CALL_STATE_HANGUP;
	}
#else
	else if (iState == CALL_STATE_CALLING2)
	{
		if (Sip_pCurLcb->bEarlyDlg)
		{
			sip_send_cancel();
			Sip_pCurLcb->iState = CALL_STATE_HANGUP;
		}
		else
		{
			sip_tr_free(Sip_pCurLcb->pInvTcb);
			sip_ln_free();
		}
	}
#endif
	else if (iState != CALL_STATE_FREE)
	{
		sip_ln_free();
	}

	sip_check_altline();
}

void SipFlash()
{
	UCHAR iState;

	Sip_pCurLcb = Sip_pActiveLcb;
	iState = Sip_pCurLcb->iState;
//	if (iState == CALL_STATE_FREE)		return;
	if (iState == CALL_STATE_RINGING)	return;

	if (iState == CALL_STATE_DIALING || iState == CALL_STATE_CALLING || iState == CALL_STATE_HANGUP)
	{
		// cancel current call
		sip_ln_free();
	}
	else if (iState == CALL_STATE_CALLING2)
	{
		// cancel current call
		sip_send_cancel();
		Sip_pCurLcb->iState = CALL_STATE_HANGUP;
	}
	else if (iState == CALL_STATE_TALKING)
	{
		// hold current call
		if (Sip_pCurLcb->iLocalMode & AUDIO_MODE_RECVONLY)
		{
			sip_hold_call();
		}
	}

	// Active another call session
	Sip_pActiveLcb = sip_ln_alt();
	Sip_pCurLcb = Sip_pActiveLcb;
	iState = Sip_pCurLcb->iState;
	if (iState == CALL_STATE_FREE)
	{
		SipDial();
	}
	else if (iState == CALL_STATE_TALKING)
	{
		sip_unhold_call();
	}
	else if (iState == CALL_STATE_RINGING)
	{
		SipAnswer();
	}
	else if (iState == CALL_STATE_HANGUP)
	{
		TaskUIHandler(UI_CALL_FINISHED, 0);
	}
}

#ifdef OEM_IP20

void SipTransfer(PCHAR pNumber)
{
	SIP_LCB_HANDLE pAltLcb;
	UCHAR pBuf[256];
	PCHAR pTempBuf;
	PCHAR pTemp;

	pAltLcb = sip_ln_alt();	
	if (Sip_pActiveLcb->iState != CALL_STATE_TALKING)
	{	
		if (Sip_pActiveLcb->iState == CALL_STATE_DIALING)	SipCall(pNumber);		
		TransFlagResp = 1;
		if (pAltLcb->iState == CALL_STATE_TALKING)
		{
 			TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_CALL_XFER);
			line_start(pBuf);
			if (Sys_bRegister)
			{
				sip_new_nameuri(pBuf, Sip_pActiveLcb->pPeerNumber);
				line_update_len();
			}
			else
			{
				line_add_str(_cTokenSip);
				line_add_str(Sip_pActiveLcb->pPeerNumber);
			}
			line_add_str("?Replaces=");
			pTemp = line_get_buf();
	
			pTempBuf = Adapter_pUdpBuf;

			strcpy(pTempBuf, Sip_pActiveLcb->pCallId);
			strcat(pTempBuf, ";to-tag=");
			strcat(pTempBuf, Sip_pActiveLcb->pToTag);
			strcat(pTempBuf, ";from-tag=");
			strcat(pTempBuf, Sip_pActiveLcb->pFromTag);
			sip_escape_str(pTemp, pTempBuf);
			Sip_pCurLcb = pAltLcb;
			Sip_pCurLcb->iXferPart = XFER_PART_TRANSFEROR;
			Sip_pActiveLcb->iXferPart = XFER_PART_TRANSFEROR;
		
			Delay(1000);
			sip_send_refer(pBuf);	
		}
		else
		{
			TaskUIHandler(UI_CALL_DISPLAY_IDS2, IDS_XFER_FAILED);
		}
	}
	else if (Sip_pActiveLcb->iState == CALL_STATE_TALKING && pAltLcb->iState == CALL_STATE_TALKING)
	{
		TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_CALL_XFER);
		line_start(pBuf);
		if (Sys_bRegister)
		{
			sip_new_nameuri(pBuf, Sip_pActiveLcb->pPeerNumber);
			line_update_len();
		}
		else
		{
			line_add_str(_cTokenSip);
			line_add_str(Sip_pActiveLcb->pPeerNumber);
		}
		line_add_str("?Replaces=");
		pTemp = line_get_buf();

		pTempBuf = Adapter_pUdpBuf;

		strcpy(pTempBuf, Sip_pActiveLcb->pCallId);
		strcat(pTempBuf, ";to-tag=");
		strcat(pTempBuf, Sip_pActiveLcb->pToTag);
		strcat(pTempBuf, ";from-tag=");
		strcat(pTempBuf, Sip_pActiveLcb->pFromTag);
		sip_escape_str(pTemp, pTempBuf);
		
		Sip_pCurLcb = pAltLcb;
		Sip_pCurLcb->iXferPart = XFER_PART_TRANSFEROR;
		Sip_pActiveLcb->iXferPart = XFER_PART_TRANSFEROR;
		sip_send_refer(pBuf);
	}
}

#else

void SipBlindTransfer(PCHAR pNumber)
{
	SIP_LCB_HANDLE pAltLcb;
	UCHAR pBuf[256];

	pAltLcb = sip_ln_alt();
	if (strlen(pNumber))
	{
		if (pAltLcb->iState == CALL_STATE_TALKING)
		{
//			UdpDebugString("Blind transfer");
			TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_CALL_XFER);
			Sip_pCurLcb = Sip_pActiveLcb;
			sip_ln_free();
			Sip_pActiveLcb = pAltLcb;
			Sip_pCurLcb = pAltLcb;
			Sip_pCurLcb->iXferPart = XFER_PART_TRANSFEROR;
			if (Sys_bRegister)
			{
				sip_new_nameuri(pBuf, pNumber);
			}
			else
			{
				// change '*' to '.'
				sip_star2dot(pNumber);
				strcpy(pBuf, _cTokenSip);
				strcat(pBuf, pNumber);
			}
			sip_send_refer(pBuf);
		}
		else
		{
			TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_XFER_FAILED);
		}
	}
}

void SipTransfer()
{
	SIP_LCB_HANDLE pAltLcb;
	UCHAR pBuf[256];
	PCHAR pTempBuf;
	PCHAR pTemp;

	if (Sip_pActiveLcb->iState != CALL_STATE_TALKING)	return;

	pAltLcb = sip_ln_alt();

	if (pAltLcb->iState == CALL_STATE_TALKING)
	{
		TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_CALL_XFER);
		line_start(pBuf);
		if (Sys_bRegister)
		{
			sip_new_nameuri(pBuf, Sip_pActiveLcb->pPeerNumber);
			line_update_len();
		}
		else
		{
			line_add_str(_cTokenSip);
			line_add_str(Sip_pActiveLcb->pPeerNumber);
		}
		line_add_str("?Replaces=");
		pTemp = line_get_buf();

		pTempBuf = Adapter_pUdpBuf;

		strcpy(pTempBuf, Sip_pActiveLcb->pCallId);
		strcat(pTempBuf, ";to-tag=");
		strcat(pTempBuf, Sip_pActiveLcb->pToTag);
		strcat(pTempBuf, ";from-tag=");
		strcat(pTempBuf, Sip_pActiveLcb->pFromTag);
		sip_escape_str(pTemp, pTempBuf);
		
		Sip_pCurLcb = pAltLcb;
		Sip_pCurLcb->iXferPart = XFER_PART_TRANSFEROR;
		Sip_pActiveLcb->iXferPart = XFER_PART_TRANSFEROR;
		Delay(1000);
		sip_send_refer(pBuf);
	}
	else if (pAltLcb->iState == CALL_STATE_FREE)
	{
		// hold current call
		if (Sip_pActiveLcb->iLocalMode & AUDIO_MODE_RECVONLY)
		{
			Sip_pCurLcb = Sip_pActiveLcb;
			sip_hold_call();
		}
		Sip_pActiveLcb = pAltLcb;
		Sip_pCurLcb = Sip_pActiveLcb;
		Sip_pCurLcb->iXferPart = XFER_PART_TRANSFEROR;
		SipDial();
	}
}
#endif

void SipCall(PCHAR pCalledUri)
{
	UCHAR pBuf[128];

	strcpy(pBuf, pCalledUri);
	Sip_pCurLcb = Sip_pActiveLcb;

#ifdef OEM_IP20
	TaskUIHandler(UI_CALL_CALLING, (USHORT)pCalledUri);
	SipCallStep1(pBuf);	
#ifdef __ADSL
	Sip_pCurLcb->brtpSession = FALSE;
	Sip_pCurLcb->brtpIpPrivate = TRUE;
#endif

#else
	if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEROR)
	{
		SipBlindTransfer(pBuf);
	}
	else
	{
		TaskUIHandler(UI_CALL_CALLING, (USHORT)pCalledUri);
		SipCallStep1(pBuf);
	}
#endif
}


void SipOutgoingKey(UCHAR iKey)
{
	UCHAR iDtmfCode;
	
	iDtmfCode = ascii2keyid(iKey);
	if (iDtmfCode == MAP_KEY_UNKNOWN)	
	{
		return;
	}
	if (Sys_iDtmfType == DTMF_TYPE_RFC2833)
	{
		if (!(Sip_pActiveLcb->iLocalMode & AUDIO_MODE_SENDONLY) || !(Sip_pActiveLcb->iRemoteMode & AUDIO_MODE_RECVONLY))
		{
			return;
		}
		RtpSendKey(Sip_pActiveLcb, iDtmfCode);
	}
	else if (Sys_iDtmfType == DTMF_TYPE_SIPINFO)
	{
		Sip_pCurLcb = Sip_pActiveLcb;
		sip_send_info(iKey);
	}
}

void SipSendReq(UCHAR iMethod)
{
	switch (iMethod)
	{
	case SIP_REQ_REGISTER:
		sip_send_register();
		break;

	case SIP_REQ_SUBSCRIBE:
		sip_send_subscribe();
		break;

	case SIP_REQ_INVITE:
		sip_send_invite();
		break;

	case SIP_REQ_ACK:
		sip_send_ack();
		break;

	case SIP_REQ_BYE:
		sip_send_bye();
		break;
	
	case SIP_REQ_CANCEL:
		sip_send_cancel();
		break;

	case SIP_REQ_PRACK:
		sip_send_prack();
		break;
	}
}

void SipSendResp(USHORT sResp)
{
	UCHAR i;
	BOOLEAN bAddSdp;
	SIP_LCB_HANDLE pLcb;

	pLcb = NULL;
	bAddSdp = FALSE;
	if (Sip_iRecvMethod == SIP_REQ_OPTIONS)
	{
		bAddSdp = TRUE;
	}
	else if ((Sip_iRecvMethod == SIP_REQ_INVITE && (sResp == 200 || sResp == 183)) || (Sip_iRecvMethod == SIP_REQ_UPDATE && Sip_pRecvSdp && sResp == 200))
	{
		pLcb = Sip_pCurLcb;
		bAddSdp = TRUE;
	}
	if (Sip_iRecvMethod == SIP_REQ_INVITE && sResp == 183)
	{
		//183 is 180 with sdp
		sResp = 180;
	}

	sip_debug(_cSend, sResp);

	line_start(Sip_pTempBuf);
	for (i = 0; i < Sip_iRecvViaNum; i ++)
	{
		sip_add_header(_cVia, Sip_pRecvVia[i]);
	}
	for (i = 0; i < Sip_iRecvRRNum; i ++)
	{
		sip_add_header(_cRecord_Route, Sip_pRecvRecordRoute[i]);
	}
	sip_add_header(_cCSeq, Sip_pRecvCSeq);
	sip_add_header(_cCall_ID, Sip_pRecvCallId);
	sip_add_header(_cFrom, Sip_pRecvFrom);

	line_start(Adapter_pUdpBuf);
	sip_add_status_line(sResp);
	line_add_str(Sip_pTempBuf);
	if (Sip_pRecvSessionExpires && sResp == 200)
	{
		sip_add_session_expires(Sip_pCurLcb);
		sip_add_supported();
	}
	sip_add_header(_cTo, Sip_pRecvTo);
	if (Sip_iRecvMethod == SIP_REQ_INVITE)
	{
		if (sResp < 200 && Sip_pCurTcb->b100rel)
		{
			sip_add_require(_c100rel);
			sip_add_rseq(Sip_pCurLcb->lRseq);
		}
	}
	if (Sip_iRecvMethod == SIP_REQ_OPTIONS)
	{
		sip_add_allow();
		sip_add_supported();
	}
	if (sResp == 415 || Sip_iRecvMethod == SIP_REQ_OPTIONS)
	{
		sip_add_accept();
	}
	if (Sip_iRecvMethod == SIP_REQ_UNKNOWN)
	{
		sip_add_allow();
	}
	if (Sip_iRecvMethod == SIP_REQ_INVITE && sResp == 302)
	{
		sip_add_fwdcontact();
	}
	else
	{
		sip_add_contact();
	}
	sip_add_user_agent();
	// add content type and length
	if (bAddSdp)
	{
		sip_add_content_sdp(pLcb);
	}
	else
	{
		sip_add_content_length(0);
		line_add_crlf();
	}
	// send reponse
	sip_tr_send_resp(UdpGetDstIP(Sip_pSocket), Sip_sRespDstPort, sResp);
}

void SipDnd()
{
	Sip_pCurLcb = Sip_pActiveLcb;
	sip_send_invresp(480);
	Sip_pCurLcb->iState = CALL_STATE_HANGUP;
	TaskCallLogAdd(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, CALL_LOG_TYPE_RECVED);
	sip_check_altline();
}


BOOLEAN SipXfereeDone(USHORT sResp)
{
	SIP_LCB_HANDLE pAltLcb;

	Sip_pCurLcb->iXferPart = XFER_PART_NONE;
	if (Sip_pCurLcb->pReplaces)		
	{
		free(Sip_pCurLcb->pReplaces);
		Sip_pCurLcb->pReplaces = NULL;
	}
	if (Sip_pCurLcb->pReferredBy)	
	{
		free(Sip_pCurLcb->pReferredBy);
		Sip_pCurLcb->pReferredBy = NULL;
	}

	pAltLcb = Sip_pCurLcb;
	Sip_pCurLcb = sip_ln_alt();
	if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEREE_ORIG)
	{
		Sip_pCurLcb->iXferPart = XFER_PART_NONE;
#ifdef OEM_IP20
		if (Sip_pCurLcb->iState == CALL_STATE_TALKING || Sip_pCurLcb->iState == CALL_STATE_DIALING)
#else
		if (Sip_pCurLcb->iState == CALL_STATE_TALKING)
#endif
		{
			sip_send_notify(sResp);
			if (sResp >= 300)
			{
				Sip_pActiveLcb = Sip_pCurLcb;
				if (sResp != USHORT_MAX)
				{
					Sip_pCurLcb = pAltLcb;
					sip_ln_free();
					Sip_pCurLcb = Sip_pActiveLcb;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

void SipHandleEvent(UCHAR iEvent, USHORT sParam)
{
	PCHAR p;
	UCHAR iState;

	iState = Sip_pActiveLcb->iState;
	switch (iEvent)
	{
	case UI_EVENT_LOGON:
		SipStartup();
		SipLogon();
		break;

	case UI_EVENT_HANDSET_UP:
		SipHandsetUp((BOOLEAN)sParam);
		break;

	case UI_EVENT_HANDSET_DOWN:
		SipHandsetDown();
		break;

	case UI_EVENT_FLASH:
		SipFlash();
		break;

	case UI_EVENT_HOLD:
		if (sParam)
		{
			SipUnhold();
		}
		else
		{
			SipHold();
		}
		break;

	case UI_EVENT_CALL:
		if (iState == CALL_STATE_DIALING)
		{
			Sip_pCurLcb = Sip_pActiveLcb;
			SipCall((PCHAR)sParam);
		}
		break;

	case UI_EVENT_MEM_CALL:
		if (iState == CALL_STATE_DIALING)
		{
			p = strchr((PCHAR)sParam, 'T');
			if (p)
			{
				strcpy(Sip_pActiveLcb->pKeyOut, p);
				Sip_pActiveLcb->iKeyIndex = 0;
				*p = 0;
			}
			if (Sip_pActiveLcb->bMemCall)
			{
				strcpy(Sip_pActiveLcb->pPeerNumber, (PCHAR)sParam);
			}
			else
			{
				TaskUIHandler(UI_CALL_PLAYKEYS, sParam);
				SipCall((PCHAR)sParam);
			}
		}
		else if (iState == CALL_STATE_TALKING)
		{
			strcpy(Sip_pActiveLcb->pKeyOut, (PCHAR)sParam);
			Sip_pActiveLcb->iKeyIndex = 0;
		}
		break;

	case UI_EVENT_TRANSFER:
#ifdef OEM_IP20
		SipTransfer((PCHAR)sParam);
#else
		SipTransfer();
#endif
		break;

	case UI_EVENT_DIGIT:
		TaskUIHandler(UI_CALL_PLAYKEY, (UCHAR)sParam);
		SipOutgoingKey((UCHAR)sParam);
		break;

	case UI_EVENT_MUTE_DND:
		if (Sip_pActiveLcb->iState == CALL_STATE_RINGING)
		{
			SipDnd();
		}
		break;

	case UI_EVENT_TEXT:
		SipSendMessage((PCHAR)sParam);
		break;

	// Event from SIPR bank
	case SIPR_EVENT_LOGON_OK:
		sip_logon_ok();
		break;

	case SIPR_EVENT_LOGON_FAILED:
		sip_logon_failed();
		break;

	case SIPR_EVENT_SUB_FAILED:
		sip_sub_failed();
		break;

	case SIPR_EVENT_SEND_REQ:
		SipSendReq((UCHAR)sParam);
		break;

	case SIPR_EVENT_SEND_RESP:
		SipSendResp(sParam);
		break;

	case SIPR_EVENT_CALL_CANCELED:
		SipCallCanceled();
		break;

	case SIPR_EVENT_CALL_FORWARDED:
		SipCallStep3();
		break;

	case SIPR_EVENT_AUTO_ANSWER:
		SipAnswer();
		break;

	case SIPR_EVENT_LINE_FREE:
		sip_ln_free();
		break;
	
	case SIPR_EVENT_XFEREE_DONE:
		SipXfereeDone(sParam);
		break;

	case SIPR_EVENT_NEW_TAG:
		sip_new_token((PCHAR)sParam, MAX_TAG_LEN);
		break;

	case SIPR_EVENT_MODIFY_RTPPORT:
		sip_modify_rtpport();
		break;
	}
}

USHORT SipGetCallInfo(UCHAR iType)
{
	switch (iType)
	{
	case CALL_INFO_STATE:
		return Sip_pActiveLcb->iState;

	case CALL_INFO_IS_CALLEE:
		return Sip_pActiveLcb->bCallee;

	case CALL_INFO_PEER_NAME:
		return (USHORT)(Sip_pActiveLcb->pPeerName);

	case CALL_INFO_PEER_NUMBER:
		return (USHORT)(Sip_pActiveLcb->pPeerNumber);

	case CALL_INFO_CALL_TIME:
		return (USHORT)(&Sip_pActiveLcb->ct);
	}
	return 0;
}

void SipFrameTimer()
{
	UCHAR i, iCoder;
	USHORT sTimeStamp;

	if (!Sip_pActiveLcb->bSendingKey)	return;

	if (Sip_pActiveLcb->bHolding || Sip_pActiveLcb->iXferPart == XFER_PART_TRANSFEROR)
	{
		return;
	}
	iCoder = Sip_pActiveLcb->iVoiceCoder;
	if (iCoder == VOICE_CODER_NONE || iCoder > VOICE_CODER_NUM)
	{
		return;
	}

	/* Handle DTMF Keys */
	// Net2phone MTA adapter's RFC2833 handling
	if (Sip_pActiveLcb->sSendKeyLen < RTP_DTMF_DURATION)
	{
		sTimeStamp = GetFrameTime(iCoder) << 3;
		if (Sip_pActiveLcb->sSendKeyLen)	
		{
			RtpSendKeyPacket(Sip_pActiveLcb, FALSE, FALSE);
			Sip_pActiveLcb->sSendKeyLen += sTimeStamp;
		}
		else
		{
			Sip_pActiveLcb->lRtpTimeStamp = rl_32x8((Sys_lTicker - Sip_pActiveLcb->lRtpOffset), 3);
			for (i = 0; i < 3; i ++)
			{
				RtpSendKeyPacket(Sip_pActiveLcb, TRUE, FALSE);
			}
			Sip_pActiveLcb->sSendKeyLen += sTimeStamp;
			RtpSendKeyPacket(Sip_pActiveLcb, FALSE, FALSE);
			Sip_pActiveLcb->sSendKeyLen += sTimeStamp;
		}
	}
	else 
	{
		for (i = 0; i < 3; i ++)
		{
			RtpSendKeyPacket(Sip_pActiveLcb, FALSE, TRUE);
		}
		Sip_pActiveLcb->bSendingKey = FALSE;
	}
}

void SipSendVoice(UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2)
{
	UCHAR iState;

	iState = Sip_pActiveLcb->iState;
#if defined OEM_IP20 || defined OEM_BT2008
	if (iState == CALL_STATE_RINGING)
	{
		return;	//added in order to not make RTP connection during ringing
	}
#else
	if (iState != CALL_STATE_CALLING2 && iState != CALL_STATE_TALKING && iState != CALL_STATE_RINGING)
	{
		return;
	}
#endif
	if (Sip_pActiveLcb->bHolding || Sip_pActiveLcb->iXferPart == XFER_PART_TRANSFEROR)
	{
		return;
	}
/*	// If not sending packets continuously, ringback tone packets may not be able to reach caller.
	if (iState == CALL_STATE_CALLING2)
	{
		if (Sip_pActiveLcb->iPacketCtr >= 6)
		{
			Sip_pActiveLcb->bVoiceStart = TRUE;
			return;
		}
	}
*/
	if (!(Sip_pActiveLcb->iLocalMode & AUDIO_MODE_SENDONLY) || !(Sip_pActiveLcb->iRemoteMode & AUDIO_MODE_RECVONLY))
	{
		return;
	}
	if (Sip_pActiveLcb->iVoiceCoder == VOICE_CODER_NONE || Sip_pActiveLcb->iVoiceCoder > VOICE_CODER_NUM)
	{
		return;
	}
	if (Sip_pActiveLcb->bSendingKey)
	{
		return;
	}

	RtpSendVoice(Sip_pActiveLcb, iFrame1, pBuf1, iFrame2, pBuf2);
}

void SipTimer()
{
	UCHAR i;
	SIP_LCB_HANDLE pLcb;
	SIP_LCB_HANDLE pAltLcb;
	UCHAR iState, iKey;

	sip_tr_timer();
	
	if (Sys_bRegister)
	{
		sip_logon_timer();
		sip_sub_timer();
	}

	pLcb = Sip_pLCB;
	for (i = 0; i < LCB_TSIZE; i ++, pLcb ++)
	{
		iState = pLcb->iState;
		Sip_pCurLcb = pLcb;
		if (iState == CALL_STATE_RINGING)
		{
			pLcb->iTimer ++;
#ifdef OEM_IP20
			if (TransferFlag == 1 && pLcb->iTimer > 2)
			{
				Sip_pCurLcb = pLcb;
				sip_send_invresp(480);
				pLcb->iState = CALL_STATE_HANGUP;
			
				//TaskCallLogAdd(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, CALL_LOG_TYPE_MISSED);
				if (pLcb == Sip_pActiveLcb)
				{
					sip_check_altline();
				}
				TransferFlag = 0 ;
			}
#endif
			if (Sys_iNoanswerTimeout && (pLcb->iTimer > Sys_iNoanswerTimeout))
			{
				if (Sys_bAutoAnswer)
				{
					if (pLcb == Sip_pActiveLcb)
					{
						TaskUIHandler(UI_CALL_AUTOANSWER, 0); 
						SipAnswer();
					}
				}
				else
				{
					sip_send_invresp(Sys_bFwdNoanswer ? 302 : 480);
					pLcb->iState = CALL_STATE_HANGUP;
					TaskCallLogAdd(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, CALL_LOG_TYPE_MISSED);
					//sip_ln_free();
					if (pLcb == Sip_pActiveLcb)
					{
						sip_check_altline();
					}
				}
			}
		}
		else if (iState == CALL_STATE_TALKING)
		{
			CallTimeTick(&pLcb->ct);
			if (pLcb == Sip_pActiveLcb)
			{
				pAltLcb = sip_ln_alt();
#ifdef OEM_IP20
				if (pLcb->iLocalMode == AUDIO_MODE_SENDRECV)
#else
				if (pLcb->iLocalMode == AUDIO_MODE_SENDRECV && pLcb->iXferPart == XFER_PART_NONE)
#endif
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
						}
					}
					else
					{
						TaskUIHandler(UI_CALL_PLAYKEY, (USHORT)iKey);
						SipOutgoingKey(iKey);
						pLcb->iKeyIndex ++;
						pLcb->iTimer = 0;
					}
				}
			}
			if (pLcb->iXferPart == XFER_PART_FAIL)
			{
				pLcb->iTimer ++;
				if (pLcb->iTimer >= 2)
				{
					pLcb->iTimer = 0;
					pLcb->iXferPart = XFER_PART_NONE;
				}
			}
			if (pLcb->lSessionExpires)
			{
				pLcb->lSessionTimer ++;
				if (pLcb->lSessionTimer == pLcb->lSessionExpires)
				{
					sip_send_bye();
					Sip_pCurLcb->iState = CALL_STATE_HANGUP;
				}
				else if (pLcb->lSessionTimer == rr_32x8(pLcb->lSessionExpires, 1))
				{
					if ((pLcb->bCallee && pLcb->iRefresher == SIP_REFRESHER_UAS) || (!pLcb->bCallee && pLcb->iRefresher == SIP_REFRESHER_UAC))
					{
						sip_send_reinvite();
					}
				}
			}
		}
		else if (iState == CALL_STATE_DIALING)
		{
			if (pLcb == Sip_pActiveLcb)
			{
				if (pLcb->bMemCall)
				{
					pLcb->iTimer ++;
					if (pLcb->iTimer >= 2)
					{
						pLcb->iTimer = 0;
						pLcb->bMemCall = FALSE;
						TaskUIHandler(UI_CALL_PLAYKEYS, (USHORT)pLcb->pPeerNumber);
						SipCall(pLcb->pPeerNumber);
					}
				}
			}
		}
		else if (iState == CALL_STATE_HANGUP)
		{
			pLcb->iTimer ++;
			if (pLcb->iTimer >= HANGUP_TIMEOUT)
			{
				sip_ln_free();
				if (pLcb == Sip_pActiveLcb)
				{
					TaskUIHandler(UI_CALL_HANGUP, 0);
				}
			}
		}
	}
}

void SipStunDone(PCHAR pMappedAddr, USHORT sMappedPort, UCHAR iTask)
{
	Sip_pCurLcb = Sip_pActiveLcb;
	memcpy4(Sip_pNatIP, (pMappedAddr) ? pMappedAddr : Sys_pIpAddress);

	switch(iTask)
	{
	case STUN_TASK_LOGON:
		if (sMappedPort)
		{
			Sip_sNatPort = sMappedPort;
		}
		SipLogonStep1();
		break;

	case STUN_TASK_CALL:
		if (Sip_pCurLcb->iState == CALL_STATE_CALLING2)
		{
			if (sMappedPort)
			{
				Sip_pCurLcb->sRtpPort = sMappedPort;
			}
			sip_send_invite();
		}
		break;

	case STUN_TASK_ANSWER:
		if (Sip_pCurLcb->iState == CALL_STATE_TALKING)
		{
			if (sMappedPort)
			{
				Sip_pCurLcb->sRtpPort = sMappedPort;
			}
			sip_send_invresp(200);
		}
		break;
	}
}

// Send text message if there is active call
void SipSendMessage(PCHAR pMsg)
{
	UCHAR pBranch[MAX_BRANCH_LEN+1];
	PCHAR pUri;
	USHORT sLen;

	if (Sip_pActiveLcb->iState == CALL_STATE_TALKING) 
	{
		sip_debug(_cSend, SIP_REQ_MESSAGE);
		// compose MESSAGE body
		sLen = strlen(pMsg);
		// compose MESSAGE msg
		pUri = Sip_pActiveLcb->pReqUri;
		sip_add_req_line(SIP_REQ_MESSAGE, pUri);
		sip_new_branch(pBranch);
		sip_add_via(pBranch);
		sip_add_header(_cRoute, Sip_pActiveLcb->pRoute);
		sip_add_call_id(Sip_pActiveLcb->pCallId);
		sip_add_contact();
		sip_add_max_forwards();
		sip_add_user_agent();
		sip_add_proxy_require();
		sip_add_header(_cFrom, Sip_pActiveLcb->pFrom);
		sip_add_header(_cTo, Sip_pActiveLcb->pTo);
		sip_add_cseq(Sip_pActiveLcb->lCurSeq, SIP_REQ_MESSAGE);
		Sip_pActiveLcb->lCurSeq ++;
		sip_add_auth(pUri, SIP_REQ_MESSAGE);
		sip_add_content_type(_cTextPlain);
		sip_add_content_length(sLen);
		line_add_crlf();
		line_add_str(pMsg);

		sip_tr_send_req(Sip_pActiveLcb->pDstIP, Sip_pActiveLcb->sDstPort, pBranch, SIP_REQ_MESSAGE, Sip_pActiveLcb);
	} 
	else 
	{
		UdpDebugString("Skip MESSAGE send (No active call)");
	}
}

#endif


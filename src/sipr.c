/*-------------------------------------------------------------------------
   AR1688 SIP (receive) function copy right information

   Copyright (c) 2006-2012. Tang, Li      <tangli@palmmicro.com>
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
#include "sipr.h"
#include "bank7.h"

const UCHAR _cReplaces_r[] = "replaces";
const UCHAR _cContentType[] = "content-type";
const UCHAR _cTimer[] = "timer";

BOOLEAN sip_tr_recv_ack();

#ifdef OEM_IP20
UCHAR TransferFlag ;
UCHAR TransFlagResp ;
UCHAR atLine3;
extern UDP_SOCKET _pSyslogSocket;
#endif

#ifdef OEM_BT2008
PCHAR _pRecvXControlInd;	//NEC extension
#endif
/**********************************************************************
 ** SIP misc functions                                               **
 **********************************************************************
 */
PCHAR sip_save_str(PCHAR pBuf, USHORT sLen)
{
	PCHAR pStr;

	if (sLen)
	{
		pStr = (char *)malloc(sLen+1);
		if (pStr)
		{
			memcpy(pStr, pBuf, sLen);
			pStr[sLen] = 0;
			return pStr;
		}
	}

	return NULL;
}

UCHAR sip_get_request(PCHAR pBuf)
{
	UCHAR i;

	if (pBuf)
	{
		for (i = 0; i < SIP_REQ_NUMBER; i ++)
		{
			if (!strcmp(pBuf, _cRequest[i]))
			{
				return i;
			}
		}
	}
	return SIP_REQ_UNKNOWN;
}

PCHAR sip_get_token(PCHAR pData, PCHAR pToken)
{
	PCHAR pCur;
	PCHAR pStart;
	UCHAR iLen, iTokenLen;

	if (!pData)		return NULL;

	iTokenLen = strlen(pToken);
	pCur = strchr(pData, ';');
//	pCur = SkipField(pData, ';');	// can not call SkipField here
	while (pCur)
	{
		pCur ++; // pass ';'
		pCur += count_space(pCur);
		if (!memcmp_str(pCur, pToken))
		{
			pCur += iTokenLen;
			pStart = pCur;
			iLen = 0;
			while (*pCur && *pCur != (UCHAR)';' && *pCur != (UCHAR)' ')
			{
				iLen ++;
				pCur ++;
			}
			return sip_save_str(pStart, iLen);
		}
		pCur = strchr(pCur, ';');
//		pCur = SkipField(pCur, ';');
	}

	return NULL;
}

void sip_get_route(PCHAR pRoute[], BOOLEAN bClient)
{
	UCHAR i, j;

	if (bClient)
	{
		for (i = 0; i < Sip_iRecvRRNum; i ++)
		{
			j = Sip_iRecvRRNum - 1 - i;
			pRoute[i] = Sip_pRecvRecordRoute[j];
		}
	}
	else
	{
		for (i = 0; i < Sip_iRecvRRNum; i ++)
		{
			pRoute[i] = Sip_pRecvRecordRoute[i];
		}
	}
}

PCHAR sip_get_uri(PCHAR pHeader, BOOLEAN bStripeParam)
{
	PCHAR pCur;
	PCHAR pTemp;
	UCHAR iLen;

	if (!pHeader)	return NULL;

	pCur = pHeader;
	pTemp = NULL;
	iLen = 0;
	while (*pCur)
	{
		if (!pTemp)
		{
			if (!memcmp_str(pCur, _cTokenSip))
			{
				pTemp = pCur;
				pCur += TOKEN_SIP_LEN;
				iLen += TOKEN_SIP_LEN;
				continue;
			}
		}
		else
		{
			if (*pCur == (UCHAR)'>' || *pCur == (UCHAR)'?' || (bStripeParam && *pCur == (UCHAR)';'))
			{
				break;
			}
			else
			{
				iLen ++;
			}
		}
		pCur ++;
	}

	return sip_save_str(pTemp, iLen);
}

void sip_get_requri(PCHAR pHeader)
{
	PCHAR pUri;

	pUri = sip_get_uri(pHeader, TRUE);
	if (pUri)
	{
		free(Sip_pCurLcb->pReqUri);
		Sip_pCurLcb->pReqUri = pUri;
	}
}

PCHAR sip_get_replaces(PCHAR pHeader)
{
	UCHAR iChar;
	PCHAR pCur;
	PCHAR pTemp;
	UCHAR pBuf[3];
	UCHAR pDst[256];

	pBuf[2] = 0;
	SkipField(pHeader, '>');
	
	pCur = strchr(pHeader, '?');
	while (pCur)
	{
		pCur ++;
		if (!memcmp_str(pCur, "Replaces="))
		{
			pCur += 9;

			SkipField(pCur, '&');

			pTemp = pDst;
			while (*pCur)
			{
				iChar = *pCur ++;
				if (iChar == (UCHAR)'%')
				{
					memcpy(pBuf, pCur, 2);
					iChar = (UCHAR)atoi(pBuf, 16);
					pCur += 2;
				}
				*pTemp ++ = iChar;
			}
			*pTemp = 0;
			return heap_save_str(pDst);
		}
		pCur = strchr(pCur, '&');
	}

	return NULL;
}

void sip_get_dstaddr(PCHAR pUri)
{
	UCHAR pDstIP[IP_ALEN];
	PCHAR pCur;
	PCHAR pTemp;
	PCHAR pColon;

	pCur = (PCHAR)(pUri + TOKEN_SIP_LEN);
	pColon = strchr(pCur, ';');
	if (pColon)
	{
		*pColon = 0;
	}

	pTemp = strchr(pCur, '@');
	if (pTemp)	
	{
		pCur = (PCHAR)(pTemp + 1);
	}
	if (str2ip(pCur, pDstIP))
	{
		memcpy4(Sip_pCurLcb->pDstIP, pDstIP);
	}

	pTemp = strchr(pCur, ':');
	if (pTemp)	
	{
		Sip_pCurLcb->sDstPort = atoi((PCHAR)(pTemp + 1), 10);
	}

	if (pColon)
	{
		pCur = pColon;
		while (pCur)
		{
			pCur ++;
			if (!memcmp_str(pCur, "maddr="))
			{
				pCur += 6;
				if (str2ip(pCur, pDstIP))
				{
					memcpy4(Sip_pCurLcb->pDstIP, pDstIP);
				}
				break;
			}
			pCur = strchr(pCur, ';');
		}
		*pColon = ';';
	}
}

void sip_get_sentby()
{
	PCHAR pCur;

	Sip_sRespDstPort = Sip_sServerPort;
	pCur = Sip_pRecvVia[0];
	if (!pCur)	return;

	//"SIP/2.0/UDP "
	if (strlen(pCur) <= 12)		return;
	pCur += 12;
	while (*pCur && *pCur != (UCHAR)';')
	{
		if (*pCur == (UCHAR)':')
		{
			pCur ++;
			Sip_sRespDstPort = atoi(pCur, 10);
			return;
		}
		pCur ++;
	}
}

void sip_get_target(PCHAR pHeader)
{
	PCHAR pUri;

	pUri = sip_get_uri(pHeader, FALSE);
	if (pUri)
	{
		sip_get_dstaddr(pUri);
		free(pUri);
	}
}

void sip_get_route_uri(BOOLEAN bClient)
{
	PCHAR pCur;
	PCHAR pRoute[16];
	UCHAR i;
	BOOLEAN bHasLR;
	BOOLEAN bTemp;

	bTemp = (!Sys_bRegister || !Sys_bOutboundProxy);
	// The received Contact header is remote target
	/*	The UAC uses the remote target and route set to build the Request-URI and 
		Route header field of the request. If the route set is empty, the UAC MUST 
		place the remote target URI into the Request-URI. The UAC MUST NOT add a 
		Route header field to the request.
	*/
	if (!Sip_iRecvRRNum)
	{
		sip_get_requri(Sip_pRecvContact);
		if (bTemp)
		{
			sip_get_target(Sip_pRecvContact);
		}
		return;
	}

	/*	If the route set is not empty, and the first URI in the route set contains the lr parameter (see Section 19.1.1),
		the UAC MUST place the remote target URI into the Request-URI and MUST include a Route header field
		containing the route set values in order, including all parameters.
	*/
	sip_get_route(pRoute, bClient);
	bHasLR = FALSE;
	pCur = strchr(pRoute[0], ';');
	while (pCur)
	{
		pCur ++;	// pass ';'
		if ((*pCur == (UCHAR)'l' || *pCur == (UCHAR)'L') && (pCur[1] == (UCHAR)'r' || pCur[1] == (UCHAR)'R'))
		{
			bHasLR = TRUE;
			break;
		}
		pCur = strchr(pCur, ';');
	}

	if (bTemp)
	{
		sip_get_target(pRoute[0]);
	}
	if (bHasLR)
	{
		sip_get_requri(Sip_pRecvContact);
	}
	else
	{
		/*	If the route set is not empty, and its first URI does not contain the lr parameter, the UAC MUST place the first
			URI from the route set into the Request-URI, stripping any parameters that are not allowed in a Request-
			URI. The UAC MUST add a Route header field containing the remainder of the route set values in order,
			including all parameters. The UAC MUST then place the remote target URI into the Route header field as
			the last value.
		*/
		sip_get_requri(pRoute[0]);
		// save the remainder of the route set excluding the first one
		for (i = 1; i < Sip_iRecvRRNum; i ++)
		{
			pRoute[i-1] = pRoute[i];
		}
		pRoute[Sip_iRecvRRNum-1] = Sip_pRecvContact;
	}

	line_start(Sip_pTempBuf);
	line_add_str(pRoute[0]);
	for (i = 1; i < Sip_iRecvRRNum; i ++)
	{
		line_add_char(',');
		line_add_str(pRoute[i]);
	}
	free(Sip_pCurLcb->pRoute);
	Sip_pCurLcb->pRoute = heap_save_str(Sip_pTempBuf);
}

UCHAR sip_get_ctype(PCHAR pData)
{
	UCHAR pBuf[40];
	PCHAR pCur;

	memcpy(pBuf, pData, 40);
	pCur = pBuf;
	if (!memcmp_lowercase(pCur, "application/"))
	{
		pCur += 12;
		if (!memcmp_lowercase(pCur, "sdp"))
		{
			return CTYPE_APP_SDP;
		}
		else if (!memcmp_lowercase(pCur, "dtmf-relay"))
		{
			return CTYPE_APP_DTMF;
		}
		else if (!memcmp_lowercase(pCur, "simple-message-summary"))
		{
			return CTYPE_APP_SMS;
		}
	}
	else if (!memcmp_lowercase(pCur, _cMsgFrag))
	{
		return CTYPE_MSG_SIPFRAG;
	}
	else if (!memcmp_lowercase(pCur, _cTextPlain))
	{
		return CTYPE_TEXT_PLAIN;
	}
	else if (!memcmp_lowercase(pCur, _cTextHtml))
	{
		return CTYPE_TEXT_HTML;
	}
	else if (!memcmp_lowercase(pCur, "multipart/"))
	{
		free(Sip_pRecvBoundary);
		Sip_pRecvBoundary = sip_get_token(pData, "boundary=");
		return CTYPE_MULTIPART;
	}
	return CTYPE_UNKNOWN;
}

UCHAR sip_get_etype(PCHAR pBuf)
{
	if (pBuf)
	{
		if (!memcmp_lowercase(pBuf, "refer"))
		{
			return EVENT_TYPE_REFER;
		}
		else if (!memcmp_lowercase(pBuf, "message-summary"))
		{
			return EVENT_TYPE_MSG_SUMMARY;
		}
#ifdef OEM_INNOMEDIA
		else if (!strcmp_lowercase(pBuf, "check-sync"))
		{
			return EVENT_CHECK_SYNC;
		}
#endif
	}
	return EVENT_TYPE_UNKNOWN;
}

BOOLEAN sip_from_proxy()
{
	return (Sys_bRegister && !memcmp(Sip_pServerIP, UdpGetDstIP(Sip_pSocket), IP_ALEN)) ? TRUE : FALSE;
}

void sip_get_cid(PCHAR pName, PCHAR pNumber, PCHAR pSrc)
{
	UCHAR iTemp;
	BOOLEAN bQuoted;
	PCHAR pUri;
	PCHAR pDst;
	PCHAR pCur;

	if (!pSrc)	return;

	pCur = pSrc;
	pUri = NULL;
	while (*pCur)
	{
		if (!memcmp_str(pCur, _cTokenSip))
		{
			pUri = pCur;
			break;
		}
		pCur ++;
	}

	if (!pUri)	return;

	// Get Name
	bQuoted = FALSE;
	pDst = pName;
	while (*pSrc && pSrc != pUri)
	{
		iTemp = *pSrc;
		if (iTemp == (UCHAR)'"')
		{
			if (bQuoted)	break;
			else			bQuoted = TRUE;
		}
		else 
		{	if ((iTemp == (UCHAR)' ' || iTemp == (UCHAR)'<') && !bQuoted)
			{
				break;
			}
			else
			{
				*pDst= iTemp;
				pDst ++;
				if ((USHORT)(pDst - pName) >= MAX_USER_NAME_LEN-1)
				{
					break;
				}
			}
		}
		pSrc ++;
	}
	*pDst = 0;

	// Get Number
	pUri += TOKEN_SIP_LEN;
	pDst = pNumber;
	while (*pUri)
	{
		iTemp = *pUri;
		if (iTemp == (UCHAR)';' || iTemp == (UCHAR)'>' || iTemp == (UCHAR)' ' || iTemp == (UCHAR)':')
		{
			break;
		}
		else if (iTemp == (UCHAR)'@' && sip_from_proxy())
		{
			break;
		}
		else
		{
			*pDst= iTemp;
			pDst ++;
			if ((USHORT)(pDst - pNumber) >= MAX_USER_NUMBER_LEN-1)
			{
				break;
			}
		}
		pUri ++;
	}
	*pDst = 0;
#ifndef OEM_IP20
	if (!strlen(pName))
	{
		strcpy(pName, "++++");
	}
	if (!strlen(pNumber))
	{
		strcpy(pNumber, "++++");
	}
#endif
}

BOOLEAN sip_is_autoanswer()
{
	PCHAR pIntercom;
	
	if (Sip_pRecvTo)
	{
		pIntercom=strchr(Sip_pRecvTo, ';');
		while (pIntercom)
		{
			if (!memcmp_str(pIntercom, ";intercom=true"))
			{
				return TRUE;
			}
			pIntercom=strchr(pIntercom+1, ';');
		}
	}

	if (Sip_pRecvCallInfo)
	{
		pIntercom = Sip_pRecvCallInfo;
		do
		{
			if (!memcmp_str(pIntercom, "answer-after=0"))
			{
				return TRUE;
			}
			pIntercom=strchr(pIntercom,';');
			if (pIntercom)
			{
				pIntercom ++;
				pIntercom += count_space(pIntercom);
			}
		} while(pIntercom);
	}
	return FALSE;
}

BOOLEAN sip_has_token(PCHAR pHeader, PCHAR pToken)
{
	PCHAR pCur;
	PCHAR pTemp;

	if (!pHeader)	return FALSE;

	pCur = pHeader;
	do
	{
		pTemp = strchr(pCur, ',');
		if (!memcmp_str(pCur, pToken))
		{
			return TRUE;
		}
		if (pTemp)
		{
			pCur = pTemp;
			pCur ++;
			pCur += count_space(pCur);
		}
	}while (pTemp);

	return FALSE;
}

BOOLEAN sip_support_require()
{
	PCHAR pCur;
	PCHAR pTemp;

	if (!Sip_pRecvRequire)	return TRUE;

	pCur = Sip_pRecvRequire;
	do
	{
		pTemp = strchr(pCur, ',');
		if (memcmp_str(pCur, _c100rel) && memcmp_str(pCur, _cReplaces_r) && memcmp_str(pCur, _cTimer))
		{
			return FALSE;
		}
		if (pTemp)
		{
			pCur = pTemp;
			pCur ++;
			pCur += count_space(pCur);
		}
	}while (pTemp);

	return TRUE;
}

UCHAR sip_get_header_name(PCHAR pBuf, PCHAR pName)
{
	PCHAR pCur;
	PCHAR pDst;
	UCHAR iLen;

	pCur = pBuf;
	pDst = pName;
	iLen = 0;
	while (*pCur && *pCur != (UCHAR)' ' && *pCur != (UCHAR)':')
	{
		*pDst++ = *pCur++;
		iLen ++;
		if (iLen >= 32)		return 0;
	}
	*pDst = 0;

	while (*pCur == (UCHAR)' ' || *pCur == (UCHAR)':')
	{
		pCur ++;
		iLen ++;
	}
	if (!strlen(pCur))	return 0;

	return iLen;
}


void sip_get_session_expires()
{
	PCHAR pCur;

	Sip_pCurLcb->lMinSE = (Sip_lRecvMinSE) ? Sip_lRecvMinSE : SIP_DEFAULT_MIN_SE;
	Sip_pCurLcb->lSessionExpires = atol(Sip_pRecvSessionExpires, 10);
	pCur = strchr(Sip_pRecvSessionExpires, ';');
	while (pCur)
	{
		pCur ++;
		pCur += count_space(pCur);
		if (!memcmp_str(pCur, "refresher="))
		{
			pCur += 10;
			if (!memcmp_str(pCur, "uac"))
			{
				Sip_pCurLcb->iRefresher = SIP_REFRESHER_UAC;
			}
			else if (!memcmp_str(pCur, "uas"))
			{
				Sip_pCurLcb->iRefresher = SIP_REFRESHER_UAS;
			}
			return;
		}
		pCur = strchr(pCur, ';');
	}
}

/**********************************************************************
 ** SDP functions                                                 **
 **********************************************************************
 */
const UCHAR _cCoderName[VOICE_CODER_NUM][8] = {"pcmu", "pcma", "g726-32", "gsm", "ilbc", "speex", "g729"};

UCHAR RtpNameToCoder(PCHAR pName)
{
	UCHAR i;

	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		if (!strcmp_lowercase(pName, _cCoderName[i]))
		{
			return (UCHAR)(i + 1);
		}
	}
	return VOICE_CODER_NONE;
}

UCHAR RtpPayloadToCoder(UCHAR iPayload)
{
	UCHAR i;

	for (i = 1; i < VOICE_CODER_NUM + 1; i ++)
	{
		if (iPayload == Rtp_pPayLoad[i])
		{
			return i;
		}
	}

	return VOICE_CODER_NONE;
}

BOOLEAN RtpMatchCoder(UCHAR iVoiceCoder)
{
	UCHAR i;

	if (iVoiceCoder == VOICE_CODER_NONE)	return FALSE;

	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		if (iVoiceCoder == Sys_pVoiceCoders[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}

void sip_new_sdp()
{
	SIP_SDP_HANDLE pSdp;

	pSdp = (SIP_SDP_HANDLE)malloc(sizeof(struct sip_sdp));

	if (pSdp)
	{
		Sip_pRecvSdp = pSdp;
		pSdp->bValid = TRUE;
		pSdp->bValidIP = FALSE;
		pSdp->bMediaAudio = FALSE;
		pSdp->iCoderNum = 0;
		memset(pSdp->pCoder, VOICE_CODER_NONE, MAX_SDP_CODER_NUM);
		pSdp->sRtpDstPort = 0;
		pSdp->iVoiceCoder = VOICE_CODER_NONE;
		pSdp->bVad = Sys_bOptVad;
		pSdp->iRemoteMode = AUDIO_MODE_SENDRECV;
		pSdp->iCoderPayload = 255;
		pSdp->iDtmfPayload = 255;
	}
}

BOOLEAN sip_handle_sdp(SIP_SDP_HANDLE pSdp, PCHAR pLine)
{
	UCHAR i, iPayload;
	PCHAR pCur;
	PCHAR pTemp;

	switch (pLine[0])
	{
	case 'v':
		if (strcmp(pLine, "v=0"))
		{
			UdpDebugString("SDP version mismatch");
			return FALSE;
		}
		break;

	case 'o':	// o=<username> <session id> <version> <network type> <address type> <address> 
		if (!pSdp->bValidIP)
		{
			pCur = pLine;
			for (i = 0; i < 5; i ++)
			{
				pCur = SkipField(pCur, ' ');
				if (!pCur)		return FALSE;
			}
			str2ip(pCur, pSdp->pRtpDstIP);
//			UdpDebugString("dst rtp ip");
//			UdpDebugVals(pSdp->pRtpDstIP, IP_ALEN);
		}
		break;

	case 'c':	// c=<network type> <address type> <connection address>
		pCur = pLine;
		for (i = 0; i < 2; i ++)
		{
			pCur = SkipField(pCur, ' ');
			if (!pCur)		return FALSE;
		}
		// <connection address>
		if (!str2ip(pCur, pSdp->pRtpDstIP))
		{
			// address is a domain name;
			UdpDebugString("SDP connection address error");
			return FALSE;
		}
		else
		{
//			UdpDebugString("rtp dst ip");
//			UdpDebugVals(pSdp->pRtpDstIP, IP_ALEN);
			pSdp->bValidIP = TRUE;
		}
		break;

	case 'm':	// m=<media> <port> <transport> <fmt list>
		// m=<media>
		pCur = pLine;
		if (!memcmp_str(pCur, "m=audio "))
		{
			pSdp->bMediaAudio = TRUE;
			pCur += 8;
		}
		else
		{
			pSdp->bMediaAudio = FALSE;
			break;
		}

		// <port>
		pTemp = SkipField(pCur, ' ');
		if (!pTemp)		return FALSE;
		pSdp->sRtpDstPort = atoi(pCur, 10);
//		UdpDebugString("rtp dst port");
//		UdpDebugVal(pSdp->sRtpDstPort, 10);
		pCur = pTemp;

		// <transport>
		pCur = SkipField(pCur, ' ');
		if (!pCur)		return FALSE;

		// <fmt list>
		do
		{
			pTemp = SkipField(pCur, ' ');
			pSdp->pPayload[pSdp->iCoderNum] = (UCHAR)atoi(pCur, 10);
			pSdp->iCoderNum ++;
			if (pSdp->iCoderNum >= MAX_SDP_CODER_NUM)	
			{
				break;
			}
			pCur = pTemp;
		} while (pTemp);							
		break;

	case 'a':
		if (pSdp->bMediaAudio)
		{
			pCur = (PCHAR)(pLine + 2);
			if (!memcmp_str(pCur, "rtpmap:"))
			{
				pCur += 7;
				iPayload = (UCHAR)atoi(pCur, 10);
				pCur = SkipField(pCur, ' ');
				if (!pCur)	break;

				pTemp = strchr(pCur, '/');
				if (pTemp)	*pTemp = 0;
				pTemp ++;
				if (memcmp_str(pTemp, "8000"))
				{
					break;
				}
				if (!strcmp_lowercase(pCur, "telephone-event"))
				{
					pSdp->iDtmfPayload = iPayload;
					break;
				}
				else
				{
					for (i = 0; i < pSdp->iCoderNum; i ++)
					{
						if (pSdp->pPayload[i] == iPayload)
						{
							pSdp->pCoder[i] = RtpNameToCoder(pCur);
							break;
						}
					}
				}
			}
			else if (!memcmp_str(pCur, "fmtp:"))
			{
				pCur += 5;
				iPayload = (UCHAR)atoi(pCur, 10);
				if (iPayload == RTP_G729)
				{
					pCur = SkipField(pCur, ' ');
					if (!pCur)	break;

					if (!strcmp(pCur, "annexb=no"))
					{
#ifdef OEM_ROIP
						UdpDebugString("Warning: G729B not supported in signalling");
#else
						pSdp->bVad = FALSE;
#endif
					}
				}
				break;
			}
			else
			{
				for (i = 0; i < AUDIO_MODE_NUM; i ++)
				{
					if (!strcmp(pCur, _cAudioMode[i]))
					{
						pSdp->iRemoteMode = i;
						break;
					}
				}			
			}
		}
		break;
	}

	return TRUE;
}

void sip_check_sdp(SIP_SDP_HANDLE pSdp)
{
	UCHAR i, iCoder, iPayload;

	for (i = 0; i < pSdp->iCoderNum; i ++)
	{
		iCoder = pSdp->pCoder[i];
		iPayload = pSdp->pPayload[i];
		if (iCoder == VOICE_CODER_NONE && iPayload < RTP_REDUNDENCE)
		{
			iCoder = RtpPayloadToCoder(iPayload);
		}
		if (RtpMatchCoder(iCoder))
		{
			pSdp->iVoiceCoder = iCoder;
			pSdp->iCoderPayload = iPayload;
			break;
		}
	}

	if (pSdp->iVoiceCoder == VOICE_CODER_NONE)
	{
		UdpDebugString("Voice coder mismatch!");
		pSdp->bValid = FALSE;
		return;
	}

	if (!IsValidIP(pSdp->pRtpDstIP))
	{
		pSdp->iRemoteMode &= ~AUDIO_MODE_RECVONLY;
	}
}

void sip_copy_sdp()
{
	memcpy(&Sip_pCurLcb->iVoiceCoder, &Sip_pRecvSdp->iVoiceCoder, SIP_SDP_LENGTH);
}
/**********************************************************************
 ** sipr line functions                                              **
 **********************************************************************
*/
SIP_LCB_HANDLE sip_match_callid()
{
	UCHAR i;
	PCHAR pCallId;
	SIP_LCB_HANDLE pLcb;

	if (Sip_pRecvCallId)
	{
		pLcb = Sip_pLCB;
		for (i = 0; i < LCB_TSIZE; i ++, pLcb ++)
		{
			pCallId = pLcb->pCallId;
			if (pCallId && !strcmp(pCallId, Sip_pRecvCallId))
			{
				return pLcb;
			}
		}
	}
	return NULL;
}

SIP_LCB_HANDLE sipr_ln_alt()
{
	SIP_LCB_HANDLE pLcb;

	pLcb = Sip_pLCB;
	if (pLcb == Sip_pActiveLcb)
	{
		pLcb ++;
	}
	return pLcb;
}

void sipr_show_cid(PCHAR pName, PCHAR pNumber, BOOLEAN bShowLine0)
{
#ifdef OEM_IP20
	UCHAR pNumName[64];
#endif

#ifdef UI_4_LINES
	bShowLine0;
#ifdef OEM_IP20
	strcpy(pNumName, pNumber);
	if(strcmp(pName,""))
	strcat_char(pNumName, '<');
	strcat(pNumName, pName);
	if(strcmp(pName,""))
	strcat_char(pNumName, '>');

	TaskUIHandler((atLine3) ? UI_CALL_DISPLAY_LINE3 : UI_CALL_DISPLAY_LINE2, (USHORT)pNumName);	
#else
	TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pName);
	if (strcmp(pName, pNumber))
	{
		TaskUIHandler(UI_CALL_DISPLAY_LINE2, (USHORT)pNumber);
	}
#endif
	
#else
	if (bShowLine0 && strcmp(pName, pNumber))
	{
		TaskUIHandler(UI_CALL_DISPLAY_LINE0, (USHORT)pName);
	}
	TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pNumber);
#endif
}
/**********************************************************************
 ** received msg handling functions                                  **
 **********************************************************************
*/

void sip_display_msg()
{
	TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);
	if (Sip_iRecvBodyLine)
	{
		TaskUIHandler(UI_CALL_DISPLAY_MSG, (USHORT)Sip_pRecvBody);
	}
}

void sip_notify_req()
{
	UCHAR i, iLen;
	PCHAR pCur;
	UCHAR pHeader[32];
	USHORT sResp;
	UCHAR pMsg[2];
	BOOLEAN bPostMsg;

#ifdef OEM_INNOMEDIA
	if (Sip_iRecvEvent == EVENT_CHECK_SYNC)
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);	
		TaskStartProv();
		return;
	}
#endif

	if (Sip_iRecvEvent == EVENT_TYPE_REFER)
	{
		if (!Sip_pCurLcb)
		{
			TaskHandleEvent(SIPR_EVENT_SEND_RESP, 400);
			return;
		}
		if (Sip_iContentType != CTYPE_MSG_SIPFRAG)
		{
			TaskHandleEvent(SIPR_EVENT_SEND_RESP, 415);
			return;
		}
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);	
		if (Sip_iRecvBodyLine && Sip_pCurLcb->iState == CALL_STATE_TALKING && Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEROR)
		{
			pCur = Sip_pRecvBody[0];
			if (!memcmp_str(pCur, "SIP/2.0 "))
			{
				pCur += 8;
				sResp = atoi(pCur, 10);
				if (sResp >= 200)
				{
					Sip_pCurLcb->iXferPart = XFER_PART_NONE;
					if (sResp < 300)
					{
						TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_BYE);
						Sip_pCurLcb->iState = CALL_STATE_HANGUP;
						if (Sip_pCurLcb == Sip_pActiveLcb)
						{
							TaskUIHandler(UI_CALL_ENDED, CALL_END_XFER_DONE);
						}
					}
					else
					{
						if (Sip_pCurLcb == Sip_pActiveLcb)
						{
							Sip_pCurLcb->iXferPart = XFER_PART_FAIL;
							Sip_pCurLcb->iTimer = 0;
							TaskUIHandler(UI_CALL_XFERFAIL, 0);
//							TaskHandleEvent(UI_EVENT_HOLD, 0);
						}
					}
				}
			}
		}
	}
	else if (Sip_iRecvEvent == EVENT_TYPE_MSG_SUMMARY)
	{
		if (Sip_iContentType == CTYPE_APP_SMS || Sip_iContentType == CTYPE_TEXT_PLAIN)	
		{
			TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);
			bPostMsg = FALSE;
			pMsg[1] = 0;
			pMsg[0] = 0;
			for (i = 0; i < Sip_iRecvBodyLine; i ++)
			{
				pCur = Sip_pRecvBody[i];
				iLen = sip_get_header_name(pCur, pHeader);
				if (iLen)
				{
					pCur += iLen;
					if (!strcmp_lowercase(pHeader, "messages-waiting"))
					{
						if (!strcmp_lowercase(pCur, "yes"))
						{
							pMsg[1] = 0xff;
							pMsg[0] = 0xff;
						}
						bPostMsg = TRUE;
					}
					else if (!strcmp_lowercase(pHeader, "voice-message"))
					{
						pMsg[1] = atoi(pCur, 10);
						pCur = SkipField(pCur, '/');
						if (pCur)
						{
							pMsg[0] = atoi(pCur, 10);
						}

						bPostMsg = TRUE;
					}
				}
			}
			if (bPostMsg)
			{
				TaskUIHandler(UI_VOICE_MESSAGE, (USHORT)pMsg);
			}
			else
			{
				sip_display_msg();
			}
		}
		else
		{
			TaskHandleEvent(SIPR_EVENT_SEND_RESP, 415);
		}
	}
	else
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 400);
	}
}

void sip_message_req()
{
	if (Sip_iContentType == CTYPE_TEXT_PLAIN)
	{
		sip_display_msg();
	}
	else if (Sip_iContentType == CTYPE_TEXT_HTML)
	{
		sip_display_msg();
	}
	else
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 415);
	}
}

void sip_options_req()
{
	UCHAR i;
	BOOLEAN bBusy;
	SIP_LCB_HANDLE pLcb;

	pLcb = Sip_pLCB;
	bBusy = TRUE;
	for (i = 0; i < LCB_TSIZE; i ++, pLcb ++)
	{
		if (pLcb->iState == CALL_STATE_FREE)
		{
			bBusy = FALSE;
			break;
		}
	}

	TaskHandleEvent(SIPR_EVENT_SEND_RESP, (bBusy) ? 486 : 200);
}

void sip_update_req()
{
	USHORT sResp;

	if (Sip_pRecvSessionExpires)
	{
		sip_get_session_expires();	
		Sip_pCurLcb->lSessionTimer = 0;
	}
	if (Sip_pRecvSdp)
	{
		if (Sip_pRecvSdp->bValid)
		{
			if (Sip_pCurLcb == Sip_pActiveLcb && Sip_pRecvSdp->iVoiceCoder != Sip_pCurLcb->iVoiceCoder)
			{
				TaskUIHandler(UI_CALL_UPDATE_CODEC, (USHORT)(&Sip_pRecvSdp->iVoiceCoder));
			}
			sip_copy_sdp();
			sResp = 200;
			Sip_pCurLcb->iSdpType = SDP_TYPE_ANSWER;
		}
		else
		{
			sResp = 415;
		}
	}
	else
	{
		sResp = 200;
		Sip_pCurLcb->iSdpType = SDP_TYPE_OFFER;
	}
	TaskHandleEvent(SIPR_EVENT_SEND_RESP, sResp);
}

void sip_reinvite_req()
{
	sip_update_req();
	Sip_pCurLcb->pInvTcb = Sip_pCurTcb;
/*	if (sResp != 200)
	{
		Sip_pCurTcb->iState = TRS_COMPLETED;
	}
*/
}

void sip_save_header()
{
	UCHAR i;

	if (!Sip_pCurLcb->pInvHeader)
	{
		// save INVITE header for later reponse
		line_start(Sip_pTempBuf);
		for (i = 0; i < Sip_iRecvViaNum; i ++)
		{
			line_add_str("Via: ");
			line_add_str_with_crlf(Sip_pRecvVia[i]);
		}
		for (i = 0; i < Sip_iRecvRRNum; i ++)
		{
			line_add_str("Record-Route: ");
			line_add_str_with_crlf(Sip_pRecvRecordRoute[i]);
		}
		line_add_str("CSeq: ");
		line_add_str_with_crlf(Sip_pRecvCSeq);
		Sip_pCurLcb->pInvHeader = heap_save_str(Sip_pTempBuf);
	}
}

UCHAR sip_match_replaces()
{
	PCHAR pCur;
	PCHAR pTemp;
	BOOLEAN bEarlyOnly;

	pCur = Sip_pRecvReplaces;
	pTemp = SkipField(pCur, ';');
	if (!pTemp)	return 0;
	if (strcmp(pCur, Sip_pActiveLcb->pCallId))	return 0;

	bEarlyOnly = FALSE;
	while(pTemp)
	{
		pCur = pTemp;
		pTemp = SkipField(pCur, ';');
		if (!memcmp_str(pCur, "to-tag="))
		{
			pCur += 7;
			if (strcmp(pCur, Sip_pActiveLcb->pFromTag))	return 0;
		}
		else if (!memcmp_str(pCur, "from-tag="))
		{
			pCur += 9;
			if (strcmp(pCur, Sip_pActiveLcb->pToTag))	return 0;
		}
		else if (!strcmp(pCur, "early-only"))
		{
			bEarlyOnly = TRUE;
		}
	}
	return (bEarlyOnly)	? 1 : 2;
}

void sip_invite_req()
{
	USHORT sResp;
	UCHAR pName[MAX_USER_NAME_LEN];
	UCHAR pNumber[MAX_USER_NUMBER_LEN];
	BOOLEAN bAutoAnswer;
	BOOLEAN bWaiting;
	UCHAR i, iVoiceCoder, iActiveState, iMatchReplaces;
	PCHAR pDstIP;
	PCHAR pDst;
	struct sip_lcb lcbReplace;
	SIP_LCB_HANDLE pReplaceLcb;

	// display caller id
	pName[0] = 0;
	pNumber[0] = 0;
	sip_get_cid(pName, pNumber, Sip_pRecvFrom);

	// check if we are able to accept call
	if (Sys_bDnd ||(Sys_iCurMode != SYS_MODE_STANDBY && Sys_iCurMode != SYS_MODE_CALL))
	{
		sResp = 486;
		goto Reject;
	}
	// check if forward immediatley
	if (Sys_bFwdAlways)
	{
		sResp = 302;
		goto Reject;
	}
	// check codec compatibility
	iVoiceCoder = VOICE_CODER_NONE;
	if (Sip_pRecvSdp)
	{
		if (Sip_pRecvSdp->bValid)
		{
			iVoiceCoder = Sip_pRecvSdp->iVoiceCoder;
		}
		else
		{
			sResp = 415;
			goto Reject;
		}
	}
	// check if we are busy
	bWaiting = FALSE;
#ifdef OEM_IP20
	atLine3 = 0;
#endif
	bAutoAnswer = FALSE;
	pReplaceLcb = NULL;
	iActiveState = Sip_pActiveLcb->iState;
	if (Sip_pRecvReplaces)
	{
#ifdef OEM_IP20
		if (Sip_pActiveLcb->iState != CALL_STATE_TALKING && Sip_pActiveLcb->iState != CALL_STATE_CALLING2)
		{
			TransferFlag = 1;
			Sip_pCurLcb = sipr_ln_alt();
			Sip_pActiveLcb = Sip_pCurLcb;
			Sip_pCurLcb->iXferPart = XFER_PART_TRANSFERTO;
				
			Sip_pCurLcb = Sip_pActiveLcb;
			sResp = 183;		
			
			if (Sip_pRecvSdp->bValid)
			{
				sip_copy_sdp();
			}				
			goto Forward;			
		}
#else
		if (iActiveState != CALL_STATE_TALKING && iActiveState != CALL_STATE_CALLING2)
		{
			sResp = 603;
			goto Reject;
		}
#endif	
	
		iMatchReplaces = sip_match_replaces();
		if (!iMatchReplaces)
		{
			sResp = 481;
			goto Reject;
		}
		if (iActiveState == CALL_STATE_TALKING)
		{
			if (iMatchReplaces == 1)
			{
				// include "early-only" tag
				sResp = 486;
				goto Reject;
			}
		}
		else
		{
			if (Sip_pActiveLcb->bCallee)
			{
				sResp = 481;
				goto Reject;
			}
		}
		Sip_pCurLcb = Sip_pActiveLcb;
		pReplaceLcb = (SIP_LCB_HANDLE)&lcbReplace;
		memcpy((PCHAR)pReplaceLcb, (PCHAR)Sip_pCurLcb, SIP_LCB_LENGTH);
		memset((PCHAR)Sip_pCurLcb, 0, SIP_LCB_LENGTH);
		sResp = 200;
		goto End;
	}

	if (Sip_pActiveLcb->iState == CALL_STATE_FREE)
	{
		// Huawei softswitch and uc dont's play ringback tone if 180 with sdp is sent.
		Sip_pCurLcb = Sip_pActiveLcb;
#ifndef OEM_BT2008
		if (Sip_pRecvSdp && (Sip_pRecvSdp->iRemoteMode & AUDIO_MODE_RECVONLY))
		{
			sResp = 183;
		}
		else
#endif
		{
			sResp = 180;
		}
	}
	else
	{
		Sip_pCurLcb = sipr_ln_alt();
		if (Sys_bCallWaiting && Sip_pCurLcb->iState == CALL_STATE_FREE)
		{
			bWaiting = TRUE;
			sResp = 180;
		}
		else
		{
			sResp = (Sys_bFwdBusy) ? 302 : 486;
			goto Reject;
		}
	}
#ifdef OEM_IP20
Forward:
#endif
	strcpy(Sip_pCurLcb->pPeerName, pName);
	strcpy(Sip_pCurLcb->pPeerNumber, pNumber);
	if (bWaiting)
	{
		TaskUIHandler(UI_CALL_WAITING, 0);
#ifdef OEM_IP20
		atLine3 = 1;
#endif
		sipr_show_cid(pName, pNumber, FALSE);
	}
	else
	{
		bAutoAnswer = sip_is_autoanswer() || (Sys_bAutoAnswer && !Sys_iNoanswerTimeout);
		if (bAutoAnswer)
		{
			sResp = 200;
		}
		else
		{
			TaskUIHandler(UI_CALL_RINGING, iVoiceCoder);
			sipr_show_cid(pName, pNumber, TRUE);
		}
	}

End:
	// Save SDP information
	if (iVoiceCoder == VOICE_CODER_NONE)
	{
		Sip_pCurLcb->iSdpType = SDP_TYPE_OFFER;
	}
	else
	{
		sip_copy_sdp();
		Sip_pCurLcb->iSdpType = SDP_TYPE_ANSWER;
	}
	// save line information
	Sip_pCurTcb->pLcb = Sip_pCurLcb;
	Sip_pCurLcb->pInvTcb = Sip_pCurTcb;
	Sip_pCurLcb->bCallee = TRUE;

	if (Sip_pRecvCallId)	Sip_pCurLcb->pCallId = heap_save_str(Sip_pRecvCallId);
	if (Sip_pRecvTo)		
	{
		Sip_pCurLcb->pFrom = heap_save_str(Sip_pRecvTo);
		Sip_pCurLcb->pFromTag = sip_get_token(Sip_pRecvTo, _cTokenTag);
	}
	if (Sip_pRecvFrom)		
	{
		Sip_pCurLcb->pTo = heap_save_str(Sip_pRecvFrom);
		Sip_pCurLcb->pToTag = sip_get_token(Sip_pRecvFrom, _cTokenTag);
	}
	Sip_pCurLcb->lCurSeq = rand();
	if (Sip_pRecvCSeq)	Sip_pCurLcb->lInvSeq = atol(Sip_pRecvCSeq, 10);

//	sip_new_random(Sip_pCurLcb->pSdpSessionId, 8);
	pDst = Sip_pCurLcb->pSdpSessionId;
	for (i = 0; i < 8; i ++)
	{
		*pDst = mod_16x8((USHORT)rand(), 10) + '0';
		pDst ++;
	}
	*pDst = 0;
	Sip_pCurLcb->lSdpVersion = Sys_lCurrentTime; 
	Sip_pCurLcb->iLocalMode = AUDIO_MODE_SENDRECV;
	rand_array(Sip_pCurLcb->pRtpSSRC, RTP_SSRC_LEN);
	Sip_pCurLcb->sRtpSeq = P_MAKEWORD(rand(), rand());
	Sip_pCurLcb->lRtpTimeStamp = 0;
	Sip_pCurLcb->lRtpOffset = Sys_lTicker;
	Sip_pCurLcb->bVoiceStart = TRUE;
	Sip_pCurLcb->sRtpPort = Sip_sRtpPort;
	TaskHandleEvent(SIPR_EVENT_MODIFY_RTPPORT, 0);
	Sip_pCurLcb->pRtpSocket = UdpListen(Sip_pCurLcb->sRtpPort, SIPR_BANK_OFFSET, (USHORT)RtpRun);

	pDstIP = (Sys_bRegister && Sys_bOutboundProxy) ? Sip_pServerIP : UdpGetDstIP(Sip_pSocket);
	memcpy4(Sip_pCurLcb->pDstIP, pDstIP);
	Sip_pCurLcb->sDstPort = Sip_sServerPort;
	sip_get_route_uri(FALSE);
	if (sip_has_token(Sip_pRecvSupported, _cTimer))
	{
		if (Sip_pRecvSessionExpires)
		{
			sip_get_session_expires();
			if (Sip_pCurLcb->iRefresher == SIP_REFRESHER_NONE)
			{
				Sip_pCurLcb->iRefresher = SIP_REFRESHER_UAS;
			}
		}
		else
		{
			Sip_pCurLcb->lSessionExpires = SIP_DEFAULT_SESSION_EXPIRE;
			Sip_pCurLcb->lMinSE = SIP_DEFAULT_MIN_SE;
			Sip_pCurLcb->iRefresher = SIP_REFRESHER_UAS;
		}
		Sip_pCurLcb->lSessionTimer = 0;
	}
	else
	{
		Sip_pCurLcb->lSessionExpires = 0;
		Sip_pCurLcb->iRefresher = SIP_REFRESHER_NONE;
	}

	sip_save_header();
	memcpy4(Sip_pCurLcb->pRespDstIP, UdpGetDstIP(Sip_pSocket));
	Sip_pCurLcb->sRespDstPort = Sip_sRespDstPort;

	if (sResp < 200)
	{
		Sip_pCurLcb->iState = CALL_STATE_RINGING;
		if (sip_has_token(Sip_pRecvRequire, _c100rel) || (Sys_bPrack && sip_has_token(Sip_pRecvSupported, _c100rel)))
		{
			Sip_pCurTcb->b100rel = TRUE;
			Sip_pCurLcb->lRseq = P_MAKEWORD(rand(), rand());
		}
		else
		{
			Sip_pCurLcb->lRseq = 0;
		}
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, sResp);
	}
	else
	{
		if (bAutoAnswer)
		{
			TaskUIHandler(UI_CALL_AUTOANSWER, 0); 
		}
		TaskHandleEvent(SIPR_EVENT_AUTO_ANSWER, 0);
		if (pReplaceLcb)
		{
			Sip_pCurLcb = pReplaceLcb;
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, (iActiveState == CALL_STATE_TALKING) ? SIP_REQ_BYE : SIP_REQ_CANCEL);
			Sip_pCurLcb->pInvTcb->pLcb = NULL;
			TaskHandleEvent(SIPR_EVENT_LINE_FREE, 0);
		}
	}
	return;

Reject:
	TaskHandleEvent(SIPR_EVENT_SEND_RESP, sResp);
	Sip_pCurTcb->iState = TRS_COMPLETED;
	TaskCallLogAdd(pName, pNumber, CALL_LOG_TYPE_MISSED);
}

void sip_ack_req()
{
	if (sip_tr_recv_ack())
	{
		if (Sip_pCurLcb->iState == CALL_STATE_HANGUP)
		{
			TaskHandleEvent(SIPR_EVENT_LINE_FREE, 0);
		}
		else if (Sip_pCurLcb->iState == CALL_STATE_TALKING)
		{
			if (Sip_pRecvSdp && Sip_pRecvSdp->bValid)
			{
				if (Sip_pCurLcb == Sip_pActiveLcb && Sip_pRecvSdp->iVoiceCoder != Sip_pCurLcb->iVoiceCoder)
				{
					TaskUIHandler(UI_CALL_DECODE_ENCODE, (USHORT)(&Sip_pRecvSdp->iVoiceCoder));
				}
				sip_copy_sdp();
			}
			else
			{
				if (Sip_pCurLcb->iVoiceCoder == VOICE_CODER_NONE)
				{
					TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_BYE);
					Sip_pCurLcb->iState = CALL_STATE_HANGUP;
					if (Sip_pCurLcb == Sip_pActiveLcb)
					{
						TaskUIHandler(UI_CALL_ENDED, CALL_END_CODER_MISMATCH);
					}			
				}
			}
		}
	}
}

void sip_cancel_req()
{
	TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);
	TaskHandleEvent(SIPR_EVENT_CALL_CANCELED, 0);
}

void sip_call_forwarded(PCHAR pUri)
{
	Sip_pCurLcb->iState = CALL_STATE_CALLING2;
	Sip_pCurLcb->pReqUri = pUri;
	Sip_pCurLcb->pInvUri = heap_save_str(pUri);
	if (Sys_bRegister)
	{
		memcpy4(Sip_pCurLcb->pDstIP, Sip_pServerIP);
	}
	Sip_pCurLcb->sDstPort = SIP_SERVER_PORT;
	sip_get_dstaddr(pUri);
	TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_CALL_FORWARDED);
	sipr_show_cid(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, FALSE);
	TaskHandleEvent(SIPR_EVENT_CALL_FORWARDED, 0);
}

void sip_refer_req()
{
	SIP_LCB_HANDLE pAltLcb;
	PCHAR pReplaces;
	PCHAR pReferedBy;
	PCHAR pUri;

	pAltLcb = sipr_ln_alt();
	if (pAltLcb->iState != CALL_STATE_FREE)
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 488);
		return;
	}
	if (!Sip_pRecvReferTo)
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 488);
		return;
	}
	pUri = sip_get_uri(Sip_pRecvReferTo, FALSE);
	if (!pUri)
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 488);
		return;
	}
	Sip_pCurLcb->iXferPart = XFER_PART_TRANSFEREE_ORIG;
	pReferedBy = (Sip_pRecvReferredBy) ? heap_save_str(Sip_pRecvReferredBy) : NULL;
	pReplaces = sip_get_replaces(Sip_pRecvReferTo);

	TaskHandleEvent(SIPR_EVENT_SEND_RESP, 202);

	Sip_pActiveLcb = pAltLcb;
	Sip_pCurLcb = Sip_pActiveLcb;
	Sip_pCurLcb->pReplaces = pReplaces;
	Sip_pCurLcb->pReferredBy = pReferedBy;
#ifdef OEM_IP20
	Sip_pCurLcb->iState = CALL_STATE_DIALING;
#endif
	Sip_pCurLcb->iXferPart = XFER_PART_TRANSFEREE_NEW;
	sip_get_cid(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, pUri);
	sip_call_forwarded(pUri);
#ifdef OEM_IP20
	TaskHandleEvent(SIPR_EVENT_XFEREE_DONE, 200);	//added for blind transfer
#endif
}

void sip_info_req()
{
	UCHAR i, iVal;
	PCHAR pCur;
#ifdef OEM_BT2008
	UCHAR pMsg[2];
#endif

	if (!Sip_sContentLen)
	{
#ifdef OEM_BT2008
		if (_pRecvXControlInd)
		{
			pCur = _pRecvXControlInd;
			if (!memcmp_lowercase(pCur, "message-waiting-indication "))
			{
				pCur += 27;
				if (!strcmp(pCur, "off"))
				{
					pMsg[0] = 0;
					pMsg[1] = 0;
					TaskUIHandler(UI_VOICE_MESSAGE, (USHORT)pMsg);
				}
				else if (!strcmp(pCur, "on"))
				{
					pMsg[0] = 0xff;
					pMsg[1] = 0xff;
					TaskUIHandler(UI_VOICE_MESSAGE, (USHORT)pMsg);
				}
			}
		}
#endif
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);
		return;
	}
	if (Sip_iContentType == CTYPE_APP_DTMF)
	{
		if (Sip_pCurLcb)
		{
			if (Sip_iRecvBodyLine && Sip_pCurLcb == Sip_pActiveLcb && (Sip_pCurLcb->iLocalMode & AUDIO_MODE_RECVONLY))
			{
				for (i = 0; i < Sip_iRecvBodyLine; i ++)
				{
					pCur = Sip_pRecvBody[i];
					if (!memcmp_lowercase(pCur, "signal"))
					{
						pCur += 6;
						pCur = strchr(pCur, '=');
						if (pCur)
						{
							pCur ++;
							pCur += count_space(pCur);
							iVal = pCur[0];
							if (iVal == (UCHAR)'1')								// added by Matthias Siegele
							{
								if (pCur[1] == (UCHAR)'1')			iVal = '#';
								else if (pCur[1] == (UCHAR)'0')	iVal = '*';
							}
							TaskUIHandler(UI_CALL_INCOMING_DTMF, (USHORT)iVal);
						}
					}
				}
			}
			TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);
		}
		else
		{
			TaskHandleEvent(SIPR_EVENT_SEND_RESP, 481);
		}
	}
	else if (Sip_iContentType == CTYPE_TEXT_PLAIN)
	{
		sip_display_msg();
	}
	else if (Sip_iContentType == CTYPE_TEXT_HTML)
	{
		sip_display_msg();
	}
	else
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 415);
	}
}

void sip_prack_req()
{
	SIP_TCB_HANDLE pTcb;

	pTcb = Sip_pCurLcb->pInvTcb;
	if (pTcb->b100rel)
	{
		if (Sip_pRecvRAck)
		{
			if (Sip_pCurLcb->lRseq == atol(Sip_pRecvRAck, 10))
			{
				pTcb->b100rel = FALSE;
				TaskHandleEvent(SIPR_EVENT_SEND_RESP, 200);
				return;
			}
		}
	}
	TaskHandleEvent(SIPR_EVENT_SEND_RESP, 481);
}

void sip_rebuild_to()
{
	PCHAR pSavedTag;
	PCHAR pBuf;
	UCHAR pNewTag[64];
	PCHAR pTag;

	pSavedTag = sip_get_token(Sip_pRecvTo, _cTokenTag);
	if (pSavedTag)
	{
		free(pSavedTag);
	}
	else
	{
		if (Sip_pCurLcb)
		{
			pTag = Sip_pCurLcb->pFromTag;
		}
		else
		{
			TaskHandleEvent(SIPR_EVENT_NEW_TAG, (USHORT)pNewTag);
			pTag = pNewTag;
		}
		if (pTag)
		{
			pBuf = (char *)malloc(strlen(Sip_pRecvTo)+strlen(pTag)+6);
			if (pBuf)
			{
				strcpy(pBuf, Sip_pRecvTo);
				strcat(pBuf, ";tag=");
				strcat(pBuf, pTag);
				free(Sip_pRecvTo);
				Sip_pRecvTo = pBuf;
			}
		}
	}
}

void sip_handle_req()
{
	if (!sip_support_require())
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 420);
		return;
	}

	Sip_pCurLcb = sip_match_callid();
	if (Sip_pRecvTo)
	{
		sip_rebuild_to();
	}
	if (Sip_iRecvMethod == SIP_REQ_NOTIFY)
	{
		sip_notify_req();
	}
	else if (Sip_iRecvMethod == SIP_REQ_MESSAGE)
	{
		sip_message_req();
	}
	else if (Sip_iRecvMethod == SIP_REQ_INFO)
	{
		sip_info_req();
	}
	else if (Sip_iRecvMethod == SIP_REQ_OPTIONS)
	{
		sip_options_req();
	}
	else if (Sip_iRecvMethod == SIP_REQ_SUBSCRIBE)
	{
		TaskHandleEvent(SIPR_EVENT_SEND_RESP, 400);
	}
	else
	{
		if (Sip_iRecvMethod == SIP_REQ_INVITE)
		{
			if (Sip_pCurLcb)	
			{
				sip_reinvite_req();
			}
			else
			{
				sip_invite_req();
			}
		}
		else if (Sip_iRecvMethod == SIP_REQ_ACK)
		{
			sip_ack_req();
		}	
		else
		{
			if (Sip_pCurLcb)
			{
				switch (Sip_iRecvMethod)
				{
				case SIP_REQ_BYE:
				case SIP_REQ_CANCEL:
					sip_cancel_req();
					break;

				case SIP_REQ_REFER:
					sip_refer_req();
					break;

				case SIP_REQ_PRACK:
					sip_prack_req();
					break;

				case SIP_REQ_UPDATE:
					sip_update_req();
					break;
				}	
			}
			else
			{
				TaskHandleEvent(SIPR_EVENT_SEND_RESP, 481);
			}
		}
	}
}

void sip_reg_ttl()
{
	PCHAR pCur;

	if (Sip_pRecvContact)
	{
		pCur = strchr(Sip_pRecvContact, ';');
		while (pCur)
		{
			pCur ++;
			pCur += count_space(pCur);
			if (!memcmp_str(pCur, "expires="))
			{
				pCur += 8;
				Sip_sRegTimeout = atoi(pCur, 10);
				return;
			}
			pCur = strchr(pCur, ';');
		}
	}

	if (Sip_sRecvExpires != USHORT_MAX)
	{
		Sip_sRegTimeout = Sip_sRecvExpires;
	}
}

void sip_reg_resp()
{
	UCHAR pUri[TOKEN_SIP_LEN+MAX_DNS_LEN];
	UCHAR pAuth[255];

	if (Sip_sRecvResp < 200)	return;

	if (Sip_sRecvResp == 401 || Sip_sRecvResp == 407)
	{
		if (Sip_iRegAuthRetry < SIP_MAX_AUTH_RETRY)
		{
			Sip_iRegAuthRetry ++;
			strcpy(pUri, _cTokenSip);
			strcpy((PCHAR)(pUri + TOKEN_SIP_LEN), Sys_pSipDomain);
			if (Sip_pRecvWWWAuth)
			{
				if (Sip_pRegWWWAuth)	free(Sip_pRegWWWAuth);
				TaskHttpDigest(pAuth, Sip_pRecvWWWAuth, pUri, Sys_pAuthID, Sys_pAuthPin, _cRequest[SIP_REQ_REGISTER]);
				Sip_pRegWWWAuth = heap_save_str(pAuth);
			}
			if (Sip_pRecvProxyAuth)
			{
				if (Sip_pRegProxyAuth)	free(Sip_pRegProxyAuth);
				TaskHttpDigest(pAuth, Sip_pRecvProxyAuth, pUri, Sys_pAuthID, Sys_pAuthPin, _cRequest[SIP_REQ_REGISTER]);
				Sip_pRegProxyAuth = heap_save_str(pAuth);
			}
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_REGISTER);
			return;
		}
	}
	else if (Sip_sRecvResp == 423)
	{
		if (Sip_sRecvMinExpires != USHORT_MAX)
		{
			Sip_sRegTimeout = Sip_sRecvMinExpires;
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_REGISTER);
			return;
		}
	}
	else if (Sip_sRecvResp >= 200 && Sip_sRecvResp < 300)
	{
		sip_reg_ttl();
		if (Sip_sRegTimeout)
		{
			TaskHandleEvent(SIPR_EVENT_LOGON_OK, 0);
			return;
		}
	}

	TaskHandleEvent(SIPR_EVENT_LOGON_FAILED, 0);
}

void sip_sub_resp()
{
	UCHAR pAuth[255];

	if (Sip_sRecvResp < 200)	return;

	if (strcmp(Sip_pRecvCallId, Sip_pSubCallId))	return;

	if (Sip_sRecvResp == 401 || Sip_sRecvResp == 407)
	{
		if (Sip_iSubAuthRetry < SIP_MAX_AUTH_RETRY)
		{
			Sip_iSubAuthRetry ++;
			if (Sip_pRecvWWWAuth)
			{
				if (Sip_pSubWWWAuth)	free(Sip_pSubWWWAuth);
				TaskHttpDigest(pAuth, Sip_pRecvWWWAuth, Sip_pSubReqUri, Sys_pAuthID, Sys_pAuthPin, _cRequest[SIP_REQ_SUBSCRIBE]);
				Sip_pSubWWWAuth = heap_save_str(pAuth);
			}
			if (Sip_pRecvProxyAuth)
			{
				if (Sip_pSubProxyAuth)	free(Sip_pSubProxyAuth);
				TaskHttpDigest(pAuth, Sip_pRecvProxyAuth, Sip_pSubReqUri, Sys_pAuthID, Sys_pAuthPin, _cRequest[SIP_REQ_SUBSCRIBE]);
				Sip_pSubProxyAuth = heap_save_str(pAuth);
			}
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_SUBSCRIBE);
			return;
		}
	}
	else if (Sip_sRecvResp >= 200 && Sip_sRecvResp < 300)
	{
		if (Sip_sRecvExpires != USHORT_MAX)
		{
			Sip_sSubTimeout = Sip_sRecvExpires;
		}
		if (Sip_sSubTimeout)
		{
			if (!Sip_pSubToTag)	
			{
				Sip_pSubToTag = sip_get_token(Sip_pRecvTo, _cTokenTag);
			}
			Sip_iSubAuthRetry = 0;
			return;
		}
	}
	else if (Sip_sRecvResp == 423)
	{
		if (Sip_sRecvMinExpires != USHORT_MAX)
		{
			Sip_sSubTimeout = Sip_sRecvMinExpires;
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_SUBSCRIBE);
			return;
		}
	}

	TaskHandleEvent(SIPR_EVENT_SUB_FAILED, 0);
	if (Sip_sRecvResp == 403 || Sip_sRecvResp == 404 || Sip_sRecvResp == 405 || Sip_sRecvResp == 489)
	{
		Sip_bSubscribe = FALSE;
	}
}

void sip_retrieve_to()
{
	PCHAR pTag;

	pTag = sip_get_token(Sip_pRecvTo, _cTokenTag);
	if (Sip_pCurLcb->pToTag)
	{
		if (strcmp(pTag, Sip_pCurLcb->pToTag))
		{
			free(Sip_pCurLcb->pToTag);
			Sip_pCurLcb->pToTag = pTag;
		}
		else
		{
			free(pTag);
			return;
		}
	}
	else
	{
		Sip_pCurLcb->pToTag = pTag;
	}

	if (Sip_pCurLcb->pTo)	free(Sip_pCurLcb->pTo);
	Sip_pCurLcb->pTo = Sip_pRecvTo;
	Sip_pRecvTo = NULL;
}

void sip_invite_resp()
{
	UCHAR iState;
	ULONG lRseq;
	BOOLEAN bValid;
	PCHAR pUri;
	PCHAR pProxyAuth;
	PCHAR pWWWAuth;
	PCHAR pContact;

#ifdef OEM_IP20
	UCHAR temp[MAX_USER_NAME_LEN];
	UCHAR dispNameR[MAX_USER_NAME_LEN];
	PCHAR pRecvTemp = NULL ;
	PCHAR pTemp ;
	
	pRecvTemp = &temp[0];
	strcpy(pRecvTemp, Sip_pRecvContact);
	//LcdDisplay(pRecvTemp,3);
	pTemp = strchr(pRecvTemp, '"');
		
	if (pTemp) 
	{
		pRecvTemp = pTemp;
		pRecvTemp ++;
		pTemp = SkipField(pRecvTemp, '"');
		strcpy(dispNameR, (pTemp) ? pRecvTemp : "");
	}
	else
	{
		strcpy(dispNameR, "");
	}
#endif

	if (Sip_sRecvResp <= 100 || Sip_sRecvResp >= 700)	return;

	Sip_pCurLcb->bEarlyDlg = TRUE;
	if (Sip_pRecvTo)	sip_retrieve_to();

	iState = Sip_pCurLcb->iState;
	if (Sip_sRecvResp >= 300)
	{
		if (Sip_sRecvResp == 422)
		{
			Sip_pCurLcb->lSessionExpires = Sip_lRecvMinSE;
			Sip_pCurLcb->lMinSE = Sip_lRecvMinSE;
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_ACK);
			free(Sip_pCurLcb->pTo);
			Sip_pCurLcb->pTo = NULL;
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_INVITE);
			return;
		}

		if (Sip_sRecvResp == 407 || Sip_sRecvResp == 401)
		{
			pProxyAuth = NULL;
			pWWWAuth = NULL;
			if (iState == CALL_STATE_CALLING2)
			{
				if (!Sip_pCurLcb->pProxyAuth && Sip_pRecvProxyAuth)
				{
					pProxyAuth = heap_save_str(Sip_pRecvProxyAuth);
					bValid = TRUE;
				}
				else if (!Sip_pCurLcb->pWWWAuth && Sip_pRecvWWWAuth)
				{
					bValid = TRUE;
					pWWWAuth = heap_save_str(Sip_pRecvWWWAuth);
				}
				else
				{
					bValid = FALSE;
				}
				if (bValid)
				{
					TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_ACK);
					free(Sip_pCurLcb->pTo);
					Sip_pCurLcb->pTo = NULL;
					if (pProxyAuth)		Sip_pCurLcb->pProxyAuth = pProxyAuth;
					if (pWWWAuth)		Sip_pCurLcb->pWWWAuth = pWWWAuth;
					TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_INVITE);
					return;
				}
			}
			else if (iState == CALL_STATE_TALKING)
			{
				if (Sip_pRecvProxyAuth)
				{
					pProxyAuth = heap_save_str(Sip_pRecvProxyAuth);
				}
				if (Sip_pRecvWWWAuth)
				{
					pWWWAuth = heap_save_str(Sip_pRecvWWWAuth);
				}
				TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_ACK);
				if (pProxyAuth)
				{
					free(Sip_pCurLcb->pProxyAuth);
					Sip_pCurLcb->pProxyAuth = pProxyAuth;
				}
				if (pWWWAuth)
				{
					free(Sip_pCurLcb->pWWWAuth);
					Sip_pCurLcb->pWWWAuth = pWWWAuth;
				}
				free(Sip_pCurLcb->pInvUri);
				Sip_pCurLcb->pInvUri = heap_save_str(Sip_pCurLcb->pReqUri);
				TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_INVITE);
				return;
			}
		}

		if (Sip_sRecvResp == 301 || Sip_sRecvResp == 302)
		{
			if (iState == CALL_STATE_CALLING2)
			{
				pUri = sip_get_uri(Sip_pRecvContact, FALSE);
				if (pUri)
				{
					pContact = heap_save_str(Sip_pRecvContact);
					TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_ACK);
					TaskHandleEvent(SIPR_EVENT_LINE_FREE, 0);
					sip_get_cid(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, pContact);
					free(pContact);
					sip_call_forwarded(pUri);
					return;
				}
			}
		}

		TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_ACK);
		if (iState == CALL_STATE_CALLING2)
		{
			if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEREE_NEW)
			{
				TaskHandleEvent(SIPR_EVENT_XFEREE_DONE, Sip_sRecvResp);
			}
			else
			{
				if (Sip_pCurLcb == Sip_pActiveLcb)
				{
#ifdef OEM_IP20
					if ((Sip_sRecvResp == 480 || Sip_sRecvResp == 603) && TransFlagResp == 1)
					{
						TaskUIHandler(UI_CALL_FINISHED, 0);
						TaskUIHandler(UI_CALL_DISPLAY_IDS0, IDS_TRANSFER_YAP);
						TaskUIHandler(UI_CALL_DISPLAY_IDS1, IDS_FEN_KAPAT);
					}			
					else
					{
						Sip_pCurLcb->iState = CALL_STATE_HANGUP;
						TaskUIHandler(UI_CALL_FINISHED, (USHORT)Sip_pRecvInfo);
					}
#else
					Sip_pCurLcb->iState = CALL_STATE_HANGUP;
					TaskUIHandler(UI_CALL_FINISHED, (USHORT)Sip_pRecvInfo);
#endif
				}
				else 
				{
					TaskHandleEvent(SIPR_EVENT_LINE_FREE, 0);
				}
			}
		}
		else if (iState == CALL_STATE_TALKING)
		{
			if (Sip_pCurLcb->bHolding)
			{
				if (Sip_pCurLcb->iLocalMode & AUDIO_MODE_RECVONLY)
				{
					Sip_pCurLcb->iLocalMode &= ~AUDIO_MODE_RECVONLY;
				}
				else
				{
					Sip_pCurLcb->iLocalMode |= AUDIO_MODE_RECVONLY;
				}
				Sip_pCurLcb->bHolding = FALSE;
			}
			else
			{
				TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_BYE);
				Sip_pCurLcb->iState = CALL_STATE_HANGUP;
			}
		}
		else
		{
			TaskHandleEvent(SIPR_EVENT_LINE_FREE, 0);
		}
		return;
	}

	if (Sip_sRecvResp < 200)
	{
		if (sip_has_token(Sip_pRecvRequire, _c100rel))
		{
			if (Sip_pRecvRSeq)
			{
				lRseq = atol(Sip_pRecvRSeq, 10);
				if (!Sip_pCurLcb->lRseq || lRseq == (Sip_pCurLcb->lRseq + 1))
				{
					if (!Sip_pCurLcb->lRseq)
					{
						sip_get_route_uri(TRUE);
					}
					Sip_pCurLcb->lRseq = lRseq;
					TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_PRACK);
				}
			}
		}
		if (Sip_sRecvResp == 183 || Sip_sRecvResp == 180)
		{
			if (Sip_pCurLcb == Sip_pActiveLcb && Sip_pCurLcb->iState == CALL_STATE_CALLING2)
			{				
				UdpDebugString("Ringing");
				if (Sip_pRecvSdp && Sip_pRecvSdp->bValid)
				{
//					strcpy(Adapter_pUdpBuf, "\r\n\r\n");
//					UdpSendTo(Sip_pCurLcb->pRtpSocket, 4, Sip_pRecvSdp->pRtpDstIP, Sip_pRecvSdp->sRtpDstPort);
					sip_copy_sdp();
				}
				if (!Sip_pCurLcb->iRemoteRb)
				{
					Sip_pCurLcb->iRemoteRb = RINGBACK_LOCAL;
					TaskUIHandler(UI_CALL_RINGBACK, Sip_pCurLcb->iVoiceCoder);
				}
			}
		}
		return;
	}

	if (Sip_pCurLcb->iState == CALL_STATE_CALLING2)
	{
		sip_get_route_uri(TRUE);
	}
	if (Sip_pRecvSessionExpires)
	{
		sip_get_session_expires();
	}
	else
	{
		Sip_pCurLcb->lSessionExpires = 0;
		Sip_pCurLcb->iRefresher = SIP_REFRESHER_NONE;
	}
	Sip_pCurLcb->lSessionTimer = 0;
	TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_ACK);

	if (Sip_pCurLcb->iState == CALL_STATE_CALLING2)
	{
		if (Sip_pRecvSdp && Sip_pRecvSdp->bValid)
		{
			sip_copy_sdp();
		}
#ifdef OEM_IP20
#ifdef __ADSL
		Sip_pCurLcb->brtpIpPrivate = IsPrivateIP(Sip_pCurLcb->pRtpDstIP);
#endif			
#endif
		Sip_pCurLcb->iState = CALL_STATE_TALKING;
		if (Sip_pCurLcb == Sip_pActiveLcb)
		{
			TaskUIHandler(UI_CALL_CONNECTED, (USHORT)(&Sip_pCurLcb->iVoiceCoder));
#ifdef OEM_IP20
			strcpy(Sip_pCurLcb->pPeerName , dispNameR);
			TaskCallLogAdd(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, CALL_LOG_TYPE_DIALED);
#endif
			sipr_show_cid(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, FALSE);
			if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEREE_NEW)
			{
				TaskHandleEvent(SIPR_EVENT_XFEREE_DONE, Sip_sRecvResp);
			}
		}
		if (Sip_pCurLcb->iVoiceCoder == VOICE_CODER_NONE)
		{
			TaskHandleEvent(SIPR_EVENT_SEND_REQ, SIP_REQ_BYE);
			Sip_pCurLcb->iState = CALL_STATE_HANGUP;
			if (Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEREE_NEW)
			{
				TaskHandleEvent(SIPR_EVENT_XFEREE_DONE, USHORT_MAX);
			}
			else
			{
				if (Sip_pCurLcb == Sip_pActiveLcb)
				{
					TaskUIHandler(UI_CALL_ENDED, CALL_END_CODER_MISMATCH);
				}			
			}
		}
	}
	else if (Sip_pCurLcb->iState == CALL_STATE_TALKING)
	{
		if (Sip_pRecvSdp && Sip_pRecvSdp->bValid)
		{
			if (Sip_pCurLcb == Sip_pActiveLcb)
			{
				if (Sip_pCurLcb->bHolding)
				{
					TaskUIHandler((Sip_pCurLcb->iLocalMode & AUDIO_MODE_RECVONLY) ? UI_CALL_CONNECTED : UI_CALL_HOLD, (USHORT)(&Sip_pRecvSdp->iVoiceCoder));
					sipr_show_cid(Sip_pCurLcb->pPeerName, Sip_pCurLcb->pPeerNumber, FALSE);
				}
				else
				{
					if (Sip_pRecvSdp->iVoiceCoder != Sip_pCurLcb->iVoiceCoder)
					{
						TaskUIHandler(UI_CALL_UPDATE_CODEC, (USHORT)(&Sip_pRecvSdp->iVoiceCoder));
					}
				}
			}
			sip_copy_sdp();
		}
		Sip_pCurLcb->bHolding = FALSE;
	}
	else if (Sip_pCurLcb->iState == CALL_STATE_HANGUP)
	{
		TaskHandleEvent(SIPR_EVENT_LINE_FREE, 0);
	}
}

void sip_refer_resp()
{
	if (Sip_sRecvResp < 300)	return;

	if (Sip_pCurLcb->iState == CALL_STATE_TALKING && Sip_pCurLcb->iXferPart == XFER_PART_TRANSFEROR)
	{
		// transfer failed, unhold current call
		if (Sip_pCurLcb == Sip_pActiveLcb)
		{
//			TaskHandleEvent(UI_EVENT_HOLD, 1);
			Sip_pCurLcb->iXferPart = XFER_PART_FAIL;
			Sip_pCurLcb->iTimer = 0;
			TaskUIHandler(UI_CALL_XFERFAIL, 0);
		}
		else
		{
			Sip_pCurLcb->iXferPart = XFER_PART_NONE;
		}
	}
}

void sip_handle_resp()
{
	if (Sip_iRecvMethod == SIP_REQ_REGISTER)
	{
		sip_reg_resp();
	}
	else if (Sip_iRecvMethod == SIP_REQ_SUBSCRIBE)
	{
		sip_sub_resp();
	}
	else
	{
		if (!Sip_pCurLcb)			return;
		if (Sip_iRecvMethod == SIP_REQ_INVITE)
		{
			sip_invite_resp();
		}
		else
		{
			if (Sip_sRecvResp < 200)	return;
			if (Sip_sRecvResp == 401 || Sip_sRecvResp == 407)
			{
				if (Sip_pRecvProxyAuth)
				{
					free(Sip_pCurLcb->pProxyAuth);
					Sip_pCurLcb->pProxyAuth = heap_save_str(Sip_pRecvProxyAuth);
				}
				if (Sip_pRecvWWWAuth)
				{
					free(Sip_pCurLcb->pWWWAuth);
					Sip_pCurLcb->pWWWAuth = heap_save_str(Sip_pRecvWWWAuth);
				}
				TaskHandleEvent(SIPR_EVENT_SEND_REQ, Sip_iRecvMethod);
				return;
			}
			switch (Sip_iRecvMethod)
			{
			case SIP_REQ_BYE:
				if (Sip_pCurLcb == Sip_pActiveLcb)
				{
					TaskUIHandler(UI_CALL_ENDED, CALL_END_PEER_HANGUP);
				}
				else
				{
					TaskHandleEvent(SIPR_EVENT_LINE_FREE, 0);
				}
				break;

			case SIP_REQ_REFER:
				sip_refer_resp();
				break;
			}
		}
	}
}
/**********************************************************************
 ** transaction layer receive functions                              **
 **********************************************************************
 */
SIP_TCB_HANDLE sip_tr_match(BOOLEAN bRequest)
{
	UCHAR i;
	SIP_TCB_HANDLE pTcb;

	if (Sip_pRecvBranch)
	{
		pTcb = bRequest? Sip_pReqTCB : Sip_pRespTCB;
		for (i = 0; i < TR_TSIZE; i ++, pTcb ++)
		{
			if (pTcb->iState != TRS_FREE && pTcb->iMethod == Sip_iRecvMethod && !strcmp(pTcb->pBranch, Sip_pRecvBranch))
			{
				return pTcb;
			}
		}
	}
	return NULL;
}

SIP_TCB_HANDLE sip_tr_new_resp()
{
	UCHAR i;
	SIP_TCB_HANDLE pTcb;

	pTcb = Sip_pRespTCB;
	for (i = 0; i < TR_TSIZE; i ++)
	{
		if (pTcb->iState == TRS_FREE)
		{
			return pTcb;
		}
		pTcb ++;
	}

	UdpDebugString("No Free Resp Transactions");
	return NULL;
}

void sip_tr_rexmit()
{
	if (Sip_pCurTcb->pData)
	{
		strcpy(Adapter_pUdpBuf, Sip_pCurTcb->pData);
		UdpSendTo(Sip_pSocket, strlen(Sip_pCurTcb->pData), Sip_pCurTcb->pDstIP, Sip_pCurTcb->sDstPort);
	}
}

void sip_tr_acked(SIP_TCB_HANDLE pTcb)
{
	pTcb->iState = TRS_COMPLETED;
	if (pTcb->pData)	free(pTcb->pData);
	pTcb->pData = NULL;
}

BOOLEAN sip_tr_recv_ack()
{
	UCHAR i;
	SIP_TCB_HANDLE pTcb;

	if (Sip_pCurLcb)
	{
		pTcb = Sip_pCurLcb->pInvTcb;
		if (pTcb->iState == TRS_PROCEEDING)
		{
//			UdpDebugString("Invite tr acked");
			sip_tr_acked(pTcb);
			return TRUE;
		}
	}
	else
	{
		pTcb = Sip_pRespTCB;
		for (i = 0; i < TR_TSIZE; i ++, pTcb ++)
		{
			if (pTcb->iState == TRS_PROCEEDING && pTcb->iMethod == SIP_REQ_INVITE)
			{
				if (!strcmp(pTcb->pBranch, Sip_pRecvBranch))
				{
//					UdpDebugString("Invite tr terminated");
					sip_tr_acked(pTcb);
				}
			}
		}
	}

	return FALSE;
}

void sip_tr_recv_req()
{
	Sip_pCurTcb = sip_tr_match(FALSE);
	if (Sip_pCurTcb)
	{
		sip_tr_rexmit();
	}
	else
	{
		Sip_pCurTcb = sip_tr_new_resp();
		if (!Sip_pCurTcb)		return;

		Sip_pCurTcb->iState = TRS_TRYING;
		Sip_pCurTcb->iMethod = Sip_iRecvMethod;
		// save branch
		Sip_pCurTcb->pBranch = Sip_pRecvBranch;
		Sip_pRecvBranch = NULL;

		sip_handle_req();
	}
}

void sip_tr_recv_resp()
{
	Sip_pCurTcb = sip_tr_match(TRUE);
	if (!Sip_pCurTcb)
	{
		UdpDebugString("No matching transaction!");
		return;
	}

	switch (Sip_pCurTcb->iState)
	{
	case TRS_TRYING:
	case TRS_PROCEEDING:
		Sip_pCurLcb = Sip_pCurTcb->pLcb;
		if (Sip_iRecvMethod == SIP_REQ_INVITE && !Sip_pCurLcb)
		{
			sip_tr_free(Sip_pCurTcb);
			break;
		}
		
		if (Sip_sRecvResp < 200)
		{
			if (Sip_iRecvMethod == SIP_REQ_INVITE)
			{
				Sip_pCurTcb->iState = TRS_PROCEEDING;
				Sip_pCurTcb->iTimer1 = 0;
			}
		}
		else
		{
			Sip_pCurTcb->iState = TRS_COMPLETED;
			// free packet buffer
			if (Sip_pCurTcb->pData)	
			{
				free(Sip_pCurTcb->pData);
				Sip_pCurTcb->pData = NULL;
			}
		}
		// inform TU of this response ...
		sip_handle_resp();
		break;

	case TRS_COMPLETED:
		if (Sip_iRecvMethod == SIP_REQ_INVITE && Sip_sRecvResp >= 200)
		{
			sip_tr_rexmit();
		}
		break;
	}
}

void sip_save_via(PCHAR pBuf)
{
	if (Sip_iRecvViaNum < MAX_RECV_VIA_NUM)
	{
		Sip_pRecvVia[Sip_iRecvViaNum] = pBuf;
		Sip_iRecvViaNum ++;
	}
}

void sip_handle_header(PCHAR pBuf)
{
	UCHAR iLen;
	UCHAR pHeader[32];
	PCHAR pCur;
	PCHAR pTemp;

	iLen = sip_get_header_name(pBuf, pHeader);
	if (!iLen)
	{
		return;
	}
	pCur = (PCHAR)(pBuf + iLen);
	if (!strcmp_lowercase(pHeader, "via") || !strcmp_lowercase(pHeader, "v"))
	{
		sip_save_via(pCur);
	}
	else if (!strcmp_lowercase(pHeader, "record-route"))
	{
		while (pCur)
		{
			pTemp = SkipField(pCur, ',');
			if (pTemp)
			{
				pTemp += count_space(pTemp);
			}
			if (strlen(pCur))
			{
				if (Sip_iRecvRRNum < MAX_RECV_RR_NUM)
				{
					Sip_pRecvRecordRoute[Sip_iRecvRRNum] = pCur;
					Sip_iRecvRRNum ++;
				}
			}
			pCur = pTemp;
		}
	}
	else if (!strcmp_lowercase(pHeader, "from") || !strcmp_lowercase(pHeader, "f"))
	{
		Sip_pRecvFrom = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "to") || !strcmp_lowercase(pHeader, "t"))
	{
		Sip_pRecvTo = heap_save_str(pCur);
	}
	else if (!strcmp_lowercase(pHeader, "call-id") || !strcmp_lowercase(pHeader, "i"))
	{
		Sip_pRecvCallId = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "contact") || !strcmp_lowercase(pHeader, "m"))
	{
		Sip_pRecvContact = pCur;
	}
	else if (!strcmp_lowercase(pHeader, _cContentType) || !strcmp_lowercase(pHeader, "c"))
	{	
		Sip_iContentType = sip_get_ctype(pCur);
	}
	else if (!strcmp_lowercase(pHeader, "content-length") || !strcmp_lowercase(pHeader, "l"))
	{	
		Sip_sContentLen = atoi(pCur, 10);
	}
	else if (!strcmp_lowercase(pHeader, "cseq"))
	{
		Sip_pRecvCSeq = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "www-authenticate"))
	{
		Sip_pRecvWWWAuth = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "proxy-authenticate"))
	{
		Sip_pRecvProxyAuth = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "min-expires"))
	{
		Sip_sRecvMinExpires = atoi(pCur, 10);
	}
	else if (!strcmp_lowercase(pHeader, "expires"))
	{
		Sip_sRecvExpires = atoi(pCur, 10);
	}
	else if (!strcmp_lowercase(pHeader, "event") || !strcmp_lowercase(pHeader, "o"))
	{
		Sip_iRecvEvent = sip_get_etype(pCur);
	}
	else if (!strcmp_lowercase(pHeader, "require"))
	{
		Sip_pRecvRequire = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "rseq"))
	{
		Sip_pRecvRSeq = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "rack"))
	{
		Sip_pRecvRAck = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "supported"))
	{
		Sip_pRecvSupported = pCur;
	}
	else if (!strcmp_lowercase(pHeader, _cReplaces_r))
	{
		Sip_pRecvReplaces = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "refer-to"))
	{
		Sip_pRecvReferTo = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "referred-by"))
	{
		Sip_pRecvReferredBy = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "call-info"))
	{
		Sip_pRecvCallInfo = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "session-expires"))
	{
		Sip_pRecvSessionExpires = pCur;
	}
	else if (!strcmp_lowercase(pHeader, "min-se"))
	{
		Sip_lRecvMinSE = atol(pCur, 10);
	}
#ifdef OEM_BT2008
	else if (!strcmp_lowercase(pHeader, "x-control-indication"))
	{
		_pRecvXControlInd = pCur;	//NEC extension
	}
#endif
}

/**********************************************************************
 ** SIP receive functions                                            **
 **********************************************************************
 */
void SiprInit()
{
	UCHAR i;

	Sip_pTempBuf = Adapter_pReceivePacket;
	Sip_iRecvMethod = SIP_REQ_UNKNOWN;
	Sip_pRecvBoundary = NULL;
	Sip_pRecvBranch = NULL;
	Sip_pRecvInfo = NULL;
	Sip_pRecvSdp = NULL;
	
	for (i= 0; i < MAX_RECV_BODY_LINE; i ++)
	{
		Sip_pRecvBody[i] = NULL;
	}
	Sip_iRecvBodyLine = 0;
	for (i = 0; i < MAX_RECV_VIA_NUM; i ++)
	{
		Sip_pRecvVia[i] = NULL;
	}
	Sip_iRecvViaNum = 0;
	for (i = 0; i < MAX_RECV_RR_NUM; i ++)
	{
		Sip_pRecvRecordRoute[i] = NULL;
	}
	Sip_iRecvRRNum = 0;
	Sip_pRecvFrom = NULL;
	Sip_pRecvTo = NULL;
	Sip_pRecvCallId = NULL;
	Sip_pRecvContact = NULL;
	Sip_pRecvCSeq = NULL;
	Sip_pRecvWWWAuth = NULL;
	Sip_pRecvProxyAuth = NULL;
	Sip_pRecvRequire = NULL;
	Sip_pRecvRSeq = NULL;
	Sip_pRecvRAck = NULL;
	Sip_pRecvSupported = NULL;
	Sip_pRecvReplaces = NULL;
	Sip_pRecvReferTo = NULL;
	Sip_pRecvReferredBy = NULL;
	Sip_pRecvCallInfo = NULL;
	Sip_iRecvEvent = EVENT_TYPE_UNKNOWN;
	Sip_iContentType = CTYPE_UNKNOWN;
	Sip_sContentLen = USHORT_MAX;
	Sip_sRecvExpires = USHORT_MAX;
	Sip_sRecvMinExpires = USHORT_MAX;
	Sip_pRecvSessionExpires = NULL;
	Sip_lRecvMinSE = 0;
#ifdef OEM_BT2008
	_pRecvXControlInd = NULL;	//NEC extension
#endif
}

void sipr_free()
{
	UCHAR i;

	for (i= 0; i < Sip_iRecvBodyLine; i ++)
	{
		if (Sip_pRecvBody[i])	free(Sip_pRecvBody[i]);
	}
	if (Sip_pRecvTo)		free(Sip_pRecvTo);
	if (Sip_pRecvBoundary)		free(Sip_pRecvBoundary);
	if (Sip_pRecvBranch)	free(Sip_pRecvBranch);
	if (Sip_pRecvInfo)		free(Sip_pRecvInfo);
	if (Sip_pRecvSdp)		free(Sip_pRecvSdp);
}

#define SIP_RECV_FIRSTLINE		0
#define SIP_RECV_HEADER			1
#define SIP_RECV_BODY			2
#define SIP_RECV_BODY_HEADER	3
#define SIP_RECV_BODY_BODY		4

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

#define BOUNDARY_NEGATIVE	0
#define BOUNDARY_BEGIN		1
#define BOUNDARY_END		2
UCHAR _IsBoundary(PCHAR pLine, USHORT sLineLen)
{
	USHORT sLen;
	PCHAR pCur;

	if (Sip_pRecvBoundary)
	{
		sLen = strlen(Sip_pRecvBoundary);
		pCur = pLine;
		if (sLineLen >= sLen + 2)
		{
			if (pLine[0] == (UCHAR)'-' && pLine[1] == (UCHAR)'-' && !memcmp_str((PCHAR)(pLine + 2), Sip_pRecvBoundary))
			{
				sLen += 2;
				sLineLen -= sLen;
				pLine += sLen;
				return (sLineLen >= 2 && pLine[0] == (UCHAR)'-' && pLine[1] == (UCHAR)'-') ? BOUNDARY_END : BOUNDARY_BEGIN;
			}
		}
	}
	return BOUNDARY_NEGATIVE;
}

void SipRun(UDP_SOCKET pUcb)
{
	USHORT sIndex, sLineLen, sActualLen;
	PCHAR pLine;
	PCHAR pBackup;
	PCHAR pCur;
	PCHAR pBuf;
	BOOLEAN bBadMsg;
	BOOLEAN bRequestMsg;
	UCHAR iState, iCurCType, iBoundary, iLen;
	UCHAR pHeader[32];
#ifdef OEM_IP20
	PCHAR pTempSys;
#endif

	if (pUcb != Sip_pSocket)	return;
	if (TaskStunRun(pUcb))		return;

#ifdef OEM_IP20
	pTempSys = heap_save_str(Adapter_pUdpBuf);	//added for syslog sip signaling 
	
	line_start(Adapter_pUdpBuf);		
	line_add_str("PPP");
	line_add_short(111);
	line_add_char(' ');
	line_add_str(pTempSys);	
		
	UdpSend(_pSyslogSocket, strlen(Adapter_pUdpBuf)+1);
	
	strcpy(Adapter_pUdpBuf, pTempSys);
	
	free(pTempSys);
	pTempSys = NULL;
	
#endif

	if (pUcb->sLen < 13)	return;

	pBuf = Adapter_pUdpBuf;
	pBuf[pUcb->sLen] = 0;
	bRequestMsg = FALSE;
	bBadMsg = FALSE;
	sIndex = 0;
	pBackup = NULL;
	iState = SIP_RECV_FIRSTLINE;
	iCurCType = CTYPE_UNKNOWN;
	while (sIndex < pUcb->sLen)
	{
		pLine = (PCHAR)(pBuf + sIndex);
		sActualLen = _get_line(pLine, &sLineLen);
		sIndex += sLineLen;
		if (pBackup)
		{
			strcat(pBackup, pLine);
			pLine = pBackup;
			pBackup = NULL;
		}
		if (sIndex < pUcb->sLen && (pBuf[sIndex] == (UCHAR)' ' || pBuf[sIndex] == (UCHAR)'\t'))
		{
			pBackup = pLine;
			continue;
		}
		if (iState == SIP_RECV_FIRSTLINE)
		{
			pCur = pLine;
			if (!memcmp_str(pCur, "SIP/2.0 "))
			{
				pCur += 8;
				Sip_sRecvResp = atoi(pCur, 10);
//				UdpDebugString(pCur);	// pCur in send buffer, can not debug correctly here
				if (Sip_sRecvResp < 100 || Sip_sRecvResp >= 700)
				{
					goto End;
				}
				Sip_pRecvInfo = heap_save_str(pCur);
			}
			else
			{
				pCur = SkipField(pCur, ' ');
				if (!pCur)	goto End;

				pCur = SkipField(pCur, ' ');
				if (!pCur)	goto End;

				if (strcmp(pCur, _cTokenSipVer))	goto End;

				Sip_iRecvMethod = sip_get_request(pLine);
				Sip_pRecvInfo = heap_save_str(pLine);
				bRequestMsg = TRUE;
			}
			iState = SIP_RECV_HEADER;
		}
		else if (iState == SIP_RECV_HEADER)
		{
			if (sActualLen)
			{
				sip_handle_header(pLine);
			}
			else 
			{
				Sip_pTempBuf = (PCHAR)(pBuf + sIndex);
				if (Sip_sContentLen != USHORT_MAX)
				{
					if (pUcb->sLen < (sIndex + Sip_sContentLen))	
					{
						UdpDebugString("Bad Message: length error");
//						bBadMsg = TRUE;
//						break;
					}
				}
				else
				{
					if (pUcb->sLen > sIndex)
					{
						Sip_sContentLen = pUcb->sLen - sIndex;
					}
					else
					{
						break;
					}
				}
				if (Sip_iContentType == CTYPE_UNKNOWN)
				{
					break;
				}
				if (Sip_iContentType == CTYPE_APP_SDP && Sip_sContentLen)
				{
					sip_new_sdp();
				}
				iCurCType = Sip_iContentType;
				Sip_pTempBuf[Sip_sContentLen] = 0;
				iState = SIP_RECV_BODY;
			}
		}
		else
		{
			if (Sip_iContentType == CTYPE_MULTIPART)
			{
				iBoundary = _IsBoundary(pLine, sActualLen);
				if (iBoundary == BOUNDARY_BEGIN)
				{
					iState = SIP_RECV_BODY_HEADER;
					iCurCType = CTYPE_UNKNOWN;
					continue;
				}
				else if (iBoundary == BOUNDARY_END)
				{
					break;
				}
				else
				{
					if (iState == SIP_RECV_BODY)
					{
						continue;
					}
					else if (iState == SIP_RECV_BODY_HEADER)
					{
						if (sActualLen)
						{
							iLen = sip_get_header_name(pLine, pHeader);
							if (iLen)
							{
								if (!strcmp_lowercase(pHeader, _cContentType) || !strcmp_lowercase(pHeader, "c"))
								{	
									iCurCType = sip_get_ctype((PCHAR)(pLine + iLen));
								}
							}
						}
						else
						{
							iState = SIP_RECV_BODY_BODY;
							if (iCurCType == CTYPE_APP_SDP && Sip_sContentLen)
							{
								sip_new_sdp();
							}

						}
						continue;
					}
				}
			}
			if (sActualLen)
			{
				if (iCurCType == CTYPE_APP_SDP)
				{
					if (Sip_pRecvSdp && Sip_pRecvSdp->bValid)
					{
						Sip_pRecvSdp->bValid = sip_handle_sdp(Sip_pRecvSdp, pLine);
					}
				}
				else if (iCurCType != CTYPE_UNKNOWN)
				{
					if (Sip_iRecvBodyLine < MAX_RECV_BODY_LINE)
					{
						Sip_pRecvBody[Sip_iRecvBodyLine] = heap_save_str(pLine);
						Sip_iRecvBodyLine ++;
					}
					else
					{
						break;
					}
				}
			}
			if (Sip_sContentLen > sLineLen)
			{
				Sip_sContentLen -= sLineLen;
			}
			else
			{
				break;
			}
		}
	}

	// Handle first line
	if (iState == SIP_RECV_FIRSTLINE) 	return;

	TaskMiniRun();

	UdpDebugString(Sip_pRecvInfo);
	if (Sip_pRecvSdp && Sip_pRecvSdp->bValid)
	{
		sip_check_sdp(Sip_pRecvSdp);
	}
	if (!bRequestMsg)
	{
		if (Sip_pRecvCSeq)
		{
			pCur = SkipField(Sip_pRecvCSeq, ' ');

			Sip_iRecvMethod = sip_get_request(pCur);
		}
//		if (Sip_iRecvMethod == SIP_REQ_UNKNOWN || bBadMsg || Sip_iRecvViaNum != 1)
		if (Sip_iRecvMethod == SIP_REQ_UNKNOWN)
		{
			UdpDebugString("Invalid response!");
//			goto End;
		}
		else
		{
			Sip_pRecvBranch = sip_get_token(Sip_pRecvVia[0], _cTokenBranch);
			sip_tr_recv_resp();
		}
	}
	else
	{
		if (Sip_iRecvViaNum)
		{
			sip_get_sentby();
		}
		else
		{
			UdpDebugString("Bad Message: no via header");
			bBadMsg = TRUE;
		}
		if (Sip_iRecvMethod == SIP_REQ_UNKNOWN || bBadMsg)
		{
			UdpDebugString("Unknown Request or Bad Message!");
			if (Sip_iRecvMethod != SIP_REQ_ACK)
			{
				TaskHandleEvent(SIPR_EVENT_SEND_RESP, 400);
			}
//			goto End;
		}
		else
		{
			if (Sip_iRecvMethod == SIP_REQ_ACK)
			{
				sip_handle_req();
			}
			else
			{
				Sip_pRecvBranch = sip_get_token(Sip_pRecvVia[0], _cTokenBranch);
				sip_tr_recv_req();
			}
		}
	}

End:
	sipr_free();
	SiprInit();
}


#endif

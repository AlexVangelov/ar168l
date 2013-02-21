/*-------------------------------------------------------------------------
   AR1688 SIP (common) function copy right information

   Copyright (c) 2006-2010. Tang, Li <tangli@palmmicro.com>
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
#include "bank7.h"

UDP_SOCKET Sip_pSocket;
UCHAR Sip_pNatIP[IP_ALEN];
USHORT Sip_sNatPort;
USHORT Sip_sRtpPort;
PCHAR Sip_pTempBuf;

PCHAR Sip_pRecvBody[MAX_RECV_BODY_LINE];
UCHAR Sip_iRecvBodyLine;
PCHAR Sip_pRecvBoundary;
PCHAR Sip_pRecvBranch;
PCHAR Sip_pRecvInfo;
SIP_SDP_HANDLE Sip_pRecvSdp;
UCHAR Sip_iRecvMethod;
USHORT Sip_sRecvResp;
USHORT Sip_sRespDstPort;

PCHAR Sip_pRecvVia[MAX_RECV_VIA_NUM];
UCHAR Sip_iRecvViaNum;
PCHAR Sip_pRecvRecordRoute[MAX_RECV_RR_NUM];
UCHAR Sip_iRecvRRNum;
PCHAR Sip_pRecvFrom;
PCHAR Sip_pRecvTo;
PCHAR Sip_pRecvCallId;
PCHAR Sip_pRecvContact;
PCHAR Sip_pRecvCSeq;
PCHAR Sip_pRecvWWWAuth;
PCHAR Sip_pRecvProxyAuth;
PCHAR Sip_pRecvRequire;
PCHAR Sip_pRecvRSeq;
PCHAR Sip_pRecvRAck;
PCHAR Sip_pRecvSupported;
PCHAR Sip_pRecvReplaces;
PCHAR Sip_pRecvReferTo;
PCHAR Sip_pRecvReferredBy;
PCHAR Sip_pRecvCallInfo;
PCHAR Sip_pRecvSessionExpires;
UCHAR Sip_iRecvEvent;
UCHAR Sip_iContentType;
USHORT Sip_sContentLen;
USHORT Sip_sRecvExpires;
USHORT Sip_sRecvMinExpires;
ULONG Sip_lRecvMinSE;

UCHAR Sip_iRegState;
USHORT Sip_sRegTimer;
USHORT Sip_sRegTimeout;
ULONG Sip_lRegSeq;
UCHAR Sip_iRegAuthRetry;
PCHAR Sip_pRegWWWAuth;
PCHAR Sip_pRegProxyAuth;
UCHAR Sip_pRegFromTag[MAX_TAG_LEN+1];
UCHAR Sip_pRegCallId[MAX_CALL_ID_LEN+1];
UCHAR Sip_pServerIP[IP_ALEN];
USHORT Sip_sServerPort;
UCHAR Sip_iNatTimer;

BOOLEAN Sip_bSubscribe;
PCHAR Sip_pSubReqUri;
PCHAR Sip_pSubCallId;
PCHAR Sip_pSubFromTag;
PCHAR Sip_pSubToTag;
ULONG Sip_lSubSeq;
USHORT Sip_sSubTimer;
USHORT Sip_sSubTimeout;
UCHAR Sip_iSubAuthRetry;
PCHAR Sip_pSubWWWAuth;
PCHAR Sip_pSubProxyAuth;

// Transaction control block
struct sip_tcb Sip_pReqTCB[TR_TSIZE];
struct sip_tcb Sip_pRespTCB[TR_TSIZE];
SIP_TCB_HANDLE Sip_pCurTcb;

// Line control block
struct sip_lcb Sip_pLCB[LCB_TSIZE];
SIP_LCB_HANDLE Sip_pActiveLcb;
SIP_LCB_HANDLE Sip_pCurLcb;

PCHAR Rtp_pData;

const UCHAR _cRequest[SIP_REQ_NUMBER][10] = 
{
	"INVITE", "ACK", "OPTIONS", "BYE", "CANCEL", "REGISTER", "REFER", "NOTIFY", "INFO", "PRACK", "UPDATE", "MESSAGE", "SUBSCRIBE"
};

const UCHAR _cAudioMode[][9] = {"inactive", "recvonly", "sendonly", "sendrecv"};

const UCHAR _cTokenSipVer[] = "SIP/2.0";
const UCHAR _cTokenSip[] = "sip:";
const UCHAR _c100rel[] = "100rel";
const UCHAR _cTokenTag[] = "tag=";
const UCHAR _cTokenBranch[] = "branch=";
const UCHAR _cMsgFrag[] = "message/sipfrag";
const UCHAR _cTextPlain[] = "text/plain";
const UCHAR _cTextHtml[] = "text/html";

void sip_debug(PCHAR pStrInfo, USHORT iMessage)
{
	UCHAR pDebug[32];

	strcpy(pDebug, pStrInfo);
	strcat(pDebug, ": ");
	if (iMessage < SIP_REQ_NUMBER)
	{
		strcat(pDebug, _cRequest[iMessage]);
	}
	else
	{
		itoa(iMessage, (PCHAR)(pDebug + strlen(pDebug)), 10);
	}
	UdpDebugString(pDebug);
}

void sip_tr_free(SIP_TCB_HANDLE pTcb)
{
	sip_debug("sip_tr_free", pTcb->iMethod);
	if (pTcb->pBranch)
	{
		free(pTcb->pBranch);
	}
	if (pTcb->pData)
	{
		free(pTcb->pData);
	}
	memset((PCHAR)pTcb, 0, SIP_TCB_LENGTH);
}


#endif

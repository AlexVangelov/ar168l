/*-------------------------------------------------------------------------
   AR1688 RTP function copy right information

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

#ifdef CALL_SIP

#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "sip.h"
#include "bank7.h"

SIP_LCB_HANDLE RtpGetLcb(UDP_SOCKET pUcb)
{
	UCHAR i;
	SIP_LCB_HANDLE pLcb;

	pLcb = Sip_pLCB;
	for (i = 0; i < LCB_TSIZE; i ++, pLcb ++)
	{
		if (pLcb->iState != CALL_STATE_FREE)
		{
			if (pUcb == pLcb->pRtpSocket)
			{
				return pLcb;
			}
		}
	}

	return NULL;
}
		

void RtpRun(UDP_SOCKET pUcb)
{
	UCHAR iType, iLen, iState;
//	USHORT sCurSeq;
//	ULONG lTimeStamp;
	PCHAR pCur;
	PCHAR pData;
	SIP_LCB_HANDLE pLcb;

	if (TaskStunRun(pUcb))		return;

	pData = Adapter_pUdpBuf;
#ifdef OEM_IP20
	pLcb = RtpGetLcb(pUcb);
	if (pLcb->brtpSession == 0)
	{
		pLcb->brtpSession = 1;
	}
	if (!pLcb)	
	{
		pLcb = Sip_pActiveLcb;
		memcpy4(pLcb->pRtpDstSSRC, (PCHAR)(pData + RTP_SSRC));
#ifdef __ADSL
		if (pLcb->brtpSession == 0)
		{
			pLcb->brtpSession = 1;
		}
#endif
	}
	else
	{
		if (pLcb != Sip_pActiveLcb)	return;
	}
#else
	pLcb = RtpGetLcb(pUcb);
	if (!pLcb)	return;
	memcpy4(pLcb->pRtpDstSSRC, (PCHAR)(pData + RTP_SSRC));
	if (pLcb != Sip_pActiveLcb)	return;
#endif
	iState = pLcb->iState;
	if (iState != CALL_STATE_CALLING2 && iState != CALL_STATE_TALKING)
	{
		return;
	}
	if (pLcb->bHolding)
	{
		return;
	}
	if (!(pLcb->iLocalMode & AUDIO_MODE_RECVONLY))
	{
		return;
	}
	if (pLcb->iVoiceCoder == VOICE_CODER_NONE || pLcb->iVoiceCoder > VOICE_CODER_NUM)
	{
		return;
	}

	if (iState == CALL_STATE_CALLING2)
	{
		if (pLcb->iRemoteRb != RINGBACK_REMOTE)
		{
			pLcb->iRemoteRb = RINGBACK_REMOTE;
			TaskUIHandler(UI_CALL_RINGBACK_REMOTE, pLcb->iVoiceCoder);
		}
	}

	iType = pData[RTP_PAYLOAD] & 0x7f;
	// Some device will add RTP padding, we handle it here
	if (pData[RTP_VERSION] & 0x20)
	{
		iLen = pData[pUcb->sLen-1];
		pUcb->sLen -= iLen;
	}

//	sCurSeq = PCHAR2USHORT((PCHAR)(pData+RTP_SEQ));
//	lTimeStamp = PCHAR2ULONG((PCHAR)(pData+RTP_TIMESTAMP));

	// handle packet loss or packet misorder here
	pUcb->sLen -= RTP_DATA;
	pCur = pData;
	pCur += RTP_DATA;
	if (iType == pLcb->iCoderPayload)
	{
		TaskIncomingData(pUcb->sLen, pCur);
	}
	else
	{
		if (iType != pLcb->iDtmfPayload || Sys_iDtmfType != DTMF_TYPE_RFC2833)
		{
			return;
		}
		if (pLcb->bKeyRecved)
		{
			if ((pCur[1] & 0x80) == 0x80)
			{
				// DTMF key end
				pLcb->bKeyRecved = FALSE;
				return;
			}
			if (memcmp(pLcb->pKeyTimeStamp, (PCHAR)(pData+RTP_TIMESTAMP), 4))
			{
				pLcb->bKeyRecved = FALSE;
			}
		}
		if (!pLcb->bKeyRecved)
		{
			memcpy4(pLcb->pKeyTimeStamp, (PCHAR)(pData+RTP_TIMESTAMP));
			pLcb->bKeyRecved = TRUE;
			TaskUIHandler(UI_CALL_INCOMING_DTMF, (USHORT)keyid2ascii(pCur[0]));
		}

	}
}

#endif


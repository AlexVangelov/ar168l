/*-------------------------------------------------------------------------
   AR1688 RTP function copy right information

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
#include "bank5.h"

UCHAR RtpGetPayload(UCHAR iVoiceCoder, SIP_LCB_HANDLE pLcb)
{
	if (iVoiceCoder == VOICE_CODER_ILBC || iVoiceCoder == VOICE_CODER_G726_32 || iVoiceCoder == VOICE_CODER_SPEEX)
	{
		if (pLcb->iSdpType != SDP_TYPE_OFFER)
		{
			return pLcb->iCoderPayload;
		}
	}
	return Rtp_pPayLoad[iVoiceCoder];
}

void RtpSendKeyPacket(SIP_LCB_HANDLE pLcb, BOOLEAN bHead, BOOLEAN bEnd)
{
	PCHAR pDst;

	pDst = Adapter_pUdpBuf;

	pDst[RTP_VERSION] = 0x80;
	memcpy4((PCHAR)(pDst+RTP_SSRC), pLcb->pRtpSSRC);
	USHORT2PCHAR(pLcb->sRtpSeq, (PCHAR)(pDst+RTP_SEQ));
	pLcb->sRtpSeq ++;
	ULONG2PCHAR(pLcb->lRtpTimeStamp, (PCHAR)(pDst+RTP_TIMESTAMP));
	pDst[RTP_PAYLOAD] = Sys_iDtmfPayload;
	if (bHead)
	{
		pDst[RTP_PAYLOAD] |= 0x80;
	}

	Rtp_pData[0] = pLcb->iSendKey;
	Rtp_pData[1] = bEnd ? 0x80 : 0x00;
	USHORT2PCHAR(pLcb->sSendKeyLen, (PCHAR)(Rtp_pData + 2));

	UdpSendTo(pLcb->pRtpSocket, (USHORT)(RTP_DATA + 4), pLcb->pRtpDstIP, pLcb->sRtpDstPort);
}	

void RtpSendKey(SIP_LCB_HANDLE pLcb, UCHAR iKey)
{
	UCHAR i;

	// finish current DTMF key
	if (pLcb->bSendingKey)
	{
		for (i = 0; i < 3; i ++)
		{
			RtpSendKeyPacket(pLcb, FALSE, TRUE);
		}
	}

	pLcb->iSendKey = iKey;
	pLcb->sSendKeyLen = 0;
	pLcb->bSendingKey = TRUE;
}

USHORT rtp_add_voice(PCHAR pSendBuf, UCHAR iFrame, PCHAR pBuf)
{
	UCHAR i, iLen;
	USHORT sLen;

	sLen = 0;
	for (i = 0; i < iFrame; i ++)
	{
		iLen = *pBuf ++;
		if (!iLen)	break;
		memcpy((PCHAR)(pSendBuf + sLen), pBuf, iLen);
		pBuf += iLen;
		sLen += iLen;
	}

	return sLen;
}

void RtpSendVoice(SIP_LCB_HANDLE pLcb, UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2)
{
	PCHAR pDst;
	USHORT sLen;

	if (!iFrame1 && !iFrame2)
	{
		// Send 0 length UDP packet to keep RTP NAT alive, do NOT work with Asterisk and TPLINK router!
//		UdpSendTo(pLcb->pRtpSocket, 0, pLcb->pRtpDstIP, pLcb->sRtpDstPort);
//		UdpDebugString("send empty udp packet");
		return;
	}

	pLcb->lRtpTimeStamp = rl_32x8((Sys_lTicker - pLcb->lRtpOffset), 3);

	pDst = Adapter_pUdpBuf;
	pDst[RTP_VERSION] = 0x80;
	memcpy4((PCHAR)(pDst+RTP_SSRC), pLcb->pRtpSSRC);
	USHORT2PCHAR(pLcb->sRtpSeq, (PCHAR)(pDst+RTP_SEQ));
	ULONG2PCHAR(pLcb->lRtpTimeStamp, (PCHAR)(pDst+RTP_TIMESTAMP));
	pDst[RTP_PAYLOAD] = RtpGetPayload(pLcb->iVoiceCoder, pLcb);
	if (pLcb->bVoiceStart)
	{
		pDst[RTP_PAYLOAD] |= 0x80;
		pLcb->bVoiceStart = FALSE;
	}

	sLen = rtp_add_voice(Rtp_pData, iFrame1, pBuf1);
	sLen += rtp_add_voice((PCHAR)(Rtp_pData + sLen), iFrame2, pBuf2);
	if (sLen)
	{
#ifdef OEM_IP20
#ifdef __ADSL
		if (pLcb->brtpIpPrivate == FALSE && pLcb->brtpSession == 1)
		{
			pLcb->brtpSession = 2;
		}
		if (pLcb->brtpIpPrivate == FALSE)
		{
			if (Sip_pCurLcb->brtpSession)//rtp packet received from destination
			{
				UdpSendTo(pLcb->pRtpSocket, sLen + RTP_DATA, pLcb->pRtpDstIP, pLcb->sRtpDstPort);
			}
		}
		else
		{
			UdpSendTo(pLcb->pRtpSocket, sLen + RTP_DATA, pLcb->pRtpDstIP, pLcb->sRtpDstPort);
		}
#else		
		UdpSendTo(pLcb->pRtpSocket, sLen + RTP_DATA, pLcb->pRtpDstIP, pLcb->sRtpDstPort);
#endif

#else
		UdpSendTo(pLcb->pRtpSocket, sLen + RTP_DATA, pLcb->pRtpDstIP, pLcb->sRtpDstPort);
#endif
		pLcb->sRtpSeq ++;
	}
}

#endif


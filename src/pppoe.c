/*-------------------------------------------------------------------------
   AR1688 PPPoE function copy right information

   Copyright (c) 2006-2010. Tang, Li      <tangli@palmmicro.com>
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
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "bank2.h"

#define PPPOE_PADI		0x09
#define PPPOE_PADO		0x07
#define PPPOE_PADR		0x19
#define PPPOE_PADS		0x65
#define PPPOE_PADT		0xa7
#define PPPOE_PPP		0

#define PPPOE_STATE_INIT	0
#define PPPOE_STATE_PADI	1
#define PPPOE_STATE_PADR	2
#define PPPOE_STATE_PPP		3

#define PPPOE_VER_TYPE		0x11

#define PPPOE_INIT_TIMER	3
#define PPPOE_INIT_RETRY	3

#define HOST_UNIQ_LEN		14
const UCHAR _cHostUniq[HOST_UNIQ_LEN] = {0x01, 0x01, 0x00, 0x00, 0x01, 0x03, 0x00, 0x06, 0x00, 0xe0, 0x4c, 0x39, 0x02, 0x74};

UCHAR PPPoE_pSendHead[PPPOE_INFO];

UCHAR _iPppoeState, _iPppoeTimer, _iPppoeRetry;
USHORT _sPppoeSessionId;
PCHAR _pPppoePacket;
USHORT _sPppoePacketLen;

void PppoeInit()
{
	_iPppoeTimer = 0;
	_iPppoeRetry = 0;
	_sPppoeSessionId = 0;
	_pPppoePacket = NULL;
	_sPppoePacketLen = 0;
	_iPppoeState = PPPOE_STATE_INIT;
}

void PppoeSendData(USHORT sLen)
{
	if (!Net_bConnected)	return;

	USHORT2PCHAR(sLen + 2, (PCHAR)(PPPoE_pSendHead + PPPOE_LENGTH));
	NetSend(sLen + EP_DATA, NET_SEND_PPPOE);
}

void PppoeSendPacket(UCHAR iCode, USHORT sLen)
{
	PCHAR pSend;

	pSend = Adapter_pPacketBuf;

	USHORT2PCHAR(EPT_PPPOE_INIT, (PCHAR)(pSend + EP_TYPE));

	pSend[EP_DATA + PPPOE_VER] = PPPOE_VER_TYPE;	
	pSend[EP_DATA + PPPOE_CODE] = iCode;
	USHORT2PCHAR(_sPppoeSessionId, (PCHAR)(pSend + EP_DATA + PPPOE_SESSION_ID));
	USHORT2PCHAR(sLen, (PCHAR)(pSend + EP_DATA + PPPOE_LENGTH));

	NetSend(sLen + EP_DATA + PPPOE_DATA, NET_SEND_NORMAL);

	USHORT2PCHAR(EPT_PPPOE, (PCHAR)(pSend + EP_TYPE));		// restore to EPT_PPPOE for IP performance
}

void PppoeDiscover()
{
	memcpy((PCHAR)(Adapter_pPacketBuf + EP_DATA + PPPOE_DATA), _cHostUniq, HOST_UNIQ_LEN);
	PppoeSendPacket(PPPOE_PADI, HOST_UNIQ_LEN);
	_iPppoeState = PPPOE_STATE_PADI;
}

void PppoePadr(PCHAR pData, USHORT sLen)
{
	if (sLen)
	{
		memcpy((PCHAR)(Adapter_pPacketBuf + EP_DATA + PPPOE_DATA), pData, sLen);
	}
	PppoeSendPacket(PPPOE_PADR, sLen);
}

void PppoePadt()
{
	PppoeSendPacket(PPPOE_PADT, 0);
}

void PppoeFreePacket()
{
	if (_pPppoePacket)
	{
		free(_pPppoePacket);
		_pPppoePacket = NULL;
	}
	_sPppoePacketLen = 0;
}

void PppoeTerminated()
{
	Net_bConnected = FALSE;
	_iPppoeState = PPPOE_STATE_INIT;
	PppoeFreePacket();
	TaskUIHandler(UI_STBY_DISPLAY_IDS0, IDS_PPPOE_FAILED);
}

void PppoeSavePacket(PCHAR pData, USHORT sLen)
{
	if (sLen)
	{
		if (_pPppoePacket)
		{
			free(_pPppoePacket);
		}
		_pPppoePacket = heap_save_data(pData, sLen);
		_sPppoePacketLen = (_pPppoePacket) ? sLen : 0;
	}
}

void PppoeTimer()
{
	if (_iPppoeState == PPPOE_STATE_INIT)	return;
	
	if (_iPppoeState < PPPOE_STATE_PPP)
	{
		_iPppoeTimer ++;
		if (_iPppoeTimer > PPPOE_INIT_TIMER)
		{
			_iPppoeTimer = 0;
			if (_iPppoeRetry < PPPOE_INIT_RETRY)
			{
				_iPppoeRetry ++;
				if (_iPppoeState == PPPOE_STATE_PADI)
				{
					PppoeDiscover();
				}
				else
				{
					PppoePadr(_pPppoePacket, _sPppoePacketLen);
				}
			}
			else
			{
				PppoePadt();
				PppoeTerminated();
			}
		}
	}
	else
	{
		PppTimer();
	}
}

void PppoeFillHead()
{
	PPPoE_pSendHead[PPPOE_VER] = PPPOE_VER_TYPE;
	PPPoE_pSendHead[PPPOE_CODE] = PPPOE_PPP;
	USHORT2PCHAR(_sPppoeSessionId, (PCHAR)(PPPoE_pSendHead + PPPOE_SESSION_ID));
	USHORT2PCHAR(PPP_PROT_IP, (PCHAR)(PPPoE_pSendHead + PPPOE_PROTOCOL_ID));
}

void PppoeInitRun(PCHAR pHead, PCHAR pSrcMac, USHORT sDataLen)
{
	UCHAR iCode;

	iCode = pHead[PPPOE_CODE];
	if (iCode == PPPOE_PADO)
	{
		if (_iPppoeState == PPPOE_STATE_PADI)
		{
			memcpy(Adapter_pPacketBuf, pSrcMac, HW_ALEN);	// save PPPoE server MAC to send buffer directly
			// send PADR
			PppoePadr(Adapter_pReceivePacket, sDataLen);
			// save packet for retransmission
			PppoeSavePacket(Adapter_pReceivePacket, sDataLen);
			_iPppoeState = PPPOE_STATE_PADR;
		}
	}
	else
	{
		if (memcmp(Adapter_pPacketBuf, pSrcMac, HW_ALEN))	// compare PPPoE server MAC with send buffer MAC
		{
			return;
		}
		if (iCode == PPPOE_PADS)
		{
			if (_iPppoeState == PPPOE_STATE_PADR)
			{
				_sPppoeSessionId = PCHAR2USHORT((PCHAR)(pHead + PPPOE_SESSION_ID));
				_iPppoeState = PPPOE_STATE_PPP;
				PppoeFreePacket();
				PppoeFillHead();
				PppInit();
				LcpCfgReq();
			}
		}
		else if (iCode == PPPOE_PADT)
		{
			if (_sPppoeSessionId != PCHAR2USHORT((PCHAR)(pHead + PPPOE_SESSION_ID)))
			{
				return;
			}
			PppoeTerminated();
		}
	}
}

BOOLEAN PppoeCheckHead(PCHAR pPppoeHead, PCHAR pSrcMac)
{
	if (_iPppoeState != PPPOE_STATE_PPP)	
	{
		return FALSE;
	}
	if (_sPppoeSessionId != PCHAR2USHORT((PCHAR)(pPppoeHead + PPPOE_SESSION_ID)))
	{
		return FALSE;
	}
	if (memcmp(Adapter_pPacketBuf, pSrcMac, HW_ALEN))	// compare PPPoE server MAC with send buffer MAC
	{
		return FALSE;
	}
	return TRUE;
}

void PppSendMisc(USHORT sProtocol, USHORT sLen, UCHAR iCode, UCHAR iId)
{
	PCHAR pCur;
	
	pCur = (PCHAR)(Adapter_pPacketBuf + EP_DATA + PPPOE_INFO);
	pCur[PPP_CODE] = iCode;
	pCur[PPP_ID] = iId;
	sLen += PPP_DATA;
	USHORT2PCHAR(sLen, (PCHAR)(pCur + PPP_LEN));

	memcpy((PCHAR)(Adapter_pPacketBuf + EP_DATA), PPPoE_pSendHead, PPPOE_INFO);
	USHORT2PCHAR(sProtocol, (PCHAR)(Adapter_pPacketBuf + EP_DATA + PPPOE_PROTOCOL_ID));
	USHORT2PCHAR(sLen + 2, (PCHAR)(Adapter_pPacketBuf + EP_DATA + PPPOE_LENGTH));

	NetSend(sLen + EP_DATA + PPPOE_INFO, NET_SEND_NORMAL);
}


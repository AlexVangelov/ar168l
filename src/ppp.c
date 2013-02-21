/*-------------------------------------------------------------------------
   AR1688 PPP function copy right information

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

#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "bank2.h"

//PPP Lcp Code
#define LCP_CFG_REQ		1
#define LCP_CFG_ACK		2
#define LCP_CFG_NAK		3
#define LCP_CFG_REJ		4
#define LCP_TERM_REQ	5
#define LCP_TERM_ACK	6
#define LCP_CODE_REJ	7
#define LCP_PROT_REJ	8
#define LCP_ECHO_REQ	9
#define LCP_ECHO_REPLY	10
#define LCP_DISC_REQ	11
#define LCP_IDENTIFY	12

//PPP Ipcp Code
#define IPCP_CFG_REQ	1
#define IPCP_CFG_ACK	2
#define IPCP_CFG_NAK	3
#define IPCP_CFG_REJ	4
#define IPCP_TERM_REQ	5
#define IPCP_TERM_ACK	6
#define IPCP_CODE_REJ	7

//PPP Pap Code
#define PAP_AUTH_REQ	1
#define PAP_AUTH_ACK	2
#define PAP_AUTH_NAK	3

//PPP Chap Code
#define CHAP_CHALLENGE	1
#define CHAP_RESPONSE	2
#define CHAP_SUCCESS	3
#define CHAP_FAILURE	4

// PPP Option Header
#define PPP_OPT_TYPE	0
#define PPP_OPT_LEN		1
#define PPP_OPT_DATA	2

//Lcp Configuration Options
#define LCP_OPT_MRU			1
#define LCP_OPT_AUTH_PROT	3
#define LCP_OPT_MAGIC_NUM	5


//Ipcp Configuration Options
#define IPCP_OPT_IP		3
#define IPCP_OPT_PRIM_DNS	129
#define IPCP_OPT_SEC_DNS	131

#define MASK_IPCP_IP		1
#define MASK_IPCP_PRIM_DNS	2
#define MASK_IPCP_SEC_DNS	4
#define MASK_IPCP_ALL		7

//Auth Protocol option 
#define PPP_AUTH_PAP	0xc023
#define PPP_AUTH_CHAP	0xc223

//PPP Timer Handle
#define PPP_RESTART_TIMER	3
#define MAX_TERMINATE		2
#define MAX_CONFIGURE		10
#define MAX_FAILURE			5
#define MAX_ECHO_TIMER		60

//PPP state definition
#define PPP_STATE_INIT		0
#define PPP_STATE_TERM		1	// Link is terminating
#define PPP_STATE_LCP		2
#define PPP_STATE_PAP		3
#define PPP_STATE_IPCP		4
#define PPP_STATE_IP		5

UCHAR _iPppState, _iPppIdentifier;
UCHAR _iPppCfgTimer, _iPppCfgRetry;
UCHAR _iIpcpOptMask;
UCHAR _iLcpEchoTimer;
USHORT _sPppAuth;
USHORT _sPppOptionLen;
BOOLEAN _bPppAckSend;
BOOLEAN _bPppAckRecvd;

#define LCP_MAGIC_LEN	4
UCHAR _pLcpMagicNumber[LCP_MAGIC_LEN];

const UCHAR _cAuthChapConst[5] = {LCP_OPT_AUTH_PROT, 5, 0xc2, 0x23, 0x05};

void PppInitTimer()
{
	_iPppCfgTimer = 0;
	_iPppCfgRetry = 0;
}

void PppInit()
{
	_iPppState = PPP_STATE_INIT;
	_bPppAckSend = FALSE;
	_bPppAckRecvd = FALSE;
	_iLcpEchoTimer = 0;
	_iPppIdentifier = 1;
	_sPppAuth = 0;
	rand_array(_pLcpMagicNumber, LCP_MAGIC_LEN);
}

void PppoeTearLink()
{
	_iPppState = PPP_STATE_INIT;
	PppoePadt();
	PppoeTerminated();
}

void LcpCfgReq()
{
	PCHAR pCur;

	// LCP options
	pCur = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA);
	pCur[PPP_OPT_TYPE] = LCP_OPT_MAGIC_NUM;
	pCur[PPP_OPT_LEN] = LCP_MAGIC_LEN + 2;
	memcpy((PCHAR)(pCur+PPP_OPT_DATA), _pLcpMagicNumber, LCP_MAGIC_LEN);

	// PPP LCP layer
	PppSendMisc(PPP_PROT_LCP, LCP_MAGIC_LEN + 2, LCP_CFG_REQ, _iPppIdentifier);

	_iPppIdentifier ++;
	_iPppState = PPP_STATE_LCP;
}

void LcpEchoReq()
{
	memcpy((PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA), _pLcpMagicNumber, LCP_MAGIC_LEN);
	PppSendMisc(PPP_PROT_LCP, LCP_MAGIC_LEN, LCP_ECHO_REQ, _iPppIdentifier);
	_iPppIdentifier ++;
}

void LcpTermReq()
{
	PppSendMisc(PPP_PROT_LCP, 0, LCP_TERM_REQ, _iPppIdentifier);
	_iPppIdentifier ++;
	_iPppState = PPP_STATE_TERM;
}

void PapCfgReq()
{
	UCHAR iLen;
	USHORT sTotalLen;
	PCHAR pCur;

	pCur = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA);
	// add PPP ID
	OptionsGetString((PCHAR)(pCur + 1), OPT_PPPOE_ID, MAX_PPPOE_ID_LEN);
	iLen = strlen((PCHAR)(pCur + 1));
	*pCur = iLen;
	sTotalLen = iLen + 1;
	pCur += sTotalLen;
	// add PPP password
	OptionsGetString((PCHAR)(pCur + 1), OPT_PPPOE_PIN, MAX_PPPOE_PIN_LEN);
	iLen = strlen((PCHAR)(pCur + 1));
	*pCur = iLen;
	sTotalLen += iLen + 1;

	PppSendMisc(PPP_PROT_PAP, sTotalLen, PAP_AUTH_REQ, _iPppIdentifier);
	_iPppIdentifier ++;
	_iPppState = PPP_STATE_PAP;
}

void PapStart()
{
	if (_bPppAckRecvd && _bPppAckSend)	// we ignore this because remote is 100% sure to return the ack
	{
		_bPppAckSend = FALSE;
		_bPppAckRecvd = FALSE;
		if (_sPppAuth == PPP_AUTH_PAP && _iPppState != PPP_STATE_PAP)
		{
			PapCfgReq();
			PppInitTimer();
		}
	}
}

UCHAR IpcpAddOpt(USHORT sOffset, UCHAR iType)
{
	PCHAR pCur;

	pCur = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA+sOffset);
	// add IP address
	pCur[PPP_OPT_TYPE] = iType;
	pCur[PPP_OPT_LEN] = IP_ALEN + 2;
	memset((PCHAR)(pCur+PPP_OPT_DATA), 0, IP_ALEN);

	return (IP_ALEN + 2);
}

void IpcpCfgReq()
{
	USHORT sTotalLen;

	sTotalLen = IpcpAddOpt(0, IPCP_OPT_IP);

	// add Dns Address
	if (_iIpcpOptMask & MASK_IPCP_PRIM_DNS)
	{
		sTotalLen += IpcpAddOpt(sTotalLen, IPCP_OPT_PRIM_DNS);
	}
	if (_iIpcpOptMask & MASK_IPCP_SEC_DNS)
	{
		sTotalLen += IpcpAddOpt(sTotalLen, IPCP_OPT_SEC_DNS);
	}

	PppSendMisc(PPP_PROT_IPCP, sTotalLen, IPCP_CFG_REQ, _iPppIdentifier);
	_iPppIdentifier ++;
	_iPppState = PPP_STATE_IPCP;
}

void PppCopyOption(PCHAR pOption, UCHAR iLen)
{
	memcpy((PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA+_sPppOptionLen), pOption, iLen);
	_sPppOptionLen += iLen;
}

UCHAR LcpHandleOption(PCHAR pOption, USHORT sOptionLen)
{
	UCHAR iType, iLen, iRtCode;
	PCHAR pCur;
	USHORT sAuth;

	iRtCode = LCP_CFG_ACK;
	pCur = pOption;
	_sPppOptionLen = 0;
	while (sOptionLen)
	{
		iType = pCur[PPP_OPT_TYPE];
		iLen = pCur[PPP_OPT_LEN];
		
		switch (iType)
		{
		case LCP_OPT_MRU:
			if (iRtCode != LCP_CFG_REJ)
			{
				PppCopyOption(pCur, iLen);
			}
			break;

		case LCP_OPT_AUTH_PROT:
			if (iRtCode != LCP_CFG_REJ)
			{
				sAuth = PCHAR2USHORT((PCHAR)(pCur + PPP_OPT_DATA));
				if (sAuth == PPP_AUTH_PAP || (sAuth == PPP_AUTH_CHAP && pCur[PPP_OPT_DATA+2] == 5))
				{
					_sPppAuth = sAuth;
					PppCopyOption(pCur, iLen);
				}
				else
				{
					iRtCode = LCP_CFG_NAK;
					PppCopyOption(_cAuthChapConst, 5);
				}
			}
			break;

		case LCP_OPT_MAGIC_NUM:
			if (iRtCode != LCP_CFG_REJ)
			{
				PppCopyOption(pCur, iLen);
			}
			break;

		default:
			if (iRtCode != LCP_CFG_REJ)
			{
				iRtCode = LCP_CFG_REJ;
				_sPppOptionLen = 0;
			}
			PppCopyOption(pCur, iLen);
			break;
		}

		pCur += iLen;
		if (sOptionLen <= iLen)		break;
		sOptionLen -= iLen;
	}

	return iRtCode;
}

void LcpRun(PCHAR pOption, USHORT sLen, UCHAR iCode, UCHAR iId)
{
	UCHAR iRtCode;
	PCHAR pData;

	switch (iCode)
	{
	case LCP_CFG_REQ:
		iRtCode = LcpHandleOption(pOption, sLen);
		// Send response
		PppSendMisc(PPP_PROT_LCP, _sPppOptionLen, iRtCode, iId);
		if (iRtCode == LCP_CFG_ACK)
		{
			_bPppAckSend = TRUE;
			PapStart();
		}
		break;

	case LCP_CFG_ACK:
		_bPppAckRecvd = TRUE;
		PapStart();
		break;
	
	case LCP_CFG_NAK:
		TaskUIHandler(UI_PPP_DEBUG, 3);
		break;
	
	case LCP_CFG_REJ:
		TaskUIHandler(UI_PPP_DEBUG, 4);
		break;
	
	case LCP_TERM_REQ:
		pData = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA);
		if (sLen)
		{
			memcpy(pData, pOption, sLen);
		}
		PppSendMisc(PPP_PROT_LCP, sLen, LCP_TERM_ACK, iId);
		PppoeTearLink();
		break;
	
	case LCP_TERM_ACK:
		PppoeTearLink();
		break;
	
	case LCP_ECHO_REQ:
		pData = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA);
		if (sLen)
		{
			memcpy(pData, pOption, sLen);
		}
		memcpy(pData, _pLcpMagicNumber, LCP_MAGIC_LEN);
		PppSendMisc(PPP_PROT_LCP, sLen, LCP_ECHO_REPLY, iId);
		break;
		
	case LCP_ECHO_REPLY:
		_iLcpEchoTimer = 0;
		break;
	}
}

void PapRun(PCHAR pOption, UCHAR iCode, UCHAR iId)
{
	UCHAR iMsgLen;

	switch (iCode)
	{
	case PAP_AUTH_REQ:
		Adapter_pPacketBuf[EP_DATA+PPPOE_INFO+PPP_DATA] = 0;
		PppSendMisc(PPP_PROT_PAP, 1, PAP_AUTH_ACK, iId);
		break;

	case PAP_AUTH_ACK:	//Start IPCP in this case
		PppInitTimer();
		_iIpcpOptMask = MASK_IPCP_ALL;
		IpcpCfgReq();
		break;

	case PAP_AUTH_NAK:
		PppInitTimer();
		LcpTermReq();
		iMsgLen = *pOption;
		pOption ++;
		pOption[iMsgLen] = 0;
		TaskUIHandler(UI_PPP_DEBUG, 0);
		TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pOption);
		break;
	}
}

void ChapRun(PCHAR pOption, USHORT sLen, UCHAR iCode, UCHAR iId)
{
	PCHAR pCur;
	UCHAR iChallengeLen ,iLen;
	UCHAR pHash[MD5_SIGNATURE_SIZE];

	switch (iCode)
	{
	case CHAP_CHALLENGE:
		iChallengeLen = *pOption;
		pOption ++;
		pCur = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA);
		*pCur = iId;
		pCur ++;
		OptionsGetString(pCur, OPT_PPPOE_PIN, MAX_PPPOE_PIN_LEN);
		iLen = strlen(pCur);
		pCur += iLen;
		memcpy(pCur, pOption, iChallengeLen);
		iLen += iChallengeLen + 1;
		pCur = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA);
		TaskMD5GenValue(pHash, pCur, iLen);
		*pCur = MD5_SIGNATURE_SIZE;
		pCur ++;
		memcpy(pCur, pHash, MD5_SIGNATURE_SIZE);
		pCur += MD5_SIGNATURE_SIZE;
		OptionsGetString(pCur, OPT_PPPOE_ID, MAX_PPPOE_ID_LEN);
		iLen = strlen(pCur);
		iLen += MD5_SIGNATURE_SIZE + 1;
		PppSendMisc(PPP_PROT_CHAP, iLen, CHAP_RESPONSE, iId);
		_iPppState = PPP_STATE_PAP;
		break;

	case CHAP_SUCCESS:
		PppInitTimer();
		_iIpcpOptMask = MASK_IPCP_ALL;
		IpcpCfgReq();
		break;

	case CHAP_FAILURE:
		PppInitTimer();
		LcpTermReq();
		pOption[sLen] = 0;
		TaskUIHandler(UI_PPP_DEBUG, 1);
		TaskUIHandler(UI_CALL_DISPLAY_LINE1, (USHORT)pOption);
		break;
	}
}

UCHAR IpcpHandleOption(PCHAR pOption, USHORT sOptionLen)
{
	UCHAR iType, iLen, iRtCode;
	PCHAR pCur;

	pCur = pOption;
	iRtCode = IPCP_CFG_ACK;
	_sPppOptionLen = 0;
	while (sOptionLen)
	{
		iType = pCur[PPP_OPT_TYPE];
		iLen = pCur[PPP_OPT_LEN];	
		if (iType == IPCP_OPT_IP)
		{
			if (iRtCode != IPCP_CFG_REJ)
			{
				PppCopyOption(pCur, iLen);
			}
		}
		else
		{
			if (iRtCode != IPCP_CFG_REJ)
			{
				iRtCode = IPCP_CFG_REJ;
				_sPppOptionLen = 0;
			}
			PppCopyOption(pCur, iLen);
		}
		pCur += iLen;
		if (sOptionLen <= iLen)		break;
		sOptionLen -= iLen;
	}

	return iRtCode;
}

void IpcpGetAddress(PCHAR pOption, USHORT sOptionLen)
{
	UCHAR iType, iLen;
	PCHAR pCur;

	pCur = pOption;
	while (sOptionLen)
	{
		iType = pCur[PPP_OPT_TYPE];
		iLen = pCur[PPP_OPT_LEN];
		
		switch (iType)
		{
		case IPCP_OPT_IP:
			memcpy4(Sys_pIpAddress, (PCHAR)(pCur + PPP_OPT_DATA));
			break;

		case IPCP_OPT_PRIM_DNS:
			memcpy4(Sys_pDnsIp, (PCHAR)(pCur + PPP_OPT_DATA));
			break;

		case IPCP_OPT_SEC_DNS:
			memcpy4(Sys_pDnsIp2, (PCHAR)(pCur + PPP_OPT_DATA));
			break;
		}

		pCur += iLen;
		if (sOptionLen <= iLen)		break;
		sOptionLen -= iLen;
	}
}

UCHAR IpcpGetRejOpt(PCHAR pOption, USHORT sOptionLen)
{
	UCHAR iType, iLen, iMask;
	PCHAR pCur;

	pCur = pOption;
	iMask = MASK_IPCP_ALL;
	while (sOptionLen)
	{
		iType = pCur[PPP_OPT_TYPE];
		iLen = pCur[PPP_OPT_LEN];
		
		switch (iType)
		{
		case IPCP_OPT_IP:
			iMask &= ~MASK_IPCP_IP;
			break;

		case IPCP_OPT_PRIM_DNS:
			iMask &= ~MASK_IPCP_PRIM_DNS;
			break;

		case IPCP_OPT_SEC_DNS:
			iMask &= ~MASK_IPCP_SEC_DNS;
			break;
		}

		pCur += iLen;
		if (sOptionLen <= iLen)		break;
		sOptionLen -= iLen;
	}
	return iMask;
}

void PppConnected()
{
	_bPppAckSend = FALSE;
	_bPppAckRecvd = FALSE;
	_iPppState = PPP_STATE_IP;
	Net_bConnected = TRUE;
	TaskUIHandler(UI_NET_CONNECTED, 0);
}

void IpcpRun(PCHAR pOption, USHORT sLen, UCHAR iCode, UCHAR iId)
{
	UCHAR iRtCode;

	switch (iCode)
	{
	case IPCP_CFG_REQ:
		iRtCode = IpcpHandleOption(pOption, sLen);
		PppSendMisc(PPP_PROT_IPCP, _sPppOptionLen, iRtCode, iId);

		if (iRtCode == IPCP_CFG_ACK)
		{
			_bPppAckSend = TRUE;
			if (_bPppAckRecvd)
			{
				PppConnected();
			}
		}
		break;

	case IPCP_CFG_ACK:
		IpcpGetAddress(pOption, sLen);
		_bPppAckRecvd = TRUE;
		if (_bPppAckSend)
		{
			PppConnected();
		}
		break;

	case IPCP_CFG_NAK:
		if (sLen)
		{
			memcpy((PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA), pOption, sLen);
		}
		PppSendMisc(PPP_PROT_IPCP, sLen, IPCP_CFG_REQ, _iPppIdentifier);
		_iPppIdentifier ++;
		break;

	case IPCP_CFG_REJ:
		_iIpcpOptMask = IpcpGetRejOpt(pOption, sLen);
		if (!_iIpcpOptMask)
		{
			// All option rejected, terminate this link
			PppInitTimer();
			LcpTermReq();
		}
		else
		{
			IpcpCfgReq();
		}
		break;
	}
}

void PppoeSessionRun(PCHAR pPppoeHead)
{
	USHORT sProtocol, sLen;
	UCHAR iCode, iId;
	PCHAR pOption;

	sProtocol = PCHAR2USHORT((PCHAR)(pPppoeHead + PPPOE_PROTOCOL_ID));
	pOption = Adapter_pReceivePacket;

	sLen = PCHAR2USHORT((PCHAR)(pOption + PPP_LEN));
	if (sLen < PPP_DATA)
	{
		return;
	}

	iCode = pOption[PPP_CODE];
	iId = pOption[PPP_ID];
	pOption += PPP_DATA;
	sLen -= PPP_DATA;
	switch (sProtocol)
	{
	case PPP_PROT_LCP:
		LcpRun(pOption, sLen, iCode, iId);
		break;

	case PPP_PROT_PAP:
		PapRun(pOption, iCode, iId);
		break;

	case PPP_PROT_CHAP:
		ChapRun(pOption, sLen, iCode, iId);
		break;

	case PPP_PROT_IPCP:
		IpcpRun(pOption, sLen, iCode, iId);
		break;

	default:
		pOption = (PCHAR)(Adapter_pPacketBuf+EP_DATA+PPPOE_INFO+PPP_DATA);
		// add PPP ID
		USHORT2PCHAR(sProtocol, pOption);
		pOption += 2;	
		memcpy(pOption, Adapter_pReceivePacket, sLen + PPP_DATA);
		PppSendMisc(PPP_PROT_LCP, sLen + PPP_DATA + 2, LCP_PROT_REJ, iId);
		break;
	}
}

void PppTimer()
{
	if (_iPppState == PPP_STATE_INIT)	return;
	
	if (_iPppState  < PPP_STATE_IP)
	{
		_iPppCfgTimer ++;
		if (_iPppCfgTimer > PPP_RESTART_TIMER)
		{
			_iPppCfgTimer = 0;
			if (_iPppState == PPP_STATE_TERM)
			{
				if (_iPppCfgRetry < MAX_FAILURE)
				{
					_iPppCfgRetry ++;
					LcpTermReq();
				}
				else
				{
					PppoeTearLink();
				}
			}
			else
			{
				if (_iPppCfgRetry < MAX_CONFIGURE)
				{
					_iPppCfgRetry ++;
					if (_iPppState == PPP_STATE_LCP)
					{
						LcpCfgReq();
					}
					else if (_iPppState == PPP_STATE_PAP)
					{
						if (_sPppAuth == PPP_AUTH_PAP)
						{
							PapCfgReq();
						}
					}
					else if (_iPppState == PPP_STATE_IPCP)
					{
						IpcpCfgReq();
					}
				}
				else
				{
					PppInitTimer();
					LcpTermReq();
				}
			}
		}
	}
	if (_iPppState > PPP_STATE_LCP)
	{
		_iLcpEchoTimer ++;
		if (_iLcpEchoTimer > MAX_ECHO_TIMER)
		{
			_iLcpEchoTimer = 0;
			LcpEchoReq();
		}
	}
}

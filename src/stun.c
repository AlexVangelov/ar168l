/*-------------------------------------------------------------------------
   AR1688 STUN function copy right information

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

// stun.fwdnet.net:3478
// stun.sipgate.net:10000

/*
Public STUN servers:
stun.ekiga.net
stun.fwdnet.net (no XOR_MAPPED_ADDRESS support) 
stun.ideasip.com (no XOR_MAPPED_ADDRESS support) 
stun01.sipphone.com (no DNS SRV record) 
stun.softjoys.com (no DNS SRV record) (no XOR_MAPPED_ADDRESS support) 
stun.voipbuster.com (no DNS SRV record) (no XOR_MAPPED_ADDRESS support) 
stun.voxgratia.org (no DNS SRV record) (no XOR_MAPPED_ADDRESS support) 
stun.xten.com 
stunserver.org (see their usage policy)
stun.sipgate.net:10000 
numb.viagenie.ca (http://numb.viagenie.ca) (no XOR_MAPPED_ADDRESS support) 

Various types of NAT (still according to the RFC):

Full Cone:A full cone NAT is one where all requests from the same internal 
	  IP address and port are mapped to the same external IP address 
	  and port. Furthermore, any external host can send a packet to 
	  the internal host, by sending a packet to the mapped external address. 

Restricted Cone: A restricted cone NAT is one where all requests from the 
	         same internal IP address and port are mapped to the same
		 external IP address and port. Unlike a full cone NAT, an 
		 external host (with IP address X) can send a packet to the 
		 internal host only if the internal host had previously sent 
		 a packet to IP address X. 

Port Restricted Cone: A port restricted cone NAT is like a restricted cone NAT, 
		      but the restriction includes port numbers. Specifically, 
		      an external host can send a packet, with source IP address X 
	              and source port P, to the internal host only if the internal 
                      host had previously sent a packet to IP address X and port P. 

Symmetric: A symmetric NAT is one where all requests from the same internal IP address 
           and port, to a specific destination IP address and port, are mapped to the 
           same external IP address and port. If the same host sends a packet with the
           same source address and port, but to a different destination, a different 
           mapping is used. Furthermore, only the external host that receives a packet 
           can send a UDP packet back to the internal host. 
*/


#include "version.h"
#include "type.h"

#ifdef CALL_SIP

#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "bank2.h"

// NAT type
#define NAT_TYPE_UNKNOWN			0
#define NAT_TYPE_BLOCKED			1
#define NAT_TYPE_OPEN_INTERNET		2
#define NAT_TYPE_SYM_FIREWALL		3
#define NAT_TYPE_FULL_CONE			4
#define NAT_TYPE_RESTRICTED			5
#define NAT_TYPE_PORT_RESTRICTED	6	
#define NAT_TYPE_SYMMETRIC			7

#define MSG_FIELD_TYPE		0
#define MSG_FIELD_LENGTH	2
#define MSG_FIELD_TRANS_ID	4
#define MSG_FIELD_DATA		20

#define MSG_TYPE_BREQ		0x0001  // Binding Request
#define MSG_TYPE_BRESP		0x0101  // Binding Response
#define MSG_TYPE_BERESP		0x0111  // Binding Error Response
#define MSG_TYPE_SSREQ		0x0002  // Shared Secret Request
#define MSG_TYPE_SSRESP		0x0102  // Shared Secret Response
#define MSG_TYPE_SSERESP	0x0112  // Shared Secret Error Response

#define MSG_ATTR_TYPE		0
#define MSG_ATTR_LENGTH		2
#define MSG_ATTR_VALUE		4

#define ATTR_MAPPED_ADDRESS		0x01
#define ATTR_RESPONSE_ADDRESS	0x02
#define ATTR_CHANGE_REQUEST		0x03
#define ATTR_SOURCE_ADDRESS		0x04
#define ATTR_CHANGED_ADDRESS	0x05
#define ATTR_USERNAME			0x06
#define ATTR_PASSWORD			0x07
#define ATTR_MESSAGE_INTEGRITY  0x08
#define ATTR_ERROR_CODE			0x09
#define ATTR_UNKNOWN_ATTRIBUTES 0x0a
#define ATTR_REFLECTED_FROM		0x0b

#define CHANGE_IP_MASK		0x04
#define CHANGE_PORT_MASK	0x02

#define STUN_STATE_IDLE			0
#define STUN_STATE_FIRST_TEST1	1
#define STUN_STATE_TEST2		2
#define STUN_STATE_SECOND_TEST1	3
#define STUN_STATE_TEST3		4
#define STUN_STATE_CHECKING		5

#define STUN_TRANS_ID_LEN	16
#define STUN_SEND_LEN		28

UDP_SOCKET _pStunSocket;
UCHAR _iStunNatType;
UCHAR _iStunState;
UCHAR _iStunTask;
UCHAR _iStunRetry;
UCHAR _iStunTimer;
UCHAR _pStunTranId[STUN_TRANS_ID_LEN];
UCHAR _pStunServerIP[IP_ALEN];
UCHAR _pStunMappedAddr[IP_ALEN];
USHORT _sStunMappedPort;
UCHAR _pStunChangedAddr[IP_ALEN];
USHORT _sStunChangedPort;

/**********************************************************************
 ** STUN functions                                                   **
 **********************************************************************
 */
void StunInit()
{
	_pStunSocket = NULL;
	_iStunTask = STUN_TASK_NONE;
	_iStunState = STUN_STATE_IDLE;
	memset(_pStunTranId, 0, STUN_TRANS_ID_LEN);
	memset(_pStunMappedAddr, 0, IP_ALEN);
	memset(_pStunChangedAddr, 0, IP_ALEN);
	_sStunMappedPort = 0;
	_sStunChangedPort = 0;
}

void stun_send_req()
{
	PCHAR pCur;

	pCur = Adapter_pUdpBuf;
	// Header
	USHORT2PCHAR(MSG_TYPE_BREQ, pCur);
	pCur[2] = 0;
	pCur[3] = 8;
	pCur += 4;
	memcpy(pCur, _pStunTranId, STUN_TRANS_ID_LEN);
	pCur += STUN_TRANS_ID_LEN;
	// Change-Request Attribute
	memset(pCur, 0, 8);
	pCur[1] = ATTR_CHANGE_REQUEST;
	pCur[3] = 4;

	if (_iStunState == STUN_STATE_TEST2)
	{
		// change IP and port
		pCur[7] |= CHANGE_IP_MASK;
		pCur[7] |= CHANGE_PORT_MASK;
	}
	else if (_iStunState == STUN_STATE_TEST3)
	{
		// change port only
		pCur[7] |= CHANGE_PORT_MASK;
	}

	if (_iStunState == STUN_STATE_SECOND_TEST1 || _iStunState == STUN_STATE_TEST3)
	{
		UdpSendTo(_pStunSocket, STUN_SEND_LEN, _pStunChangedAddr, _sStunChangedPort);
	}
	else
	{
		UdpSendTo(_pStunSocket, STUN_SEND_LEN, _pStunServerIP, OptionsGetShort(OPT_STUN_PORT));
	}
}

void stun_start_test()
{
	rand_array(_pStunTranId, STUN_TRANS_ID_LEN);
	_iStunRetry = 0;
	_iStunTimer = 0;
	stun_send_req();
}


void stun_start(PCHAR pServerIP)
{
	if (pServerIP)
	{
		UdpDebugString("first test1");
		memcpy4(_pStunServerIP, pServerIP);
		_iStunState = STUN_STATE_FIRST_TEST1;
		stun_start_test();
	}
	else
	{
		UdpDebugString("Nat type: unknown");
		_iStunNatType = NAT_TYPE_UNKNOWN;
		TaskStunDone(NULL, 0, _iStunTask);
	}
}

void StunMap(UCHAR iTask, UDP_SOCKET pSocket)
{
	UCHAR pDns[MAX_DNS_LEN];
	UCHAR pDstIP[IP_ALEN];

	_iStunTask = iTask;
	_pStunSocket = pSocket;
	if (iTask == STUN_TASK_LOGON)
	{
		_iStunNatType = NAT_TYPE_UNKNOWN;
		OptionsGetString(pDns, OPT_STUN_SERVER, MAX_DNS_LEN);
		if (strlen(pDns))
		{
			if (str2ip(pDns, pDstIP))
			{
				stun_start(pDstIP);
			}
			else
			{
				TaskDnsQuery(pDns, DNS_TYPE_A, NET_BANK_OFFSET, (USHORT)stun_start);
			}
		}
		else
		{
			stun_start(NULL);
		}
	}
	else
	{
		if (_iStunNatType == NAT_TYPE_UNKNOWN)
		{
			TaskStunDone(NULL, 0, _iStunTask);
		}
		else
		{
			_iStunState = STUN_STATE_CHECKING;
			stun_start_test();
		}
	}
}

BOOLEAN StunRun(UDP_SOCKET pUcb)
{
	PCHAR pCur;
	USHORT sCurLen, sMsgLen, sMsgType, sMappedPort, sChangedPort;
	PCHAR pMappedAddr;
	PCHAR pChangedAddr;
	PCHAR pData;

	if (_iStunState == STUN_STATE_IDLE)		return FALSE;
	if (pUcb != _pStunSocket)		return FALSE;

	pData = Adapter_pUdpBuf;

	if (memcmp((PCHAR)(pData + MSG_FIELD_TRANS_ID), _pStunTranId, STUN_TRANS_ID_LEN))		return FALSE;

	sMsgType = PCHAR2USHORT((PCHAR)(pData + MSG_FIELD_TYPE));
	if (sMsgType == MSG_TYPE_BERESP)
	{
		TaskStunDone(NULL, 0, _iStunTask);
		return TRUE;
	}
	if (sMsgType != MSG_TYPE_BRESP)		return TRUE;
	if (pUcb->sLen < MSG_FIELD_DATA)			return TRUE;

	pMappedAddr = NULL;
	pChangedAddr = NULL;
	sMappedPort = 0;
	sChangedPort = 0;
	sMsgLen = PCHAR2USHORT((PCHAR)(pData + MSG_FIELD_LENGTH));
	sMsgLen += MSG_FIELD_DATA;
	sCurLen = MSG_FIELD_DATA;
	while (sCurLen < sMsgLen)
	{
		pCur = (PCHAR)(pData + sCurLen);
		sCurLen += MSG_ATTR_VALUE;
		sCurLen += PCHAR2USHORT((PCHAR)(pCur + MSG_ATTR_LENGTH));
		if (pCur[0] != 0)	continue;
		if (pCur[1] == ATTR_MAPPED_ADDRESS)
		{
			pCur += MSG_ATTR_VALUE;
			pMappedAddr = (PCHAR)(pCur + 4);
			sMappedPort = PCHAR2USHORT((PCHAR)(pCur + 2));
		}
		else if (pCur[1] == ATTR_CHANGED_ADDRESS)
		{
			pCur += MSG_ATTR_VALUE;
			pChangedAddr = (PCHAR)(pCur + 4);
			sChangedPort = PCHAR2USHORT((PCHAR)(pCur + 2));
		}
	}

	if (_iStunState == STUN_STATE_CHECKING)
	{
		if (pMappedAddr)
		{
			memcpy4(_pStunMappedAddr, pMappedAddr);
			if (_iStunNatType == NAT_TYPE_FULL_CONE || _iStunNatType == NAT_TYPE_RESTRICTED || _iStunNatType == NAT_TYPE_PORT_RESTRICTED)
			{
				_sStunMappedPort = sMappedPort;
				TaskStunDone(_pStunMappedAddr, _sStunMappedPort, _iStunTask);
			}
			else
			{
				TaskStunDone(_pStunMappedAddr, 0, _iStunTask);
			}
		}
		else
		{
			TaskStunDone(NULL, 0, _iStunTask);
		}
	}
	else if (_iStunState == STUN_STATE_FIRST_TEST1)
	{
		if (pMappedAddr)
		{
			memcpy4(_pStunMappedAddr, pMappedAddr);
			_sStunMappedPort = sMappedPort;
		}
		if (pChangedAddr)
		{
			memcpy4(_pStunChangedAddr, pChangedAddr);
			_sStunChangedPort = sChangedPort;
		}
		UdpDebugString("test2");
		_iStunState = STUN_STATE_TEST2;
		stun_start_test();
	}
	else if (_iStunState == STUN_STATE_SECOND_TEST1)
	{
		if (memcmp(_pStunMappedAddr, pMappedAddr, IP_ALEN) || _sStunMappedPort != sMappedPort)
		{
			UdpDebugString("Nat type: symmetric");
			_iStunNatType = NAT_TYPE_SYMMETRIC;
			TaskStunDone(_pStunMappedAddr, 0, _iStunTask);
		}
		else
		{
			UdpDebugString("test3");
			_iStunState = STUN_STATE_TEST3;
			stun_start_test();
		}
	}
	else if (_iStunState == STUN_STATE_TEST2)
	{
		UdpDebugString("Nat type: full cone");
		_iStunNatType = NAT_TYPE_FULL_CONE;
		TaskStunDone(_pStunMappedAddr, _sStunMappedPort, _iStunTask);
	}
	else if (_iStunState == STUN_STATE_TEST3)
	{
		UdpDebugString("Nat type: resricted");
		_iStunNatType = NAT_TYPE_RESTRICTED;
		TaskStunDone(_pStunMappedAddr, _sStunMappedPort, _iStunTask);
	}
	return TRUE;
}

void StunTimer()
{
	if (_iStunState == STUN_STATE_IDLE)		return;

	_iStunTimer ++;
	if (_iStunTimer > 2)
	{
		_iStunTimer = 0;
		_iStunRetry ++;
		if (_iStunRetry > 2)
		{
			switch (_iStunState)
			{
			case STUN_STATE_CHECKING:
				TaskStunDone(NULL, 0, _iStunTask);
				break;

			case STUN_STATE_FIRST_TEST1:
			case STUN_STATE_SECOND_TEST1:
				UdpDebugString("Nat type: unknown");
				_iStunNatType = NAT_TYPE_UNKNOWN;
				TaskStunDone(NULL, 0, _iStunTask);
				break;

			case STUN_STATE_TEST2:
				UdpDebugString("second test1");
				_iStunState = STUN_STATE_SECOND_TEST1;
				stun_start_test();
				break;

			case STUN_STATE_TEST3:
				UdpDebugString("Nat type: port restricted");
				_iStunNatType = NAT_TYPE_PORT_RESTRICTED;
				TaskStunDone(_pStunMappedAddr, _sStunMappedPort, _iStunTask);
				break;
			}
		}
		else
		{
			stun_send_req();
		}
	}
}

#endif

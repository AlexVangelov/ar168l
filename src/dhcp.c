/*-------------------------------------------------------------------------
   AR1688 DHCP function copy right information

   Copyright (c) 2006-2011. Tang, Li      <tangli@palmmicro.com>
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

/* Dynamic Host Configuration Protocol (see RFC 2131, 1533) */
#define DHCP_SERVER_PORT	67
#define DHCP_CLIENT_PORT	68

#define DHCP_MIN_PACKET_SIZE	300

/* Field offset of an BOOTP&DHCP packet (following the UDP header) */
#define DHCP_OP			0	/* specifies whether the message is a request(1) or a reponse(2) */
#define DHCP_HTYPE		1	/* network hardware type (ehernet has type 1) */
#define DHCP_HLEN		2	/* length of hardware address length */
#define DHCP_HOPS		3	/* allow bootstraping across multiple routers */
#define DHCP_XID		4	/* an integer used to match response with request */
#define DHCP_SECS		8	/* number of seconds since the client boot */
#define DHCP_FLAGS		10	/* flags field */
#define DHCP_CIADDR		12	/* client know about its IP address */
#define DHCP_YIADDR		16	/* server assigned IP address */
#define DHCP_SIADDR		20	/* IP address of next server to use*/
#define DHCP_GIADDR		24	/* Relay Agent IP address */
#define DHCP_CHADDR		28	/* client hardware address */
#define DHCP_SNAME		44	/* server host name */
#define DHCP_FILE		108	/* boot file name */
#define DHCP_OPTIONS	236	/* DHCP options begins, 4 bytes magic cookie*/
#define DHCP_OPLIST		240	/* DHCP options list begins*/

/* DHCP options item = code + length + value */
#define DHCP_OPCODE_PADDING			0
#define DHCP_OPCODE_SUBNET_MASK		1
#define DHCP_OPCODE_DAY_TIME		2
#define DHCP_OPCODE_ROUTER_IP		3
#define DHCP_OPCODE_TIME_IP			4
#define DHCP_OPCODE_DNS_IP			6
#define DHCP_OPCODE_REQUESTED_IP	50
#define DHCP_OPCODE_LEASE_TIME		51	/* in units of seconde */
#define DHCP_OPCODE_OPTION_OVERLOAD	52
#define DHCP_OPCODE_MESSAGE_TYPE	53
#define DHCP_OPCODE_SERVER_ID		54	/* this is also server IP */ 
#define DHCP_OPCODE_PARAM_LIST		55
#define DHCP_OPCODE_MESSAGE			56	/* provide an error message */
#define DHCP_OPCODE_MAX_SIZE		57	/* minimum leagal value is 576 */
#define DHCP_OPCODE_RENEWAL_TIME	58	/* T1 */
#define DHCP_OPCODE_REBIND_TIME		59
#define DHCP_OPCODE_CLIENT_ID		61
#define DHCP_OPCODE_TFTP_SERVER		66
#define DHCP_OPCODE_RESERVED		128
#define DHCP_OPCODE_END				255

/* DHCP options overload item value */
#define DHCP_OVERLOAD_FILE		1
#define DHCP_OVERLOAD_SNAME		2
#define DHCP_OVERLOAD_BOTH		3

/* DHCP options message item value */
#define DHCP_MTYPE_DISCOVER		1
#define DHCP_MTYPE_OFFER		2
#define DHCP_MTYPE_REQUEST		3
#define DHCP_MTYPE_DECLINE		4
#define DHCP_MTYPE_ACK			5
#define DHCP_MTYPE_NACK			6
#define DHCP_MTYPE_RELEASE		7
#define DHCP_MTYPE_INFORM		8
#define DHCP_MTYPE_NONE			255

#define DHCP_OP_REQUEST		1
#define DHCP_OP_REPLY		2

#define DHCP_FLAG_BROADCAST		0x80
#define DHCP_FLAG_UNICAST		0x00

// Dhcp state
#define DHCP_STATE_INIT			0
#define DHCP_STATE_SELECTING	1
#define DHCP_STATE_REQUESTING	2
#define DHCP_STATE_BOUND		3
#define DHCP_STATE_RENEWING		4
#define DHCP_STATE_REBINDING	5
#define DHCP_STATE_CHECKING		6

#define DHCP_FLAGS_LEN		2
#define DHCP_SECS_LEN		2
#define DHCP_XID_LEN		4
#define MAGIC_COOKIE_LEN	4

// Dhcp Timer
#define DHCP_RETRY_TIMEOUT		6
#define DHCP_MAX_RETRY			5

const UCHAR _cMagicCookie[MAGIC_COOKIE_LEN] = {99, 130, 83, 99};
const UCHAR _cPacketHead[4] = {DHCP_OP_REQUEST, 1, HW_ALEN, 0};
const UCHAR _cBroadcastFlags[2] = {0x80, 0x00};

void DhcpRun(UDP_SOCKET pUcb);

UDP_SOCKET _pDhcpSocket;

UCHAR _iDhcpState, _iDhcpTimer, _iDhcpRetry;
UCHAR _pDhcpXID[DHCP_XID_LEN];

UCHAR _pDhcpServerIP[IP_ALEN];
UCHAR _pDhcpServerID[IP_ALEN];
UCHAR _pAssignedIP[IP_ALEN];

ULONG _lLeaseTime, _lT1, _lT2;
ULONG _lCurrent;

#ifdef OEM_AUTOTEST
UDP_SOCKET Dhcpd_pSocket;
void DhcpdRun(UDP_SOCKET pUcb);
#endif

void _SendPacket(UCHAR iOptionsLen)
{
	USHORT sLength;
	PCHAR pBuf;

	pBuf = Adapter_pUdpBuf;

	memcpy4(pBuf, _cPacketHead);
	memcpy4((PCHAR)(pBuf+DHCP_XID), _pDhcpXID);
	memset((PCHAR)(pBuf+DHCP_SECS), 0, DHCP_SECS_LEN);
	memset((PCHAR)(pBuf+DHCP_YIADDR), 0, DHCP_OPTIONS - DHCP_YIADDR);
	memcpy((PCHAR)(pBuf+DHCP_CHADDR), Sys_pMacAddress, HW_ALEN);
	memcpy4((PCHAR)(pBuf+DHCP_OPTIONS), _cMagicCookie);

	sLength = iOptionsLen;
	sLength += DHCP_OPLIST;
	if (sLength < DHCP_MIN_PACKET_SIZE)
	{
		memset((PCHAR)(pBuf + sLength), 0, (UCHAR)(DHCP_MIN_PACKET_SIZE - sLength));
		sLength = DHCP_MIN_PACKET_SIZE;
	}

	UdpSendTo(_pDhcpSocket, sLength, (_iDhcpState == DHCP_STATE_RENEWING) ? _pDhcpServerIP : (PCHAR)_pBroadcastIP, DHCP_SERVER_PORT);
}

UCHAR _FillMsgType(PCHAR pBuf, UCHAR iMsgType)
{
	pBuf[0] = DHCP_OPCODE_MESSAGE_TYPE;
	pBuf[1] = 1;
	pBuf[2] = iMsgType;
	return 3;
}

UCHAR _FillClientId(PCHAR pBuf)
{
	pBuf[0] = DHCP_OPCODE_CLIENT_ID;
	pBuf[1] = 1 + HW_ALEN;
	pBuf[2] = 1;
	memcpy((PCHAR)(pBuf + 3), Sys_pMacAddress, HW_ALEN);
	return (3 + HW_ALEN);
}

#define PARAM_LIST_LEN	5
const UCHAR _cParamList[PARAM_LIST_LEN] = {DHCP_OPCODE_PARAM_LIST, 3, DHCP_OPCODE_SUBNET_MASK,	DHCP_OPCODE_ROUTER_IP, DHCP_OPCODE_DNS_IP}; 
UCHAR _FillParamList(PCHAR pBuf)
{
	memcpy(pBuf, _cParamList, PARAM_LIST_LEN);
	return (UCHAR)PARAM_LIST_LEN;
}

#define MAX_SIZE_LEN	4
const UCHAR _cMaxSize[MAX_SIZE_LEN] = {DHCP_OPCODE_MAX_SIZE, 2, 4, 0};
UCHAR _FillMaxSize(PCHAR pBuf)
{
	memcpy4(pBuf, _cMaxSize);
	return (UCHAR)MAX_SIZE_LEN;
}

UCHAR _FillIPAddr(PCHAR pBuf, PCHAR pIPAddr, UCHAR iOpCode)
{
	pBuf[0] = iOpCode;
	pBuf[1] = IP_ALEN;
	memcpy4((PCHAR)(pBuf + 2), pIPAddr);
	return (UCHAR)(2 + IP_ALEN);
}

void _DhcpAck()
{
	UdpDebugString("DHCP bound");
	UdpClose(_pDhcpSocket);

	Net_bConnected = TRUE;
	Dhcp_bRenewing = FALSE;
	_lCurrent = 0;
	_iDhcpState = DHCP_STATE_BOUND;
}

void _DhcpNack()
{
	UdpClose(_pDhcpSocket);
	_iDhcpState = DHCP_STATE_INIT;
	Net_bConnected = FALSE;
	Dhcp_bRenewing = FALSE;
#ifdef OEM_IP20
	TaskUIHandler(UI_STBY_DISPLAY_IDS2, IDS_DHCP_FAILED);
#else
	TaskUIHandler(UI_STBY_DISPLAY_IDS0, IDS_DHCP_FAILED);
#endif
}

void _DhcpDone()
{
	memcpy4(Sys_pIpAddress, _pAssignedIP); 
	_DhcpAck();
	TaskUIHandler(UI_NET_CONNECTED, 0);
}

void DhcpOpenUdp()
{
	_pDhcpSocket = UdpListen(DHCP_CLIENT_PORT, NET_BANK_OFFSET, (USHORT)DhcpRun);
}

void DhcpInit()
{
	rand_array(_pDhcpXID, DHCP_XID_LEN);
	_iDhcpState = DHCP_STATE_INIT;
	_iDhcpRetry = 0;
	_lLeaseTime = 0;
	_lCurrent = 0;

	DhcpOpenUdp();
}

#ifdef OEM_INNOMEDIA
#define INNOMEDIA_PARAM_LEN		11
const UCHAR _cInnomediaParam[INNOMEDIA_PARAM_LEN] = {60, 9, 'i','n','n','o','m','e','d','i','a'};
#endif
void DhcpDiscover()
{
	UCHAR iLen;
	PCHAR pOptions;
	PCHAR pBuf;

	pBuf = Adapter_pUdpBuf;
	pOptions = (PCHAR)(pBuf + DHCP_OPLIST);
	iLen = _FillMsgType(pOptions, DHCP_MTYPE_DISCOVER);
	iLen += _FillClientId((PCHAR)(pOptions + iLen));
	iLen += _FillMaxSize((PCHAR)(pOptions + iLen));
#ifdef OEM_INNOMEDIA
	memcpy((PCHAR)(pOptions + iLen), _cInnomediaParam, INNOMEDIA_PARAM_LEN);
	iLen += INNOMEDIA_PARAM_LEN;
#endif
	pOptions[iLen] = DHCP_OPCODE_END;
	iLen ++;

	// set flags and ciaddr
//	pBuf[DHCP_FLAGS] = DHCP_FLAG_BROADCAST;
	pBuf[DHCP_FLAGS] = DHCP_FLAG_UNICAST;
	pBuf[DHCP_FLAGS+1] = 0;
	memset((PCHAR)(pBuf+DHCP_CIADDR), 0, IP_ALEN);

	_SendPacket(iLen);
	_iDhcpState = DHCP_STATE_SELECTING;
	// Set this timer to avoid users to wait overly long before give up
	_iDhcpTimer = 0;
}

void DhcpDecline()
{
	UCHAR iLen;
	PCHAR pOptions;
	PCHAR pBuf;

	pBuf = Adapter_pUdpBuf;
	pOptions = (PCHAR)(pBuf + DHCP_OPLIST);
	iLen = _FillMsgType(pOptions, DHCP_MTYPE_DECLINE);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen), _pAssignedIP, DHCP_OPCODE_REQUESTED_IP);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen) ,_pDhcpServerID, DHCP_OPCODE_SERVER_ID);
	iLen += _FillClientId((PCHAR)(pOptions + iLen));
	pOptions[iLen] = DHCP_OPCODE_END;
	iLen ++;
	// set flags and ciaddr
	pBuf[DHCP_FLAGS] = 0;
	pBuf[DHCP_FLAGS+1] = 0;
	memset((PCHAR)(pBuf+DHCP_CIADDR), 0, IP_ALEN);

	_SendPacket(iLen);
}

void _DhcpRequest()
{
	UCHAR iLen;
	PCHAR pOptions;
	PCHAR pBuf;

	pBuf = Adapter_pUdpBuf;
	pOptions = (PCHAR)(pBuf + DHCP_OPLIST);
	iLen = _FillMsgType(pOptions, DHCP_MTYPE_REQUEST);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen), _pAssignedIP, DHCP_OPCODE_REQUESTED_IP);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen) ,_pDhcpServerID, DHCP_OPCODE_SERVER_ID);
	iLen += _FillClientId((PCHAR)(pOptions + iLen));
	iLen += _FillParamList((PCHAR)(pOptions + iLen));
	iLen += _FillMaxSize((PCHAR)(pOptions + iLen));
	pOptions[iLen] = DHCP_OPCODE_END;
	iLen ++;
	// set flags and ciaddr
	pBuf[DHCP_FLAGS] = DHCP_FLAG_BROADCAST;
	pBuf[DHCP_FLAGS+1] = 0;
	memset((PCHAR)(pBuf+DHCP_CIADDR), 0, IP_ALEN);

	_SendPacket(iLen);
	_iDhcpTimer = 0;
}

void _DhcpReqRenew()
{
	UCHAR iLen;
	PCHAR pOptions;
	PCHAR pBuf;

	pBuf = Adapter_pUdpBuf;
	pOptions = (PCHAR)(pBuf + DHCP_OPLIST);
	iLen = _FillMsgType(pOptions, DHCP_MTYPE_REQUEST);
	iLen += _FillClientId((PCHAR)(pOptions + iLen));
	iLen += _FillParamList((PCHAR)(pOptions + iLen));
	iLen += _FillMaxSize((PCHAR)(pOptions + iLen));
	pOptions[iLen] = DHCP_OPCODE_END;
	iLen ++;
	// set flags and ciaddr
//	pBuf[DHCP_FLAGS] = 0;
//	pBuf[DHCP_FLAGS+1] = 0;
	USHORT2PCHAR(0, (PCHAR)(pBuf + DHCP_FLAGS));
	memcpy4((PCHAR)(pBuf + DHCP_CIADDR), Sys_pIpAddress);

	_SendPacket(iLen);
}

void DhcpRun(UDP_SOCKET pUcb)
{
	UCHAR iLength, iType, iMsgType;
	PCHAR pOption;
	PCHAR pBuf;
	USHORT sIndex;
	UCHAR pServerID[IP_ALEN];
	UCHAR pSubnetMask[IP_ALEN];
	UCHAR pRouteIP[IP_ALEN];
	UCHAR pDnsIP[IP_ALEN];
	UCHAR pDnsIP2[IP_ALEN];
	BOOLEAN bDns2;
	ULONG lLeaseTime, lT1, lT2;
#ifdef OEM_INNOMEDIA
	BOOLEAN bHasProvUrl;
#endif

	if (pUcb != _pDhcpSocket)	return;

	pBuf = Adapter_pUdpBuf;

	if (_iDhcpState == DHCP_STATE_CHECKING || 
		pBuf[DHCP_OP] != DHCP_OP_REPLY || 
		memcmp((PCHAR)(pBuf + DHCP_XID), _pDhcpXID, DHCP_XID_LEN) ||
		memcmp((PCHAR)(pBuf + DHCP_CHADDR), Sys_pMacAddress, HW_ALEN))
	{
		return;
	}

	if (_iDhcpState == DHCP_STATE_REQUESTING || _iDhcpState == DHCP_STATE_RENEWING)
	{
//		if (memcmp(_pDhcpServerIP, (IsValidIP((PCHAR)(pBuf + DHCP_GIADDR))) ? (PCHAR)(pBuf + DHCP_GIADDR) : UdpGetDstIP(pUcb), IP_ALEN))
		if (memcmp(_pDhcpServerIP, UdpGetDstIP(pUcb), IP_ALEN))
		{
			return;
		}
	}

#ifdef OEM_INNOMEDIA
	bHasProvUrl = FALSE;
#endif
	sIndex = DHCP_OPLIST;
	lT1 = 0;
	lT2 = 0;
	bDns2 = FALSE;
	iMsgType = DHCP_MTYPE_NONE;
	lLeaseTime = 0;
	while (sIndex < pUcb->sLen)
	{
		iType = pBuf[sIndex];
		if (iType == DHCP_OPCODE_END)
		{
			break;
		}
		if (iType == DHCP_OPCODE_PADDING)
		{
			sIndex ++;
			continue;
		}
		else
		{
			iLength = pBuf[sIndex+1];
		}
		sIndex += 2;
		pOption = (PCHAR)(pBuf + sIndex);
		switch (iType)
		{
		case DHCP_OPCODE_MESSAGE_TYPE:
			iMsgType = pOption[0];
			break;
/*		sometimes server ID is not the same as the server IP address */
		case DHCP_OPCODE_SERVER_ID:
			memcpy4(pServerID, pOption);
			break;

		case DHCP_OPCODE_SUBNET_MASK:
			memcpy4(pSubnetMask, pOption);
			break;

		case DHCP_OPCODE_ROUTER_IP:
			memcpy4(pRouteIP, pOption);
			break;

		case DHCP_OPCODE_DNS_IP:
			memcpy4(pDnsIP, pOption);
			if (iLength > IP_ALEN)
			{
				memcpy4(pDnsIP2, (PCHAR)(pOption+IP_ALEN));
				bDns2 = TRUE;
			}
			break;

		case DHCP_OPCODE_LEASE_TIME:
			lLeaseTime = PCHAR2ULONG(pOption);
			if (!lT1)	lT1 = rr_32x8(lLeaseTime, 1);
			if (!lT2)	lT2 = lLeaseTime - rr_32x8(lLeaseTime, 3);
			break;

		case DHCP_OPCODE_MESSAGE:
			break;

		case DHCP_OPCODE_RENEWAL_TIME:
			lT1 = PCHAR2ULONG(pOption);
			break;

		case DHCP_OPCODE_REBIND_TIME:
			lT2 = PCHAR2ULONG(pOption);
			break;
#ifdef OEM_INNOMEDIA
		case DHCP_OPCODE_TFTP_SERVER:
			if (iLength > 7 && !memcmp_str(pOption, "http://"))
			{
				iLength -= 7;
				pOption += 7;
				if (Sys_pProvUrl)
				{
					free(Sys_pProvUrl);
				}
				Sys_pProvUrl = (PCHAR)malloc(iLength + 1);
				memcpy(Sys_pProvUrl, pOption, iLength);
				Sys_pProvUrl[iLength] = 0;
				bHasProvUrl = TRUE;
			}
			break;
#endif
		default:
			// other reserved options are in pOption, iType, sLen
			break;
		}
		sIndex += iLength;
	}

	// Respond message
	switch (_iDhcpState)
	{
	case DHCP_STATE_SELECTING:
		if (iMsgType == DHCP_MTYPE_OFFER)
		{
#ifdef OEM_INNOMEDIA
			if (bHasProvUrl)
#endif
			{
				_iDhcpState = DHCP_STATE_REQUESTING;
				_iDhcpRetry = 0;
				memcpy4(_pAssignedIP, (PCHAR)(pBuf + DHCP_YIADDR));
				memcpy4(_pDhcpServerID, pServerID);
				memcpy4(_pDhcpServerIP, UdpGetDstIP(pUcb));
				_DhcpRequest();
			}
		}
		break;

	case DHCP_STATE_REQUESTING:
		if (iMsgType == DHCP_MTYPE_ACK)
		{
			memcpy4(_pAssignedIP, (PCHAR)(pBuf + DHCP_YIADDR));
			memcpy4(Sys_pSubnetMask, pSubnetMask);
			memcpy4(Sys_pRouterIp, pRouteIP);
			if (OptionsGetByte(OPT_DNS_TYPE))
			{
				memcpy4(Sys_pDnsIp, pDnsIP);
				if (bDns2)
				{
					memcpy4(Sys_pDnsIp2, pDnsIP2);
				}
				else
				{
					memset(Sys_pDnsIp2, 0, IP_ALEN);
				}
			}
			_lLeaseTime = lLeaseTime;
			_lT1 = lT1;
			_lT2 = lT2;

			// for now, escape IP checking first
			_DhcpDone();
		}
		else if (iMsgType == DHCP_MTYPE_NACK)
		{
			_DhcpNack();
		}
		break;

	case DHCP_STATE_RENEWING:
	case DHCP_STATE_REBINDING:
		if (iMsgType == DHCP_MTYPE_ACK)
		{
			_lLeaseTime = lLeaseTime;
			_lT1 = lT1;
			_lT2 = lT2;
			_DhcpAck();
		}
		else if (iMsgType == DHCP_MTYPE_NACK)
		{
			_DhcpNack();
		}
		break;
	}
}

void DhcpTimer()
{
	if (_iDhcpState == DHCP_STATE_CHECKING || _iDhcpState == DHCP_STATE_INIT)
	{
		return;
	}
	if (_iDhcpState < DHCP_STATE_BOUND)
	{
		_iDhcpTimer ++;
		if (_iDhcpTimer >= DHCP_RETRY_TIMEOUT)
		{
			_iDhcpTimer = 0;
			if (_iDhcpRetry < DHCP_MAX_RETRY)
			{
				_iDhcpRetry ++;
				if (_iDhcpState == DHCP_STATE_REQUESTING)
				{
					_DhcpRequest();
				}
				else
				{
					DhcpDiscover();
				}
			}
			else
			{
				_DhcpNack();
			}
		}
	}
	else
	{
		_lCurrent ++;

		if (_iDhcpState == DHCP_STATE_BOUND)
		{
			if (_lCurrent >= _lT1)
			{
				UdpDebugString("DHCP renewing");
				Dhcp_bRenewing = TRUE;
				_iDhcpState = DHCP_STATE_RENEWING;
				DhcpOpenUdp();
				_DhcpReqRenew();
				_iDhcpRetry = 0;
			}
		}
		else if (_iDhcpState == DHCP_STATE_RENEWING)
		{
			if (_lCurrent >= _lT2)
			{
				UdpDebugString("DHCP rebinding");
				_iDhcpState = DHCP_STATE_REBINDING;
				_iDhcpRetry = 0;
				DhcpOpenUdp();
				_DhcpReqRenew();
				Dhcp_bRenewing = TRUE;
			}
			else
			{
				if (_iDhcpRetry < DHCP_MAX_RETRY)
				{
					_DhcpReqRenew();
					_iDhcpRetry ++;
				}
			}
		}
		else if (_iDhcpState == DHCP_STATE_REBINDING)
		{
			if (_lCurrent >= _lLeaseTime)
			{
				DhcpInit();
				DhcpDiscover();
			}
			else
			{
				if (_iDhcpRetry < DHCP_MAX_RETRY)
				{
					_DhcpReqRenew();
					_iDhcpRetry ++;
				}
			}
		}
	}
}

#ifdef OEM_AUTOTEST
const UCHAR _cClientIP[IP_ALEN] = {192, 168, 1, 200};
const UCHAR _cPacketReplyHead[4] = {DHCP_OP_REPLY, 1, HW_ALEN, 0};


UCHAR _FillLeaseTime(PCHAR pBuf, ULONG lLeaseTime)
{
	pBuf[0] = DHCP_OPCODE_LEASE_TIME;
	pBuf[1] = 4;
	ULONG2PCHAR(lLeaseTime, (PCHAR)(pBuf + 2));
	return 6;
}

void DhcpdInit()
{
	Dhcpd_pSocket = UdpListen(DHCP_SERVER_PORT, NET_BANK_OFFSET, (USHORT)DhcpdRun);
}

void DhcpdRun(UDP_SOCKET pUcb)
{
	PCHAR pSendBuf;
	PCHAR pBuf;
	PCHAR pOptions;
	UCHAR iMsgType, iType, iLen;
	USHORT sIndex;

	if (pUcb != Dhcpd_pSocket)
	{
		return;
	}

	pBuf = Adapter_pUdpBuf;

	if (pBuf[DHCP_OP] != DHCP_OP_REQUEST)
	{
		return;
	}
	sIndex = DHCP_OPLIST;
	iMsgType = DHCP_MTYPE_NONE;
	while (sIndex < pUcb->sLen)
	{
		iType = pBuf[sIndex];
		if (iType == DHCP_OPCODE_END)
		{
			break;
		}
		if (iType == DHCP_OPCODE_PADDING)
		{
			sIndex ++;
			continue;
		}
		else
		{
			iLen = pBuf[sIndex+1];
		}
		sIndex += 2;
		pOptions = (PCHAR)(pBuf + sIndex);
		if (iType == DHCP_OPCODE_MESSAGE_TYPE)
		{
			iMsgType = pOptions[0];
			break;
		}
		sIndex += iLen;
	}
	
	if (iMsgType == DHCP_MTYPE_NONE)
	{
		return;
	}

	pSendBuf = pBuf;
	pOptions = (PCHAR)(pSendBuf + DHCP_OPLIST);
	iLen = _FillMsgType(pOptions, (iMsgType == DHCP_MTYPE_DISCOVER) ? DHCP_MTYPE_OFFER : DHCP_MTYPE_ACK);
	iLen += _FillLeaseTime((PCHAR)(pOptions + iLen), 86400);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen), Sys_pSubnetMask, DHCP_OPCODE_SUBNET_MASK);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen), Sys_pDnsIp, DHCP_OPCODE_DNS_IP);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen), Sys_pRouterIp, DHCP_OPCODE_ROUTER_IP);
	iLen += _FillIPAddr((PCHAR)(pOptions + iLen), Sys_pIpAddress, DHCP_OPCODE_SERVER_ID);
	pOptions[iLen] = DHCP_OPCODE_END;
	iLen ++;

	memcpy4(pSendBuf, _cPacketReplyHead);
//	memcpy4((PCHAR)(pSendBuf+DHCP_XID), (PCHAR)(pBuf+DHCP_XID));
	memset((PCHAR)(pSendBuf+DHCP_SECS), 0, DHCP_SECS_LEN);
	pSendBuf[DHCP_FLAGS] = DHCP_FLAG_BROADCAST;
	pSendBuf[DHCP_FLAGS+1] = 0;
	memset((PCHAR)(pSendBuf+DHCP_CIADDR), 0, IP_ALEN);
	memcpy4((PCHAR)(pSendBuf+DHCP_YIADDR), _cClientIP);
	memset((PCHAR)(pSendBuf+DHCP_SIADDR), 0, IP_ALEN);
	memset((PCHAR)(pSendBuf+DHCP_GIADDR), 0, IP_ALEN);
//	memcpy((PCHAR)(pSendBuf+DHCP_CHADDR), (PCHAR)(pBuf+DHCP_CHADDR), HW_ALEN);
	memset((PCHAR)(pSendBuf+DHCP_SNAME), 0, DHCP_OPTIONS - DHCP_SNAME);
	memcpy4((PCHAR)(pSendBuf+DHCP_OPTIONS), _cMagicCookie);

	sIndex = iLen;
	sIndex += DHCP_OPLIST;
	if (sIndex < DHCP_MIN_PACKET_SIZE)
	{
		memset((PCHAR)(pSendBuf + sIndex), 0, (UCHAR)(DHCP_MIN_PACKET_SIZE - sIndex));
		sIndex = DHCP_MIN_PACKET_SIZE;
	}
	UdpSendTo(Dhcpd_pSocket, sIndex, _pBroadcastIP, DHCP_CLIENT_PORT);
}
#endif


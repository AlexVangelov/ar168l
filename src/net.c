/*-------------------------------------------------------------------------
   AR1688 Network function copy right information

   Copyright (c) 2006-2012. Lin, Rongrong <woody@palmmicro.com>
                            Tang, Li      <tangli@palmmicro.com>
                            
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
#ifdef RTL8019AS
#include "ne2000.h"
#elif defined KSZ8842
#include "ksz8842.h"
#endif
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "bank2.h"

#define NET_DISCOVER_TIME			120		// the maximum time delay before restarting initliazation procedure

USHORT _sLayer2Qos;
BOOLEAN _bVLan;

#ifdef RTL8019AS
USHORT _sCurPacketAddr;
USHORT _sPrevPacketAddr;
BOOLEAN _bMiniRunned;
#endif

#if defined KSZ8842
BOOLEAN _bPort1Linked;
BOOLEAN _bPort2Linked;
BOOLEAN _bPort1Data;
#endif

#ifdef KSZ8842
extern UCHAR _iRegBank;
#define NET_HEADER_SIZE		KSZ8842_HEADER_SIZE
#endif

BOOLEAN Net_bConnected;
BOOLEAN Dhcp_bRenewing;

#ifdef OEM_BROADCAST
BOOLEAN Net_bBroadcast;
#endif

UCHAR Adapter_pPacketBuf[ETHERNET_MAX_SIZE + 1 + IP_DATA_MAX_SIZE + 1];

PACKET_LIST * _pFragList;

UCHAR _iDiscoverTimer;

void NetInit()
{
#ifdef CALL_NONE
	_bVLan = FALSE;
#else
	USHORT sVLanID;

	sVLanID = OptionsGetShort(OPT_VLAN_TAG) & VLAN_ID_MASK;
	if (sVLanID)
	{
		_sLayer2Qos = OptionsGetByte(OPT_QOS_PRIORITY) & 0x07;
		_sLayer2Qos <<= 13;
		_sLayer2Qos += sVLanID; 
		_bVLan = TRUE;
	}
	else
	{
		_bVLan = FALSE;
		_sLayer2Qos = 0;
	}
#endif

#ifdef RTL8019AS
	_bMiniRunned = FALSE;
	_sPrevPacketAddr = _sCurPacketAddr = NE2000_RAM_BASE + XMIT_BUF_LEN;
#endif

#if defined KSZ8842
	_bPort1Linked = _bPort2Linked = FALSE;
#endif

	Net_bConnected = Sys_iIpType ? FALSE : TRUE;	// set TRUE when using static ip
	Dhcp_bRenewing = FALSE;
	_iDiscoverTimer = 0;

	_pFragList = NULL;

	// MAC layer
	memset(Adapter_pPacketBuf, 0xff, HW_ALEN);
	memcpy((PCHAR)(Adapter_pPacketBuf + EP_SRC), Sys_pMacAddress, HW_ALEN);
	USHORT2PCHAR((Sys_iIpType == IP_TYPE_PPPOE) ? EPT_PPPOE : EPT_IP, (PCHAR)(Adapter_pPacketBuf + EP_TYPE));

	if (Sys_iIpType != IP_TYPE_STATIC)
	{
		memset(Sys_pIpAddress, 0, IP_ALEN);
	}

#ifdef RTL8019AS
	Ne2000Init();
#elif defined KSZ8842
	Ksz8842Init();
#endif

	IpInit();
	ArpInit();
	UdpInit();
	SyslogOpen();
}

#ifndef CALL_NONE

void NetDiscover()
{
	if (Sys_iIpType == IP_TYPE_DHCP)
	{
		DhcpInit();
		DhcpDiscover();
#ifdef OEM_IP20
		TaskUIHandler(UI_STBY_DISPLAY_IDS2, IDS_DHCP_START);
#else
		TaskUIHandler(UI_STBY_DISPLAY_IDS1, IDS_DHCP_START);
#endif
	}
	else if (Sys_iIpType == IP_TYPE_PPPOE)
	{
		PppoeInit();
		PppoeDiscover();
#ifdef OEM_IP20
		TaskUIHandler(UI_STBY_DISPLAY_IDS2, IDS_PPPOE_START);
#else
		TaskUIHandler(UI_STBY_DISPLAY_IDS1, IDS_PPPOE_START);
#endif
	}
}

#if defined KSZ8842
void NetLinkTimer()
{
	BOOLEAN bLinked;

	if (!Net_bConnected)	return;

	// check status in P1SR[5]/link good
	bLinked = (ksz8842_read_reg_byte(REG_P1SR_BANK, REG_P1SR_OFFSET) & 0x20) ? TRUE : FALSE;
	if (bLinked != _bPort1Linked)
	{
		_bPort1Linked = bLinked;
		TaskUIHandler(UI_CALL_DISPLAY_IDS0, bLinked ? IDS_PORT1_LINKED : IDS_PORT1_UNPLUGGED);
	}

	// check status in P2SR[5]/link good
	bLinked = (ksz8842_read_reg_byte(REG_P2SR_BANK, REG_P2SR_OFFSET) & 0x20) ? TRUE : FALSE;
	if (bLinked != _bPort2Linked)
	{
		_bPort2Linked = bLinked;
		TaskUIHandler(UI_CALL_DISPLAY_IDS1, bLinked ? IDS_PORT2_LINKED : IDS_PORT2_UNPLUGGED);
	}
/*
	if (_bPort1Data)
	{
		UdpDebugString("data from port1");
	}
	else
	{
		UdpDebugString("data from port2");
	}
*/
}
#endif

#endif	// !CALL_NONE

void NetTimer()
{
#ifndef CALL_NONE

#if defined KSZ8842
	NetLinkTimer();
#endif

	if (Sys_iIpType)
	{
		if (!Net_bConnected)
		{
			_iDiscoverTimer ++;
			if (_iDiscoverTimer == NET_DISCOVER_TIME)
			{
				_iDiscoverTimer = 0;
				NetDiscover();
				return;
			}
		}
		
		if (Sys_iIpType == IP_TYPE_DHCP)
		{
			DhcpTimer();
		}
		else if (Sys_iIpType == IP_TYPE_PPPOE)
		{
			PppoeTimer();
		}
	}
#endif

	if (Net_bConnected)
	{
#ifndef CALL_NONE
		DnsTimer();
		SntpTimer();
#ifdef CALL_SIP
		StunTimer();
#endif
#endif
		TftpTimer();
	}

	ArpTimer();
}

#if defined KSZ8842
void _SaveFragData(PACKET_LIST * p)
{
	PCHAR pData;

	if (p->sLen)
	{
		pData = (PCHAR)malloc(p->sLen + 1);		// KSZ8842 always reads even bytes
		if (pData)
		{
			NetRead(pData, p->sLen);
			p->sAddr = (USHORT)pData;
		}
	}
}
#endif

void _FragAddTail(PACKET_LIST * p)
{
	PACKET_LIST * pCur;
	PACKET_LIST * pPrev;
	PACKET_LIST * pNext;

	pCur = (PACKET_LIST *)malloc(sizeof(PACKET_LIST));
	if (!pCur)	return;

	memcpy((PCHAR)pCur, (PCHAR)p, sizeof(PACKET_LIST));
	// add to tail, copy pRecvIP data point
	pPrev = _pFragList;
	do
	{
//		pPrev->pRecvIP = pCur->pRecvIP;
//		memcpy4(pPrev->pRecvIP, pCur->pRecvIP);
		pNext = pPrev->next;
		if (pNext)
		{
			pPrev = pNext;
		}
		else
		{
			pPrev->next = pCur;
			break;
		}
	} while (1);
}

void FreePacketList()
{
	PACKET_LIST * pCur;
	PACKET_LIST * pNext;

	pCur = _pFragList;
	while (pCur)
	{
#if defined KSZ8842
		if (pCur->sAddr)
		{
			free((void *)pCur->sAddr);
		}
#endif
		pNext = pCur->next;
		free(pCur);
		pCur = pNext;
	}
	_pFragList = NULL;
}

PACKET_LIST * _CheckIpHead(PACKET_LIST * p, PCHAR pHead)
{
	UCHAR iHeadLen;
	USHORT sFragOffset;
	PACKET_LIST * pCur;
/*	UCHAR iLen;
	USHORT sCheckSum;
	ULONG lSum;
	PCHAR pExtra;
*/

	p->sLen = PCHAR2USHORT((PCHAR)(pHead + IP_LEN));
/*	if (p->sLen < IP_DATA || p->sLen > (ETHERNET_MAX_SIZE - EP_DATA))
	{	// error length
		return NULL;
	}
*/
	iHeadLen = (pHead[IP_VERLEN] & 0x0f) << 2;

/*
	// check IP checksum
	sCheckSum = PCHAR2USHORT_L((PCHAR)(pHead + IP_CKSUM));
	USHORT2PCHAR(0, (PCHAR)(pHead + IP_CKSUM));
	lSum = short_sum(pHead, IP_DATA);
	if (iHeadLen > IP_DATA)
    {
		iLen = iHeadLen - IP_DATA;
		pExtra = (PCHAR)malloc(iLen);
		if (pExtra)
		{
			if (Ne2000Read(p->sAddr + IP_DATA, pExtra, iLen))
			{
				lSum += short_sum(pExtra, iLen);
			}
			free(pExtra);
		}
    }
	if (sCheckSum != ip_checksum(lSum))
	{
		UdpDebugString("IP checksum error");
		return NULL;
	}
*/	
	p->sLen -= iHeadLen;
#ifdef RTL8019AS
	p->sAddr += iHeadLen;
#elif defined KSZ8842
	p->sAddr = 0;	// NULL
	if (iHeadLen > IP_DATA)
	{
		iHeadLen -= IP_DATA;
		NetReadBypass(iHeadLen);
	}
#endif
//	p->pRecvIP = (PCHAR)(pHead + IP_SRC);
	memcpy4(p->pRecvIP, (PCHAR)(pHead + IP_SRC));
	p->next = NULL;

	sFragOffset = PCHAR2USHORT((PCHAR)(pHead + IP_FRAGOFF)) & 0x3fff;
	if (sFragOffset)
	{
		if (sFragOffset & IP_MF)
		{
			if (sFragOffset & IP_FRAGOFF_MASK)
			{
//				UdpDebugString("more fragment");
				if (_pFragList)
				{
#if defined KSZ8842
					_SaveFragData(p);
#endif
					_FragAddTail(p);
				}
				// first fragment lost, discard this too
			}
			else
			{
//				UdpDebugString("first fragment");
				// free previous fragments, start a new one. 
				// we only handle 1 fragment IP packet
				FreePacketList();
				pCur = (PACKET_LIST *)malloc(sizeof(PACKET_LIST));
				_pFragList = pCur;	// make me head
				if (pCur)
				{
#if defined KSZ8842
					_SaveFragData(p);
#endif
					memcpy((PCHAR)pCur, (PCHAR)p, sizeof(PACKET_LIST));
				}
			}
			return NULL;	// handle it later
		}
		else
		{
//			UdpDebugString("last fragment");
			if (_pFragList)
			{
				_FragAddTail(p);
				p = _pFragList;
//				_pFragList = NULL;
			}
			else
			{
				return NULL;	// first fragment lost, discard it
			}
		}
	}
	return p;
}

#ifdef RTL8019AS

BOOLEAN _IPReceive(PACKET_LIST * p, BOOLEAN bMini)
{
	UCHAR iVal;
	PACKET_LIST * pCur;
	UCHAR pHead[IP_DATA];
	BOOLEAN bRet;

	if (!Ne2000Read(p->sAddr, pHead, IP_DATA))	return FALSE;

	pCur = _CheckIpHead(p, pHead);

	if (!pCur)	return FALSE;

	bRet = FALSE;
	iVal = pHead[IP_PROTO];
    if (iVal == IPT_UDP)
    {
#ifndef CALL_NONE
		if (bMini)
		{
			bRet = UdpMiniRun(pCur);
		}
		else
#endif
		{
			UdpRun(pCur);
		}
    }
	else if (!bMini)
	{
		if (Net_bConnected)
		{
			if (iVal == IPT_TCP)
			{
#ifndef CALL_NONE
				if (Ne2000Read(pCur->sAddr, Adapter_pReceivePacket, pCur->sLen))
				{
					TaskTcpRun(pCur);
				}
#endif
			}
			else if (iVal == IPT_ICMP)
			{
				IcmpRun(pCur);
			}
		}
    }

	if (_pFragList == pCur)
	{
		FreePacketList();
	}

	return bRet;
}

void _HandleARP(USHORT sAddr, BOOLEAN bMini)
{
	UCHAR pBuf[ARP_LENGTH];

	if (!bMini)
	{
		if (Ne2000Read(sAddr, pBuf, ARP_LENGTH))
		{
			ArpRun(pBuf);
		}
	}
}

#ifndef CALL_NONE
void _HandlePppoeInit(USHORT sAddr, PCHAR pSrcMac, BOOLEAN bMini)
{
	UCHAR pHead[PPPOE_INIT_INFO];
	USHORT sLen;

	if (Sys_iIpType == IP_TYPE_PPPOE && !bMini)
	{
		if (Ne2000Read(sAddr, pHead, PPPOE_INIT_INFO))
		{
			sLen = PCHAR2USHORT((PCHAR)(pHead + PPPOE_LENGTH));
			if (sLen)
			{
				if (!Ne2000Read(sAddr + PPPOE_INIT_INFO, Adapter_pReceivePacket, sLen))		return;
			}
			PppoeInitRun(pHead, pSrcMac, sLen);
		}
	}
}

void _HandlePppoeSession(USHORT sAddr, PCHAR pPppoeHead)
{
	USHORT sLen;

	sLen = PCHAR2USHORT((PCHAR)(pPppoeHead + PPPOE_LENGTH));
	if (sLen > 2)
	{
		if (Ne2000Read(sAddr, Adapter_pReceivePacket, sLen - 2))
		{
			PppoeSessionRun(pPppoeHead);
		}
	}
}
#endif

UCHAR _UpdateNextPacket(UCHAR iCurPacket)
{
	USHORT s;
	UCHAR iNext;

	s = iCurPacket;
	s <<= 8;
	if (!Ne2000Read(s + HEADER_NEXT_PACKET, &iNext, 1))
	{
		iNext = 0;
	}
	_bMiniRunned = FALSE;

	return iNext;
}

UCHAR ReadPacket(UCHAR iCurPacket, BOOLEAN bMini)
{
	USHORT sType;
	PACKET_LIST packet;
	UCHAR pHead[NE2000_HEADER_SIZE + EP_DATA];	// MAC head, also used as VLAN head and PPPoE head
	UCHAR iStatus, iNextPacket;

	_sPrevPacketAddr = _sCurPacketAddr;
	_sCurPacketAddr = iCurPacket;
	_sCurPacketAddr <<= 8;
	packet.sAddr = _sCurPacketAddr;
	if (!Ne2000Read(packet.sAddr, pHead, NE2000_HEADER_SIZE + EP_DATA))		return 0;

	iStatus = pHead[HEADER_STATUS];
	iNextPacket = pHead[HEADER_NEXT_PACKET];
	if (iStatus & 0x5e || (iNextPacket < NE2000_NIC_PAGE_START || iNextPacket >= NE2000_NIC_PAGE_STOP))
	{	// Header error
		return 0;
	}
/*
	s = PCHAR2USHORT_L((PCHAR)(pHead + HEADER_PACKET_LEN_LOW));
	if (s < (ETHERNET_MIN_SIZE + NE2000_HEADER_SIZE) || s > (ETHERNET_MAX_SIZE + NE2000_HEADER_SIZE))
	{	// Header len error
		return 0;
	}
*/
	packet.sAddr += NE2000_HEADER_SIZE + EP_DATA;
	sType = PCHAR2USHORT((PCHAR)(pHead + NE2000_HEADER_SIZE + EP_TYPE));
#ifndef CALL_NONE	// never enable VLAN in page0
	if (_bVLan)
	{
		if (sType == EPT_VLAN)	    // Check if it is VLAN packet
		{
			if (!Ne2000Read(packet.sAddr, pHead, VLAN_HEADER_SIZE))		return 0;	// the first 4 bytes of pHead can be used now as VLAN head
			packet.sAddr += VLAN_HEADER_SIZE;
//			LcdDebugVal(PCHAR2USHORT(pHead), 10, 3);
			if ((_sLayer2Qos & VLAN_ID_MASK) != (PCHAR2USHORT(pHead) & VLAN_ID_MASK))	goto SkippedPacket;
			sType = PCHAR2USHORT((PCHAR)(pHead + VLAN_TCI));
		}
		else	goto SkippedPacket;
	}
#endif

	// Check if it is tcp/ip packet
	if (sType == EPT_IP)
	{
#ifdef OEM_BROADCAST
		Net_bBroadcast = FALSE;
#endif

#ifndef	OEM_AUTOTEST
		if (iStatus & 0x20)
		{	// this is a broadcast packet, check if we need it
#ifdef OEM_BROADCAST
			Net_bBroadcast = TRUE;
#else
			if (Net_bConnected && !Dhcp_bRenewing)	
			{	
				goto SkippedPacket;
			}
#endif
		}
#endif
		if (!_IPReceive(&packet, bMini))
		{	
			if (!bMini && _bMiniRunned)
			{
				iNextPacket = _UpdateNextPacket(iCurPacket);
			}
			goto End;
		}
	}
	else if (sType == EPT_ARP)
	{
		_HandleARP(packet.sAddr, bMini);
		goto End;
	}
#ifndef CALL_NONE
	else if (sType == EPT_PPPOE_INIT)
	{
		_HandlePppoeInit(packet.sAddr, (PCHAR)(pHead + NE2000_HEADER_SIZE + EP_SRC), bMini);
		goto End;
	}
	else if (sType == EPT_PPPOE)
	{
		if (Sys_iIpType == IP_TYPE_PPPOE)
		{
			if (Ne2000Read(packet.sAddr, pHead, PPPOE_INFO))	// the first 8 bytes of pHead can be used now as PPPoE head
			{
				packet.sAddr += PPPOE_INFO;
				if (PppoeCheckHead(pHead, (PCHAR)(pHead + NE2000_HEADER_SIZE + EP_SRC)))
				{
					if (PCHAR2USHORT((PCHAR)(pHead + PPPOE_PROTOCOL_ID)) == PPP_PROT_IP)
					{
						if (!Net_bConnected)	goto End;
						if (!_IPReceive(&packet, bMini))
						{	
							if (!bMini && _bMiniRunned)
							{
								iNextPacket = _UpdateNextPacket(iCurPacket);
							}
						}
						else
						{
							goto SkippedPacket;
						}
					}
					else
					{
						_HandlePppoeSession(packet.sAddr, pHead);
					}
				}
			}
		}
		goto End;
	}
#endif

    // We do not welcome other type of packets
SkippedPacket:
	if (bMini)
	{	// mini run handled this packet, mark it
		if (Ne2000Write(_sPrevPacketAddr + HEADER_NEXT_PACKET, &iNextPacket, 1))
		{
			_bMiniRunned = TRUE;
			_sCurPacketAddr = _sPrevPacketAddr;
		}
		else
		{
			iNextPacket = 0;
		}
	}
End:
	return iNextPacket;
}

#elif defined KSZ8842

void _HandleARP()
{
	UCHAR pBuf[ARP_LENGTH];

	NetRead(pBuf, ARP_LENGTH);
	ArpRun(pBuf);
}

#ifndef CALL_NONE

void _HandlePppoeInit(PCHAR pSrcMac)
{
	UCHAR pHead[PPPOE_INIT_INFO];
	USHORT sLen;

	if (Sys_iIpType == IP_TYPE_PPPOE)
	{
		NetRead(pHead, PPPOE_INIT_INFO);
		sLen = PCHAR2USHORT((PCHAR)(pHead + PPPOE_LENGTH));
		if (sLen)
		{
			NetRead(Adapter_pReceivePacket, sLen);
		}
		PppoeInitRun(pHead, pSrcMac, sLen);
	}
}

void _HandlePppoeSession(PCHAR pPppoeHead)
{
	USHORT sLen;

	sLen = PCHAR2USHORT((PCHAR)(pPppoeHead + PPPOE_LENGTH));
	if (sLen > 2)
	{
		NetRead(Adapter_pReceivePacket, sLen - 2);
		PppoeSessionRun(pPppoeHead);
	}
}

#endif

void _IPReceive()
{
	UCHAR iVal;
	PACKET_LIST packet;
	PACKET_LIST * pCur;
	UCHAR pIpHead[IP_DATA];

	NetRead(pIpHead, IP_DATA);

	pCur = _CheckIpHead(&packet, pIpHead);
	if (!pCur)
	{	
		return;
	}

	iVal = pIpHead[IP_PROTO];
	if (iVal == IPT_UDP)
	{
		UdpRun(pCur);
	}
	else if (Net_bConnected)
	{
		if (iVal == IPT_TCP)
		{
#ifndef CALL_NONE
			NetRead(Adapter_pReceivePacket, pCur->sLen);
			TaskTcpRun(pCur);
#endif
		}
		else if (iVal == IPT_ICMP)
		{
			IcmpRun(pCur);
		}
	}

	if (_pFragList == pCur)
	{
		FreePacketList();
	}
}

void HandlePacket()
{
	USHORT sType;
	UCHAR pHead[NET_HEADER_SIZE + EP_DATA];
#ifdef KSZ8842
	UCHAR iStatus;
#endif

	NetRead(pHead, NET_HEADER_SIZE + EP_DATA);

#ifdef KSZ8842
	iStatus = pHead[0];
	if (iStatus & 0x07)		return;		// receive error
	if (!(pHead[1] & 0x80))	return;		// bad frame
	_bPort1Data = (pHead[1] & 0x01) ? TRUE : FALSE;
#else
//	_bPort1Data = (dm9k_read_reg(NIC_RSR) & 0x0c) ? TRUE : FALSE;
	_bPort1Data = (pHead[1] & 0x0c) ? TRUE : FALSE;
//	_sLenRemain = PCHAR2USHORT_L((PCHAR)(pHead + 2));
#endif

	sType = PCHAR2USHORT((PCHAR)(pHead + NET_HEADER_SIZE + EP_TYPE));

#ifndef CALL_NONE	// never enable VLAN in page0
	if (_bVLan)
	{
		if (sType == EPT_VLAN)	    // Check if it is VLAN packet
		{
			NetRead(pHead, VLAN_HEADER_SIZE);	// use first 4 bytes of pHead as VLAN head
			if ((_sLayer2Qos & VLAN_ID_MASK) != (PCHAR2USHORT(pHead) & VLAN_ID_MASK))	return;
			sType = PCHAR2USHORT((PCHAR)(pHead + VLAN_TCI));
		}
#ifndef OEM_VLAN
		else	return;
#endif
	}
#endif

	if (sType == EPT_IP)
	{
#ifdef OEM_BROADCAST
		Net_bBroadcast = FALSE;
#endif

#ifndef	OEM_AUTOTEST
#ifdef KSZ8842
		if (iStatus & 0xc0)
#else
		if (memcmp((PCHAR)(pHead + NET_HEADER_SIZE), Sys_pMacAddress, HW_ALEN))
#endif
		{	// this is a broadcast packet, check if we need it
#ifdef OEM_BROADCAST
			Net_bBroadcast = TRUE;
#else
			if (Net_bConnected && !Dhcp_bRenewing)	
			{	
				return;
			}
#endif
		}
#endif
		_IPReceive();
	}
	else if (sType == EPT_ARP)
	{
		_HandleARP();
	}
#ifndef CALL_NONE
	else if (sType == EPT_PPPOE_INIT)
	{
		_HandlePppoeInit((PCHAR)(pHead + NET_HEADER_SIZE + EP_SRC));
	}
	else if (sType == EPT_PPPOE)
	{
		if (Sys_iIpType == IP_TYPE_PPPOE)
		{
			NetRead(pHead, PPPOE_INFO);		// use first 8 bytes of pHead as PPPoE head
			if (PppoeCheckHead(pHead, (PCHAR)(pHead + NET_HEADER_SIZE + EP_SRC)))
			{
				if (PCHAR2USHORT((PCHAR)(pHead + PPPOE_PROTOCOL_ID)) == PPP_PROT_IP)
				{
					if (Net_bConnected)
					{
						_IPReceive();
					}
				}
				else
				{
					_HandlePppoeSession(pHead);
				}
			}
		}
	}
#endif
}

#endif

#if defined KSZ8842

void Ksz8842Init()
{
	UCHAR i, j;
//	USHORT sVal;
	UCHAR pMac[HW_ALEN];
	PSHORT p;

	_iRegBank = 0xff;

	// read base address
//	if (_reg_read_word(REG_BASE_ADDR_BANK, REG_BASE_ADDR_OFFSET) != 0x0300)	return;									

	// read device ID
//	if ((_reg_read_word(REG_SIDER_BANK, REG_CHIP_ID_OFFSET) & 0xfff0) != 0x8800)	return;		

	// global reset, GRR[0]
/*	ksz8842_or_reg_byte(REG_GLOBAL_CTRL_BANK, REG_GLOBAL_CTRL_OFFSET, 0x01);
	Delay(100);
	ksz8842_and_reg_byte(REG_GLOBAL_CTRL_BANK, REG_GLOBAL_CTRL_OFFSET, ~0x01);
*/
	// write QMU MAC, MARL/M/H
	for (i = 0; i < HW_ALEN; i += 2)
	{
		pMac[i] = Sys_pMacAddress[i + 1];
		pMac[i + 1] = Sys_pMacAddress[i];
	}
	p = (PSHORT)pMac;
	j = 2;
	for (i = REG_ADDR_0_OFFSET; i <= REG_ADDR_4_OFFSET; i += 2)
	{
		ksz8842_write_reg_word(REG_ADDR_0_BANK, i, p[j]);
		j --;
	}
/*
	// write switch MAC, MACAR1/2/3
	pMac[HW_ALEN - 1] ++;
	j = REG_MACAR1_OFFSET;
	for (i = 0; i < 3; i ++)
	{
		ksz8842_write_reg_word(REG_MACAR1_BANK, j, p[i]);
		j += 2;
	}
*/

#ifndef CALL_NONE
	if (_bVLan)
	{
		// VID control, P1VIDCR/P2VIDCR/P3VIDCR
		ksz8842_write_reg_word(REG_P1VIDCR_BANK, REG_P1VIDCR_OFFSET, _sLayer2Qos);
		ksz8842_write_reg_word(REG_P2VIDCR_BANK, REG_P2VIDCR_OFFSET, _sLayer2Qos);
		ksz8842_write_reg_word(REG_P3VIDCR_BANK, REG_P3VIDCR_OFFSET, _sLayer2Qos);

		// write VLAN table
		ksz8842_write_reg_word(REG_IADR4_BANK, REG_IADR4_OFFSET, _sLayer2Qos);
		ksz8842_write_reg_word(REG_IADR5_BANK, REG_IADR5_OFFSET, 0x0f);
		ksz8842_write_reg_word(REG_IACR_BANK, REG_IACR_OFFSET, 0x0400);

		ksz8842_write_reg_word(REG_IADR5_BANK, REG_IADR5_OFFSET, 0);
		for (i = 1; i < 16; i ++)
		{
			ksz8842_write_reg_word(REG_IACR_BANK, REG_IACR_OFFSET, 0x0400 + i);
		}

		// 802.1Q VLAN enable, SGCR2[15]. VLAN table must be set up before the operation
		ksz8842_or_reg_word(REG_SGCR2_BANK, REG_SGCR2_OFFSET, 0x8000);

		// 802.1p priority classification enable, P3CR1[5]
		// tag insertion, P3CR1[2]
		// tag removal, P3CR1[1]
		// normal
		// unicast port-VLAN mismatch discard, SGCR2[7]
		ksz8842_or_reg_byte(REG_SGCR2_BANK, REG_SGCR2_OFFSET, 0x80);
		ksz8842_or_reg_byte(REG_P1CR1_BANK, REG_P1CR1_OFFSET, 0x24);
		ksz8842_and_reg_byte(REG_P1CR1_BANK, REG_P1CR1_OFFSET, ~0x02);
		ksz8842_or_reg_byte(REG_P2CR1_BANK, REG_P2CR1_OFFSET, 0x24);
		ksz8842_or_reg_byte(REG_P3CR1_BANK, REG_P3CR1_OFFSET, 0x24);

#ifdef OEM_VLAN
		// unicast port-VLAN mismatch discard, SGCR2[7]
		ksz8842_and_reg_byte(REG_SGCR2_BANK, REG_SGCR2_OFFSET, ~0x80);
		// port1 as none-VLAN uplink
		ksz8842_and_reg_byte(REG_P1CR1_BANK, REG_P1CR1_OFFSET, ~0x04);
		ksz8842_or_reg_byte(REG_P1CR1_BANK, REG_P1CR1_OFFSET, 0x02);
#endif
	}
	else
	{
		// 802.1Q VLAN enable, SGCR2[15]. 
		ksz8842_and_reg_word(REG_SGCR2_BANK, REG_SGCR2_OFFSET, ~0x8000);
	}

	// set host port ingress rate control, 2Mbps, P3IRCR
	ksz8842_write_reg_word(REG_P3IRCR_BANK, REG_P3IRCR_OFFSET, 0x6666);

	// set host port egress rate control, 2Mbps, P3ERCR
	ksz8842_write_reg_word(REG_P3ERCR_BANK, REG_P3ERCR_OFFSET, 0x6666);

	// Ingress Limit Mode, P3CR3[3-2]
//	ksz8842_or_reg_byte(REG_P3CR3_BANK, REG_P3CR3_OFFSET, 0x08);	// limit and count broadcast and multicast frames only
#endif

	// enable QMU transmit flow control / transmit padding / transmit CRC, TXCR
	ksz8842_write_reg_word(REG_TX_CTRL_BANK, REG_TX_CTRL_OFFSET, TX_CTRL_FLOW_ENABLE | TX_CTRL_PAD_ENABLE | TX_CTRL_CRC_ENABLE);	

	// enable QMU receive flow control / receive all broadcast frames / receive all multicast frames / receive unicast frames / receive strip the CRC, RXCR
	ksz8842_write_reg_word(REG_RX_CTRL_BANK, REG_RX_CTRL_OFFSET, RX_CTRL_FLOW_ENABLE | RX_CTRL_BROADCAST | RX_CTRL_ALL_MULTICAST | RX_CTRL_UNICAST | RX_CTRL_STRIP_CRC);

	// enable QMU transmit frame data pointer auto increment, TXFDPR
	ksz8842_write_reg_word(REG_TX_ADDR_PTR_BANK, REG_TX_ADDR_PTR_OFFSET, 0x4000);

	// enable QMU receive frame data pointer auto increment, RXFDPR
	ksz8842_write_reg_word(REG_RX_ADDR_PTR_BANK, REG_RX_ADDR_PTR_OFFSET, 0x4000);

	// configure QMU receive high water mark to 2KBytes to avoid loss packets (big packet size) under flow control, QRFCR[12]
	// actually we do not care if there is big packet lost!
//	sVal = _reg_read_word(REG_RX_WATERMARK_BANK, REG_RX_WATERMARK_OFFSET);
//	ksz8842_write_reg_word(REG_RX_WATERMARK_BANK, REG_RX_WATERMARK_OFFSET, sVal | RX_HIGH_WATERMARK_2KB);

	// enable aggressive back off algorithm in half duplex mode, SGCR1[8]
	ksz8842_or_reg_word(REG_SGCR1_BANK, REG_SGCR1_OFFSET, 0x0100);

	// enable no excessive collision drop, SGCR2[3]
	ksz8842_or_reg_byte(REG_SGCR2_BANK, REG_SGCR2_OFFSET, 0x08);

	// enable full-duplex flow control on Switch Host port, SGCR3[5]
	ksz8842_or_reg_byte(REG_SGCR3_BANK, REG_SGCR3_OFFSET, 0x20);

	// enable port 1 force flow control / back pressure / transmit / receive, P1CR2
	ksz8842_write_reg_word(REG_P1CR2_BANK, REG_P1CR2_OFFSET, 0x1e07);

	// enable port 2 force flow control / back pressure / transmit / receive, P2CR2
	ksz8842_write_reg_word(REG_P2CR2_BANK, REG_P2CR2_OFFSET, 0x1e07);

	// restart Port 1 auto-negotiation, P1CR4[13]
	ksz8842_or_reg_word(REG_P1CR4_BANK, REG_P1CR4_OFFSET, 0x2000);

	// restart Port 2 auto-negotiation, P2CR4[13]
	ksz8842_or_reg_word(REG_P2CR4_BANK, REG_P2CR4_OFFSET, 0x2000);

	// clear the interrupts status, ISR
	ksz8842_write_reg_word(REG_INT_STATUS_BANK, REG_INT_STATUS_OFFSET, 0xffff);

	// enable QMU transmit, TXCR[0]
	ksz8842_or_reg_byte(REG_TX_CTRL_BANK, REG_TX_CTRL_OFFSET, 0x01);

	// enable QMU receive, RXCR[0]
	ksz8842_or_reg_byte(REG_RX_CTRL_BANK, REG_RX_CTRL_OFFSET, 0x01);

	// enable Switch engine, SIDER[0]
	ksz8842_or_reg_byte(REG_SIDER_BANK, REG_SIDER_OFFSET, 0x01);
/*
	// check status in P1SR[6]/AN done, P1SR[5]/link good
	if (_reg_read_byte(REG_P1SR_BANK, REG_P1SR_OFFSET) & 0x20)
	{
		while (!(_reg_read_byte(REG_P1SR_BANK, REG_P1SR_OFFSET) & 0x40));
		_reg_and_word(REG_P1CR4_BANK, REG_P1CR4_OFFSET, ~0x0800);
	}
	else
	{	// Power Down, P1CR4[11], can NOT check link while power down!
		ksz8842_or_reg_word(REG_P1CR4_BANK, REG_P1CR4_OFFSET, 0x0800);
	}

	// check status in P2SR[6]/AN done, P2SR[5]/link good
	if (_reg_read_byte(REG_P2SR_BANK, REG_P2SR_OFFSET) & 0x20)
	{
		while (!(_reg_read_byte(REG_P2SR_BANK, REG_P2SR_OFFSET) & 0x40));
	}
*/

	Delay(2000);
}

void Ksz8842ReadBypass(USHORT sLen)
{
	PCHAR p;

	p = malloc(sLen + 1);	// Ksz8842Read always reads even bytes
	if (p)
	{
		Ksz8842Read(p, sLen);
		free(p);
	}
}
#endif


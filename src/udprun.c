/*-------------------------------------------------------------------------
   AR1688 UDP function copy right information

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
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "ar168.h"
#include "bank2.h"

extern struct ucb _pUcbData[UCB_TSIZE];

#ifdef OEM_BROADCAST
extern BOOLEAN Net_bBroadcast;
#endif

void UdpInit()
{
	UCHAR i;
	PCHAR pUcb;

	pUcb = (PCHAR)_pUcbData;
	for (i = 0; i < UCB_TSIZE; i ++)
	{
		memset(pUcb, 0, sizeof(struct ucb));
		pUcb += sizeof(struct ucb);
	}
}

UDP_SOCKET _UdpCheckHead(PCHAR pHead, PCHAR pRecvIP)
{
	UCHAR i;
	USHORT sDstPort, sSrcPort;
	UDP_SOCKET pUcb;

	sDstPort = PCHAR2USHORT((PCHAR)(pHead + UDP_DST));
	sSrcPort = PCHAR2USHORT((PCHAR)(pHead + UDP_SRC));

	pUcb = _pUcbData;
	for (i = 0; i < UCB_TSIZE; i ++, pUcb ++)
	{
		if (pUcb->iState == UDPS_FREE)	continue;

		if (pUcb->sSrcPort == sDstPort)
		{
			if (pUcb->iState == UDPS_OPEN)
			{
				if (pUcb->sDstPort != sSrcPort || memcmp(pUcb->pDstIP, pRecvIP, IP_ALEN))
				{
					break;
				}
			}
			else
			{
				memcpy4(pUcb->pDstIP, pRecvIP);
				pUcb->sDstPort = sSrcPort;
			}
			return pUcb;
		}
	}
	if (Net_bConnected)
	{
		UdpDebugIpPort("Unknown UDP data from ", pRecvIP, sDstPort);
	}
	return NULL;
}

void _UdpHandleData(PACKET_LIST * p, PCHAR pHead, UDP_SOCKET pUcb)
{
	PCHAR pData;
	PCHAR pRecvIP;
	PACKET_LIST * pCur;
	PACKET_LIST * pNext;
#ifdef VERIFY_CHECKSUM
	USHORT sCheckSum;
#endif

	pData = (PCHAR)(pHead + UDP_DATA);
	pRecvIP = p->pRecvIP;

	pUcb->sLen = p->sLen - UDP_DATA;
	if (!pUcb->sLen)
	{
		UdpDebugString("empty udp packet");
//		Sys_sDebug ++;
		return;
	}
	
#ifdef RTL8019AS
	if (!Ne2000Read(p->sAddr + UDP_DATA, pData, pUcb->sLen))	return;
#elif defined KSZ8842
	if (p->next)
	{
		if (p->sAddr)
		{
			memcpy(pData, (PCHAR)(p->sAddr + UDP_DATA), pUcb->sLen);
		}
	}
	else
	{
		NetRead(pData, pUcb->sLen);
	}
#endif
	if (p->next)
	{	// fragment
		pData += pUcb->sLen;
		pCur = p;
		do
		{
			pNext = pCur->next;
			if (pNext)
			{
				if ((pUcb->sLen + pNext->sLen) <= 2 * IP_DATA_MAX_SIZE - UDP_DATA)
				{
#ifdef RTL8019AS
					if (!Ne2000Read(pNext->sAddr, pData, pNext->sLen))	return;
#elif defined KSZ8842
					if (pNext->next)
					{
						if (pNext->sAddr)
						{
							memcpy(pData, (PCHAR)pNext->sAddr, pNext->sLen);
						}
					}
					else
					{
						NetRead(pData, pNext->sLen);
					}
#endif
					pData += pNext->sLen;
					pUcb->sLen += pNext->sLen;
				}
				pCur = pNext;
			}
			else
			{
				break;
			}
		} while (1);
	}

	// check UDP check sum
#ifdef VERIFY_CHECKSUM
	sCheckSum = PCHAR2USHORT_L((PCHAR)(pHead + UDP_CKSUM));
	if (sCheckSum)	// Calculate checksum if it is not 0
	{
//		if (sCheckSum != UdpCheckSum(pUcb->sLen + UDP_DATA, pHead, pRecvIP, (PCHAR)(pRecvIP + IP_ALEN)))
		if (Net_bConnected && !Dhcp_bRenewing)	
		{	// only check when Sys_pIpAddress is valid	
			if (sCheckSum != UdpCheckSum(pUcb->sLen + UDP_DATA, pHead, pRecvIP, Sys_pIpAddress))
			{
				UdpDebugString("Udp Checksum Error");
				UdpDebugVal(sCheckSum, 16);
				return;
			}
		}
	}
#endif

	if (pUcb->sCallBack)
	{
		TaskUdpRunData(pUcb);
	}
}

void UdpRun(PACKET_LIST * p)
{
	PCHAR pHead;
	UDP_SOCKET pUcb;

	pHead = Adapter_pIpBuf;
#ifdef RTL8019AS
	if (!Ne2000Read(p->sAddr, pHead, UDP_DATA))	return;
#elif defined KSZ8842
	if (p->next)
	{
		if (p->sAddr)
		{
			memcpy(pHead, (PCHAR)p->sAddr, UDP_DATA);
		}
	}
	else
	{
		NetRead(pHead, UDP_DATA);
	}
#endif
#ifdef OEM_BROADCAST
	if (Net_bBroadcast)
	{
		if (Net_bConnected && !Dhcp_bRenewing)
		{
			if (PCHAR2USHORT((PCHAR)(pHead + UDP_DST)) != Sys_sRtpPort)	return;
		}
	}
#endif
	pUcb = _UdpCheckHead(pHead, p->pRecvIP);
	if (pUcb)
	{
		_UdpHandleData(p, pHead, pUcb);
	}
}

#ifndef CALL_NONE
#ifdef RTL8019AS

BOOLEAN UdpMiniRun(PACKET_LIST * p)
{
	PCHAR pHead;
	UCHAR pMiniHead[UDP_DATA];
	PCHAR pBackup;
	USHORT sBackupLen;
	UDP_SOCKET pUcb;

	pHead = Adapter_pIpBuf;

	if (!Ne2000Read(p->sAddr, pMiniHead, UDP_DATA))		return FALSE;
	if (PCHAR2USHORT((PCHAR)(pMiniHead + UDP_DST)) == Sys_sRtpPort)
	{
		pUcb = _UdpCheckHead(pMiniHead, p->pRecvIP);
		if (pUcb)
		{
			sBackupLen = p->sLen + 1;	// extra byte for checksum
			pBackup = malloc(sBackupLen);
			if (pBackup)	// fail silently
			{
				memcpy(pBackup, pHead, sBackupLen);
				memcpy(pHead, pMiniHead, UDP_DATA);
				_UdpHandleData(p, pHead, pUcb);
				memcpy(pHead, pBackup, sBackupLen);
				free(pBackup);
//				UdpDebugString("Mini recv");
			}
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#endif

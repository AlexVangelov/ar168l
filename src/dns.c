/*-------------------------------------------------------------------------
   AR1688 DNS function copy right information

   Copyright (c) 2006-2011. Tang, Li <tangli@palmmicro.com>

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
/* RFC 1035 & RFC 2136 & RFC 2782*/

#define DNS_SERVER_PORT		53
#define DNS_SRC_PORT		1027

/*
The header contains the following fields:

                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/
#define DNS_HEADER_ID		0
#define DNS_HEADER_FALG0	2
#define DNS_HEADER_FALG1	3
#define DNS_HEADER_QDCOUNT	4	/* the number of entries in the question section */
#define DNS_HEADER_ANCOUNT	6	/* the number of resource records in the answer section */
#define DNS_HEADER_NSCOUNT	8	/* the number of name server resource records in the authority records section */
#define DNS_HEADER_ARCOUNT	10	/* the number of resource records in the additional records section */
#define DNS_HEADER_DATA		12

/*
Question section format:

                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/
#define DNS_QNAME	0	/* domain name , vairiable length */
#define DNS_QTYPE	0	
#define DNS_QCLASS	2
#define DNS_QDATA	4

/*
Resource record format:
                                   1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                                               /
    /                      NAME                     /
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     CLASS                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TTL                      |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                   RDLENGTH                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
    /                     RDATA                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/
#define DNS_RR_NAME			0	/* a domain name to which this resource record pertains */
#define DNS_RR_TYPE			0
#define DNS_RR_CLASS		2
#define DNS_RR_TTL			4
#define DNS_RR_RDLENGTH		8
#define DNS_RR_RDATA		10


#define DNS_MASK_QR		0x80
#define DNS_MASK_RCODE	0x0f

#define DNS_OPCODE_QUERY	0	/* a standard query (QUERY) */
#define DNS_OPCODE_IQUERY	1	/* an inverse query (IQUERY) */
#define DNS_OPCODE_STATUS	2	/* a server status request (STATUS) */
#define DNS_OPCODE_UPDATE	5	/* DNS UPDATE message */

#define DNS_RCODE_NOERROR			0	/* No error condition */
#define DNS_RCODE_FORMERR			1	/* Format error */	
#define DNS_RCODE_SERVFAIL			2	/* Server failure */
#define DNS_RCODE_NXDOMAIN			3	/* Name Error */
#define DNS_RCODE_NOTIMP			4	/* Not Implemented */
#define DNS_RCODE_REFUSED			5	/* Refused  */
#define DNS_RCODE_YXDOMAIN			6	/* Some name that ought not to exist does exist. */
#define DNS_RCODE_YXRRSET			7	/* Some RRset that ought not to exist does exist. */
#define DNS_RCODE_NXRRSET			8	/* Some RRset that ought to exist does not exist */
#define DNS_RCODE_NOTAUTH			9	/* The server is not authoritative for the zone named in the Zone Section */ 
#define DNS_RCODE_NOTZONE			10	/* A name used in the Prerequisite or Update Section is not within the zone denoted by the Zone Section. */

#define DNS_CLASS_IN	1	/* the internet */
#define DNS_CLASS_CS	2	/* the CSNET class (Obsolete - used only for examples in some obsolete RFCs) */
#define DNS_CLASS_CH	3	/* the CHAOS class */
#define DNS_CLASS_HS	4	/* Hesiod [Dyer 87] */

#define DNS_RETRY_TIMEOUT	5
#define DNS_MAX_RETRY		4
#define DNS_CB_TSIZE		3	

/* DNS entry status */
#define DS_FREE     0   /* Entry is unused (initial value) */
#define DS_PENDING  1   /* Entry is used but incomplete */
#define DS_RESOLVED 2   /* Entry has been resolved */

#define DNS_ID_LEN		2

// DNS control block
struct dns_cb
{
	UCHAR iState;
	UCHAR iType;
	UCHAR pID[DNS_ID_LEN];
	PCHAR pName;
	UCHAR iTimer;
	UCHAR iRetry;
	UCHAR iBankOffset;
	USHORT sCallBack;
	PCHAR pAnswer;		// return a IP address if type A, return a list of name is type SRV
	BOOLEAN bDns1Failed;
	BOOLEAN bDns2Failed;
};

#define DNS_CB_LENGTH		(sizeof(struct dns_cb))

void DnsRun(UDP_SOCKET pUcb);

UDP_SOCKET _pDnsSocket;
struct dns_cb _pDcbData[DNS_CB_TSIZE];

void _DnsDeleteAns(DNS_HANDLE pDcb)
{
	if (pDcb->pAnswer)
	{
		free(pDcb->pAnswer);
		pDcb->pAnswer = NULL;
	}
}

void dns_dealloc(DNS_HANDLE pDcb)
{
	_DnsDeleteAns(pDcb);
	if (pDcb->pName)
	{
		free(pDcb->pName);
	}
	memset((PCHAR)pDcb, 0, DNS_CB_LENGTH);
}

DNS_HANDLE dns_alloc()
{
	UCHAR i;
	DNS_HANDLE pDcb;

	pDcb = _pDcbData;
	for (i = 0; i < DNS_CB_TSIZE; i ++)
	{
		if (pDcb->iState == DS_FREE)
		{
			return pDcb;
		}
		pDcb ++;
	}

	pDcb = _pDcbData;
	for (i = 0; i < DNS_CB_TSIZE; i ++)
	{
		if (pDcb->iState == DS_RESOLVED)
		{
			dns_dealloc(pDcb);
			return pDcb;
		}
		pDcb ++;
	}

	return NULL;
}

void dns_send(DNS_HANDLE pDcb)
{
	UCHAR i;
	USHORT sSendLen;
	PCHAR pLabel;
	PCHAR pDst;
	PCHAR pCur;

	pDst = Adapter_pUdpBuf;
	memset(pDst, 0, DNS_HEADER_DATA);
	memcpy(pDst, pDcb->pID, DNS_ID_LEN);
	pDst[DNS_HEADER_FALG0] = 1;
	pDst[DNS_HEADER_QDCOUNT+1] = 1;

	sSendLen = DNS_HEADER_DATA;
	i = 0;
	pDst += DNS_HEADER_DATA;
	pLabel = pDst;
	pDst ++;
	pCur = pDcb->pName;
	if (!pCur)	return;

	while (sSendLen < 255)
	{
		if (*pCur == 0x2e || *pCur == 0)
		{
			sSendLen += i;
			sSendLen ++;
			*pLabel = i;
			if (*pCur)
			{
				i = 0;
				pLabel = pDst;
			}
			else
			{
				sSendLen ++;
				*pDst = 0;
				pDst ++;
				break;
			}
		}
		else
		{
			*pDst = *pCur;
			i ++;
		}
		pDst ++;
		pCur ++;
	}

	pDst[DNS_QTYPE] = 0;
	pDst[DNS_QTYPE + 1] = pDcb->iType;
	pDst[DNS_QCLASS] = 0;
	pDst[DNS_QCLASS+1] = DNS_CLASS_IN;
	sSendLen += DNS_QDATA;

	if (!pDcb->bDns1Failed)
	{
		UdpSendTo(_pDnsSocket, sSendLen, Sys_pDnsIp, DNS_SERVER_PORT);
	}
	if (!pDcb->bDns2Failed)
	{
		UdpSendTo(_pDnsSocket, sSendLen, Sys_pDnsIp2, DNS_SERVER_PORT);
	}
}

void dns_failed(USHORT sAddr, UCHAR iBank)
{
	UdpDebugString("Dns failed");
	if (sAddr)
	{
		TaskDnsDone(NULL, sAddr, iBank);
	}
}

void dns_save_send(DNS_HANDLE pDcb, PCHAR pDomainName, USHORT sAddr, UCHAR iBankOffset, UCHAR iType)
{
	PCHAR pBuf;
	BOOLEAN bDns1Failed, bDns2Failed;

	bDns1Failed = IsValidIP(Sys_pDnsIp) ? FALSE : TRUE;
	bDns2Failed = IsValidIP(Sys_pDnsIp2) ? FALSE : TRUE;
	if (bDns1Failed && bDns2Failed)
	{
		dns_failed(sAddr, iBankOffset);
		return;
	}

	pBuf = heap_save_str(pDomainName);
	if (pBuf)
	{
		pDcb->iBankOffset = iBankOffset;
		pDcb->sCallBack = sAddr;
		pDcb->iState = DS_PENDING;
		rand_array(pDcb->pID, DNS_ID_LEN);
		pDcb->iType = iType;
		pDcb->pName = pBuf;
		pDcb->bDns1Failed = bDns1Failed;
		pDcb->bDns2Failed = bDns2Failed;
		dns_send(pDcb);
	}
	else
	{
		// dns module resource limited, try later
		dns_failed(sAddr, iBankOffset);
	}
}

void DnsInit()
{
	memset((PCHAR)_pDcbData, 0, DNS_CB_LENGTH*DNS_CB_TSIZE);
	_pDnsSocket = UdpListen(DNS_SRC_PORT, NET_BANK_OFFSET, (USHORT)DnsRun); 
}

void DnsQuery(PCHAR pDomain, UCHAR iType, UCHAR iBankOffset, USHORT sCallBackAddr)
{
	UCHAR i;
	DNS_HANDLE pDcb;

	pDcb = _pDcbData;
	for (i = 0; i < DNS_CB_TSIZE; i ++)
	{
		if(pDcb->iState == DS_RESOLVED && iType == pDcb->iType)	
		{
			// this should be a case insensitive string compare
			if (!strcmp(pDomain, pDcb->pName))
			{
				// domain name alreay resolved
				TaskDnsDone(pDcb->pAnswer, sCallBackAddr, iBankOffset);
				return;
			}
		}
		pDcb ++;
	}

	pDcb = dns_alloc();
	if (pDcb)
	{
		dns_save_send(pDcb, pDomain, sCallBackAddr, iBankOffset, iType);
	}
	else
	{
		dns_failed(sCallBackAddr, iBankOffset);
	}
}

UCHAR _GetNameLength(PCHAR pData)
{
	UCHAR iLen, iTemp;
	PCHAR pCur;

	iLen = 0;
	pCur = pData;
	while (iLen < 255)
	{
		if ((pCur[0] & 0xc0) == 0xc0)
		{
			// This is a pointer
			iLen += 2;
			break;
		}
		else if (!pCur[0])
		{
			iLen ++;
			break;
		}
		else
		{
			iTemp = pCur[0];
			iTemp ++;
			iLen += iTemp;
			pCur += iTemp;
		}
	}
	return iLen;
}

void _GetDomainName(PCHAR pDstName, PCHAR pSrcName, PCHAR pBuf)
{
	UCHAR iLen;
	USHORT sOffset;
	PCHAR pDst;
	PCHAR pSrc;

	pSrc = pSrcName;
	pDst = pDstName;
	while ((USHORT)(pDst - pDstName) < MAX_DNS_LEN)
	{
		if ((*pSrc & 0xc0) == 0xc0)
		{
			// This is a pointer
			sOffset = PCHAR2USHORT(pSrc) & 0x3fff;
			pSrc = (PCHAR)(pBuf + sOffset);
			continue;
		}
		else
		{
			iLen = *pSrc;
			pSrc ++;
			memcpy(pDst, pSrc, iLen);
			pDst += iLen;
			pSrc += iLen;
			if (*pSrc)
			{
				pDst[0] = 0x2e;
				pDst ++;
			}
			else
			{
				break;
			}
		}
	}
	*pDst = 0;
}

BOOLEAN _DnsAllocAns(DNS_HANDLE pDcb, USHORT sCount)
{
	USHORT sSize;

	_DnsDeleteAns(pDcb);
	if (pDcb->iType == DNS_TYPE_A)
	{
		sSize = IP_ALEN;
	}
	else
	{
		sSize = sizeof(T_DNSSRV_ANS) * sCount + 1;
	}
	pDcb->pAnswer = (PCHAR)malloc(sSize);
	if (pDcb->pAnswer == NULL)
	{
		UdpDebugString("_DnsAllocAns malloc failed");
		return FALSE;
	}
	memset(pDcb->pAnswer, 0, sSize);
	return TRUE;
}

void _DnsCheckSeconday(DNS_HANDLE pDcb, PCHAR pDstIp)
{
	// dns failed, do something here
	if (!memcmp(pDstIp, Sys_pDnsIp, IP_ALEN))
	{
		pDcb->bDns1Failed = TRUE;
	}
	if (!memcmp(pDstIp, Sys_pDnsIp2, IP_ALEN))
	{
		pDcb->bDns2Failed = TRUE;
	}
	if (pDcb->bDns1Failed && pDcb->bDns2Failed)
	{
		dns_failed(pDcb->sCallBack, pDcb->iBankOffset);
		dns_dealloc(pDcb);
	}		
}

void DnsRun(UDP_SOCKET pUcb)
{
	UCHAR i, iBankOffset;
	USHORT sAnCount, sQdCount, sLength, sAnswerType, sAddr;
	UCHAR pSrvDomain[MAX_DNS_LEN];
	DNS_HANDLE pDcb;
	DNS_HANDLE pTempDcb;
	PCHAR pDnsIp;
	PCHAR pBuf;
	PT_DNSSRV_ANS ptAns;

	if (pUcb != _pDnsSocket)	return;

	pBuf = Adapter_pUdpBuf;

	pDcb = NULL;
	pTempDcb = _pDcbData;
	for (i = 0; i < DNS_CB_TSIZE; i ++)
	{
		if (!memcmp(pTempDcb->pID, pBuf, DNS_ID_LEN))
		{
			pDcb = pTempDcb;
			break;
		}
		pTempDcb ++;
	}
	if (pDcb == NULL)
	{
		// transaction id mismatch, this response is not what we need
		return;
	}
	if (pDcb->iState == DS_RESOLVED)
	{
		// this is a retransmission which has already been handled, ignore it
		return;
	}

	iBankOffset = pDcb->iBankOffset;
	sAddr = pDcb->sCallBack;
	if (!sAddr)		return;

	pDnsIp = UdpGetDstIP(pUcb);
	sAnCount = PCHAR2USHORT((PCHAR)(pBuf + DNS_HEADER_ANCOUNT));
	if ((pBuf[DNS_HEADER_FALG1] & DNS_MASK_RCODE) != DNS_RCODE_NOERROR || sAnCount == 0)
	{
		_DnsCheckSeconday(pDcb, pDnsIp);
		return;
	}

	/* pass the question section */
	sLength = DNS_HEADER_DATA;
	sQdCount = PCHAR2USHORT((PCHAR)(pBuf + DNS_HEADER_QDCOUNT));
	for (i = 0; i < sQdCount; i ++)
	{
		sLength += _GetNameLength((PCHAR)(pBuf + sLength));
		sLength += 4;
	}
	if (sLength > pUcb->sLen)	
	{
		_DnsCheckSeconday(pDcb, pDnsIp);
		return;
	}

	/* handle the answer section */
	if (!_DnsAllocAns(pDcb, sAnCount))
	{
		dns_failed(pDcb->sCallBack, pDcb->iBankOffset);
		dns_dealloc(pDcb);
		return;
	}
	for (i = 0; i < sAnCount, sLength < pUcb->sLen; i ++)
	{
		sLength += _GetNameLength((PCHAR)(pBuf + sLength));
		sAnswerType = PCHAR2USHORT((PCHAR)(pBuf + sLength + DNS_RR_TYPE));
		if (sAnswerType == pDcb->iType)
		{
			// set the cache time out
			if (PCHAR2USHORT((PCHAR)(pBuf+sLength+DNS_RR_TTL)))
			{
				pDcb->iTimer = 255;
				pDcb->iRetry = 255;
			}
			else
			{
				pDcb->iTimer = pBuf[sLength+DNS_RR_TTL+3];
				pDcb->iRetry = pBuf[sLength+DNS_RR_TTL+2];
			}
			if (sAnswerType == DNS_TYPE_A)
			{
				pDcb->iState = DS_RESOLVED;
				memcpy4(pDcb->pAnswer, (PCHAR)(pBuf + sLength + DNS_RR_RDATA));
				break;
			}
			else if (sAnswerType == DNS_TYPE_SRV)
			{
				_GetDomainName(pSrvDomain, (PCHAR)(pBuf + sLength + DNS_RR_RDATA + 6), pBuf);
				if (strcmp(pSrvDomain, "."))
				{
					pDcb->iState = DS_RESOLVED;
					ptAns = (PT_DNSSRV_ANS)(pDcb->pAnswer + 1);
					ptAns += pDcb->pAnswer[0];
					strcpy(ptAns->pcTarget, pSrvDomain);
					ptAns->sPort = PCHAR2USHORT((PCHAR)(pBuf + sLength + DNS_RR_RDATA + 4));
					ptAns->sPriority = PCHAR2USHORT((PCHAR)(pBuf + sLength + DNS_RR_RDATA + 0));
			//		UdpDebugVal(PCHAR2USHORT((PCHAR)(pBuf + sLength + DNS_RR_RDATA + 0)),10);	//fv show Priority
			//		UdpDebugVal(PCHAR2USHORT((PCHAR)(pBuf + sLength + DNS_RR_RDATA + 2)),10);	//fv show weight
					pDcb->pAnswer[0] ++;
				}
			}
		}
		sLength += DNS_RR_RDATA + PCHAR2USHORT((PCHAR)(pBuf + sLength + DNS_RR_RDLENGTH));
	}

	if (pDcb->iState == DS_RESOLVED)
	{
		TaskDnsDone(pDcb->pAnswer, pDcb->sCallBack, pDcb->iBankOffset);
	}
	else
	{
		_DnsDeleteAns(pDcb);
		_DnsCheckSeconday(pDcb, pDnsIp);
	}
}

void DnsTimer()
{
	UCHAR i;
	DNS_HANDLE pDcb;

	pDcb = _pDcbData;
	for (i = 0; i < DNS_CB_TSIZE; i ++)
	{
		if (pDcb->iState == DS_PENDING)
		{
			pDcb->iTimer ++;
			if (pDcb->iTimer >= DNS_RETRY_TIMEOUT)
			{
				pDcb->iTimer = 0;
				pDcb->iRetry ++;
				if (pDcb->iRetry > DNS_MAX_RETRY)
				{
					// dns failed, do something here
					pDcb->iRetry = 0;
					dns_failed(pDcb->sCallBack, pDcb->iBankOffset);
					dns_dealloc(pDcb);
				}
				else
				{
					dns_send(pDcb);
				}
			}
		}
		else if (pDcb->iState == DS_RESOLVED)
		{
			if (!pDcb->iTimer)
			{
				if (!pDcb->iRetry)
				{
					// cache time out, remove this entry
					dns_dealloc(pDcb);
				}
				else
				{
					pDcb->iRetry --;
					pDcb->iTimer = 255;
				}
			}
			else
			{
				pDcb->iTimer --;
			}
		}
		pDcb ++;
	}
}


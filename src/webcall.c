#include "version.h"
#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "tcpip.h"
#include "bank3.h"

#ifdef SYS_WEBCALL

#define WEBCALL_SRC_PORT	0
#define WEBCALL_CB_TSIZE	11

#define WS_FREE				0
#define WS_BUSY				1

struct webcall_cb 
{
	UCHAR iState;		// state of this WEBCALL control block
	TCP_HANDLE pTcb;	// TCP handle of this WEBCALL control block
	PCHAR pRecvBuf;		// buffer to store incomplete message from last received packet 
	UCHAR iRecvLen;		// length of the incomplete message
};
typedef struct webcall_cb * WEBCALL_HANDLE;

#define WEBCALL_CB_LENGTH		(sizeof(struct webcall_cb))

struct webcall_cb _pWcbData[WEBCALL_CB_TSIZE];
WEBCALL_HANDLE _pCurWcb;
TCP_HANDLE _pWebcallTcb;

UCHAR Webcall_pNumber[MAX_DNS_LEN];

/****************************************************************************
**** WEBCALL control block functions                                          **
****************************************************************************/
void webcall_cb_free(WEBCALL_HANDLE pHcb)
{
	if (pHcb->pRecvBuf)
	{
		free(pHcb->pRecvBuf);
	}
	memset((PCHAR)pHcb, 0, WEBCALL_CB_LENGTH);
}

WEBCALL_HANDLE webcall_cb_match(TCP_HANDLE pTcb)
{
	UCHAR i;
	WEBCALL_HANDLE pHcb;

	pHcb = _pWcbData;
	for (i = 0; i < WEBCALL_CB_TSIZE ; i ++)
	{
		if ((pHcb->iState != WS_FREE) && ((USHORT)pHcb->pTcb == (USHORT)pTcb))
		{
			return pHcb;
		}
		pHcb ++;
	}

	return NULL;
}

WEBCALL_HANDLE webcall_alloc()
{
	UCHAR i;
	WEBCALL_HANDLE pHcb;

	pHcb = _pWcbData;
	for (i = 0; i < WEBCALL_CB_TSIZE; i ++, pHcb ++)
	{
		if (pHcb->iState == WS_FREE)
		{
			pHcb->pRecvBuf = (PCHAR)malloc(HTTP_MAX_RECV_LEN + 1);
			if (pHcb->pRecvBuf)
			{
				return pHcb;
			}
			else
			{
				UdpDebugString("No more space for WEBCALL");
				heap_debug();
				return NULL;
			}
		}
	}
	UdpDebugString("No WEBCALL control block");
	return NULL;
}

void WebcallWriteStr(PCHAR pString)
{
	TcpWriteStr(_pWebcallTcb, pString);
}

void WebcallSaveRecv(UCHAR iVal)
{
	if (_pCurWcb->iRecvLen < HTTP_MAX_RECV_LEN-1)
	{
		_pCurWcb->pRecvBuf[_pCurWcb->iRecvLen] = iVal;
		_pCurWcb->iRecvLen ++;
	}
}

void WebcallSendCmd()
{
	UCHAR pDns[MAX_DNS_LEN];
	PCHAR pTemp;
	UCHAR iLen;

	// Fill first line
	WebcallWriteStr("GET /");
	OptionsGetString(pDns, OPT_NETCB_SERVER, MAX_DNS_LEN);
	pTemp = SkipField(pDns, '/');
	if (pTemp)
	{
		iLen = strlen(pTemp);
		if (iLen)
		{
			WebcallWriteStr(pTemp);
		}
	}

	WebcallWriteStr(Webcall_pNumber);
	WebcallWriteStr(" HTTP/1.1\r\n");

	// Fill header
	WebcallWriteStr("Pragma: no-cache\r\nAccept: */*\r\nHost: ");
	WebcallWriteStr(pDns);
	WebcallWriteStr("\r\nConnection: close\r\n\r\n");
}

void WebcallHandleLine(PCHAR pBuf, UCHAR iLen)
{
	USHORT sRespCode;
	PCHAR pCur;
	
	iLen;

	pCur = pBuf;
	if (!memcmp_str(pCur, "HTTP/1.1 "))
	{
		pCur += 9;
		sRespCode = atoi(pCur, 10);
		if (sRespCode == 200)
		{
			UdpDebugString("Web Call OK");
		}
		else
		{
			UdpDebugString("Web Call failed");
		}
	}
}

void WebcallRunData(USHORT sLen, PCHAR pBuf)
{
	UCHAR iVal, iNextVal;
	BOOLEAN bBrokenLine;

	while (sLen)
	{
		bBrokenLine = FALSE;
		if (_pCurWcb->iRecvLen && _pCurWcb->pRecvBuf[_pCurWcb->iRecvLen-1] == 0x0d)
		{
			bBrokenLine = TRUE;
			_pCurWcb->iRecvLen --;
		}

		iVal = *pBuf;
		pBuf ++;
		sLen --;
		if (iVal == 0x0d)
		{
			if (sLen)
			{
				iNextVal = *pBuf;
				pBuf ++;
				sLen --;
				if (iNextVal == 0x0a)
				{
					WebcallHandleLine(_pCurWcb->pRecvBuf, _pCurWcb->iRecvLen);
					_pCurWcb->iRecvLen = 0;
				}
				else
				{
					WebcallSaveRecv(iVal);
					WebcallSaveRecv(iNextVal);
				}
				continue;
			}
		}
		else if (iVal == 0x0a)
		{
			if (bBrokenLine)
			{
				WebcallHandleLine(_pCurWcb->pRecvBuf, _pCurWcb->iRecvLen);
				_pCurWcb->iRecvLen = 0;
				bBrokenLine = FALSE;
				continue;
			}
		}
		WebcallSaveRecv(iVal);
	}
}

/*************************************************************************
**** Webcall API functions     *********************************************
**************************************************************************/

void WebcallInit()
{
	UCHAR i;
	WEBCALL_HANDLE pHcb;

	pHcb = _pWcbData;
	for (i = 0; i < WEBCALL_CB_TSIZE; i ++)
	{
		memset((PCHAR)pHcb, 0, WEBCALL_CB_LENGTH);
		pHcb ++;
	}
}

void WebcallStartStep2(PCHAR pServerIP)
{
	if (pServerIP)
	{
		_pCurWcb = webcall_alloc();
		if (_pCurWcb)
		{
			_pCurWcb->iState = WS_BUSY;
			_pCurWcb->pTcb = TcpOpen(pServerIP, HTTP_SERVER_PORT, WEBCALL_SRC_PORT, HTTP_BANK_OFFSET, (USHORT)WebcallRun);
			if (_pCurWcb->pTcb)
			{
				TcpSetTimeout(_pCurWcb->pTcb, HTTP_ACTIVE_TIMEOUT);
				return;
			}		
		}
	}
}

void WebcallStart()
{
	UCHAR pDns[MAX_DNS_LEN];
	UCHAR pDstIP[IP_ALEN];

	OptionsGetString(pDns, OPT_NETCB_SERVER, MAX_DNS_LEN);
	SkipField(pDns, '/');

	if (strlen(pDns))
	{
		if (str2ip(pDns, pDstIP))
		{
			WebcallStartStep2(pDstIP);
		}
		else
		{
			TaskDnsQuery(pDns, DNS_TYPE_A, HTTP_BANK_OFFSET, (USHORT)WebcallStartStep2);
		}
	}
}

void WebcallRun(TCP_HANDLE h, USHORT sLen, PCHAR pBuf)
{
	UCHAR iEvent;

	_pCurWcb = webcall_cb_match(h);
	if (!_pCurWcb)
	{
		UdpDebugString("unkown Webcall handle");
		TcpFree(h);
		return;
	}

	iEvent = TcpGetEvent(h);
	if (iEvent & (TCP_EVENT_RESET|TCP_EVENT_ABORT|TCP_EVENT_USERTO|TCP_EVENT_RETRYTO))
	{
		UdpDebugString("Webcall reset/abort/timeout");
		if (iEvent & TCP_EVENT_ABORT)
		{
			TcpStartSend(h);
		}
		TcpFree(h);
		webcall_cb_free(_pCurWcb);
		return;
	}
	
	_pWebcallTcb = h;	
	if (iEvent & TCP_EVENT_CONNECT)
	{
		UdpDebugString("Webcall connect");
		WebcallSendCmd();
	}
	if (iEvent & TCP_EVENT_DATA)
	{
		WebcallRunData(sLen, pBuf);
	}
	if (iEvent & TCP_EVENT_CLOSE)
	{
		UdpDebugString("Webcall close");
		TcpClose(_pWebcallTcb);
	}

	TcpClearEvent(_pWebcallTcb);

	TcpStartSend(_pWebcallTcb);
}

#endif
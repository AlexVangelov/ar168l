// tcp.c

void TcpInit();
void TcpSetTimeout(TCP_HANDLE pTcb, UCHAR iTimeout);
UCHAR TcpGetEvent(TCP_HANDLE pTcb);
void TcpClearEvent(TCP_HANDLE pTcb);
void TcpClose(TCP_HANDLE pTcb);
void TcpFree(TCP_HANDLE pTcb);
void TcpGetRemoteIP(TCP_HANDLE pTcb, PCHAR pRemoteIP);
TCP_HANDLE TcpListen(USHORT sSrcPort, UCHAR iBankOffset, USHORT sCallBack);
TCP_HANDLE TcpOpen(PCHAR pDstIP, USHORT sDstPort, USHORT sSrcPort, UCHAR iBankOffset, USHORT sCallBack);
BOOLEAN TcpCanWrite(TCP_HANDLE pTcb, USHORT sMaxLen);
void TcpWriteData(TCP_HANDLE pTcb, PCHAR pData, USHORT sLen);
void TcpWriteStr(TCP_HANDLE pTcb, PCHAR pStr);
void TcpStartSend(TCP_HANDLE pTcb);
void TcpRun(PACKET_LIST * p);
void TcpTimer();

#define TCP_SRC_PORT		3000
#define HTTP_SERVER_PORT	80

#define HTTP_ACTIVE_TIMEOUT		60
#define HTTP_REBOOT_TIMEOUT		2
#define HTTP_MAX_RECV_LEN		255

// http.c
void HttpdInit();
void HttpdRun(TCP_HANDLE h, USHORT sLen, PCHAR pBuf);
void HttpdTimer();

void HttpInit();
//void HttpTimer();

// httpc.c
void HttpcInit();
void HttpcStart();
void HttpcRun(TCP_HANDLE h, USHORT sLen, PCHAR pBuf);
void HttpcStartStep2(PCHAR pServerIP);
void HttpcMinuteTimer();
void HttpcTimer();

// webcall.c
void WebcallInit();
void WebcallStart();
void WebcallRun(TCP_HANDLE h, USHORT sLen, PCHAR pBuf);
void WebcallStartStep2(PCHAR pServerIP);
void WebcallMinuteTimer();
void WebcallTimer();
void SerialQuery(UCHAR iChannelNumber);

// rc4.c
#ifdef OEM_INNOMEDIA
void RC4Init(PCHAR pKey, UCHAR iKeyLen);
void RC4Calc(PCHAR pDst, PCHAR pSrc, USHORT sLen);
#endif

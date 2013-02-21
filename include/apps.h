// rtp payload type
#define RTP_ULAW		0
#define RTP_GSM			3
#define RTP_ALAW		8
#define RTP_G722		9
#define RTP_G723_1		4
#define RTP_COMFORT_NOISE	13	
#define RTP_G728		15
#define RTP_G729		18
#define RTP_REDUNDENCE	96
#define RTP_ILBC		98
#define RTP_TELEVENT	101	
#define RTP_G726_32		111
#define RTP_SPEEX		110
#define RTP_LAST		127
#define RTP_NONE		255

// phone status type sent by serial port
#define STATUS_LOGON_OK		0
#define STATUS_LOGON_FAIL	1
#define STATUS_LOGOFF_OK	2
#define STATUS_LOGOFF_FAIL	3
#define STATUS_MSG_INDI		4
#define STATUS_CONNECT		5

// Network connection type
#define IP_TYPE_STATIC		0
#define IP_TYPE_DHCP		1
#define IP_TYPE_PPPOE		2

// heap.c
void heap_init();
void heap_debug();
PCHAR heap_save_str(PCHAR pStr);
PCHAR heap_save_data(PCHAR pData, USHORT sLen);

void * malloc(USHORT sSize);
void free(void *p);

// line.c
void line_backup();
void line_restore();
void line_start(PCHAR pDst);
USHORT line_get_len();
void line_update_len();
PCHAR line_get_buf();
void line_add_data(PCHAR pData, USHORT sLen);
void line_add_str(PCHAR pStr)	__naked;
void line_add_str_with_char(PCHAR pStr, UCHAR iChar)	__naked;
void line_add_str_with_crlf(PCHAR pStr)	__naked;
void line_add_short(USHORT sVal);
void line_add_long(ULONG lVal);
void line_add_ip(PCHAR pIPAddr);
void line_add_char(UCHAR iChar);
void line_add_crlf();
UCHAR is_endof_line(PCHAR pData);
UCHAR count_space(PCHAR pStr);

void line_add_options_str(USHORT sOffset, UCHAR iMaxLen);

// Options functions
// Voice Coder
#define VOICE_CODER_NONE	0
#define VOICE_CODER_ULAW	1	/* Raw mu-law data (G.711) */
#define VOICE_CODER_ALAW	2	/* Raw a-law data (G.711) */
#define VOICE_CODER_G726_32 3   /* G.726-32 */
#define VOICE_CODER_GSM		4	/* GSM 6.10 compression */
#define VOICE_CODER_ILBC	5	/* ILBC audio, 50 bytes per 240 samples */
#define VOICE_CODER_SPEEX	6   /* Speex */
#define VOICE_CODER_G729	7	/* G.729a Audio */
#define VOICE_CODER_NUM		7

// NAT Traversal const
#define NAT_TRAVERSAL_DISABLE	0
#define NAT_TRAVERSAL_STUN		1
#define NAT_TRAVERSAL_NATIP		2

// Mode of phone
#define SYS_MODE_STANDBY	0
#define SYS_MODE_CALL		1
#define SYS_MODE_MENU		2
#define SYS_MODE_TFTPD		3
#define SYS_MODE_HTTPD		4
#define SYS_MODE_AUTOPROV	5
#ifdef OEM_IP20
#define SYS_MODE_TEST		6
#define SYS_MODE_ALARM		7
#endif

extern UCHAR Sys_iCurMode;
extern UCHAR Sys_iMissedCalls;
extern BOOLEAN Sys_bDnd;
extern PCHAR Sys_pProvUrl;

// Reason of Call Ending
#define CALL_END_UNKNOWN		0
#define CALL_END_NOT_LOGON		1
#define CALL_END_EMPTY_NUMBER	2
#define CALL_END_WRONG_NUMBER	3
#define CALL_END_PEER_BUSY		4
#define CALL_END_NO_ANSWER		5
#define CALL_END_PEER_REJECT	6
#define CALL_END_PEER_HANGUP	7
#define CALL_END_MSG_TIMEOUT	8
#define CALL_END_CODER_MISMATCH	9
#define CALL_END_RES_LIMITED	10
#define CALL_END_BAD_SERVERIP	11
#define CALL_END_XFER_FAILED	12
#define CALL_END_XFER_DONE		13

// options.c
extern UCHAR Sys_iSystemPage;
extern UCHAR Sys_pMacAddress[HW_ALEN];			// mac address, 6 bytes
extern UCHAR Sys_iIpType;						// ip type, 1 byte 0[static] 1[dhcp]
extern UCHAR Sys_pIpAddress[IP_ALEN];			// ip address, 4 bytes
extern UCHAR Sys_pSubnetMask[IP_ALEN];			// subnet mask, 4 bytes
extern UCHAR Sys_pRouterIp[IP_ALEN];			// router ip, 4 bytes
extern UCHAR Sys_pDnsIp[IP_ALEN];				// dns ip, 4 bytes
extern UCHAR Sys_pDnsIp2[IP_ALEN];				// dns ip 2, 4 bytes
extern UCHAR Sys_pLogIp[IP_ALEN];				// sys log ip, 4 bytes
#ifndef CALL_NONE
extern BOOLEAN Sys_bRegister;
extern UCHAR Sys_iTimeZone;
extern BOOLEAN Sys_bDayLight;
extern UCHAR Sys_iDtmfType;
extern UCHAR Sys_iDigitmapTimeout;
extern BOOLEAN Sys_bUseDigitmap;
extern BOOLEAN Sys_bFwdAlways;
extern BOOLEAN Sys_bFwdBusy;
extern BOOLEAN Sys_bFwdNoanswer;
extern BOOLEAN Sys_bCallWaiting;
extern BOOLEAN Sys_bAutoAnswer;
extern UCHAR Sys_iNoanswerTimeout;
#ifdef CALL_SIP
extern BOOLEAN Sys_bOutboundProxy;
extern BOOLEAN Sys_bPrack;
extern UCHAR Sys_iDtmfPayload;
extern UCHAR Sys_pSipDomain[];
extern UCHAR Sys_pSipProxy[];
extern UCHAR Sys_pProxyRequire[];
#endif
extern USHORT Sys_sRegisterTtl;
extern USHORT Sys_sLocalPort;
extern USHORT Sys_sRtpPort;

extern UCHAR Sys_pAuthID[];
extern UCHAR Sys_pAuthPin[];
extern UCHAR Sys_pUserNumber[];
extern UCHAR Sys_pUserName[];
#endif

extern BOOLEAN Sys_bOptVad;
extern UCHAR Sys_iIlbcFrame;
extern UCHAR Sys_iSpeexRate;
extern UCHAR Sys_iRingVolOutput;
extern UCHAR Sys_iSpeakerVol;
extern UCHAR Sys_iHandsetVol;
extern UCHAR Sys_pVoiceCoders[VOICE_CODER_NUM];

#ifdef OEM_ROIP
extern UCHAR Sys_iPttControl;
#endif

extern UCHAR Rtp_pPayLoad[VOICE_CODER_NUM + 1];

extern BOOLEAN Net_bConnected;

void OptionsGetString(PCHAR pStr, USHORT sOffset, UCHAR iMaxLen);
USHORT OptionsGetShort(USHORT sOffset);
void OptionsWriteBuf(PCHAR pBuf, USHORT sOffset, UCHAR iLen);
void OptionsWriteByte(USHORT sOffset, UCHAR iVal);

//UDP Control Block structure
struct ucb
{
	UCHAR iState;
	USHORT sSrcPort;
	UCHAR iBankOffset;				/* Bank offset of call back function	*/
	USHORT sCallBack;				/* Address of call back function		*/
	USHORT sDstPort;
	UCHAR pDstIP[IP_ALEN];
	USHORT sLen;
};

#define UCB_TSIZE	16

// UDP Control Block state
#define UDPS_FREE		0
#define UDPS_LISTEN		1
#define UDPS_OPEN		2

// Udp functions
UDP_SOCKET UdpListen(USHORT sSrcPort, UCHAR iBankOffset, USHORT sCallBack);
void UdpConnectTo(UDP_SOCKET pUcb, PCHAR pDstIP, USHORT sDstPort);
void UdpConnect(UDP_SOCKET pUcb);
void UdpClose(UDP_SOCKET pUcb);
void UdpSend(UDP_SOCKET pUcb, USHORT sLen);
void UdpSendTo(UDP_SOCKET pUcb, USHORT sLen, PCHAR pDstIP, USHORT sDstPort);
BOOLEAN UdpCmpAddr(UDP_SOCKET pUcb, PCHAR pIPAddr, USHORT sPort);
PCHAR UdpGetDstIP(UDP_SOCKET pUcb);
USHORT UdpGetDstPort(UDP_SOCKET pUcb);

USHORT UdpCheckSum(USHORT sLen, PCHAR pData, PCHAR pSrcIP, PCHAR pDstIP);

extern const UCHAR _pBroadcastIP[IP_ALEN];

// Syslog functions, syslog.c
#define SYSLOG_PRI_EMERGENCY	0
#define SYSLOG_PRI_ALERT		1
#define SYSLOG_PRI_CRITICAL		2
#define SYSLOG_PRI_ERROR		3
#define SYSLOG_PRI_WARNING		4
#define SYSLOG_PRI_NOTICE		5
#define SYSLOG_PRI_INFOR		6
#define SYSLOG_PRI_DEBUG		7

void SyslogOpen();
void SyslogClose();
void Syslog(UCHAR iPri, PCHAR pStr);
void SyslogAllUdpData(USHORT sLen);

// Udp debug functions
//void UdpDebugString(PCHAR pStr);
#define MAX_UDP_DEBUG		100
#define UdpDebugString(x)	Syslog(SYSLOG_PRI_DEBUG, x)

void UdpDebugVal(USHORT sVal, UCHAR iRadix);
void UdpDebugIDS(UCHAR iIDS);
void UdpDebugVals(PCHAR pVals, USHORT sLen);
void UdpDebugInfo(PCHAR pPrefix, USHORT sAddr, USHORT sLen);
void UdpDebugIpPort(PCHAR pPrefix, PCHAR pIp, USHORT sPort);
void UdpDebugChar(UCHAR iChar);

void UdpDebugTcpHead(UCHAR iHeadLen);

// misc.c
void itoa(USHORT sVal, PCHAR pBuf, UCHAR iRadix);
USHORT atoi(PCHAR pBuf, UCHAR iRadix);
void ltoa(ULONG lVal, PCHAR pBuf, UCHAR iRadix);
ULONG atol(PCHAR pBuf, UCHAR iRadix);

void ip2str(PCHAR pIP, PCHAR pStr);
BOOLEAN	str2ip(PCHAR pStr, PCHAR pDstIP);

void mac2str(PCHAR pMac, PCHAR pString);

UCHAR char2asc(UCHAR iChar, BOOLEAN bUpper);
void char2asc_str(PCHAR pDst, PCHAR pSrc, UCHAR iLen, BOOLEAN bUpper);

#define MAP_KEY_UNKNOWN		0xff
#define MAP_KEY_NUM			13
UCHAR ascii2keyid(UCHAR iAscii);
UCHAR keyid2ascii(UCHAR iKeyid);

BOOLEAN IsValidIP(PCHAR pIP);
BOOLEAN IsPrivateIP(PCHAR pIP);

void Fill2Digits(PCHAR pBuf, UCHAR iVal);
PCHAR SkipField(PCHAR pBuf, UCHAR iChar);

// data.c
ULONG div_32x8(ULONG l32, UCHAR i8);
UCHAR mod_32x8(ULONG l32, UCHAR i8);

// pseudorandom number functions
void srand(UCHAR seed);
UCHAR rand();
void rand_array(PCHAR pDst, UCHAR iLen);

// DSP buffer functions, buf.c

void BufWrite(PCHAR pBuf, USHORT iLen);
void BufRead(PCHAR pBuf, USHORT iLen);
void BufForward(USHORT iLen);
void BufSeek(USHORT sOffset);

void BufCopyDM2PM();


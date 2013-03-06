#define SIP_SERVER_PORT				5060

#ifdef OEM_ROIP
#define SIP_DEFAULT_SESSION_EXPIRE	127		// 113, 127 are 2 nearest prime number around 120. Asterisk will NOT disconnect call based on this
#else
#define SIP_DEFAULT_SESSION_EXPIRE	1800
#endif

#define SIP_DEFAULT_MIN_SE			90

// Label length
#define MAX_BRANCH_LEN		24
#define MAX_TAG_LEN			16
#define MAX_CALL_ID_LEN		32

// DTMF type
#define DTMF_TYPE_INBAND	0
#define DTMF_TYPE_RFC2833	1
#define DTMF_TYPE_SIPINFO	2

#define SIP_REQ_INVITE		0
#define SIP_REQ_ACK			1
#define SIP_REQ_OPTIONS		2
#define SIP_REQ_BYE			3
#define SIP_REQ_CANCEL		4
#define SIP_REQ_REGISTER	5
#define SIP_REQ_REFER		6
#define SIP_REQ_NOTIFY		7
#define SIP_REQ_INFO		8
#define SIP_REQ_PRACK		9
#define SIP_REQ_UPDATE		10
#define SIP_REQ_MESSAGE		11
#define SIP_REQ_SUBSCRIBE	12
#define SIP_REQ_NUMBER		13
#define SIP_REQ_UNKNOWN		99

#define SIP_MSG_UNKNOWN		0
#define SIP_MSG_REQUEST		1
#define SIP_MSG_RESPONSE	2

#define SIP_REFRESHER_NONE	0
#define SIP_REFRESHER_UAC	1
#define SIP_REFRESHER_UAS	2

// Audio transmission mode
#define AUDIO_MODE_INACTIVE		0
#define AUDIO_MODE_RECVONLY		1
#define AUDIO_MODE_SENDONLY		2
#define AUDIO_MODE_SENDRECV		3
#define AUDIO_MODE_NUM			4

// RTP header fields
#define RTP_VERSION		0
#define RTP_PAYLOAD		1
#define RTP_SEQ			2
#define RTP_TIMESTAMP	4
#define RTP_SSRC		8
#define RTP_DATA		12

#define RTP_SSRC_LEN	4
#define RTP_DTMF_DURATION	960

// Transaction definitions
#define TR_TSIZE	8	// Number of transaction control block

// transaction state
#define TRS_FREE		0
#define TRS_TRYING		1
#define TRS_PROCEEDING	2
#define TRS_COMPLETED	3

#define TR_TIMER_RETRY	3
#define TR_TIMER_LIVE	12

typedef struct sip_tcb * SIP_TCB_HANDLE;
typedef struct sip_lcb * SIP_LCB_HANDLE;


struct sip_tcb
{
	UCHAR iState;
	UCHAR iMethod;
	UCHAR pDstIP[IP_ALEN];
	USHORT sDstPort;
	UCHAR iTimer0;
	UCHAR iTimer1;
	UCHAR iTimer2;
	BOOLEAN b100rel;
	PCHAR pBranch;
	PCHAR pData;
	SIP_LCB_HANDLE pLcb;
};

#define SIP_TCB_LENGTH		(sizeof(struct sip_tcb))

// transfer role
#define XFER_PART_NONE				0
#define XFER_PART_TRANSFEROR		1
#define XFER_PART_TRANSFEREE_ORIG	2
#define XFER_PART_TRANSFEREE_NEW	3
#define XFER_PART_TRANSFERTO		4
#define XFER_PART_FAIL				5

// Ringback type
#define RINGBACK_NONE	0	// no ringback yet
#define RINGBACK_LOCAL	1	// local generated ringback tone
#define RINGBACK_REMOTE	2	// remote send ringback tone

//SDP type
#define SDP_TYPE_OFFER		0
#define SDP_TYPE_ANSWER		1
#define SDP_TYPE_REINVITE	2

#define LCB_TSIZE		2	// Number of lines supported

struct sip_lcb
{
	UCHAR iState;			// State of this line
	BOOLEAN bCallee;		// Caller or callee
	UCHAR iTimer;			// No anser timer
	UCHAR iRemoteRb;		// ringback tone type
	BOOLEAN bHolding;
	BOOLEAN bEarlyDlg;		// already received a 1xx response
 
	UCHAR iVoiceCoder;		// voice coder used by this line
	BOOLEAN bVad;
	UCHAR iRemoteMode;		// remote voice transmission mode
	UCHAR pRtpDstIP[IP_ALEN];	// RTP dst IP
	USHORT sRtpDstPort;		// RTP dst port
	UCHAR iCoderPayload;	// Peer's coder payload
	UCHAR iDtmfPayload;		// Peer's DTMF payload

	UCHAR iLocalMode;		// local voice transmission mode
	UCHAR iSdpType;
	UCHAR pSdpSessionId[9];
	ULONG lSdpVersion;
	UDP_SOCKET pRtpSocket;
	USHORT sRtpPort;		// local RTP port
	USHORT sRtpSeq;			// RTP sequence
	UCHAR pRtpSSRC[RTP_SSRC_LEN];
	UCHAR pRtpDstSSRC[RTP_SSRC_LEN];
	ULONG lRtpTimeStamp;	// RTP timestamp
	ULONG lRtpOffset;
	BOOLEAN bVoiceStart;	
	BOOLEAN bSendingKey;	// RFC2833 send key 
	UCHAR iSendKey;			// Key being sent
	USHORT sSendKeyLen;		// Duration in timestamp
	BOOLEAN bKeyRecved;
	UCHAR pKeyTimeStamp[4];

	UCHAR pDstIP[IP_ALEN];	// Dst IP of this call
	USHORT sDstPort;		// Dst port of this call
	ULONG lRseq;			// RSEQ for PRACK
	ULONG lCurSeq;			// Current sequence of this call
	ULONG lInvSeq;			// Sequence of INVITE request
	PCHAR pInvUri;			// Original request Uri of this call
	PCHAR pReqUri;			// Request URI of this call
	PCHAR pBranch;			// Invite branch
	PCHAR pCallId;			// Call-ID header of this call
	PCHAR pFrom;			// From header of this call
	PCHAR pTo;				// To header
	PCHAR pFromTag;			// Local tag	
	PCHAR pToTag;			// Remote tag
	PCHAR pProxyAuth;		// Proxy-Authenticate information
	PCHAR pWWWAuth;			// WWW-Authenticate information
	PCHAR pRoute;			// Route header to be included in request
	PCHAR pInvHeader;		// Invite header to be included in response
	SIP_TCB_HANDLE pInvTcb;	// Transaction control block of INVITE request

	UCHAR pRespDstIP[IP_ALEN];
	USHORT sRespDstPort;
	PCHAR pReferredBy;
	PCHAR pReplaces;
	UCHAR iXferPart;		// which role do we play? transferor, transferee or transfer-to?

	UCHAR iRefresher;
	ULONG lSessionExpires;
	ULONG lSessionTimer;
	ULONG lMinSE;

	UCHAR pPeerName[MAX_USER_NAME_LEN];
	UCHAR pPeerNumber[MAX_USER_NUMBER_LEN];
	CALL_TIME ct;
	UCHAR pKeyOut[MAX_USER_NUMBER_LEN];
	UCHAR iKeyIndex;
	BOOLEAN bMemCall;

//	UCHAR iPacketCtr;
#ifdef OEM_IP20
	UCHAR brtpSession;	// RFC2833 send key 
	UCHAR brtpIpPrivate;	// RFC2833 send key 
#endif
};

#define SIP_LCB_LENGTH		(sizeof(struct sip_lcb))

typedef struct sip_sdp * SIP_SDP_HANDLE;
#define MAX_SDP_CODER_NUM	16
struct sip_sdp
{
	BOOLEAN bValid;
	BOOLEAN bValidIP;
	BOOLEAN bMediaAudio;
	UCHAR iCoderNum;
	UCHAR pPayload[MAX_SDP_CODER_NUM];
	UCHAR pCoder[MAX_SDP_CODER_NUM];

	UCHAR iVoiceCoder;		// voice coder used by this line
	BOOLEAN bVad;
	UCHAR iRemoteMode;		// remote voice transmission mode
	UCHAR pRtpDstIP[IP_ALEN];	// RTP dst IP
	USHORT sRtpDstPort;		// RTP dst port
	UCHAR iCoderPayload;	// Peer's coder payload
	UCHAR iDtmfPayload;		// Peer's DTMF payload
};

#define SIP_SDP_LENGTH		(1+1+1+IP_ALEN+2+1+1)

// Registration status
#define RS_FREE				0	
#define RS_REGISTERING		1
#define RS_REGISTERED		2
#define RS_REFRESHING		3

#define SIP_RS_REFRESH_TIMEOUT	60	// Time, to wait for answer to refresh request [sec]

#define SIP_MAX_AUTH_RETRY	2
#define USHORT_MAX		0xffff

// SIP global variables
extern UDP_SOCKET Sip_pSocket;
extern UCHAR Sip_pNatIP[IP_ALEN];
extern USHORT Sip_sNatPort;
extern PCHAR Sip_pTempBuf;

#define MAX_RECV_BODY_LINE	8
extern PCHAR Sip_pRecvBody[MAX_RECV_BODY_LINE];
extern UCHAR Sip_iRecvBodyLine;
extern PCHAR Sip_pRecvBoundary;
extern PCHAR Sip_pRecvBranch;
extern PCHAR Sip_pRecvInfo;
extern SIP_SDP_HANDLE Sip_pRecvSdp;
extern UCHAR Sip_iRecvMethod;
extern USHORT Sip_sRecvResp;
extern USHORT Sip_sRespDstPort;

// headers of received message
#define MAX_RECV_VIA_NUM	6
#define MAX_RECV_RR_NUM		6
extern PCHAR Sip_pRecvVia[MAX_RECV_VIA_NUM];
extern UCHAR Sip_iRecvViaNum;
extern PCHAR Sip_pRecvRecordRoute[MAX_RECV_RR_NUM];
extern UCHAR Sip_iRecvRRNum;
extern PCHAR Sip_pRecvFrom;
extern PCHAR Sip_pRecvTo;
extern PCHAR Sip_pRecvCallId;
extern PCHAR Sip_pRecvContact;
extern PCHAR Sip_pRecvCSeq;
extern PCHAR Sip_pRecvWWWAuth;
extern PCHAR Sip_pRecvProxyAuth;
extern PCHAR Sip_pRecvRequire;
extern PCHAR Sip_pRecvRSeq;
extern PCHAR Sip_pRecvRAck;
extern PCHAR Sip_pRecvSupported;
extern PCHAR Sip_pRecvReplaces;
extern PCHAR Sip_pRecvReferTo;
extern PCHAR Sip_pRecvReferredBy;
extern PCHAR Sip_pRecvCallInfo;
extern PCHAR Sip_pRecvSessionExpires;
extern UCHAR Sip_iContentType;
extern UCHAR Sip_iRecvEvent;
extern USHORT Sip_sContentLen;
extern USHORT Sip_sRecvExpires;
extern USHORT Sip_sRecvMinExpires;
extern ULONG Sip_lRecvMinSE;

extern UCHAR Sip_iRegState;
extern USHORT Sip_sRegTimer;
extern USHORT Sip_sRegTimeout;
extern ULONG Sip_lRegSeq;
extern UCHAR Sip_iRegAuthRetry;
extern PCHAR Sip_pRegWWWAuth;
extern PCHAR Sip_pRegProxyAuth;
extern UCHAR Sip_pRegFromTag[MAX_TAG_LEN+1];
extern UCHAR Sip_pRegCallId[MAX_CALL_ID_LEN+1];
extern UCHAR Sip_pServerIP[IP_ALEN];
extern USHORT Sip_sServerPort;
extern UCHAR Sip_iNatTimer;

extern BOOLEAN Sip_bSubscribe;
extern PCHAR Sip_pSubReqUri;
extern PCHAR Sip_pSubCallId;
extern PCHAR Sip_pSubFromTag;
extern PCHAR Sip_pSubToTag;
extern ULONG Sip_lSubSeq;
extern USHORT Sip_sSubTimer;
extern USHORT Sip_sSubTimeout;
extern UCHAR Sip_iSubAuthRetry;
extern PCHAR Sip_pSubWWWAuth;
extern PCHAR Sip_pSubProxyAuth;

extern struct sip_tcb Sip_pReqTCB[TR_TSIZE];
extern struct sip_tcb Sip_pRespTCB[TR_TSIZE];
extern SIP_TCB_HANDLE Sip_pCurTcb;

extern struct sip_lcb Sip_pLCB[LCB_TSIZE];
extern SIP_LCB_HANDLE Sip_pActiveLcb;
extern SIP_LCB_HANDLE Sip_pCurLcb;
extern USHORT Sip_sRtpPort;

extern PCHAR Rtp_pData;

extern const UCHAR _cRequest[SIP_REQ_NUMBER][10];
extern const UCHAR _cAudioMode[][9];

extern const UCHAR _cTokenSipVer[];
extern const UCHAR _cTokenSip[];
extern const UCHAR _c100rel[];
extern const UCHAR _cTokenTag[];
extern const UCHAR _cTokenBranch[];
extern const UCHAR _cMsgFrag[];
extern const UCHAR _cTextPlain[];
extern const UCHAR _cTextHtml[];

// sipc.c
void sip_debug(PCHAR pStrInfo, USHORT iMessage);
void sip_tr_free(SIP_TCB_HANDLE pTcb);

#ifdef CALL_IAX2
// IAX2 functions, iax2.c
void Iax2Init();
void Iax2Logon();
void Iax2DPCall();
void Iax2Call(PCHAR pCalledUri);
void Iax2HandleEvent(UCHAR iEvent, USHORT sParam);
USHORT Iax2GetCallInfo(UCHAR iType);
void Iax2Timer();
void Iax2SendVoice(UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2);
void Iax2FrameTimer();
#endif

// protocol.c
void CallTimeZero(CALL_TIME * p);
void CallTimeTick(CALL_TIME * p);

UCHAR GetFrameTime(UCHAR iVoiceCoder);

#ifdef CALL_SIP

// RTP functions, in rtps.c
#define RTP_SSRC_LEN	4
typedef struct rtp_cb * RTP_HANDLE;
struct rtp_cb
{
	UCHAR iState;
	UCHAR iLocalMode;		// Local audio transmission mode
	UCHAR iRemoteMode;		// Remote audio transmission mode
	UCHAR iVoiceCoder;		// Voice coder this RTP is using
	UCHAR pDstIP[IP_ALEN];
	USHORT sDstPort;
	USHORT sSeq;
	UCHAR pSSRC[RTP_SSRC_LEN];
	ULONG lTimeStamp;
};

#define RCB_LENGTH	(sizeof(struct rtp_cb))

UCHAR RtpPayloadToCoder(UCHAR iPayload);

UCHAR RtpGetPayload(UCHAR iVoiceCoder, SIP_LCB_HANDLE pLcb);
void RtpSendKey(SIP_LCB_HANDLE pLcb, UCHAR iKey);
void RtpSendKeyPacket(SIP_LCB_HANDLE pLcb, BOOLEAN bHead, BOOLEAN bEnd);
void RtpSendVoice(SIP_LCB_HANDLE pLcb, UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2);

// SIP functions, sips.c
void SipInit();
void SipLogon();
void SipLogoff();
void SipCall(PCHAR pCalledUri);
void SipHandleEvent(UCHAR iEvent, USHORT sParam);
USHORT SipGetCallInfo(UCHAR iType);
void SipTimer();
void SipSendVoice(UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2);
void SipFrameTimer();
void SipStunDone(PCHAR pMappedAddr, USHORT sMappedPort, UCHAR iTask);
void SipSendMessage(PCHAR pMsg);

void SipLogonStep1();
void SipLogonStep2(PCHAR pServerIP);
void SipLogonSrvStep2(PCHAR pAnswer);
void SipHangup();
void sip_handle_error(SIP_LCB_HANDLE pLcb, UCHAR iMethod);
#endif


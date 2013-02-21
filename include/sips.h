extern UCHAR Sys_iNatTraversal;

extern const UCHAR _cTo[];
extern const UCHAR _cReferredBy[];
extern const UCHAR _cReplaces[];

extern const UCHAR _cSend[];

#define SIP_DEFAULT_REG_EXPIRE		60	/* By default require re-registration once per minute */
#define SIP_DEFAULT_SUB_EXPIRE		300

void SipCallStep1(PCHAR pCalledUri);
void SipCallStep2(PCHAR pServerIP);
void SipCallStep3();

void sip_star2dot(PCHAR pBuf);

void sip_caller_failed(UCHAR iCause);

void sip_new_callid(PCHAR pCallId);
void sip_new_from();
void sip_new_random(PCHAR pBuf, UCHAR iLen);
void sip_new_token(PCHAR pDst, UCHAR iLen);
void sip_new_branch(PCHAR pBranch);

void sip_modify_rtpport();

void sip_send_invite();

void sip_add_req_line(UCHAR iMethod, PCHAR pUri);
void sip_add_via(PCHAR pBranch);
void sip_add_common();
void sip_add_header(PCHAR pName, PCHAR pHeapBuf);
void sip_add_to(PCHAR pUri, PCHAR pTag);
void sip_add_cseq(ULONG lSeq, UCHAR iMethod);
void sip_add_auth(PCHAR pUri, UCHAR iMethod);
void sip_add_supported();
void sip_add_allow();
void sip_add_session_expires(SIP_LCB_HANDLE pLcb);
void sip_add_content_sdp(SIP_LCB_HANDLE pLcb);
void sip_add_local_uri(BOOLEAN bIP, BOOLEAN bPort);
void sip_add_token(PCHAR pToken, PCHAR pValue);

void sip_tr_send_req(PCHAR pDstIP, USHORT sDstPort, PCHAR pBranch, UCHAR iMethod, SIP_LCB_HANDLE pLcb);

SIP_TCB_HANDLE sip_tr_new_req();
void sip_tr_send(PCHAR pDstIP, USHORT sDstPort, BOOLEAN bSave);





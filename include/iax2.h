/* Frame types */
#define AST_FRAME_DTMF		1		/* A DTMF digit, subclass is the digit */
#define AST_FRAME_VOICE		2		/* Voice data, subclass is AST_FORMAT_* */
#define AST_FRAME_VIDEO		3		/* Video frame, maybe?? :) */
#define AST_FRAME_CONTROL	4		/* A control frame, subclass is AST_CONTROL_* */
#define AST_FRAME_NULL		5		/* An empty, useless frame */
#define AST_FRAME_IAX		6		/* Inter Aterisk Exchange private frame type */
#define AST_FRAME_TEXT		7		/* Text messages */
#define AST_FRAME_IMAGE		8		/* Image Frames */
#define AST_FRAME_HTML		9		/* HTML Frames */
#define AST_FRAME_CFTNOISE	10		/* Level in -dBov of comfort noise */

/* HTML subclasses */
#define AST_HTML_URL		1		/* Sending a URL */
#define AST_HTML_DATA		2		/* Data frame */
#define AST_HTML_BEGIN		4		/* Beginning frame */
#define AST_HTML_END		8		/* End frame */
#define AST_HTML_LDCOMPLETE	16		/* Load is complete */
#define AST_HTML_NOSUPPORT	17		/* Peer is unable to support HTML */
#define AST_HTML_LINKURL	18		/* Send URL and track */
#define AST_HTML_UNLINK		19		/* Request no more linkage */
#define AST_HTML_LINKREJECT	20		/* Reject LINKURL */

/* Data formats for capabilities and frames alike */
#define AST_FORMAT_G723_1	(1 << 0)	/* G.723.1 compression */
#define AST_FORMAT_GSM		(1 << 1)	/* GSM compression */
#define AST_FORMAT_ULAW		(1 << 2)	/* Raw mu-law data (G.711) */
#define AST_FORMAT_ALAW		(1 << 3)	/* Raw A-law data (G.711) */
#define AST_FORMAT_G726AAL2	(1 << 4)	/* G.726 AAL2 */
#define AST_FORMAT_ADPCM	(1 << 5)	/* ADPCM (whose?) */
#define AST_FORMAT_SLINEAR	(1 << 6)	/* Raw 16-bit Signed Linear (8000 Hz) PCM */
#define AST_FORMAT_LPC10	(1 << 7)	/* LPC10, 180 samples/frame */
#define AST_FORMAT_G729A	(1 << 8)	/* G.729a Audio */
#define AST_FORMAT_SPEEX	(1 << 9)	/* Speex Audio, variable */
#define AST_FORMAT_ILBC		(1 << 10)	/* ILBC audio, 50 bytes per 240 samples */
#define AST_FORMAT_G726		(1 << 11)	/* G.726 RFC3551 */
#define AST_FORMAT_G722		(1 << 12)	/* G.722 */

#define AST_FORMAT_JPEG			(1 << 16)	/* JPEG Images */
#define AST_FORMAT_PNG			(1 << 17)	/* PNG Images */
#define AST_FORMAT_H261			(1 << 18)	/* H.261 Video */
#define AST_FORMAT_H263			(1 << 19)	/* H.263 Video */
#define AST_FORMAT_H263P		(1 << 20)	/* H.263+ Video */
#define AST_FORMAT_H264			(1 << 21)	/* H.264 Video */

/* Control frame types */
#define AST_CONTROL_HANGUP			0x01	/* Other end has hungup */
#define AST_CONTROL_RESERVED0		0x02	/* Reserved for future use */
#define AST_CONTROL_RINGING 		0x03	/* Remote end is ringing */
#define AST_CONTROL_ANSWER			0x04	/* Remote end has answered */
#define AST_CONTROL_BUSY			0x05	/* Remote end is busy */
#define AST_CONTROL_RESERVED1		0x06	/* Reserved for future use */
#define AST_CONTROL_RESERVED2		0x07	/* Reserved for future use */
#define AST_CONTROL_CONGESTION		0x08	/* Congestion (circuits busy) */
#define AST_CONTROL_FLASH			0x09	/* Flash hook */
#define AST_CONTROL_RESERVED		0x0a	/* Reserved for future use */
#define AST_CONTROL_OPTION			0x0b	/* Set an option */
#define AST_CONTROL_KEYRADIO		0x0c	/* Key Radio */
#define AST_CONTROL_UNKEYRADIO		0x0d	/* Unkey Radio */
#define AST_CONTROL_CALLPROGRESS	0x0e	/* Call is in progress */
#define AST_CONTROL_CALLPROCEEDING	0x0f	/* Call is proceeding */
#define AST_CONTROL_HOLD			0x10	/* Call is placed on hold */
#define AST_CONTROL_UNHOLD			0x11	/* Call is taken off hold */

#define IAX_CONTROL_NUMBER	17
/* Max version of IAX protocol we support */
#define IAX_PROTO_VERSION 2

/* Subclass for AST_FRAME_IAX */
#define IAX_COMMAND_NEW		0x01
#define IAX_COMMAND_PING	0x02
#define IAX_COMMAND_PONG	0x03
#define IAX_COMMAND_ACK		0x04
#define IAX_COMMAND_HANGUP	0x05
#define IAX_COMMAND_REJECT	0x06
#define IAX_COMMAND_ACCEPT	0x07
#define IAX_COMMAND_AUTHREQ	0x08
#define IAX_COMMAND_AUTHREP	0x09
#define IAX_COMMAND_INVAL	0x0a
#define IAX_COMMAND_LAGRQ	0x0b
#define IAX_COMMAND_LAGRP	0x0c
#define IAX_COMMAND_REGREQ	0x0d	/* Registration request */
#define IAX_COMMAND_REGAUTH	0x0e	/* Registration authentication required */
#define IAX_COMMAND_REGACK	0x0f	/* Registration accepted */
#define IAX_COMMAND_REGREJ	0x10	/* Registration rejected */
#define IAX_COMMAND_REGREL	0x11	/* Force release of registration */
#define IAX_COMMAND_VNAK	0x12	/* If we receive voice before valid first voice frame, send this */
#define IAX_COMMAND_DPREQ	0x13	/* Request status of a dialplan entry */
#define IAX_COMMAND_DPREP	0x14	/* Request status of a dialplan entry */
#define IAX_COMMAND_DIAL	0x15	/* Request a dial on channel brought up TBD */
#define IAX_COMMAND_TXREQ	0x16	/* Transfer Request */
#define IAX_COMMAND_TXCNT	0x17	/* Transfer Connect */
#define IAX_COMMAND_TXACC	0x18	/* Transfer Accepted */
#define IAX_COMMAND_TXREADY	0x19	/* Transfer ready */
#define IAX_COMMAND_TXREL	0x1a	/* Transfer release */
#define IAX_COMMAND_TXREJ	0x1b	/* Transfer reject */
#define IAX_COMMAND_QUELCH	0x1c	/* Stop audio/video transmission */
#define IAX_COMMAND_UNQUELCH 	0x1d	/* Resume audio/video transmission */
#define IAX_COMMAND_POKE    	0x1e  	/* Like ping, but does not require an open connection */
#define IAX_COMMAND_RESERVED0	0x1f	/* Reserved for future use */
#define IAX_COMMAND_MWI			0x20	/* Stand-alone message waiting indicator */
#define IAX_COMMAND_UNSUPPORT	0x21	/* Unsupported message received */
#define IAX_COMMAND_TRANSFER	0x22	/* Request remote transfer */
#define IAX_COMMAND_PROVISION	0x23	/* Provision device */
#define IAX_COMMAND_FWDOWNL		0x24	/* Download firmware */
#define IAX_COMMAND_FWDATA		0x25	/* Firmware Data */

#define IAX_COMMAND_NUMBER		37

/* IAX Information elements */
#define IAX_IE_CALLED_NUMBER		0x01	/* Number/extension being called - string */
#define IAX_IE_CALLING_NUMBER		0x02	/* Calling number - string */
#define IAX_IE_CALLING_ANI			0x03	/* Calling number ANI for billing  - string */
#define IAX_IE_CALLING_NAME			0x04	/* Name of caller - string */
#define IAX_IE_CALLED_CONTEXT		0x05	/* Context for number - string */
#define IAX_IE_USERNAME				0x06	/* Username (peer or user) for authentication - string */
#define IAX_IE_PASSWORD				0x07	/* Password for authentication - string */
#define IAX_IE_CAPABILITY			0x08	/* Actual codec capability - unsigned int */
#define IAX_IE_FORMAT				0x09	/* Desired codec format - unsigned int */
#define IAX_IE_LANGUAGE				0x0a	/* Desired language - string */
#define IAX_IE_VERSION				0x0b	/* Protocol version - short */
#define IAX_IE_ADSICPE				0x0c	/* CPE ADSI capability - short */
#define IAX_IE_DNID					0x0d	/* Originally dialed DNID - string */
#define IAX_IE_AUTHMETHODS			0x0e	/* Authentication method(s) - short */
#define IAX_IE_CHALLENGE			0x0f	/* Challenge data for MD5/RSA - string */
#define IAX_IE_MD5_RESULT			0x10	/* MD5 challenge result - string */
#define IAX_IE_RSA_RESULT			0x11	/* RSA challenge result - string */
#define IAX_IE_APPARENT_ADDR		0x12	/* Apparent address of peer - struct sockaddr_in */
#define IAX_IE_REFRESH				0x13	/* When to refresh registration - short */
#define IAX_IE_DPSTATUS				0x14	/* Dialplan status - short */
#define IAX_IE_CALLNO				0x15	/* Call number of peer - short */
#define IAX_IE_CAUSE				0x16	/* Cause - string */
#define IAX_IE_IAX_UNKNOWN			0x17	/* Unknown IAX command - byte */
#define IAX_IE_MSGCOUNT				0x18	/* How many messages waiting - short */
#define IAX_IE_AUTOANSWER			0x19	/* Request auto-answering -- none */
#define IAX_IE_MUSICONHOLD			0x1a	/* Request musiconhold with QUELCH -- none or string */
#define IAX_IE_TRANSFERID			0x1b	/* Transfer Request Identifier -- int */
#define IAX_IE_RDNIS				0x1c	/* Referring DNIS -- string */
#define IAX_IE_PROVISIONING			0x1d	/* Provisioning info */
#define IAX_IE_AESPROVISIONING		0x1e	/* AES Provisioning info */
#define IAX_IE_DATETIME				0x1f	/* Date/Time */
#define IAX_IE_DEVICETYPE			0x20	/* Device Type -- string */
#define IAX_IE_SERVICEIDENT			0x21	/* Service Identifier -- string */
#define IAX_IE_FIRMWAREVER			0x22	/* Firmware revision -- u16 */
#define IAX_IE_FWBLOCKDESC			0x23	/* Firmware block description -- u32 */
#define IAX_IE_FWBLOCKDATA			0x24	/* Firmware block of data -- raw */
#define IAX_IE_PROVVER				0x25	/* Provisioning Version (u32) */
#define IAX_IE_CALLINGPRES			0x26	/* Calling presentation  */
#define IAX_IE_CALLINGTON			0x27	/* Calling type of number */
#define IAX_IE_CALLINGTNS			0x28	/* Calling transit network select */
#define IAX_IE_SAMPLINGRATE			0x29	/* Supported sampling rates */
#define IAX_IE_CAUSECODE			0x2a	/* Hangup cause */
#define IAX_IE_ENCRYPTION			0x2b	/* Encryption format */
#define IAX_IE_ENCKEY				0x2c	/* 128-bit AES encryption key */
#define IAX_IE_CODECPREFS			0x2d	/* CODEC Negotiation */
#define IAX_IE_RRJITTER				0x2e	/* Received jitter, as in rfc1889 */
#define IAX_IE_RRLOSS				0x2f	/* Received loss, as in rfc1889 */
#define IAX_IE_RRPKTS				0x30	/* total number of frames received on a call, per rfc1889 */
#define IAX_IE_RRDELAY				0x31	/* Max playout delay for received frames in ms */
#define IAX_IE_RRDROPPED			0x32	/* Dropped frames (presumably by jitterbuffer) */
#define IAX_IE_RROOO				0x33	/* Frames received Out of Order */
#define IAX_IE_NUM					0x34

// IAX cause code
#define IAX_CAUSE_NORMAL_CLEARING	16
#define IAX_CAUSE_USER_BUSY			17
#define IAX_CAUSE_NO_USER_RESPONSE	18
#define IAX_CAUSE_NO_ANSWER			19
#define IAX_CAUSE_CALL_REJECTED		21
#define IAX_CAUSE_INCOMPATIBEL		88

#define IAX_AUTH_PLAINTEXT			(1 << 0)
#define IAX_AUTH_MD5				(1 << 1)
#define IAX_AUTH_RSA				(1 << 2)

#define IAX_DPSTATUS_EXISTS			(1 << 0)
#define IAX_DPSTATUS_CANEXIST		(1 << 1)
#define IAX_DPSTATUS_NONEXISTANT	(1 << 2)
#define IAX_DPSTATUS_IGNOREPAT		(1 << 14)
#define IAX_DPSTATUS_MATCHMORE		(1 << 15)

/* Full frames are always delivered reliably */
#define IAX_FULLFR_SCALLNO	0		/* Source call number -- high bit must be 1 */
#define IAX_FULLFR_DCALLNO	2		/* Destination call number -- high bit is 1 if retransmission */
#define IAX_FULLFR_TSTAMP	4		/* 32-bit timestamp in milliseconds (from 1st transmission) */
#define IAX_FULLFR_OSEQNO	8		/* Packet number (outgoing) */
#define IAX_FULLFR_ISEQNO	9		/* Packet number (next incoming expected) */
#define IAX_FULLFR_TYPE		10		/* Frame type */
#define IAX_FULLFR_CSUB		11		/* Compressed subclass */
#define IAX_FULLFR_DATA		12


/* Mini header is used only for voice frames -- delivered unreliably */
/* Frametype implicitly VOICE_FRAME */
/* subclass implicit from last ast_iax2_full_hdr */
#define IAX_MINIFR_SCALLNO	0	/* Source call number -- high bit must be 0, rest must be non-zero */
#define IAX_MINIFR_TSTAMP	2	/* 16-bit Timestamp (high 16 bits from last ast_iax2_full_hdr) */
#define IAX_MINIFR_DATA		4

#define IAX_METAFR_ZEROS	0	/* Zeros field -- must be zero */
#define IAX_METAFR_METACMD	2	/* Meta Command */
#define IAX_METAFR_CMDDATA	3	/* Command Data */
#define IAX_METAFR_TSTAMP	4	/* time-stamp */
#define IAX_METAFR_DATA		8

#define IAX_FLAG_METAVIDEO	0x80

#define IAX_FLAG_FULL		0x8000
#define IAX_FLAG_RETRANS	0x8000
#define IAX_FLAG_SC_LOG		0x80
#define IAX_MAX_SHIFT		0x1F

#define IAX_DEFAULT_REG_EXPIRE  60	/* By default require re-registration once per minute */
#define IAX_MAX_RETRY		5
#define IAX_UDP_TOS			0x50	// Precedence 010, low-delay

// Transfer state
#define TRANSFER_NONE		0
#define TRANSFER_BEGIN		1
#define TRANSFER_READY		2

#define IAX_XFID_LEN		4

// IAX2 structures
typedef struct iax2_frame_cb * IAX2_FCB_HANDLE;
typedef struct iax2_session_cb * IAX2_SCB_HANDLE;
typedef struct iax2_line_cb * IAX2_LCB_HANDLE;

// Frame to hold packets that need to be retransmitted
#define FCB_TSIZE			8	
#define FR_RETRY_TIMEOUT	3
#define FR_MAX_RETRY		3

// Frame state
#define FRS_FREE	0
#define FRS_OPEN	1

// Frame Control Block
struct iax2_frame_cb
{
	UCHAR iState;			// frame state
	UCHAR iTimer;			// timer to do retransmission
	UCHAR iRetry;			// retransmission count
	IAX2_SCB_HANDLE pScb;	// Session control block associated with this frame
	USHORT sDataLen;		// length of this frame
	PCHAR pData;			// content of this frame
};

#define FCB_LENGTH	(sizeof(struct iax2_frame_cb))

// Session status
#define SSS_FREE		0	// session is free
#define SSS_OPEN		1	// session is in use
#define SSS_FINALACK	2	// session is waiting for final ack
#define SSS_TIMEWAIT	3	// session is waiting to be closed

#define SCB_TSIZE		8	// Number of sessions, the first session is for registration
#define SS_WAIT_TIME	8	// Time to wait before the session is destroyed

// Session Control Block
struct iax2_session_cb
{
	UCHAR iState;		// state of this session
	UCHAR pDstIP[IP_ALEN];		// Destination's IP address
	USHORT sDstPort;			// Destination's port
	UCHAR iTimer;		// Timer to destroy a session
	UCHAR iOSeqNo;		// Our next outgoing sequence number
	UCHAR iRSeqNo;		// Next sequence number that have not yet acknowledged
	UCHAR iISeqNo;		// Our last received incoming sequence number
	USHORT sSrcCallNo;	// Our call number
	USHORT sDstCallNo;	// Remote party's call number
	ULONG lLastSent;	// Last transmitted timestamp
	ULONG lOffset;		// Time value that we base our transmission on

	UCHAR iXferState;			// Transfer state
	UCHAR pXferDstIP[IP_ALEN];	// Transfer destination IP address
	USHORT sXferDstPort;		// Transfer destination port
	UCHAR pXferID[IAX_XFID_LEN];	// Transfer ID
	USHORT sXferCallNo;			// Transfer call no

	IAX2_LCB_HANDLE pLcb;	// Line control block related with this session
};

#define SCB_LENGTH	(sizeof(struct iax2_session_cb))

#define LCB_TSIZE		2	// Number of lines supported

struct iax2_line_cb
{
	UCHAR iState;			// State of this line
	UCHAR iVoiceCoder;		// voice coder used by this line
	BOOLEAN bVad;
	BOOLEAN bCallee;		// Caller or callee
	BOOLEAN bQuelch;		// Is voice quelched (hold)?
	UCHAR iTimer;			// No answer timer, digit map timer
	BOOLEAN bLocalRb;		// Play local ringback tone

	UCHAR pPeerName[MAX_USER_NAME_LEN];
	UCHAR pPeerNumber[MAX_USER_NUMBER_LEN];
	CALL_TIME ct;
	UCHAR pKeyOut[MAX_USER_NUMBER_LEN];
	UCHAR iKeyIndex;
	BOOLEAN bSendKey;
	USHORT sSendKeyLen;
	BOOLEAN bMemCall;
	BOOLEAN bXferor;

	IAX2_SCB_HANDLE pScb;	// Session control block of this line
};

#define LCB_LENGTH	(sizeof(struct iax2_line_cb))

// sockaddr structure component
#define ADDR_FAMILY_INET	0x0200
#define SOCKADDR_TYPE		0
#define SOCKADDR_PORT		2
#define SOCKADDR_IP			4
#define SOCKADDR_PADDING	8
#define SOCKADDR_DATA		16

// Registration status
#define RS_FREE				0	
#define RS_REGISTERING		1
#define RS_REGISTERED		2
#define RS_REFRESHING		3

void Iax2LogonStep2(PCHAR pServerIP);
void Iax2CallStep2(PCHAR pServerIP);
void Iax2Run(UDP_SOCKET pUcb);

void iax2_ss_to(IAX2_SCB_HANDLE pScb, UCHAR iType, UCHAR iSubClass);
void iax2_send_noie(UCHAR iCommand);
void iax2_send_hangup(UCHAR iCommand, UCHAR iCauseCode);
void iax2_answer();


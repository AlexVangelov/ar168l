// Used internally by tcp.c and bug3.c

/* Field offset of an TCP packet (following the IP header) */
#define TCP_SPORT	0	/* source port */
#define TCP_DPORT	2	/* destination port */
#define TCP_SEQ		4	/* sequence */
#define TCP_ACK		8	/* acknowledged sequence */
#define TCP_OFFSET	12	
#define TCP_CODE	13	/* control flags */
#define TCP_WINDOW	14	/* window advertisement */
#define TCP_CKSUM	16	/* check sum */
#define TCP_URGPTR	18	/* Urgent pointer */
#define TCP_DATA	20	/* data begin, minimum TCP header length */

/* TCP states */
#define TCPS_CLOSED			0
#define TCPS_LISTEN			1
#define TCPS_SYNSENT		2
#define TCPS_SYNRCVD		3
#define TCPS_ESTABLISHED	4
#define TCPS_CLOSEWAIT		5
#define TCPS_FINWAIT1		6
#define TCPS_FINWAIT2		7
#define TCPS_LASTACK		8
#define TCPS_CLOSING		9

/* TCP control Bits */
#define TCPF_URG	0x20	/* urgent pointer is valid */
#define TCPF_ACK	0x10	/* acknowledged field is valid */
#define TCPF_PSH	0x08	/* this segment requested a push */
#define TCPF_RST	0x04	/* reset the connection */
#define TCPF_SYN	0x02	/* synchronize sequence number */
#define TCPF_FIN	0x01	/* sender has reached end of its stream */

#define TCPHOFFSET		0x50	/* tcp_offset value for TCPMHLEN */
#define TCP_SYSERROR	0xff

/* implementation parameters */
#define TCP_TWOMSL			120		/* 2 minutes (2 * Max Segment Lifetime) */
#define TCP_MAXRXT			20		/* 20 seconds max rexmt time */
#define TCP_MINRXT			2		/* 2 second min rexmt time */
#define TCP_ACKDELAY		1		/* 2 second ACK delay */
#define TCP_MAXPRS			60		/* 1 minute max persist time */

/* TCP options */
#define TPO_EOOL    0   /* end of option list   */
#define TPO_NOOP    1   /* no operation         */
#define TPO_MSS     2   /* maximum segment size */

/* Max tcp data */
#define TCP_RWINDOW_SIZE	1460
#define TCP_SND_MSS			536
#define TCP_SNDBUF_SIZE		(TCP_SND_MSS << 1)

#define TCP_MAX_RETRY		5

/* maximum segment size, 1460 bytes */
#define TCP_MSS_HIGH    0x05
#define TCP_MSS_LOW     0xb4

/* Tcp sequence helper functions */
#define SEQ_LEN		4

#define SEQ_EQUAL	0
#define SEQ_PLUS	1
#define SEQ_MINUS	2

/* Transmission Control Block formats and constants */
#define TCB_TSIZE		3

/* tcb flags */
#define TCBF_NEEDOUT        0x01    /* we need output                   */	
#define TCBF_FIRSTSEND      0x02    /* no data to ACK                   */
#define TCBF_RDONE          0x04    /* no more receive data to process  */
#define TCBF_SDONE          0x08    /* no more send data allowed        */
#define TCBF_SNDFIN			0x10	/* user process has closed;send a FIN */
#define TCBF_GOTFIN			0x20	/* no more to receive				*/
#define TCBF_PUSH			0x40	/* got a push; deliver what we have */

// TCP control block structure
struct tcb 
{
	UCHAR iState;					/* TCP state				*/
	UCHAR iEvent;					/* TCP event				*/
	UCHAR iCode;					/* TCP code for next packet */
	UCHAR iFlags;					/* various TCB state flags  */
	UCHAR iBankOffset;				/* Bank offset of call back function	*/
	USHORT sCallBack;				/* Address of call back function		*/
	USHORT sLocalPort;				/* local TCP port			*/
	USHORT sRemotePort;				/* remote TCP port			*/
	UCHAR pRemoteIP[IP_ALEN];		/* remote IP address		*/

	UCHAR iTimer;					/* retransmit timer			*/
	UCHAR iRetry;					/* number of retransmit	sent	*/
	UCHAR iUserTimer;				/* User setable timer		*/
	UCHAR iUserTimeout;				/* User setable timeout value	*/
//	UCHAR iTimewait;				/* Time to wait before free this socket */

	ULONG seq_suna;					/* send unacked				*/
	ULONG seq_snext;				/* send next				*/
	ULONG seq_slast;				/* sequence of FIN, if TCBF_SNDFIN	*/
	ULONG seq_rnext;				/* receive next				*/

	USHORT sSbStart;				/* start of valid data		*/
	USHORT sSbCount;				/* data characer ount		*/
	PCHAR pSndBuf;					/* send buffer				*/
};

#define TCB_LENGTH		(sizeof(struct tcb))		/* length of TCB			*/

void tcp_send(TCP_HANDLE pTcb, BOOLEAN bRexmit);

USHORT min(USHORT sData1, USHORT sData2);
USHORT _TcpCheckSum(USHORT sLen, PCHAR pData, PCHAR pSrcIP, PCHAR pDstIP);

void tcp_data(TCP_HANDLE pTcb);
void tcp_synsent(TCP_HANDLE pTcb);
void tcp_reset();
void tcp_ackit(TCP_HANDLE pTcb);
UCHAR tcp_acked(TCP_HANDLE pTcb);
void tcp_callback(TCP_HANDLE pTcb);
void tcp_lastack(TCP_HANDLE pTcb);
void tcp_closewait(TCP_HANDLE pTcb);
void tcp_closing(TCP_HANDLE pTcb);
void tcp_fin2(TCP_HANDLE pTcb);
void tcp_fin1(TCP_HANDLE pTcb);
void tcp_established(TCP_HANDLE pTcb);
void tcp_synrcvd(TCP_HANDLE pTcb);
void tcp_listen(TCP_HANDLE pTcb);
BOOLEAN tcp_ok(TCP_HANDLE pTcb);


extern struct tcb _pTcbData[TCB_TSIZE];

extern BOOLEAN _bReceiveAckFlag;        // acknowledged field is valid 
extern BOOLEAN _bReceiveRstFlag;        // reset the connection 
extern BOOLEAN _bReceiveSynFlag;        // synchronize sequence number 
extern BOOLEAN _bReceiveFinFlag;        // sender has reached end of its stream 

extern PCHAR _pReceive;		// received TCP packet including TCP head
extern UCHAR _pReceiveIP[IP_ALEN];		// remote IP of receive TCP packet
extern PCHAR _pReceiveData;	// received TCP data
extern USHORT _sReceiveDataLen;




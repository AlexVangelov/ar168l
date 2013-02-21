// Arp functions
void ArpInit();
void ArpRun(PCHAR pRecv);
void ArpTimer();
void ArpSendData(USHORT sLen, PCHAR pIP);

// DHCP functions, dhcp.c
void DhcpInit();
void DhcpDiscover();
void DhcpDecline();
void DhcpTimer();

void DhcpdInit();

// Dns functions, dns.c
typedef struct _DNSSRV_ANS
{
	char pcTarget[MAX_DNS_LEN];
	USHORT sPort;
	USHORT sPriority;		//fv
} T_DNSSRV_ANS;
#define PT_DNSSRV_ANS	T_DNSSRV_ANS *

void DnsInit();
void DnsQuery(PCHAR pDomain, UCHAR iType, UCHAR iBankOffset, USHORT sCallBackAddr);
void DnsTimer();

// Icmp functions, icmp.c
void IcmpRun(PACKET_LIST * p);
void SendEchoReq(PCHAR pDstIP);

// Ip functions, ip.c
void IpInit();
void IpSendData(USHORT sLen, PCHAR pDstIP, UCHAR iProtocol);

// net.c
void NetInit();
void NetDiscover();
void NetTimer();

UCHAR ReadPacket(UCHAR iCurPacket, BOOLEAN bMini);	// RTL8019AS
void HandlePacket();								// KSZ8842
void FreePacketList();

// PPP functions, ppp.c
//PPP Header
#define PPP_CODE	0
#define PPP_ID		1
#define PPP_LEN		2
#define PPP_DATA	4

void PppInit();
void PppTimer();
void LcpCfgReq();

// PPPoE functions, pppoe.c
/* PPP Protocol */
#define PPP_PROT_LCP		0xc021
#define PPP_PROT_PAP		0xc023
#define PPP_PROT_CHAP		0xc223
#define PPP_PROT_IPCP		0x8021
#define PPP_PROT_IP			0x0021

#define PPPOE_VER			0
#define PPPOE_CODE			1
#define PPPOE_SESSION_ID	2
#define PPPOE_LENGTH		4
#define PPPOE_DATA			6
#define PPPOE_PROTOCOL_ID	6

extern UCHAR PPPoE_pSendHead[PPPOE_INFO];

void PppoeInit();
void PppoeDiscover();
void PppoeSendData(USHORT sLen);
void PppoePadt();
void PppoeTerminated();
void PppoeTimer();
void PppoeInitRun(PCHAR pHead, PCHAR pSrcMac, USHORT sDataLen);
BOOLEAN PppoeCheckHead(PCHAR pPppoeHead, PCHAR pSrcMac);
void PppoeSessionRun(PCHAR pPppoeHead);

void PppSendMisc(USHORT sProtocol, USHORT sLen, UCHAR iCode, UCHAR iId);

// SNTP functions, sntp.c
void SntpInit();
void SntpStart(BOOLEAN bStartup);
void SntpTimer();

// Tftp functions, tftp.c
void TftpInit();
void TftpTimer();

// stun.c
void StunInit();
void StunMap(UCHAR iTask, UDP_SOCKET pSocket);
void StunTimer();
BOOLEAN StunRun(UDP_SOCKET pUcb);

// udprun.c
void UdpRun(PACKET_LIST * p);
BOOLEAN UdpMiniRun(PACKET_LIST * p);	// RTL8019AS only
void UdpInit();



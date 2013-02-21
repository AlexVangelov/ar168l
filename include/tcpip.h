// TCP defines
#define TCP_EVENT_USERTO	0x01
#define TCP_EVENT_RETRYTO	0x02
#define TCP_EVENT_ABORT		0x04
#define TCP_EVENT_RESET		0x08	// received RST flag
#define TCP_EVENT_CLOSE		0x10	// received FIN flag (peer close)
#define TCP_EVENT_ACCEPT	0x20	// received SYN flag while listening(for passive open)
#define TCP_EVENT_CONNECT	0x40	// connection established (for both active or passive open)
#define TCP_EVENT_DATA		0x80	// incoiming data event

#define TCPIP_RESERVED_PORT	1024

// End TCP defines

/* APR packet length */
#define ARP_LENGTH      (8 + HW_ALEN + IP_ALEN + HW_ALEN + IP_ALEN)

/* ARP packet type is 0x0806 */
#define EPT_ARP			0x0806

/* IP packet type is 0x0800 */
#define EPT_IP			0x0800

/* 802.1q VLAN packet type is 0x8100 */
#define EPT_VLAN		0x8100

/* 802.1q header fields */
#define VLAN_TPID			0
#define VLAN_TCI			2
#define VLAN_HEADER_SIZE	4
#define VLAN_ID_MASK		0x0fff

/* PPPoE packet type */
#define EPT_PPPOE_INIT	0x8863
#define EPT_PPPOE		0x8864

#define PPPOE_INIT_INFO		6
#define PPPOE_INFO			8

/* Internet Protocol (IP) constants */

/* Some assigned protocol numbers */
#define IPT_ICMP    1   /* protocol type for ICMP packets */
#define IPT_IGMP    2   /* protocol type for IGMP packets */
#define IPT_TCP     6   /* protocol type for TCP packets */
#define IPT_EGP     8   /* protocol type for EGP packets */
#define IPT_UDP     17  /* protocol type for UDP packets */
#define IPT_RSVP    46  /* protocol type for RSVP packets */
#define IPT_OSPF    89  /* protocol type for OSPF packets */

/* Field offset in an IP packet */
#define EP_DST  0
#define EP_SRC  HW_ALEN
#define EP_TYPE (HW_ALEN + HW_ALEN)
#define EP_DATA (EP_TYPE + 2)     

/* Field offeset in an IP packet */
#define IP_VERLEN   0   /* IP version & header length (in longs) */
#define IP_TOS      1   /* type of service */
#define IP_LEN      2   /* total packet length (in octets) */	
#define IP_ID       4   /* datagram id */
#define IP_FRAGOFF  6   /* fragment offset (in 8-octet's) */
#define IP_TTL      8   /* time to live, in gateway hops */
#define IP_PROTO    9   /* IP protocol */
#define IP_CKSUM    10  /* header checksum */
#define IP_SRC      12  /* IP address of source */
#define IP_DST      16  /* IP address of destination */
#define IP_DATA     20  /* variable length data, minimum IP header length */

#define IP_MF		0x2000	/* more fragments bit */
#define IP_DF		0x4000	/* don't fragment bit */
#define IP_FRAGOFF_MASK		0x1fff	/* fragment offset mask */

/* IP precedence values */
#define IPP_NETCTL  0xe0    /* network control */
#define IPP_INCTL   0xc0    /* internet control */
#define IPP_CRIT    0xa0    /* critical */
#define IPP_FLASHO  0x80    /* flash over-ride */
#define IPP_FLASH   0x60    /* flash */
#define IPP_IMMED   0x40    /* immediate */
#define IPP_PRIO    0x20    /* priority */
#define IPP_NORMAL  0x00    /* normal */

/* Field offset of an UDP packet (following the IP header) */
#define UDP_SRC		0	/* source UDP port number */
#define UDP_DST		2	/* destination UDP port number */
#define UDP_LEN		4	/* length of UDP data */
#define UDP_CKSUM	6	/* UDP checksum (0 => none) */
#define UDP_DATA	8


// DNS type
#define DNS_TYPE_A		1	/* a host address */
#define DNS_TYPE_SRV	33	/* SRV record */

typedef struct dns_cb * DNS_HANDLE;

// The max length of PPPOE data packet, except enter in IP mode
#define PPPOE_SEND_LEN	64

// Hdlc head format
#define HDLC_ADDR		0
#define HDLC_CTRL		1
#define HDLC_PROT		2
#define HDLC_INFO		4
#define HDLC_FCS_LEN	2

#define FLAG_VALUE		0x7e
#define ADDR_VALUE		0xff
#define CTRL_VALUE		0x03

// Number of bytes allowed in a send (min)
#define ETHERNET_MIN_SIZE             60

// Number of bytes allowed in a receive (max)
#define ETHERNET_MAX_SIZE				1514

#define ETHERNET_CRC_SIZE				4

#define ETHERNET_MAX_READ_PACKET		5	// Number of max packet to read in a loop

#define IP_DATA_MAX_SIZE			(ETHERNET_MAX_SIZE - EP_DATA - IP_DATA)

// Global var
extern UCHAR Adapter_pPacketBuf[ETHERNET_MAX_SIZE + 1 + IP_DATA_MAX_SIZE + 1];
extern BOOLEAN Dhcp_bRenewing;

#define Adapter_pReceivePacket	(PCHAR)(Adapter_pPacketBuf + ETHERNET_MAX_SIZE + 1)
#define Adapter_pIpBuf			(PCHAR)(Adapter_pPacketBuf + EP_DATA + IP_DATA)

#define UDP_EXTRA				(EP_DATA + IP_DATA + UDP_DATA)
#define Adapter_pUdpBuf			(PCHAR)(Adapter_pPacketBuf + UDP_EXTRA)

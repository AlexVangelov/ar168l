// Type defines
#define UCHAR	unsigned char
#define USHORT	unsigned short
#define ULONG	unsigned long
//#define BOOLEAN	unsigned char
#define BOOLEAN	_Bool

#define PCHAR	unsigned char *
#define PSHORT	unsigned short *
#define PLONG	unsigned long *

#ifndef NULL
#define NULL	(void *)0
#endif

#define TRUE	1
#define FALSE	0

// Time structure
typedef struct _time_t
{
	UCHAR iSec;
	UCHAR iMin;
	UCHAR iHour;
	UCHAR iMday;
	UCHAR iMon;
	USHORT sYear;
	UCHAR iWday;
} TIME_T;

typedef struct _CALL_TIME
{
	UCHAR iSec;
	UCHAR iMin;
	UCHAR iHour;
} CALL_TIME;

#define HW_ALEN					6		// MAC address on ethernet length is 6 
#define IP_ALEN					4		// IP address length is 4 

typedef struct _PACKET_LIST PACKET_LIST;
struct _PACKET_LIST
{
	PACKET_LIST *	next;
	USHORT			sLen;
	USHORT			sAddr;
	UCHAR			pRecvIP[IP_ALEN];
};

typedef struct tcb * TCP_HANDLE;
typedef struct ucb * UDP_SOCKET;


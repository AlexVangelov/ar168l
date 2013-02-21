// UdpSocket.cpp: implementation of the CUdpSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "global.h"
#include "UdpSocket.h"

//#include "TcpSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int CUdpSocket::m_iTTL = 10;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUdpSocket::CUdpSocket()
{
}

CUdpSocket::~CUdpSocket()
{
}

BOOL CUdpSocket::Enter(HWND hWnd, UINT uMsg, ULONG uPeerIP, int iPort, int iDataSize, int iQueueSize)
{
	DWORD dwThreadId;

	m_hWnd = hWnd;
	m_uMsg = uMsg;
	m_uPeerIP = uPeerIP;
	m_uLocalIP = GetLocalIP();
	m_iPort = iPort;
	m_iDataSize = iDataSize;
	m_iQueueSize = iQueueSize;

	if (Prepare() == FALSE)
	{
		goto Error;
	}

	m_hIOThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, this, 0, &dwThreadId);
	if (m_hIOThread == NULL) 
	{
		OutputDebugString(_T("CreateThread() failed in CUdpSocket::Enter()\n"));
		goto Error;
	}
	SetThreadPriority(m_hIOThread, THREAD_PRIORITY_HIGHEST);
//	OutputDebugPrintf("Udp port: %x\n", m_iPort);

	return TRUE;

Error:
	Leave();
	return FALSE;
}

void CUdpSocket::Leave()
{
	if (m_hIOThread != NULL)
	{
		// Signal the event to close the worker threads.
		SetEvent(m_hQuitEvent);
		if (WaitForSingleObject(m_hIOThread, 3000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hIOThread, 0);
			CleanUp();
		}
		m_hIOThread = NULL;
	}
	Unprepare();
}

BOOL CUdpSocket::Write(char * pData, int iLength)
{
	CSingleLock sLock(&m_critical);
	WSABUF * p = (WSABUF *)malloc(sizeof(WSABUF));

	if (p == NULL)
	{
		OutputDebugString(_T("malloc failed in CUdpSocket::Write\n"));
		return FALSE;
	}
	p->buf = pData;
	p->len = iLength;
	sLock.Lock();
	m_sendQueue.AddTail(p);
	sLock.Unlock();
	SetEvent(m_hOutputEvent);
	return TRUE;
}

BOOL CUdpSocket::WriteList(CList<WSABUF *, WSABUF *> & list)
{
	CSingleLock sLock(&m_critical);
	WSABUF * p;
	sLock.Lock();
	while (!list.IsEmpty())
	{
		p = list.RemoveHead();
		m_sendQueue.AddTail(p);
	}
	sLock.Unlock();
	SetEvent(m_hOutputEvent);
	return TRUE;
}

BOOL CUdpSocket::Busy()
{
	return (m_sendQueue.GetCount() > m_iQueueSize);
}

BOOL CUdpSocket::Prepare()
{
	m_hIOThread = m_hQuitEvent = m_hOutputEvent = m_hNetworkEvent = NULL;
	m_socket = INVALID_SOCKET;
	m_bWriteOk = FALSE;

	m_hOutputEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hNetworkEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hOutputEvent || !m_hQuitEvent || !m_hNetworkEvent)
	{
		OutputDebugString(_T("CreateEvent() failed in CUdpSocket::prepare()\n"));
		goto Error;
	}

	m_eventArray[0] = m_hNetworkEvent;
	m_eventArray[1] = m_hOutputEvent;
	m_eventArray[2] = m_hQuitEvent;

	return TRUE;

Error:
	Unprepare();
	return FALSE;
}

void CUdpSocket::Unprepare()
{
	if (m_hQuitEvent != NULL)
	{
		CloseHandle(m_hQuitEvent);
		m_hQuitEvent = NULL;
	}
	if (m_hNetworkEvent != NULL)
	{
		CloseHandle(m_hNetworkEvent);
		m_hNetworkEvent = NULL;
	}
	if (m_hOutputEvent != NULL)
	{
		CloseHandle(m_hOutputEvent);
		m_hOutputEvent = NULL;
	}
	while (!m_sendQueue.IsEmpty())
	{
		WSABUF * p = m_sendQueue.RemoveHead();
		free(p->buf);
		free(p);
	}
}

void CUdpSocket::SetTTL(int iTTL)
{
	m_iTTL = iTTL;
}

int CUdpSocket::ReadData()
{
	int err, iPeerAddrLen;		// Peer address length
	ULONG uIP;
	SOCKADDR_IN peerAddr;	// Peer address where the data is from
	WSABUF * p = (WSABUF *)malloc(sizeof(WSABUF));

	if (p == NULL)
	{
		OutputDebugString(_T("malloc failed in CUdpSocket::ReadData()\n"));
		return -1;
	}
	p->buf = (char *)malloc(m_iDataSize);
	if (p->buf == NULL)
	{
		free(p);
		OutputDebugString(_T("malloc failed in CUdpSocket::ReadData()\n"));
		return -1;
	}

	iPeerAddrLen = sizeof(SOCKADDR_IN);
	err = recvfrom(m_socket, p->buf, m_iDataSize, 0, (PSOCKADDR)&peerAddr, &iPeerAddrLen);
	if (err > 0)
	{
		uIP = peerAddr.sin_addr.S_un.S_addr;
		if (m_uLocalIP != uIP)
		{
			p->len = err;
			PostMessage(m_hWnd, m_uMsg, (WPARAM)uIP, (LPARAM)p);
			return 0;
		}
		err = 0;
	}
	free(p->buf);
	free(p);
	return err;
}

int CUdpSocket::WriteData()
{
//	m_bWriteOk = TRUE;
	if (!m_bWriteOk)
	{	// Wait for next FD_WRITE
		return 0;
	}

	int err = 0;
	CSingleLock sLock(&m_critical);
	WSABUF * p;

	sLock.Lock();
	while (!m_sendQueue.IsEmpty() && m_bWriteOk)
	{
		p = m_sendQueue.RemoveHead();
		err = sendto(m_socket, p->buf, p->len, 0, (PSOCKADDR)&(m_peerAddr), sizeof(m_peerAddr));
		if (err >= 0)
		{
//			OutputDebugString("Write Out!\n");
			err = 0;
			free(p->buf);
			free(p);
		}
		else
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK) 
			{	//  WSAEWOULDBLOCK means we have to wait for an FD_WRITE before we can send.
				m_bWriteOk = FALSE;
				m_sendQueue.AddHead(p);
				err = 0;
//				OutputDebugString("blocked!\n");
			}
		}
	}
	sLock.Unlock();
	return err;
}

int CUdpSocket::BeginSession()
{
	BOOL bReuseAddress = TRUE;
	BOOL bBroadcast;
	BOOL bMulticast;

	// for multicast
	BOOL bLoopback = FALSE;
	int iMcastTTL = m_iTTL;
	DWORD cbRet;

	bBroadcast = (m_uPeerIP == 0) ? TRUE : FALSE;
	bMulticast = (m_uPeerIP >= 0xe0000000) ? TRUE : FALSE;

	// Allocate UDP socket 
	m_socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, bMulticast ? (WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF) : 0);
	if (m_socket == INVALID_SOCKET)
	{
		return -1;
	}

	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&bReuseAddress, sizeof(bReuseAddress)) != 0) 
	{
		OutputDebugString(_T("Could not set reuse address option\n"));
		return -1;
	}

	if (bBroadcast)
	{
		if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char *)&bBroadcast, sizeof(bBroadcast)) != 0)
		{
			OutputDebugString(_T("Could not set broadcast option\n"));
			return -1;
		}
	}

	// Bind to specified port on the interface of user's choice
	m_peerAddr.sin_family = AF_INET;
	m_peerAddr.sin_port = htons(m_iPort);
	if (bBroadcast)
	{	// broadcast socket
		m_peerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
	else
	{
		m_peerAddr.sin_addr.S_un.S_addr = m_uLocalIP;
	}
	if (bind(m_socket, (PSOCKADDR)&(m_peerAddr), sizeof(m_peerAddr)) != 0)
	{
		int iError;

		OutputDebugString(_T("Could not bind address\n"));
		iError = WSAGetLastError();
		switch (iError)
		{
		case WSANOTINITIALISED:
			OutputDebugString(_T("A successful WSAStartup must occur before using this function. "));
			break;

		case WSAENETDOWN:
			OutputDebugString(_T("The network subsystem has failed. "));
			break;

		case WSAEADDRINUSE:
			OutputDebugString(_T("A process on the machine is already bound to the same fully-qualified address and the socket has not been marked to allow address re-use with SO_REUSEADDR. For example, IP address and port are bound in the af_inet case) . (See the SO_REUSEADDR socket option under setsockopt.) "));
			break;

		case WSAEADDRNOTAVAIL:
			OutputDebugString(_T("The specified address is not a valid address for this machine "));
			break;

		case WSAEFAULT:
			OutputDebugString(_T("The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s. "));
			break;

		case WSAEINPROGRESS:
			OutputDebugString(_T("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. "));
			break;

		case WSAEINVAL:
			OutputDebugString(_T("The socket is already bound to an address. "));
			break;

		case WSAENOBUFS:
			OutputDebugString(_T("Not enough buffers available, too many connections. "));
			break;

		case WSAENOTSOCK:
			OutputDebugString(_T("The descriptor is not a socket. "));
			break;
		}
		return -1;
	}

	if (bMulticast)
	{
		if (WSAIoctl(m_socket, SIO_MULTICAST_SCOPE, &iMcastTTL, sizeof(iMcastTTL), NULL, 0, &cbRet, NULL, NULL) != 0)
		{
			OutputDebugString(_T("Could not set TTL\n"));
			return -1;
		}

		// ½ûÖ¹loopback   
/*		if (WSAIoctl(m_socket, SIO_MULTIPOINT_LOOPBACK, &bLoopback, sizeof(bLoopback), NULL, 0, &cbRet, NULL, NULL) != 0)
		{
			OutputDebugString("Could not set loop back flag\n");
//			return -1;
		}
*/   	
	}

	//Setup address for a udp session
	if (bBroadcast)
	{	// broadcast socket
		m_peerAddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
	}
	else
	{
		m_peerAddr.sin_addr.S_un.S_addr = htonl(m_uPeerIP);
	}

	if (bMulticast)
	{
		if (WSAJoinLeaf(m_socket, (PSOCKADDR)&(m_peerAddr), sizeof(m_peerAddr), NULL, NULL, NULL, NULL, JL_BOTH) == INVALID_SOCKET)
		{
			OutputDebugString(_T("Could not join multicast session\n"));
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case WSANOTINITIALISED:
				OutputDebugString(_T("A successful WSAStartup must occur before using this function. \n"));
				break;

			case WSAENETDOWN:
				OutputDebugString(_T("The network subsystem has failed. \n"));
				break;

			case WSAEADDRINUSE:
				OutputDebugString(_T("The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs at the time of bind, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be \"committed\" at the time of this function. \n"));
				break;

			case WSAEINTR:
				OutputDebugString(_T("A blockingWindows Socket 1.1 call was canceled through WSACancelBlockingCall. \n"));
				break;

			case WSAEINPROGRESS:
				OutputDebugString(_T("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n"));
				break;

			case WSAEALREADY:
				OutputDebugString(_T("A nonblocking WSAJoinLeaf call is in progress on the specified socket. \n"));
				break;

			case WSAEADDRNOTAVAIL:
				OutputDebugString(_T("The remote address is not a valid address (such as ADDR_ANY). \n"));
				break;

			case WSAEAFNOSUPPORT:
				OutputDebugString(_T("Addresses in the specified family cannot be used with this socket. \n"));
				break;

			case WSAECONNREFUSED:
				OutputDebugString(_T("The attempt to join was forcefully rejected. \n"));
				break;

			case WSAEFAULT:
				OutputDebugString(_T("The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the buffer length for lpCalleeData, lpSQOS, and lpGQOS are too small, or the buffer length for lpCallerData is too large. \n"));
				break;

			case WSAEISCONN:
				OutputDebugString(_T("The socket is already member of the multipoint session. \n"));
				break;

			case WSAENETUNREACH:
				OutputDebugString(_T("The network cannot be reached from this host at this time. \n"));
				break;

			case WSAENOBUFS:
				OutputDebugString(_T("No buffer space is available. The socket cannot be joined. \n"));
				break;

			case WSAENOTSOCK:
				OutputDebugString(_T("The descriptor is not a socket. \n"));
				break;

			case WSAEOPNOTSUPP:
				OutputDebugString(_T("The FLOWSPEC structures specified in lpSQOS and lpGQOS cannot be satisfied. \n"));
				break;

			case WSAEPROTONOSUPPORT:
				OutputDebugString(_T("The lpCallerData augment is not supported by the service provider. \n"));
				break;

			case WSAETIMEDOUT:
				OutputDebugString(_T("Attempt to join timed out without establishing a multipoint session \n"));
				break;
			}
			return -1;
		}
	}

	// Put Connection in Event Object Notification Mode.
	WSAEventSelect(m_socket, m_hNetworkEvent, FD_WRITE|FD_READ);

	return 0;
}

BOOL CUdpSocket::HandleNetworkEvent()
{
	WSANETWORKEVENTS networkEvents;		// tells us what events happened
 	int err;

	// Find out what happened and act accordingly.
	err = WSAEnumNetworkEvents(m_socket, m_hNetworkEvent, &networkEvents);
	if (err == SOCKET_ERROR) 
	{	// Handle the fatal error, break out to end.
		OutputDebugString(_T("WSAEnumNetworkEvents failed\n"));
		return FALSE;
	} 

	// The following three if statements all execute unless one gets
	// an error or closed socket, in which case we return immediately.
	if (networkEvents.lNetworkEvents & FD_READ) 
	{	// An FD_READ event has occurred on the connected socket.
		if (networkEvents.iErrorCode[FD_READ_BIT] == WSAENETDOWN) 
		{	// There is an error.
			OutputDebugString(_T("Error in FD_READ\n"));
			return FALSE;
		}
		else 
		{	// Read data off the socket...
			if (ReadData() != 0)
			{
				return FALSE;
			}
		}
	}
	if (networkEvents.lNetworkEvents & FD_WRITE) 
	{	// An FD_WRITE event has occurred on the connected socket.
		if (networkEvents.iErrorCode[FD_WRITE_BIT] == WSAENETDOWN) 
		{	// There is an error.
			OutputDebugString(_T("Error in FD_WRITE\n"));
			return FALSE;
		}
		else 
		{	// Allow to send on this socket, and signal the hOuputEvent in case 
			// there is pending output that is not completed due to WSAEWOULDBLOCK.
			m_bWriteOk = TRUE;
			SetEvent(m_hOutputEvent);
//			OutputDebugString("Set hOutputEvent\n");
		}
	}
	return TRUE;
}

void CUdpSocket::CleanUp()
{
	if (m_socket != INVALID_SOCKET)
	{
		WSAEventSelect(m_socket, m_hNetworkEvent, 0);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	CloseHandle(m_hIOThread);
}

DWORD WINAPI CUdpSocket::ThreadFunc(LPVOID pParam)
{
	DWORD dwWaitStatus;	// holds return value of the wait
	CUdpSocket * p = (CUdpSocket *)pParam;

	if (p->BeginSession() != 0)
	{
		goto EndThread;
	}

	OutputDebugString(_T("UDP IO thread starting\n"));

	// This is the main loop.  Loop until we break out.
	while (TRUE) 
	{
		// Wait for an event 
		dwWaitStatus = WSAWaitForMultipleEvents(3, p->m_eventArray,
												FALSE,        // fWaitAll
												WSA_INFINITE, // dwTimeout
												FALSE);        // fAlertable
		switch (dwWaitStatus) 
		{
		case WSA_WAIT_FAILED:	// A fatal error
			OutputDebugString(_T("WSAWaitForMultipleEvents() failed\n"));
			break;

		case WSA_WAIT_EVENT_0:	// The hNetEvent has been signaled.
			if (p->HandleNetworkEvent())
			{
				continue;
			}
			break;

		case WSA_WAIT_EVENT_0+1:	// The hOutputEvent has been signaled.
			if (p->WriteData() == 0)
			{
				continue;
			}
			break;

		case WSA_WAIT_EVENT_0+2:	// Quit time;
//			OutputDebugString(_T("leaving multicast session\n"));
			break;

		default:
			OutputDebugString(_T("Unknown error in WSAWaitForMultipleEvents()\n"));
			break;
		} // switch (dwWaitStatus)
		// Break out of the while loop.
		break;
	} // while (1)

	// Thats the end.  Now clean up.
EndThread:
	OutputDebugString(_T("UDP IO thread shutting down\n"));
	p->CleanUp();
	return 0;
}

BOOL CUdpSocket::Startup()
{
	int err;
	WORD wVersionRequested;
	WSADATA wsaData;

	// Asynchronous IO and multicast semantics we use supported starting only with WinSock 2.0 
	wVersionRequested = MAKEWORD (2, 2);
    // Start WinSock 2.  If it fails, we don't need to call WSACleanup().
	err = WSAStartup (wVersionRequested, &wsaData);
	if (err != 0)
	{
		OutputDebugString(_T("Could not find high enough version of WinSock\n"));
		return FALSE;
	}

	OutputDebugString(_T("WinSock2 Init\n"));
	return TRUE;
}

void CUdpSocket::Shutdown()
{
	OutputDebugString(_T("WinSock2 Free\n"));
	WSACleanup( );
}

ULONG CUdpSocket::Name2IP(CString strName)
{
	char pName[100];
	ULONG uIP = 0;
	HOSTENT *host;

	WideCharToMultiByte(CP_ACP, 0, strName, -1, pName, 100, NULL, NULL); 

	uIP = inet_addr(pName);
	if (uIP == INADDR_NONE) 
	{
		host = gethostbyname(pName);
		if (host) 
		{
			uIP = *((ULONG *)host->h_addr_list[0]);
		}
	}
	return uIP;
}

CString CUdpSocket::IP2Name(ULONG uIP)
{
	static CString str;
	IN_ADDR dotIP;
	dotIP.S_un.S_addr = uIP;
	str = inet_ntoa(dotIP);
	return str;
}

ULONG CUdpSocket::GetLocalIP()
{
	char szNameBuf[128];
	gethostname(szNameBuf,127);
	HOSTENT * host = gethostbyname(szNameBuf);
	return *((long*)host->h_addr);
}



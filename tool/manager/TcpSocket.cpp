// TcpSocket.cpp: implementation of the CTcpSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TcpSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TCP_BUFFER_LENGTH	1536
#define TCP_DATA_QUEUE_SIZE	128

LPWSAPROTOCOL_INFO CTcpSocket::m_pProtocolsInfo;
LPWSAPROTOCOL_INFO CTcpSocket::m_pTcpInfo;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpSocket::CTcpSocket()
{
	m_socket = INVALID_SOCKET;
	m_hIOThread = NULL;
	m_hNetEvent = m_hOutputEvent = m_hQuitEvent = NULL;
	m_bWriteOk = FALSE;
}

CTcpSocket::~CTcpSocket()
{
	if (m_socket != INVALID_SOCKET)
		Unprepare();
}

//////////////////////////////////////////////////////////////////////
// CTcpSocket public functions 

BOOL CTcpSocket::Call(ULONG uPeerIP, HWND hWnd, int iPort)
{
	SOCKADDR_IN addr;
	int err;

	m_hWnd = hWnd;
	m_iPort = (iPort == 0) ? TCP_DEFAULT_PORT : iPort;

	if (Prepare() == FALSE)	return FALSE;

	// Create a socket for this connection.
	m_socket = WSASocket(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, m_pTcpInfo, 0, 0);
	if (m_socket == INVALID_SOCKET) 
	{
		OutputDebugString(_T("Could not open a socket in Call\n"));
		goto Error;
	} 
	else 
	{	// Set up socket for windows message event notification.
		WSAAsyncSelect(m_socket, m_hWnd, WM_TCP_SOCKET_CONNECT, FD_CONNECT);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_iPort);
	addr.sin_addr.s_addr = uPeerIP;

	err = WSAConnect(m_socket, (SOCKADDR *)&addr, m_pTcpInfo->iMaxSockAddr, NULL, NULL, NULL, NULL);
	if (err == SOCKET_ERROR) 
	{
		err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK) 
		{
			OutputDebugString(_T("WSAConnect failed\n"));
			goto Error;
		}
	} 
	else 
	{
		OutputDebugString(_T("WSAConnect should have returned SOCKET_ERROR\n"));
		goto Error;
	}

	m_uPeerIP = addr.sin_addr.s_addr;
	m_strPeerName = inet_ntoa(addr.sin_addr);
	OutputDebugString(_T("Calling ") + m_strPeerName + _T("\n"));
	return TRUE;

Error:
	Unprepare();
	return FALSE;
}

void CTcpSocket::Hangup()
{
	if (m_hQuitEvent != NULL)		SetEvent(m_hQuitEvent);
}

BOOL CTcpSocket::Write(char * pData, int iLength)
{
	if (m_hOutputEvent == NULL)	return FALSE;

	CSingleLock sLock(&m_critical);
	WSABUF * p = (WSABUF *)malloc(sizeof(WSABUF));
	if (p == NULL)
	{
		OutputDebugString(_T("malloc failed in CTcpSocket::Write\n"));
		return FALSE;
	}
	p->buf = (char *)malloc(TCP_BUFFER_LENGTH);
	if (p->buf == NULL)
	{
		free(p);
		OutputDebugString(_T("malloc failed in CTcpSocket::Write\n"));
		return FALSE;
	}
	memcpy(p->buf, pData, iLength);
	p->len = iLength;
	sLock.Lock();
	m_sendQueue.AddTail(p);
	sLock.Unlock();
	SetEvent(m_hOutputEvent);
//	Sleep(0);
	return TRUE;
}

ULONG CTcpSocket::Accept(SOCKET socketListen, LPARAM lParam, HWND hWnd, int iPort)
{
	SOCKADDR_IN addr;
	int iAddrLen = sizeof(addr);
	int err;
	DWORD dwThreadId;	// needed for CreateThread

	err = WSAGETSELECTERROR(lParam);
	// Check to see if there was an error on the connection attempt.
	if (err) 
	{	// Some kind of error occurred.
		if (err == WSAENETDOWN) 
		{
			OutputDebugString(_T("The network is down\n"));
        } 
		else 
		{
			OutputDebugString(_T("Unknown error on FD_ACCEPT\n"));
		}
		return 0;
	}

	m_hWnd = hWnd;
	m_iPort = (iPort == 0) ? TCP_DEFAULT_PORT : iPort;
	if (Prepare() == FALSE)		return 0;

	m_socket = WSAAccept(socketListen, (SOCKADDR *)&addr, &iAddrLen, NULL, (DWORD)NULL);
	if (m_socket == INVALID_SOCKET) 
	{
		err = WSAGetLastError();
		if (err != WSAECONNREFUSED) 
		{
			OutputDebugString(_T("WSAAccept failed\n"));
			goto Error;
		} 
		else 
		{
			OutputDebugString(_T("The connection attempt has been refused\n"));
			goto Error;
		}
	}
	m_uPeerIP = addr.sin_addr.s_addr;
	m_strPeerName = inet_ntoa(addr.sin_addr);

	WSAAsyncSelect(m_socket, m_hWnd, 0, 0);
	// Put Connection in Event Object Notification Mode.
	WSAEventSelect(m_socket, m_hNetEvent, FD_READ | FD_WRITE | FD_CLOSE);

	// Start the I/O thread, and save the thread handle.
	m_hIOThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, this, 0, &dwThreadId);
	if (m_hIOThread == NULL) 
	{
		OutputDebugString(_T("CreateThread() failed in HandleAcceptMessage()\n"));
		goto Error;
	}

	OutputDebugString(m_strPeerName + _T(" joined the meeting\n"));
	return m_uPeerIP;

Error:
	Unprepare();
	return 0;
}

ULONG CTcpSocket::OnConnect(LPARAM lParam)
{
	int err;
	DWORD dwThreadId;	// needed for CreateThread

	err = WSAGETSELECTERROR(lParam);
	// Check to see if there was an error on the connection attempt.
	if (err) 
	{
		// Some kind of error occurred.
		if (err == WSAECONNREFUSED) 
		{
			OutputDebugString(m_strPeerName + _T(" 处没有应用程序应答\n"));
		} 
		else 
		{
			OutputDebugString(m_strPeerName + _T(" 没有在网络上\n"));
		}
		goto Error;
	}

	WSAAsyncSelect(m_socket, m_hWnd, 0, 0);
	// Put Connection in Event Object Notification Mode.
	WSAEventSelect(m_socket, m_hNetEvent, FD_READ | FD_WRITE | FD_CLOSE);

	// Start the I/O thread, and save the thread handle.
	m_hIOThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, this, 0, &dwThreadId);
	if (m_hIOThread == NULL) 
	{
		OutputDebugString(_T("CreateThread() failed in OnConnect()\n"));
		goto Error;
	}

	OutputDebugString(m_strPeerName + _T(" joined the meeting\n"));

	return m_uPeerIP;

Error:
	Unprepare();
	return 0;
}

void CTcpSocket::OnClose()
{
	Unprepare();
}

//////////////////////////////////////////////////////////////////////
// CTcpSocket protected functions 

BOOL CTcpSocket::Prepare()
{
	m_hNetEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hOutputEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hNetEvent || !m_hOutputEvent || !m_hQuitEvent)
	{
		OutputDebugString(_T("CreateEvent fail in Prepare connection data\n"));
		goto Error;
	}

	m_eventArray[0] = m_hNetEvent;
	m_eventArray[1] = m_hOutputEvent;
	m_eventArray[2] = m_hQuitEvent;

	return TRUE;

Error:
	Unprepare();
	return FALSE;
}

void CTcpSocket::Unprepare()
{
	if (m_hNetEvent != NULL)
	{
		CloseHandle(m_hNetEvent);
		m_hNetEvent = NULL;
	}
	if (m_hOutputEvent != NULL)
	{
		CloseHandle(m_hOutputEvent);
		m_hOutputEvent = NULL;
	}
	if (m_hQuitEvent != NULL)
	{
		CloseHandle(m_hQuitEvent);
		m_hQuitEvent = NULL;
	}
	if (m_hIOThread != NULL)
	{
		CloseHandle(m_hIOThread);
		m_hIOThread = NULL;
	}
	while (!m_sendQueue.IsEmpty())
	{
		WSABUF * p = m_sendQueue.RemoveHead();
		free(p->buf);
		free(p);
	}
	if (m_socket != INVALID_SOCKET)
	{
		WSAEventSelect(m_socket, m_hNetEvent, 0);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

//////////////////////////////////////////////////////////////////////
// CTcpSocket Static functions 

BOOL CTcpSocket::Startup()
{
	int i, err;
	DWORD dwBufferSize = 0;		// size of m_pProtocolsInfo buffer
	WORD wVersionRequested;
	BOOL bFoundTcp = FALSE;
	WSADATA wsaData;

	m_pProtocolsInfo = NULL;

	// Asynchronous IO and multicast semantics we use supported starting only with WinSock 2.0 
	wVersionRequested = MAKEWORD (2, 2);
    // Start WinSock 2.  If it fails, we don't need to call WSACleanup().
	err = WSAStartup (wVersionRequested, &wsaData);
	if (err != 0)
	{
		OutputDebugString(_T("Could not find high enough version of WinSock\n"));
		return FALSE;
	}
/*	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
	{	
		OutputDebugString(_T("Could not find the correct version of WinSock\n"));
		goto Error;
	}
*/
	// Call WSAEnumProtocols to figure out how big of a buffer we need.
	err = WSAEnumProtocols(NULL, NULL, &dwBufferSize);
	if (err != SOCKET_ERROR && WSAGetLastError() != WSAENOBUFS) 
	{
		OutputDebugString(_T("WSAEnumProtocols is broken\n"));
		goto Error;
	}
	// Allocate a buffer, call WSAEnumProtocols to get an array of WSAPROTOCOL_INFO structs.
	m_pProtocolsInfo = (LPWSAPROTOCOL_INFO)malloc(dwBufferSize);
	if (m_pProtocolsInfo == NULL) 
	{
		OutputDebugString(_T("malloc m_pProtocolsInfo failed\n"));
		goto Error;
	}
	err = WSAEnumProtocols(NULL, m_pProtocolsInfo, &dwBufferSize);
	if (err == SOCKET_ERROR) 
	{
		OutputDebugString(_T("WSAEnumProtocols failed\n"));
		goto Error;
    }

	for (i = 0; i < err; i ++) 
	{
		m_pTcpInfo = &m_pProtocolsInfo[i];
		ASSERT(m_pTcpInfo != NULL);
		if (m_pTcpInfo->iAddressFamily == AF_INET
			&& m_pTcpInfo->iProtocol == IPPROTO_TCP
			&& m_pTcpInfo->iSocketType == SOCK_STREAM
			&& m_pTcpInfo->iMaxSockAddr == 0x10) 
		{
			bFoundTcp = TRUE;
			break;
		}
//		OutputDebugString(m_pTcpInfo->szProtocol);
//		OutputDebugString(_T("\n");
	}

	if (!bFoundTcp)		goto Error;

	OutputDebugString(_T("WinSock2 Init\n"));
	return TRUE;

Error:
	Shutdown();
	return FALSE;
}

void CTcpSocket::Shutdown()
{
	OutputDebugString(_T("WinSock2 Free\n"));
	if (m_pProtocolsInfo != NULL)
	{
		free(m_pProtocolsInfo);
	}
	WSACleanup( );
}

CString CTcpSocket::GetHostName()
{
	static CString strName;

	char szHostname[80];

	gethostname(szHostname, 80); 
	strName = szHostname;

	return strName;
}

CString CTcpSocket::GetProtocols()
{
	static CString strProto;

	int i;
	PROTOENT * proto;

	strProto = "";

	for (i = 0; i < 20; i ++)
	{
		proto = getprotobynumber(i);
		if (proto != NULL)
		{
			strProto += proto->p_name;
			strProto += " / ";
		}
	}

	return strProto;
}

CString CTcpSocket::GetProviders()
{
	static CString strProviders;

	WSANAMESPACE_INFO info[20];
	DWORD dwBufferLength = (sizeof(WSANAMESPACE_INFO) * 20);
	int i, iNumber;

	iNumber = WSAEnumNameSpaceProviders(&dwBufferLength, &info[0]);

	strProviders = _T("");
	for (i = 0; i < iNumber; i ++)
	{
		strProviders += info[i].lpszIdentifier;
		strProviders += _T("\n");
	}

	return strProviders;
}

void CTcpSocket::IP2ByteArray(ULONG uIP, BYTE * pArray)
{
	WORD w;
	w = LOWORD(uIP);
	pArray[0] = LOBYTE(w);
	pArray[1] = HIBYTE(w);
	w = HIWORD(uIP);
	pArray[2] = LOBYTE(w);
	pArray[3] = HIBYTE(w);
}

BOOL CTcpSocket::HandleNetworkEvent()
{
	WSANETWORKEVENTS networkEvents;		// tells us what events happened
 	int err;

	// Find out what happened and act accordingly.
	err = WSAEnumNetworkEvents(m_socket, m_hNetEvent, &networkEvents);
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
			DWORD dwNumBytes;	// stores how many bytes we received
			DWORD dwFlags = 0;	// flags for WSARecv
			WSABUF * p = (WSABUF *)malloc(sizeof(WSABUF));
			if (p == NULL)
			{
				OutputDebugString(_T("malloc failed in CTcpSocket::HandleNetworkEvent()\n"));
				return FALSE;
			}
			p->buf = (char *)malloc(TCP_BUFFER_LENGTH);
			if (p->buf == NULL)
			{
				free(p);
				OutputDebugString(_T("malloc failed in CTcpSocket::HandleNetworkEvent()\n"));
				return FALSE;
			}
			p->len = TCP_BUFFER_LENGTH - 1;
			err = WSARecv(m_socket, p, 1, &dwNumBytes, &dwFlags, NULL, NULL);
			if (err == SOCKET_ERROR) 
			{
				err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					free(p->buf);
					free(p);
					OutputDebugString(_T("The remote party has reset the connection or other err\n"));
					return FALSE;
				}
			}
			p->len = dwNumBytes;
//			PostMessage(m_hWnd, WM_TCP_SOCKET_DATA, (WPARAM)m_uPeerIP, (LPARAM)p);
			PostMessage(m_hWnd, WM_TCP_SOCKET_DATA, (WPARAM)m_socket, (LPARAM)p);
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
//			OutputDebugString(_T("Set hOutputEvent\n");
		}
	}
	if (networkEvents.lNetworkEvents & FD_CLOSE) 
	{
		if (networkEvents.iErrorCode[FD_CLOSE_BIT] == 0) 
		{	// A graceful shutdown has occurred...
			return FALSE;
		}
		else 
		{	// This is some other type of abortive close or failure...
			OutputDebugString(_T("Socket aborted\n"));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CTcpSocket::HandleOutputEvent()
{
	if (!m_bWriteOk)
	{	// Wait for next FD_WRITE
		return TRUE;
	}

	BOOL bRet = TRUE;
	CSingleLock sLock(&m_critical);
	WSABUF * p;
	DWORD dwBytesSent;					// needed in WSASend
	int err;

	sLock.Lock();
	while (!m_sendQueue.IsEmpty() && m_bWriteOk)
	{
		p = m_sendQueue.RemoveHead();
		err = WSASend(m_socket, p, 1, &dwBytesSent, 0, NULL, NULL);
		if (err == SOCKET_ERROR) 
		{
			err = WSAGetLastError();
			m_bWriteOk = FALSE;
			if (err == WSAEWOULDBLOCK) 
			{	//  WSAEWOULDBLOCK means we have to wait for an FD_WRITE before we can send.
				m_sendQueue.AddHead(p);
			}
			else 
			{
				OutputDebugString(_T("Unexpected error with WSASend\n"));
				bRet = FALSE;
				free(p->buf);
				free(p);
			}
		}
		else
		{
			if (dwBytesSent != p->len)
			{
				OutputDebugString(_T("Not all data sent out in CTcpSocket::HandleOutputEvent()!\n"));
			}
			free(p->buf);
			free(p);
		}
	}
	sLock.Unlock();
	return bRet;
}

void CTcpSocket::CleanUp()
{
	PostMessage(m_hWnd, WM_TCP_SOCKET_CLOSE, (WPARAM)m_socket, (LPARAM)m_uPeerIP);
}

DWORD WINAPI CTcpSocket::ThreadFunc(LPVOID pParam)
{
	DWORD dwWaitStatus;						// holds return value of the wait
	CTcpSocket * p = (CTcpSocket *)pParam;	// connection-specific data

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
			if (p->HandleOutputEvent())
			{
				continue;
			}
			break;

		case WSA_WAIT_EVENT_0+2:	// Quit time;
			OutputDebugString(_T("Hangup by myself\n"));
			break;

		default:
			OutputDebugString(_T("Unknown error in WSAWaitForMultipleEvents()\n"));
			break;
		} // switch (dwWaitStatus)
		// Break out of the while loop.
		break;
	} // while (1)

	// Thread is ending because the connection was closed or an error occurred
	p->CleanUp();
	return 0;
}


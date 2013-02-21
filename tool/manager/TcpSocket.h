// TcpSocket.h: interface for the CTcpSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPSOCKET_H__5D096D00_9B89_11D3_8AB6_5254AB104A0B__INCLUDED_)
#define AFX_TCPSOCKET_H__5D096D00_9B89_11D3_8AB6_5254AB104A0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_TCP_SOCKET_ACCEPT	(WM_USER + 0x200)
#define WM_TCP_SOCKET_CONNECT	(WM_USER + 0x201)
#define WM_TCP_SOCKET_DATA		(WM_USER + 0x202)
#define WM_TCP_SOCKET_CLOSE		(WM_USER + 0x203)

#define TCP_DEFAULT_PORT	0x2223

class CTcpSocket  
{
public:
	CTcpSocket();
	virtual ~CTcpSocket();

	BOOL Call(ULONG uPeerIP, HWND hWnd, int iPort = 0);
	ULONG Accept(SOCKET socketListen, LPARAM lParam, HWND hWnd, int iPort = 0);
	void Hangup();
	BOOL Write(char * pData, int iLength);

	ULONG OnConnect(LPARAM lParam);
	void OnClose();

	inline SOCKET GetSocket()	{	return m_socket;	}
	inline ULONG GetPeerIP()	{	return m_uPeerIP;	}

protected:
	SOCKET m_socket;		// the socket for the connection
	ULONG  m_uPeerIP;		// IP address of the peer connection
	CString m_strPeerName;	// IP address of the peer connection in dotted string (for read)

	HWND m_hWnd;			// Window handle to receive messages and data
	int m_iPort;

	HANDLE m_hIOThread;			// handle to thread that takes care of network events
	HANDLE m_hNetEvent;			// handle to event signaled when a network event occurs
	HANDLE m_hOutputEvent;		// handle to event signaled when output is ready to be shipped
	HANDLE m_hQuitEvent;
	WSAEVENT m_eventArray[3];	// The array of events which the I/O thread is currently waiting on.

	BOOL m_bWriteOk;		// Are we waiting for an FD_WRITE?

	CList<WSABUF *, WSABUF *> m_sendQueue;	// Queue used to hold send buffers
	CCriticalSection m_critical;

protected:
	BOOL Prepare();
	void Unprepare();

public:
	static BOOL Startup();
	static void Shutdown();

	static CString GetHostName();
	static CString GetProtocols();
	static CString GetProviders();

	static void IP2ByteArray(ULONG uIP, BYTE * pArray);


	static DWORD WINAPI ThreadFunc(LPVOID pParam);

public:
	BOOL HandleNetworkEvent();
	BOOL HandleOutputEvent();
	void CleanUp();
	inline WSAEVENT * GetEventArray()	{	return m_eventArray;	}

public:
	static LPWSAPROTOCOL_INFO m_pProtocolsInfo;
	static LPWSAPROTOCOL_INFO m_pTcpInfo;
};

#endif // !defined(AFX_TCPSOCKET_H__5D096D00_9B89_11D3_8AB6_5254AB104A0B__INCLUDED_)

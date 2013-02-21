// UdpSocket.h: interface for the CUdpSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSOCKET_H__F1914EA0_7FBF_11D4_90C7_5254AB121C24__INCLUDED_)
#define AFX_UDPSOCKET_H__F1914EA0_7FBF_11D4_90C7_5254AB121C24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_UDP_SOCKET_DATA		(WM_USER + 0x280)
#define WM_SYSLOG_SOCKET_DATA	(WM_USER + 0x281)

#define UDP_DEFAULT_PORT		0x2021
#define UDP_DEFAULT_DATA_SIZE	1400
#define UDP_DEFAULT_QUEUE_SIZE	48

class CUdpSocket  
{
public:
	CUdpSocket();
	virtual ~CUdpSocket();

	// Set uPeerIP to 0 if you need an broadcast udp socket
	// if uPeerIP is a multicast address, the socket will be a multicast socket 
	BOOL Enter(HWND hWnd, UINT uMsg, ULONG uPeerIP = 0, int iPort = UDP_DEFAULT_PORT, int iDataSize = UDP_DEFAULT_DATA_SIZE, int iQueueSize = UDP_DEFAULT_QUEUE_SIZE);
	void Leave();
	BOOL Write(char * pData, int iLength);
	BOOL WriteList(CList<WSABUF *, WSABUF *> & list);
	BOOL Busy();

protected:
	BOOL Prepare();
	void Unprepare();

public:
	static BOOL Startup();
	static void Shutdown();

	static ULONG Name2IP(CString strName);
	static CString IP2Name(ULONG uIP);
	static ULONG GetLocalIP();

	static DWORD WINAPI ThreadFunc(LPVOID pParam);

	// for multicast only
	static void SetTTL(int iTTL);

// for multicast only
protected:
	static int m_iTTL;

protected:
	SOCKET m_socket;	// Udp read socket
	HWND   m_hWnd;		// Window handle to receive multicast data
	UINT   m_uMsg;		// Message to send to window when receive data
	HANDLE m_hIOThread;	// handle to thread that send and receive data

	ULONG m_uPeerIP;		// peer IP to read and write data
	ULONG m_uLocalIP;
	int m_iPort;

	HANDLE m_hNetworkEvent;	// handle to event signaled when FD_WRITE and FD_READ
	HANDLE m_hOutputEvent;	// handle to event signaled when output is ready to be shipped
	HANDLE m_hQuitEvent;	// handle to event signaled when quit
	HANDLE m_eventArray[3];	// The array of events which the I/O thread is currently waiting on.

	SOCKADDR_IN m_peerAddr;	// peer address, or multicast address to join in

	int m_iDataSize;			// Read and write data size
	int m_iQueueSize;			// data queue size
	BOOL m_bWriteOk;			// Are we waiting for an FD_WRITE?

	CList<WSABUF *, WSABUF *> m_sendQueue;	// Queue used to hold send buffers
	CCriticalSection m_critical;

public:
	int ReadData();
	int WriteData();
	int BeginSession();
	BOOL HandleNetworkEvent();
	void CleanUp();
};

#endif // !defined(AFX_UDPSOCKET_H__F1914EA0_7FBF_11D4_90C7_5254AB121C24__INCLUDED_)

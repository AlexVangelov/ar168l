#if !defined(AFX_UDPDEBUGTHREAD_H__6E7B1721_7C31_11D5_A31F_5254AB104E6A__INCLUDED_)
#define AFX_UDPDEBUGTHREAD_H__6E7B1721_7C31_11D5_A31F_5254AB104E6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UdpDebugThread.h : header file
//

//#include "TcpSocket.h"
#include "UdpSocket.h"

#define WM_UDP_DEBUG_DISCONNECT		(WM_USER + 0x101)

/////////////////////////////////////////////////////////////////////////////
// CDebugEdit
class CDebugEdit : public CEdit
{
public:
    CDebugEdit() {};

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
    DECLARE_MESSAGE_MAP()
public:
	BOOL SaveToFile(void);
	BOOL CopyAll(void);
	BOOL ClearLog(void);
};

/////////////////////////////////////////////////////////////////////////////
// CUdpDebugWnd window

class CUdpDebugWnd : public CWnd
{
// Construction
public:
	CUdpDebugWnd();

// Attributes
public:
	BOOL Create();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUdpDebugWnd)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUdpDebugWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUdpDebugWnd)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
#if _MSC_VER >= 1310
	afx_msg LRESULT OnUdpData(WPARAM wParam, LPARAM lParam);
#else
	afx_msg void OnUdpData(WPARAM wParam, LPARAM lParam);
#endif
#if _MSC_VER >= 1310
	afx_msg LRESULT OnSyslogData(WPARAM wParam, LPARAM lParam);
#else
	afx_msg void OnSyslogData(WPARAM wParam, LPARAM lParam);
#endif
	DECLARE_MESSAGE_MAP()

	int IsNewLine(char * pData);
	void DisplayDebug(char * pData, int iLength, CString strIP);
	void ModifyDisplayString(CString & str, CString strIP);

protected:
	CUdpSocket	m_udp;
	//CEdit * m_pEdit;
	CDebugEdit * m_pEdit;

	int m_iDebugOption;
	BOOL m_bDebugIPOnly;
	BOOL m_bDebugDataType;
	BOOL m_bDebugDataTime;
	ULONG m_uPeerIP;
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

/////////////////////////////////////////////////////////////////////////////
// CUdpDebugThread thread

class CUdpDebugThread : public CWinThread
{
	DECLARE_DYNCREATE(CUdpDebugThread)
public:
	CUdpDebugThread();           // protected constructor used by dynamic creation

	void operator delete(void* p);

// Attributes
public:
	static HANDLE m_hEventThreadKilled;

	static void PrepareEvent();
	static void UnprepareEvent();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUdpDebugThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CUdpDebugThread();

	// Generated message map functions
	//{{AFX_MSG(CUdpDebugThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnDisconnect(WPARAM wParam = 0, LPARAM lParam = 0);
	DECLARE_MESSAGE_MAP()

protected:
	CUdpDebugWnd m_wndDebug;
};

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UDPDEBUGTHREAD_H__6E7B1721_7C31_11D5_A31F_5254AB104E6A__INCLUDED_)

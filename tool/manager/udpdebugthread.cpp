// UdpDebugThread.cpp : implementation file
//

#include "stdafx.h"
#include "UdpDebugThread.h"

#include "resource.h"

HWND g_hDebugWnd;

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE CUdpDebugThread::m_hEventThreadKilled;

/////////////////////////////////////////////////////////////////////////////
// CUdpDebugThread

IMPLEMENT_DYNCREATE(CUdpDebugThread, CWinThread)

CUdpDebugThread::CUdpDebugThread()
{
}

CUdpDebugThread::~CUdpDebugThread()
{
}

void CUdpDebugThread::operator delete(void* p)
{
	// The exiting main application thread waits for this event before completely
	// terminating in order to avoid a false memory leak detection. 

	CWinThread::operator delete(p);
	SetEvent(m_hEventThreadKilled);
}

BOOL CUdpDebugThread::InitInstance()
{
	if (m_wndDebug.Create() == FALSE)
		return FALSE;

	// It is important to set CWinThread::m_pMainWnd to the user interface
	// window.  This is required so that when the m_pMainWnd is destroyed,
	// the CWinThread is also automatically destroyed.  For insight into
	// how the CWinThread is automatically destroyed when the m_pMainWnd
	// window is destroyed, see the implementation of CWnd::OnNcDestroy
	// in wincore.cpp of the MFC sources.

	m_pMainWnd = &m_wndDebug;
	return TRUE;
}

int CUdpDebugThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

void CUdpDebugThread::PrepareEvent()
{
	m_hEventThreadKilled = CreateEvent(NULL, FALSE, FALSE, NULL); 
}

void CUdpDebugThread::UnprepareEvent()
{
	CloseHandle(m_hEventThreadKilled);
}

BEGIN_MESSAGE_MAP(CUdpDebugThread, CWinThread)
	//{{AFX_MSG_MAP(CUdpDebugThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
#if _MSC_VER >= 1310
	ON_THREAD_MESSAGE(WM_UDP_DEBUG_DISCONNECT, OnDisconnect)
#else
	ON_MESSAGE(WM_UDP_DEBUG_DISCONNECT, OnDisconnect)
#endif	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUdpDebugThread message handlers
/////////////////////////////////////////////////////////////////////////////

void CUdpDebugThread::OnDisconnect(WPARAM wParam, LPARAM lParam)
{
	m_wndDebug.DestroyWindow();
}

// CUdpDebugWnd

CUdpDebugWnd::CUdpDebugWnd()
{
}

CUdpDebugWnd::~CUdpDebugWnd()
{
}

BOOL CUdpDebugWnd::Create()
{
	DWORD dwStyle = WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX|WS_VISIBLE|WS_SIZEBOX;

	// Register a custom WndClass and create a window.
	// This must be done because CUdpDebugWnd has a custom cursor, and no icon.
	LPCTSTR lpszUdpAudioWndClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
													LoadCursor(NULL, IDC_ARROW),
													(HBRUSH)(COLOR_WINDOW+1), NULL);

	return CWnd::CreateEx(0, lpszUdpAudioWndClass, _T("Debug Output"), dwStyle,
							0, 0, 600, 400, NULL, 0);
}

int CUdpDebugWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pEdit = new CDebugEdit();
	CRect   ClientRect(0,0,0,0);
	this->GetClientRect(&ClientRect);
	m_pEdit->Create(/*WS_DISABLED|*/WS_VISIBLE|WS_CHILD|WS_BORDER|WS_HSCROLL|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL,
		ClientRect, this, IDC_UDPDEBUG_EDIT);

/*	m_iDebugOption = AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("DebugOption"), 0);
	if (m_iDebugOption == 0 || m_iDebugOption == 2)
	{
		m_udp.Enter(GetSafeHwnd(), WM_UDP_SOCKET_DATA);
	}
*/
	m_bDebugIPOnly = AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("IPOnly"), 0);
	m_bDebugDataType = AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("SyslogDataType"), 0);
	m_bDebugDataTime = AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("SyslogDataTime"), 0);

	ULONG uPeerIP = AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("IPAddress"), 0);
	m_uPeerIP = MAKELONG(MAKEWORD(HIBYTE(HIWORD(uPeerIP)), LOBYTE(HIWORD(uPeerIP))), MAKEWORD(HIBYTE(LOWORD(uPeerIP)), LOBYTE(LOWORD(uPeerIP))));

	CString strIP;
	if (m_bDebugIPOnly)
	{
		strIP = CUdpSocket::IP2Name(m_uPeerIP);
	}
	else
	{
		strIP = _T("Debug All");
	}
	SetWindowText(strIP);
	g_hDebugWnd = GetSafeHwnd();
	
	return 0;
}

void CUdpDebugWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
/*	if (m_iDebugOption == 0 || m_iDebugOption == 2)
	{
		m_udp.Leave();
	}
*/}

BEGIN_MESSAGE_MAP(CUdpDebugWnd, CWnd)
	//{{AFX_MSG_MAP(CUdpDebugWnd)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UDP_SOCKET_DATA, OnUdpData)
	ON_MESSAGE(WM_SYSLOG_SOCKET_DATA, OnSyslogData)
//	ON_WM_SIZE()
ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUdpDebugWnd message handlers

void CUdpDebugWnd::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_MINIMIZE);
//	CWnd::OnClose();
}

BOOL CUdpDebugWnd::DestroyWindow() 
{
	if (m_pEdit != NULL)
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
	}
	
	return CWnd::DestroyWindow();
}

int CUdpDebugWnd::IsNewLine(char * pData)
{
	if (pData[0] == 0x0d)
	{
		if (pData[1] == 0x0a)	return 2;
		else					return 1;
	}
	else if (pData[0] == 0x0a)
	{
		if (pData[1] == 0x0d)	return 2;
		else					return 1;
	}
	return 0;
}

void CUdpDebugWnd::ModifyDisplayString(CString & strData, CString strIP)
{
	if (strData.Left(5) != _T("<167>"))		return;

	if (m_bDebugDataType && m_bDebugDataTime)
	{
		strData.Delete(0, 21);
	}
	else if (m_bDebugDataType)
	{
		strData.Delete(0, 5);
	}
	else if (m_bDebugDataTime)
	{
		strData.Delete(5, 16);
	}

	if (m_bDebugIPOnly)
	{
		int iPos = strData.Find(strIP);
		strData.Delete(iPos, strIP.GetLength()+1);
	}
}

void CUdpDebugWnd::DisplayDebug(char * pData, int iLength, CString strIP)
{
	char * pDisplay;
	char * pCur;
	char * pPrev;
	int iNewLine;
	CString str;

	pDisplay = (char *)malloc(iLength + 1);
	pDisplay[iLength] = 0;
	memcpy(pDisplay, pData, iLength);

	pCur = pDisplay;
	pPrev = pDisplay;
	while (*pCur)
	{
		iNewLine = IsNewLine(pCur);
		if (iNewLine)
		{
			*pCur = 0;
			pCur += iNewLine;
			if (strlen(pPrev))
			{
				str = pPrev;
				ModifyDisplayString(str, strIP);
				m_pEdit->ReplaceSel(str);
				m_pEdit->ReplaceSel(_T("\r\n"));
			}
			pPrev = pCur;
		}
		else
		{
			pCur ++;
		}
	}

	free (pDisplay);
}

#if _MSC_VER >= 1310
LRESULT CUdpDebugWnd::OnSyslogData(WPARAM wParam, LPARAM lParam)
#else
void CUdpDebugWnd::OnSyslogData(WPARAM wParam, LPARAM lParam)
#endif
{
	ULONG uPeerIP = (ULONG)wParam;
	WSABUF * p = (WSABUF *)lParam;
//	CString strData = (char *)p->buf;
	CString strIP = CUdpSocket::IP2Name(uPeerIP);

	if (m_bDebugIPOnly)
	{
		if (uPeerIP != m_uPeerIP)
		{
			goto End;
		}
	}

	DisplayDebug((char *)p->buf, p->len, strIP);
End:
	free(p->buf);
	free(p);
#if _MSC_VER >= 1310
return 0;
#endif
}

#if _MSC_VER >= 1310
LRESULT CUdpDebugWnd::OnUdpData(WPARAM wParam, LPARAM lParam)
#else
void CUdpDebugWnd::OnUdpData(WPARAM wParam, LPARAM lParam)
#endif
{
	ULONG uPeerIP = (ULONG)wParam;
	WSABUF * p = (WSABUF *)lParam;
	int iLength = p->len;
	CString strIP = CUdpSocket::IP2Name(uPeerIP);

	if (m_bDebugIPOnly)
	{
		if (uPeerIP != m_uPeerIP)
		{
			goto End;
		}
	}

	if (!m_bDebugIPOnly)
	{
		m_pEdit->ReplaceSel(strIP);
		m_pEdit->ReplaceSel(_T(": "));
	}

	DisplayDebug((char *)p->buf, iLength, strIP);
End:
	free(p->buf);
	free(p);
#if _MSC_VER >= 1310
return 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CUdpDebugWnd helper functions


//void CUdpDebugWnd::OnSize(UINT nType, int cx, int cy)
//{
//	CWnd::OnSize(nType, cx, cy);
//
//	// TODO: Add your message handler code here
//}

void CUdpDebugWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
    CRect   ClientRect(0,0,0,0);
	this->GetClientRect(&ClientRect);
	m_pEdit->MoveWindow(&ClientRect);
}

#define MES_COPY        _T("&Copy")
#define MES_COPYALL     _T("&Copy All")
#define MES_CUT         _T("C&ut")
#define MES_DELETE      _T("&Delete")
#define MES_SELECTALL   _T("Select &All")
#define MES_SAVETOFILE	_T("&Save to file")
#define MES_EXITDEBUG	_T("&Exit debug")
#define MES_CLEARLOG	_T("Clear log")
#define ME_SELECTALL    WM_USER + 0x7000
#define ME_SAVETOFILE	WM_USER + 0x7001
#define ME_EXITDEBUG	WM_USER + 0x7002
#define ME_COPYALL	    WM_USER + 0x7003
#define ME_CLEARLOG	    WM_USER + 0x7004
BEGIN_MESSAGE_MAP(CDebugEdit, CEdit)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CDebugEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	SetFocus();
    CMenu menu;
    menu.CreatePopupMenu();
	menu.InsertMenu(0, MF_BYPOSITION , WM_CUT, MES_CUT);
	menu.InsertMenu(1, MF_BYPOSITION , WM_COPY, MES_COPY);
	menu.InsertMenu(2, MF_BYPOSITION , ME_COPYALL, MES_COPYALL);
	menu.InsertMenu(3, MF_BYPOSITION | MF_SEPARATOR);
	menu.InsertMenu(4, MF_BYPOSITION , ME_SELECTALL, MES_SELECTALL);
	menu.InsertMenu(5, MF_BYPOSITION , WM_CLEAR, MES_DELETE);
	menu.InsertMenu(6, MF_BYPOSITION , ME_CLEARLOG, MES_CLEARLOG);
	menu.InsertMenu(7, MF_BYPOSITION | MF_SEPARATOR);
	menu.InsertMenu(8, MF_BYPOSITION , ME_SAVETOFILE, MES_SAVETOFILE);
	//menu.InsertMenu(7, MF_BYPOSITION | MF_SEPARATOR);
	
	
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |
        TPM_RIGHTBUTTON, point.x, point.y, this);
}

BOOL CDebugEdit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
    {
    case EM_UNDO:
    case WM_CUT:
    case WM_COPY:
    case WM_CLEAR:
    case WM_PASTE:
        return SendMessage(LOWORD(wParam));
    case ME_SELECTALL:
        return SendMessage (EM_SETSEL, 0, -1);
	case ME_SAVETOFILE:
		return SaveToFile();
	/*
	case ME_EXITDEBUG:
		return this->GetParent()->DestroyWindow();
	*/
	case ME_COPYALL:
		return CopyAll();
	case ME_CLEARLOG:
		return ClearLog();
    default:
        return CEdit::OnCommand(wParam, lParam);
	}
}

BOOL CDebugEdit::SaveToFile(void)
{
	CString str;
	this->GetWindowText(str);
	CFileDialog fileDlg(FALSE, _T("log"), _T("*.log"));
	if( fileDlg.DoModal ()==IDOK )
   {
      CString pathName = fileDlg.GetPathName();
	  CStdioFile f;
	  f.Open(pathName, CFile::modeCreate | CFile::modeWrite | CFile::typeText); 
	  f.WriteString(str);
	  f.Close();
	  return true;
   }
  return false;
}

BOOL CDebugEdit::CopyAll(void)
{
		CSharedFile sf(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT);
		CString str;
		this->GetWindowText(str); 
		sf.Write(str, str.GetLength());
		OpenClipboard();
		EmptyClipboard();
		SetClipboardData(CF_TEXT,sf.Detach());
		CloseClipboard();
	return true;
}

BOOL CDebugEdit::ClearLog(void)
{
	this->SetWindowText(_T(""));
	return 0;
}

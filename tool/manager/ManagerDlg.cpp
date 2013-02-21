// ManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Manager.h"
#include "ManagerDlg.h"
#include "..\..\include\type.h"
#include "..\..\include\ar168.h"

static TCHAR szBinFilter[] = _T("Binary files (*.bin)|*.bin|All files (*.*)|*.*||");
static TCHAR szTxtFilter[] = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*||");

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND g_hDebugWnd;
//int iCheckBroadcast;

void SystemCall(CString str)
{
	_tsystem(str);
/*    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	CString strErr;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	LPTSTR szCmdline = _tcsdup(str);
//	LPTSTR szCmdline = _tcsdup(_T("C:\\\\Windows\\System32\\TFTP.EXE"));

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        szCmdline,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
		strErr.Format(_T("CreateProcess failed %d\n"), GetLastError());
        OutputDebugString(strErr);
		goto End;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
	
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

End:
	free(szCmdline);
*/
}

#define REPORT_TIMEOUT	60
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManagerDlg dialog

CManagerDlg::CManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManagerDlg)
	m_bSelectedIPOnly = FALSE;
	m_bSyslogDataTime = FALSE;
	m_bSyslogDataType = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pUdpDebugThread = NULL;
}

void CManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManagerDlg)
	DDX_Control(pDX, IDC_PEER_IP, m_ctlPeerIP);
	DDX_Control(pDX, IDC_LIST_PHONE, m_ctlPhones);
	DDX_Check(pDX, IDC_CHECK_SELECTEDIP, m_bSelectedIPOnly);
	DDX_Check(pDX, IDC_SYSLOG_TIME, m_bSyslogDataTime);
	DDX_Check(pDX, IDC_SYSLOG_TYPE, m_bSyslogDataType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CManagerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_UPDATE_PROGRAM, OnUpdateProgram)
	ON_BN_CLICKED(IDC_GET_OPTIONS, OnGetOptions)
	ON_BN_CLICKED(IDC_SET_OPTIONS, OnSetOptions)
	ON_BN_CLICKED(IDC_START_DEBUG, OnStartDebug)
	ON_BN_CLICKED(IDC_GET_PHONEBOOK, OnGetPhonebook)
	ON_BN_CLICKED(IDC_SET_PHONEBOOK, OnSetPhonebook)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PHONE, OnClickListPhone)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UDP_SOCKET_DATA, OnUdpData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManagerDlg message handlers

BOOL CManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	DWORD dwStyle;
	
	dwStyle = m_ctlPhones.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_ctlPhones.SetExtendedStyle(dwStyle);
	m_ctlPhones.InsertColumn(0, _T("IP Address"), LVCFMT_CENTER, 140, 0);
	m_ctlPhones.InsertColumn(1, _T("Phone Number"), LVCFMT_CENTER, 180, 1);
	m_ctlPhones.InsertColumn(2, _T("Status"), LVCFMT_CENTER, 100, 2);
	m_ctlPhones.InsertColumn(3, _T("Version"), LVCFMT_CENTER, 100, 3);
	m_ctlPhones.InsertColumn(4, _T("Model"), LVCFMT_CENTER, 100, 4);
	
	m_uPeerIP = AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("IPAddress"), 0xc0a80101);
	m_ctlPeerIP.SetAddress(m_uPeerIP);
	m_bSelectedIPOnly = (BOOL)AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("IPOnly"), 0);
	m_bSyslogDataType = (BOOL)AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("SyslogDataType"), 0);
	m_bSyslogDataTime = (BOOL)AfxGetApp()->GetProfileInt(_T("AR1688Manager"), _T("SyslogDataTime"), 0);

	m_udp.Enter(GetSafeHwnd(), WM_UDP_SOCKET_DATA, 0xc0a80101, 514);
	m_nTimer = SetTimer(1, 5000, 0);

	m_bDebugging = FALSE;
	UpdateData(FALSE);

	TCHAR szCurDir[128];
	GetCurrentDirectory(128, szCurDir);
	m_strCurDir.Format(_T("%s\\"), szCurDir);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CManagerDlg::SaveSettings()
{
	UpdateData();

	AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("SyslogDataType"), (int)m_bSyslogDataType);
	AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("SyslogDataTime"), (int)m_bSyslogDataTime);
	AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("IPOnly"), (int)m_bSelectedIPOnly);

	m_ctlPeerIP.GetAddress(m_uPeerIP);
	AfxGetApp()->WriteProfileInt(_T("Peer"), _T("IPAddress"), m_uPeerIP);
}


#define REPORT_IP_POS			0											// IP
#define REPORT_NUMBER_POS		(REPORT_IP_POS + IP_ALEN)					// Phone Number
#define REPORT_STATUS_POS		(REPORT_NUMBER_POS + MAX_USER_NAME_LEN)	// ip type
#define REPORT_VERSION_POS		(REPORT_STATUS_POS + 1)
#define REPORT_MODEL_POS		(REPORT_VERSION_POS + MAX_FLAG_VERSION)
#define REPORT_TOTAL_LEN		(REPORT_MODEL_POS + MAX_FLAG_PRODUCT_LEN)

#define SIGN_TOKEN_LEN	5
const TCHAR _cSign[] = _T("<165>");

#if _MSC_VER >= 1300
LRESULT CManagerDlg::OnUdpData(WPARAM wParam, LPARAM lParam)
#else
void CManagerDlg::OnUdpData(WPARAM wParam, LPARAM lParam)
#endif
{
	ULONG uPeerIP = (ULONG)wParam;
	WSABUF * p = (WSABUF *)lParam;
	int iLength = p->len;
	CString strData = (char *)p->buf;
	CString strIP, strNumber, strStatus, strVer, strModel;
	int iPos, iCommaPos;
	CString strDst;

	OutputDebugString(strData);

	if (strData.Left(5) == _T("<165>"))
	{
		iPos = strData.Find(_T("ar1688_manager"));
		if (iPos)
		{
			strDst = strData.Mid(iPos+15);

			iCommaPos = strDst.Find(_T(","));
			strIP = strDst.Left(iCommaPos);
			strDst = strDst.Mid(iCommaPos+1);

			iCommaPos = strDst.Find(_T(","));
			strNumber = strDst.Left(iCommaPos);
			strDst = strDst.Mid(iCommaPos+1);

			iCommaPos = strDst.Find(_T(","));
			strStatus = strDst.Left(iCommaPos);
			strDst = strDst.Mid(iCommaPos+1);

			iCommaPos = strDst.Find(_T(","));
			strVer = strDst.Left(iCommaPos);
			strModel = strDst.Mid(iCommaPos+1);
		
		}
		AddPhone(strIP, strNumber, strStatus, strVer, strModel);
	}
	else
	{
		if (m_bDebugging)// && (m_iDebugOption == 1 || m_iDebugOption == 2))
		{
			::PostMessage(g_hDebugWnd, WM_SYSLOG_SOCKET_DATA, wParam, (LPARAM)p);
			return 0;
		}
	}

	free(p->buf);
	free(p);
#if _MSC_VER >= 1300
	return 0;
#endif
}

void CManagerDlg::AddPhone(CString strIP, CString strNum, CString strStatus, CString strVer, CString strModel)
{
	int i, iTotal, iItem;
	LV_ITEM	lv_item;

	iTotal = m_ctlPhones.GetItemCount();
	for (i = 0; i < iTotal; i ++)
	{
		if (strIP == m_ctlPhones.GetItemText(i, 0))
		{	// do not add same ip address phones
			m_ctlPhones.SetItemText(i, 1, strNum);
			m_ctlPhones.SetItemText(i, 2, strStatus);
			m_ctlPhones.SetItemText(i, 3, strVer);
			m_ctlPhones.SetItemText(i, 4, strModel);
			m_ctlPhones.SetItemData(i, 0);
			return;
		}
	}

	lv_item.mask = LVIF_TEXT; 
	lv_item.iItem = iTotal;
	lv_item.iSubItem = 0;
	lv_item.pszText = (LPWSTR)(LPCTSTR)strIP;
	iItem = m_ctlPhones.InsertItem(&lv_item); // insert new lv_item
	m_ctlPhones.SetItemText(iItem, 1, strNum);
	m_ctlPhones.SetItemText(iItem, 2, strStatus);
	m_ctlPhones.SetItemText(iItem, 3, strVer);
	m_ctlPhones.SetItemText(iItem, 4, strModel);
	m_ctlPhones.SetItemData(iItem, 0);
}

void CManagerDlg::OnDestroy() 
{
	// TODO: Add your message handler code here
	EndUdpDebug();
	Sleep(1);

	m_udp.Leave();
	KillTimer(m_nTimer);
	
	CDialog::OnDestroy();
}

void CManagerDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	int i, iTotal;
	UINT nTime;

	iTotal = m_ctlPhones.GetItemCount();
	for (i = 0; i < iTotal; i ++)
	{
		nTime = m_ctlPhones.GetItemData(i);
		nTime += 5;
		if (nTime >= (REPORT_TIMEOUT << 1))
		{
			m_ctlPhones.DeleteItem(i);
			i --;
			iTotal --;
			if (iTotal == 0)	break;
		}
		else
		{
			m_ctlPhones.SetItemData(i, nTime);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CManagerDlg::OnUpdateProgram() 
{
	// TODO: Add your control notification handler code here
	CString strSrcName = AfxGetApp()->GetProfileString(_T("ProgramUpdate"), _T("SrcName"), _T("demo.bin"));
	CFileDialog dlgOpen(TRUE, _T("*.bin"), strSrcName, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, szBinFilter);
	CString strCmdLine, strIP;
	int iItem;
	POSITION pos = m_ctlPhones.GetFirstSelectedItemPosition();

	SaveSettings();

	if (dlgOpen.DoModal() != IDOK)
		return;
	strSrcName = dlgOpen.GetPathName();
	AfxGetApp()->WriteProfileString(_T("ProgramUpdate"), _T("SrcName"), strSrcName);

	if (!pos)
	{
		m_ctlPeerIP.GetAddress(m_uPeerIP);
		ULONG uPeerIP = MAKELONG(MAKEWORD(HIBYTE(HIWORD(m_uPeerIP)), LOBYTE(HIWORD(m_uPeerIP))), MAKEWORD(HIBYTE(LOWORD(m_uPeerIP)), LOBYTE(LOWORD(m_uPeerIP))));
		strIP = CUdpSocket::IP2Name(uPeerIP);
		strCmdLine.Format(_T("tftp -i %s put %s"), strIP, strSrcName);
		SystemCall(strCmdLine);
	}
	else
	{
		while (pos)
		{
			iItem = m_ctlPhones.GetNextSelectedItem(pos);
			strIP = m_ctlPhones.GetItemText(iItem, 0);

			strCmdLine.Format(_T("tftp -i %s put %s"), strIP, strSrcName);
			SystemCall(strCmdLine);
		}
	}
}

void CManagerDlg::OnGetOptions() 
{
	// TODO: Add your control notification handler code here
	CString strCmdLine, strIP;
	CString strDatFile, strTxtFile;
	int iItem;
	CFileStatus status;
	POSITION pos = m_ctlPhones.GetFirstSelectedItemPosition();
	
	SaveSettings();

	SetCurrentDirectory((LPCTSTR)m_strCurDir);
	strDatFile = _T("settings.dat");
	while (pos)
	{
		iItem = m_ctlPhones.GetNextSelectedItem(pos);
		strIP = m_ctlPhones.GetItemText(iItem, 0);

		strTxtFile = _T("options_") + strIP + _T(".txt");
		strCmdLine = _T("tftp -i ") + strIP + _T(" get ") + strDatFile;
		SystemCall(strCmdLine);
		if (CFile::GetStatus(strDatFile ,status))
		{
			strCmdLine = _T("convert -C -f ") + strDatFile + _T(" ") + strTxtFile;
			SystemCall(strCmdLine);
			ShellExecute(NULL, NULL, strTxtFile, NULL, NULL, SW_SHOW);
			status.m_attribute = status.m_attribute & 0x3e;
			CFile::SetStatus(strDatFile, status);
			CFile::Remove(strDatFile);
		}
	}
}

void CManagerDlg::OnSetOptions() 
{
	// TODO: Add your control notification handler code here
	CString strTxtFile = AfxGetApp()->GetProfileString(_T("SetOptions"), _T("SrcName"), _T("demo.txt"));
	CFileDialog dlgSet(TRUE, _T("*.txt"), strTxtFile, OFN_PATHMUSTEXIST, szTxtFilter);
	CString strCmdLine, strIP;
	int iItem;
	POSITION pos = m_ctlPhones.GetFirstSelectedItemPosition();
	CString strDatFile;
	CFileStatus status;

	SaveSettings();

	if (dlgSet.DoModal() != IDOK)
		return;
	strTxtFile = dlgSet.GetPathName();
	AfxGetApp()->WriteProfileString(_T("SetOptions"), _T("SrcName"), strTxtFile);

	SetCurrentDirectory((LPCTSTR)m_strCurDir);
	strDatFile = _T("settings.dat");
	strCmdLine = _T("convert -c -f ") + strTxtFile + _T(" ") + strDatFile;
	SystemCall(strCmdLine);
	while (pos)
	{
		iItem = m_ctlPhones.GetNextSelectedItem(pos);
		strIP = m_ctlPhones.GetItemText(iItem, 0);
		strCmdLine = _T("tftp -i ") + strIP + _T(" put ") + strDatFile;
		SystemCall(strCmdLine);
	}
	if (CFile::GetStatus(strDatFile ,status))
	{
		CFile::Remove(strDatFile);
	}
}

BOOL CManagerDlg::BeginUdpDebug()
{
	if (m_pUdpDebugThread == NULL)
	{
		m_pUdpDebugThread = new CUdpDebugThread;
		m_pUdpDebugThread->CreateThread();
	}
	return TRUE;
}

void CManagerDlg::EndUdpDebug()
{
	if (m_pUdpDebugThread != NULL)
	{
		::PostThreadMessage(m_pUdpDebugThread->m_nThreadID, WM_UDP_DEBUG_DISCONNECT, 0, 0);
		WaitForSingleObject(CUdpDebugThread::m_hEventThreadKilled, INFINITE);
		m_pUdpDebugThread = NULL;
	}
}

void CManagerDlg::EnableDebug(BOOL bEnable)
{
//	int i;

	if (bEnable)
	{
		CString str;
		str.LoadString(IDS_STOP_DEBUG);
		GetDlgItem(IDC_START_DEBUG)->SetWindowText(str);
		m_bDebugging = TRUE;
	}
	else
	{
		CString str;
		str.LoadString(IDS_START_DEBUG);
		GetDlgItem(IDC_START_DEBUG)->SetWindowText(str);
		
/*		for (i = 0; i < m_ctlPhones.GetItemCount(); i++)   
		{   
			m_ctlPhones.SetItemState(i, 0, LVIS_SELECTED);   
		}
*/		m_bDebugging = FALSE;
	}
}

void CManagerDlg::OnStartDebug() 
{
	// TODO: Add your control notification handler code here
	int iItem;
	CString strIP;
	POSITION pos = m_ctlPhones.GetFirstSelectedItemPosition();
	
	SaveSettings();
	if (m_bSelectedIPOnly && pos)
	{
		iItem = m_ctlPhones.GetNextSelectedItem(pos);
		strIP = m_ctlPhones.GetItemText(iItem, 0);
		ULONG uPeerIP = CUdpSocket::Name2IP(strIP);
		m_uPeerIP = MAKELONG(MAKEWORD(HIBYTE(HIWORD(uPeerIP)), LOBYTE(HIWORD(uPeerIP))), MAKEWORD(HIBYTE(LOWORD(uPeerIP)), LOBYTE(LOWORD(uPeerIP))));

		AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("IPAddress"), m_uPeerIP);
		AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("IPOnly"), 1);
	}
	else
	{
		m_ctlPeerIP.GetAddress(m_uPeerIP);
		if (m_uPeerIP != 0 && m_bSelectedIPOnly)
		{
			AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("IPAddress"), m_uPeerIP);
			AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("IPOnly"), 1);
		}
		else
		{
			AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("IPOnly"), 0);
		}
	}

	if (m_bSyslogDataType)
		AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("SyslogDataType"), 1);
	else
		AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("SyslogDataType"), 0);

	if (m_bSyslogDataTime)
		AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("SyslogDataTime"), 1);
	else
		AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("SyslogDataTime"), 0);

//	AfxGetApp()->WriteProfileInt(_T("AR1688Manager"), _T("DebugOption"), m_iDebugOption);
	if (m_pUdpDebugThread == NULL)
	{
		BeginUdpDebug();
		EnableDebug(TRUE);
	}
	else
	{
		EndUdpDebug();
		EnableDebug(FALSE);
	}
}

void CManagerDlg::OnGetPhonebook() 
{
	// TODO: Add your control notification handler code here
	
	CString strCmdLine, strIP;
	CString strDatFile, strTxtFile;
	int iItem;
	CFileStatus status;
	POSITION pos = m_ctlPhones.GetFirstSelectedItemPosition();
	
	SaveSettings();

	SetCurrentDirectory((LPCTSTR)m_strCurDir);
	strDatFile = _T("phonebook.dat");
	while (pos)
	{
		iItem = m_ctlPhones.GetNextSelectedItem(pos);
		strIP = m_ctlPhones.GetItemText(iItem, 0);

		strTxtFile = _T("phonebook_") + strIP + _T(".txt");
		strCmdLine = _T("tftp -i ") + strIP + _T(" get ") + strDatFile;
		SystemCall(strCmdLine);

		if (CFile::GetStatus(strDatFile, status))
		{
			strCmdLine = _T("convert -P -f ") + strDatFile + _T(" ") + strTxtFile;
			SystemCall(strCmdLine);
			ShellExecute(NULL, NULL, strTxtFile, NULL, NULL, SW_SHOW);
			status.m_attribute = status.m_attribute & 0x3e;
			CFile::SetStatus(strDatFile, status);
			CFile::Remove(strDatFile);
		}
	}
}

void CManagerDlg::OnSetPhonebook() 
{
	// TODO: Add your control notification handler code here
	CString strTxtFile = AfxGetApp()->GetProfileString(_T("SetPhonebook"), _T("SrcName"), _T("demo.txt"));
	CFileDialog dlgSet(TRUE, _T("*.txt"), strTxtFile, OFN_PATHMUSTEXIST, szTxtFilter);
	CString strCmdLine, strIP;
	int iItem;
	POSITION pos = m_ctlPhones.GetFirstSelectedItemPosition();
	CString strDatFile;
	CFileStatus status;

	SaveSettings();

	if (dlgSet.DoModal() != IDOK)
		return;
	strTxtFile = dlgSet.GetPathName();
	AfxGetApp()->WriteProfileString(_T("SetPhonebook"), _T("SrcName"), strTxtFile);

	SetCurrentDirectory((LPCTSTR)m_strCurDir);
	strDatFile = _T("phonebook.dat");
	strCmdLine = _T("convert -p -f ") + strTxtFile + _T(" ") + strDatFile;
	SystemCall(strCmdLine);
	while (pos)
	{
		iItem = m_ctlPhones.GetNextSelectedItem(pos);
		strIP = m_ctlPhones.GetItemText(iItem, 0);

		strCmdLine = _T("tftp -i ") + strIP + _T(" put ") + strDatFile;
		SystemCall(strCmdLine);
	}
	if (CFile::GetStatus(strDatFile ,status))
	{
		CFile::Remove(strDatFile);
	}
}

void CManagerDlg::OnClickListPhone(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iItem;
	CString strIP;
	POSITION pos = m_ctlPhones.GetFirstSelectedItemPosition();
	
	*pResult = 0;
	iItem = m_ctlPhones.GetNextSelectedItem(pos);
	strIP = m_ctlPhones.GetItemText(iItem, 0);
	ULONG uPeerIP = CUdpSocket::Name2IP(strIP);
	m_ctlPeerIP.SetAddress((UCHAR)uPeerIP&0xff, (UCHAR)((uPeerIP>>8)&0xff), (UCHAR)((uPeerIP>>16)&0xff), (UCHAR)((uPeerIP>>24)&0xff));
}

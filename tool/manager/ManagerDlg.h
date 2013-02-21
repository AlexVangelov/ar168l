// ManagerDlg.h : header file
//

#if !defined(AFX_MANAGERDLG_H__9B294037_0560_4AC9_A3B6_35DAA70E0E73__INCLUDED_)
#define AFX_MANAGERDLG_H__9B294037_0560_4AC9_A3B6_35DAA70E0E73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UdpSocket.h"
//#include "TcpSocket.h"
#include "UdpDebugThread.h"

/////////////////////////////////////////////////////////////////////////////
// CManagerDlg dialog

class CManagerDlg : public CDialog
{
// Construction
public:
	CManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CManagerDlg)
	enum { IDD = IDD_MANAGER_DIALOG };
	CIPAddressCtrl	m_ctlPeerIP;
	CListCtrl	m_ctlPhones;
	BOOL	m_bSelectedIPOnly;
	BOOL	m_bSyslogDataTime;
	BOOL	m_bSyslogDataType;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateProgram();
	afx_msg void OnGetOptions();
	afx_msg void OnSetOptions();
	afx_msg void OnStartDebug();
	afx_msg void OnGetPhonebook();
	afx_msg void OnSetPhonebook();
	afx_msg void OnClickListPhone(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
#if _MSC_VER >= 1300
	afx_msg LRESULT OnUdpData(WPARAM wParam, LPARAM lParam);
#else
	afx_msg void OnUdpData(WPARAM wParam, LPARAM lParam);
#endif
	DECLARE_MESSAGE_MAP()

protected:
	void AddPhone(CString strIP, CString strNum, CString strStatus, CString strVer, CString strModel);

	BOOL BeginUdpDebug();
	void EndUdpDebug();
	void EnableDebug(BOOL bEnable);
	void SaveSettings();

protected:
	BOOL m_bDebugging;
	CUdpSocket m_udp;
	UINT m_nTimer;
	CUdpDebugThread	* m_pUdpDebugThread;
	CString m_strCurDir;
	ULONG m_uPeerIP;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGERDLG_H__9B294037_0560_4AC9_A3B6_35DAA70E0E73__INCLUDED_)

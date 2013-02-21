#if !defined(AFX_HEXFILE_H__D5F10B81_A776_11D3_8AB6_5254AB104A0B__INCLUDED_)
#define AFX_HEXFILE_H__D5F10B81_A776_11D3_8AB6_5254AB104A0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HexFile.h : header file
//

#define HEXFILE_SIZE			65536

#define HEX_FILE_MAX_DATA	0x10

/////////////////////////////////////////////////////////////////////////////
// CHexFile class

class CHexFile : public CStdioFile
{
	DECLARE_DYNAMIC(CHexFile)
public:
	CHexFile();

	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags = CFile::modeRead|CFile::typeText, CFileException* pError = NULL);
	BOOL ReadLine();

	inline int GetNumber()	{	return m_iNumber;	}
	inline int GetAddress()	{	return m_iAddress;	}
	inline BYTE * GetData()	{	return m_cbData;	}

	inline CString GetLine()		{	return m_strLine;	}
	inline CString GetAddressStr()	{	return m_strAddress;	}
	inline CString GetDataStr()		{	return m_strData;	}

	static int Text2Bin(char cbHigh, char cbLow);

protected:
	CString m_strLine;

	CString m_strAddress;
	CString m_strData;
	int m_iNumber;
	int m_iAddress;
	BYTE m_cbData[HEX_FILE_MAX_DATA];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEXFILE_H__D5F10B81_A776_11D3_8AB6_5254AB104A0B__INCLUDED_)

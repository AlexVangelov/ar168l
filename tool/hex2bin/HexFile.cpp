// HexFile.cpp : implementation file
//

#include "stdafx.h"
#include "HexFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHexFile

IMPLEMENT_DYNAMIC(CHexFile, CStdioFile)

CHexFile::CHexFile() : CStdioFile()
{
}

BOOL CHexFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	return CStdioFile::Open(lpszFileName, nOpenFlags, pError);
}

BOOL CHexFile::ReadLine()
{
	if (!ReadString(m_strLine))		return FALSE;

	int i, iVal, iCheckSum = 0;
//	const char * p = LPCTSTR(m_strLine);
	char p[1024];
	WideCharToMultiByte(CP_ACP, 0, m_strLine, -1, p, 1024, NULL, NULL); 
	
	if (p[0] != ':' || p[7] != '0' || p[8] != '0')	return FALSE;

	m_iNumber = Text2Bin(p[1], p[2]);
	iCheckSum += m_iNumber;

	m_strAddress = m_strLine.Mid(3, 4);
	iVal = Text2Bin(p[3], p[4]);
	iCheckSum += iVal;
	m_iAddress = iVal;
	m_iAddress <<= 8;
	iVal = Text2Bin(p[5], p[6]);
	iCheckSum += iVal;
	m_iAddress += iVal;

	m_strData = m_strLine.Mid(9, m_iNumber * 2);
	for (i = 0; i < m_iNumber; i ++)
	{
		iVal = Text2Bin(p[9+i*2], p[10+i*2]);
		iCheckSum += iVal;
		m_cbData[i] = (BYTE)iVal;
	}
	
	iCheckSum &= 0xff;
	if (iCheckSum != 0)
	{
		iCheckSum = 256 - iCheckSum;
	}
	if (iCheckSum != Text2Bin(p[9+m_iNumber*2], p[10+m_iNumber*2]))		return FALSE;

	return TRUE;
}

int CHexFile::Text2Bin(char cbHigh, char cbLow)
{
	int iBin;

	if (cbHigh > '9')
	{
		cbHigh -= 'A';
		cbHigh += 10;
	}
	else
	{
		cbHigh -= '0';
	}
	iBin = cbHigh;
	iBin <<= 4;

	if (cbLow > '9')
	{
		cbLow -= 'A';
		cbLow += 10;
	}
	else
	{
		cbLow -= '0';
	}
	iBin += cbLow;
	return iBin;
}

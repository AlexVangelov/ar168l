// reversion.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void _RewriteVersion(CString strFileName, CString strVer, CString strCall, CString strRes, CString strOem);

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
#ifndef __GNUC__
CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		printf("Fatal Error: MFC initialization failed");
		nRetCode = 1;
	}
	else
	{
		CString strFileName, strVer, strCall, strRes, strOem;
		
		CommandLineDefault(argc, argv, strFileName, strVer, strCall, strRes, strOem);
		_RewriteVersion(strFileName, strVer, strCall, strRes, strOem);
	}

	return nRetCode;
}
#else
int main(int argc, char *argv[])
{
	int nRetCode = 0;

		CString strFileName, strVer, strCall, strRes, strOem;

		CommandLineDefault(argc, argv, strFileName, strVer, strCall, strRes, strOem);
		_RewriteVersion(strFileName, strVer, strCall, strRes, strOem);
	return nRetCode;
}
#endif
BOOL _MatchString(CString str, CString strCmp)
{
	int iLen, iVal;

	iLen = strCmp.GetLength();
	if (iLen < str.GetLength())
	{
		iVal = str.GetAt(iLen);
		if (iVal != _T(' ') && iVal != _T('\t'))	return FALSE;
	}
	if (str.Left(iLen) == strCmp)	return TRUE;
	return FALSE;
}

//#define
BOOL _ProcessString(CString & str, CString strCmpVer, CString strCmpCall, CString strCmpRes, CString strCmpOem)
{
	CString str1;

	if (str.Left(9) == _T("//#define"))
	{
		str1 = str.Right(str.GetLength() - 9);
		str1.TrimLeft();
		if (_MatchString(str1, strCmpVer) || _MatchString(str1, strCmpCall)
			|| _MatchString(str1, strCmpRes) || _MatchString(str1, strCmpOem))
		{
			str = str.Right(str.GetLength() - 2);
			return TRUE;
		}
	}
	else if (str.Left(7) == _T("#define"))
	{
		str1 = str.Right(str.GetLength() - 7);
		str1.TrimLeft();
		if (_MatchString(str1, strCmpVer) || _MatchString(str1, strCmpCall)
			|| _MatchString(str1, strCmpRes) || _MatchString(str1, strCmpOem))
		{
			return FALSE;
		}
		str = _T("//") + str;
		return TRUE;
	}

	return FALSE;
}

void _RewriteVersion(CString strFileName, CString strVer, CString strCall, CString strRes, CString strOem)
{
	CStdioFile file;
	CString strName;
	CString str;
	CString strCmpVer;
	CString strCmpCall;
	CString strCmpRes;
	CString strCmpOem;
	TCHAR szCurDir[128];
	CStringList list;
	POSITION pos, old;
	BOOL bChanged;

	printf("Palmmicro AR1688 reversion utility %d.%d%d.%d%d%d\n", SOFT_VER_HIGH, SOFT_VER_LOW, SOFT_VER_BUILD, ENG_BUILD_HIGH, ENG_BUILD_MID, ENG_BUILD_LOW);

	GetCurrentDirectory(128, szCurDir);
#ifndef __GNUC__
	strName.Format(_T("%s\\%s"), szCurDir, strFileName);
//	wprintf(_T("version file: %s\n"), strFileName);
#else
   {
      char buff[511];
      sprintf(buff, "%s/%s", szCurDir, strFileName.c_str());
      strName = buff;
   }
#endif
	strCmpVer = _T("VER_") + strVer;
	strCmpVer.MakeUpper();

	strCmpCall = _T("CALL_") + strCall;
	strCmpCall.MakeUpper();

	strCmpRes = _T("RES_") + strRes;
	strCmpRes.MakeUpper();

	strCmpOem = _T("OEM_") + strOem;
	strCmpOem.MakeUpper();

	// read first
	if (!file.Open(strName, CFile::modeRead|CFile::typeText))
	{
#ifndef __GNUC__
		wprintf(_T("Can not open file %s for read\n"), strName);
#else
      printf(_T("Can not open file %s for read\n"), strName.c_str());
#endif
		return;
	}
#ifndef __GNUC__
	while (file.ReadString(str))
	{
#else
   while (!file.eof())
   {
      CString str = "";
      std::getline(file, str);
      if (*str.rbegin() == '\r') str.erase(str.length() - 1);
#endif
		list.AddTail(str);
	}
	file.Close();

	// process
	bChanged = FALSE;
#ifndef __GNUC__
	for (pos = list.GetHeadPosition(); pos != NULL;)
#else
   for (pos=list.begin(); pos != list.end(); ++pos)
#endif
	{
		old = pos;
		str = list.GetNext(pos);
		if (str.Left(10) == _T("/*********"))
		{
			break;
		}
		if (_ProcessString(str, strCmpVer, strCmpCall, strCmpRes, strCmpOem))
		{
			list.SetAt(old, str);
			bChanged = TRUE;
		}
	}

	if (!bChanged)	return;		// no need to rewrite

	// write
	file.Open(strName, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
#ifndef __GNUC__
	while (!list.IsEmpty())
	{
		str = list.RemoveHead();
		str += _T("\n");
#else
   for(pos=list.begin(); pos!=list.end(); pos++)
   {
      str = *pos;
      str += _T("\r\n");
#endif
		file.WriteString(str);
	}
	file.Close();
}

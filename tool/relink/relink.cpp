// relink.cpp : Defines the entry point for the console application.
//

#ifndef __GNUC__
#include "stdafx.h"
#include "..\\common\\common.h"
#include "..\\..\\include\\version.h"
#else
#include "../common/mfc2std.h"
#include "../common/common.h"
#include "../../include/version.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void _RewriteLinkFile(CString strFileName, CString strVer, CString strCall, CString strRes, CString strOem);

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

//		printf("Palmmicro AR1688 relink utility 0.59.028\n");
		printf("Palmmicro AR1688 relink utility %d.%d%d.%d%d%d\n", SOFT_VER_HIGH, SOFT_VER_LOW, SOFT_VER_BUILD, ENG_BUILD_HIGH, ENG_BUILD_MID, ENG_BUILD_LOW);
		CommandLineDefault(argc, argv, strFileName, strVer, strCall, strRes, strOem);
		_RewriteLinkFile(strFileName, strVer, strCall, strRes, strOem);
	}

	return nRetCode;
}
#else
int main(int argc, char *argv[])
{
	int nRetCode = 0;

		CString strFileName, strVer, strCall, strRes, strOem;
		printf("Palmmicro AR1688 relink utility %d.%d%d.%d%d%d\n", SOFT_VER_HIGH, SOFT_VER_LOW, SOFT_VER_BUILD, ENG_BUILD_HIGH, ENG_BUILD_MID, ENG_BUILD_LOW);
		CommandLineDefault(argc, argv, strFileName, strVer, strCall, strRes, strOem);
		_RewriteLinkFile(strFileName, strVer, strCall, strRes, strOem);
	return nRetCode;
}
#endif

BOOL _ProcessString(CString & str, CString strVer, CString strCall, CString strRes, CString strOem)
{
	CString strNew;
	if (str.Left(7) != _T("..\\lib\\"))		return FALSE;

	if (str.Right(7) == _T("arp.rel"))
	{
		strNew = _T("..\\lib\\arp.rel");
		if (strVer == _T("ar168p") || strVer == _T("ar168km") || strVer == _T("ar168mk") || strVer == _T("ar168mt"))
		{
			strNew = _T("..\\lib\\ksz8842\\arp.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(8) == _T("chip.rel"))
	{
		strNew = _T("..\\lib\\chip.rel");
		if (strVer == _T("ar168k") || strVer == _T("ar168km") || strVer == _T("ar168m") || strVer == _T("ar168ms") || strVer == _T("ar168mk") || strVer == _T("ar168mt")
			|| strVer == _T("gp2266") || strVer == _T("bt2008n") || strVer == _T("fwv2800") || strVer == _T("dxdt") 
			|| strVer == _T("ar168p") || strVer == _T("ar168r"))
		{
			strNew = _T("..\\lib\\chip_plus\\chip.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(9) == _T("codec.rel"))
	{
		strNew = _T("..\\lib\\codec.rel");
		if (strVer == _T("ar168k") || strVer == _T("ar168km") || strVer == _T("ar168m") || strVer == _T("ar168ms") || strVer == _T("ar168mk") || strVer == _T("ar168mt") 
			|| strVer == _T("gp2266") || strVer == _T("bt2008n") || strVer == _T("fwv2800") || strVer == _T("dxdt") 
			|| strVer == _T("ar168p") || strVer == _T("ar168r"))
		{
			strNew = _T("..\\lib\\chip_plus\\codec.rel");
		}
		else if (strVer == _T("gp1266") || strVer == _T("bt2008") || strVer == _T("ar168j"))
		{
			strNew = _T("..\\lib\\dual_mic\\codec.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(7) == _T("dsp.rel"))
	{
		strNew = _T("..\\lib\\dsp.rel");
		if (strOem == _T("ivr"))
		{
			strNew = _T("..\\lib\\ivr\\dsp.rel");
		}
		else if (strVer == _T("ar168k") || strVer == _T("ar168km") || strVer == _T("ar168m") || strVer == _T("ar168ms") || strVer == _T("ar168mk") || strVer == _T("ar168mt")
			|| strVer == _T("gp2266") || strVer == _T("bt2008n") || strVer == _T("fwv2800") || strVer == _T("dxdt") 
			|| strVer == _T("ar168p") || strVer == _T("ar168r"))
		{
			strNew = _T("..\\lib\\chip_plus\\dsp.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(9) == _T("flash.rel"))
	{
		strNew = _T("..\\lib\\flash.rel");
		if (strVer == _T("ar168k") || strVer == _T("ar168m") || strVer == _T("ar168mt") || strVer == _T("ar168r"))
		{
			strNew = _T("..\\lib\\chip_plus\\flash.rel");
		}
		else if (strVer == _T("gp2266") || strVer == _T("bt2008n") || strVer == _T("fwv2800") || strVer == _T("dxdt") || strVer == _T("ar168km")
			|| strVer == _T("ar168ms") || strVer == _T("ar168mk") || strVer == _T("ar168p"))
		{
			strNew = _T("..\\lib\\sst\\flash.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(8) == _T("icmp.rel"))
	{
		strNew = _T("..\\lib\\icmp.rel");
		if (strVer == _T("ar168p") || strVer == _T("ar168km") || strVer == _T("ar168mk") || strVer == _T("ar168mt"))
		{
			strNew = _T("..\\lib\\ksz8842\\icmp.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(7) == _T("isr.rel"))
	{
		strNew = _T("..\\lib\\isr.rel");
		if (strOem == _T("uart"))
		{
			strNew = _T("..\\lib\\uart\\isr.rel");
		}
		else if (strOem == _T("roip"))
		{
			strNew = _T("..\\lib\\isr.rel");
		}
		else if (strVer == _T("ar168m") || strVer == _T("ar168ms") || strVer == _T("ar168mk") || strVer == _T("ar168mt"))
		{
			strNew = _T("..\\lib\\uart\\isr.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(12) == _T("network2.rel"))
	{
		strNew = _T("..\\lib\\network2.rel");
		if (strVer == _T("ar168p") || strVer == _T("ar168km") || strVer == _T("ar168mk") || strVer == _T("ar168mt"))
		{
			strNew = _T("..\\lib\\ksz8842\\network2.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(7) == _T("sys.rel"))
	{
		strNew = _T("..\\lib\\sys.rel");
		if (strVer == _T("gp2266") || strVer == _T("bt2008n") || strVer == _T("fwv2800") || strVer == _T("dxdt") || strVer == _T("ar168km")
			|| strVer == _T("ar168ms") || strVer == _T("ar168mk") || strVer == _T("ar168p"))
		{
			strNew = _T("..\\lib\\sst\\sys.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else if (str.Right(8) == _T("uart.rel"))
	{
		strNew = _T("..\\lib\\uart.rel");
		if (strOem == _T("roip"))
		{
		}
		else if (strOem == _T("uart") || strVer == _T("ar168m") || strVer == _T("ar168ms") || strVer == _T("ar168mk") || strVer == _T("ar168mt"))
		{
			strNew = _T("..\\lib\\uart\\uart.rel");
		}

		if (strNew == str)		return FALSE;
	}
	else
	{
		return FALSE;
	}

	str = strNew;
	return TRUE;
}

void _RewriteLinkFile(CString strFileName, CString strVer, CString strCall, CString strRes, CString strOem)
{
	CStdioFile file;
	CString strName;
	CString str;
	TCHAR szCurDir[128];
	CStringList list;
	POSITION pos, old;
	BOOL bChanged;

	GetCurrentDirectory(128, szCurDir);
#ifndef __GNUC__
	strName.Format(_T("%s\\%s"), szCurDir, strFileName);
#else
   {
      char buff[511];
	   sprintf(buff, "%s/%s", szCurDir, strFileName.c_str());
      strName = buff;
   }
#endif
	strVer.MakeLower();
	strCall.MakeLower();
	strRes.MakeLower();
	strOem.MakeLower();
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
		if (_ProcessString(str, strVer, strCall, strRes, strOem))
		{
			list.SetAt(old, str);
			bChanged = TRUE;
		}
	}
#ifndef __GNUC__
	if (!bChanged)	return;		// no need to rewrite
	// write
	file.Open(strName, CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	while (!list.IsEmpty())
	{
		str = list.RemoveHead();
		str += _T("\n");
#else
	file.Open(strName.Left(strName.length()-4)+"_unix.lnk",std::fstream::out|std::fstream::trunc);
   for(pos=list.begin(); pos!=list.end(); pos++)
   {
      str = *pos;
      str.FindAndReplace("\\","/");
      str += _T("\r\n");
#endif
		file.WriteString(str);
	}
	file.Close();
}

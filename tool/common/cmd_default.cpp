#ifndef __GNUC__
#include <stdafx.h>
#else
#include "../common/mfc2std.h"
#endif

void CommandLineDefault1(int argc, TCHAR* argv[], CString & strVer, CString & strCall, CString & strRes, CString & strOem)
{
	strVer = _T("ar168m");
	strCall = _T("sip");
	strRes = _T("us");
	strOem = _T("Unknown");
	if (argc > 1)	strVer = argv[1];
	if (argc > 2)	strCall = argv[2];
	if (argc > 3)	strRes = argv[3];
	if (argc > 4)	strOem = argv[4];
}

void CommandLineDefault(int argc, TCHAR* argv[], CString & strFileName, CString & strVer, CString & strCall, CString & strRes, CString & strOem)
{
	strFileName = _T("version.h");
	if (argc > 1)	strFileName = argv[1];
	CommandLineDefault1(argc-1, &argv[1], strVer, strCall, strRes, strOem);
}

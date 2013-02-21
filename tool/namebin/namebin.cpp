// namebin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\\..\\include\\ar168.h"

#include "..\\common\\common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//BOOL RunProcess(CString strCmdLine);

void ZipBin(CString strDstName);
void ReportCodeSize();
CString RenameBinary(CString strFileName, CString strVer, CString strCall, CString strRes, CString strOem);
void _SetFlag(char * p, CString strVal, int iMax);
BOOL _MergeFile(CString strCurDir, CString strSrcName, CString strDstName, CString strVer, CString strCall, CString strRes, CString strOem, CString strVersion);
void _SetFile(char * p, CString strVer, CString strCall, CString strRes, CString strOem, CString strVersion);

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

void PrintHelp()
{
	printf("%s\n", "namebin srcFile hardware [protocol] [country] [oem]");
	printf("%s\n", "srcFile\t\tSpecifies the source files");
	printf("%s\n", "hardware\tphone's hardware");
	printf("%s\n", "[protocol]\tPhone's protocol");
	printf("%s\n", "[country]\tPhone's language");
	printf("%s\n", "[oem]\t\tOEM. use 'size' to report code size, use 'zip' to zip output .bin file");
}

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
		printf("Palmmicro AR1688 namebin.exe utility 0.59.028\n");
		if (argc < 2)
		{
			PrintHelp();
		}
		else
		{
			CString strFileName, strVer, strCall, strRes, strOem, strDstName;
			
			CommandLineDefault(argc, argv, strFileName, strVer, strCall, strRes, strOem);
			strDstName = RenameBinary(strFileName, strVer, strCall, strRes, strOem);
			if (strOem == _T("size"))
			{
				ReportCodeSize();
			}
//			else if (strOem == _T("zip"))
			else if (strOem != _T("Unknown"))
			{
				ZipBin(strDstName);
			}
		}
	}

	return nRetCode;
}

bool _IsOem(CString strOem)
{
	if (strOem != _T("Unknown") && strOem != _T("size") && strOem != _T("zip"))
	{
		return true;
	}
	return false;
}

CString _GetVersion(CString strCurDir)
{
	CString strVersion = _T("Unknown");
	CStdioFile in;
	CString strIn;
	CString str;
	BOOL bStart;

	if (!in.Open(strCurDir.Left(strCurDir.GetLength() - 4) + _T("include\\version.h"), CFile::modeRead|CFile::typeText))
	{
		goto End;
	}

	strVersion = "";
	bStart = FALSE;
	while (in.ReadString(strIn))
	{
		if (strIn.Left(10) == _T("/*########"))
		{
			bStart = TRUE;
			continue;
		}
		if (!bStart) continue;

		if (strIn.Left(7) == _T("#define"))
		{
			str = strIn.Right(strIn.GetLength() - 7);
			str.TrimLeft();
			if (str.Left(13) == _T("SOFT_VER_HIGH"))
			{
				str = str.Right(str.GetLength() - 13);
				str.TrimLeft();
				strVersion += str;
			}
			else if (str.Left(12) == _T("SOFT_VER_LOW"))
			{
				str = str.Right(str.GetLength() - 12);
				str.TrimLeft();
				strVersion += str;
			}
			else if (str.Left(14) == _T("SOFT_VER_BUILD"))
			{
				str = str.Right(str.GetLength() - 14);
				str.TrimLeft();
				strVersion += str;
			}
			else if (str.Left(14) == _T("ENG_BUILD_HIGH"))
			{
				str = str.Right(str.GetLength() - 14);
				str.TrimLeft();
				strVersion += str;
			}
			else if (str.Left(13) == _T("ENG_BUILD_MID"))
			{
				str = str.Right(str.GetLength() - 13);
				str.TrimLeft();
				strVersion += str;
			}
			else if (str.Left(13) == _T("ENG_BUILD_LOW"))
			{
				str = str.Right(str.GetLength() - 13);
				str.TrimLeft();
				strVersion += str;
			}
		}
	}

	in.Close();

End:
	return strVersion;
}

void _SetFlag(char * p, CString strVal, int iMax)
{
//	int i, iLen;
	int iLen;

	iLen = strVal.GetLength();
	WideCharToMultiByte(CP_ACP, 0, strVal, -1, p, iMax, NULL, NULL); 
//	memcpy(p, strVal, iLen);
/*	for (i = 0; i < iLen; i ++)
	{
		p[i] = strVal.GetAt(i);
	}
*/
	memset((char *)(p + iLen), 0, iMax - iLen);
}

#define _SetStr	_SetFlag
/*
void _SetStr(char * p, CString strVal, int iMax)
{
	int iLen;

	iLen = strVal.GetLength();
	memcpy((char *)p, strVal, iLen);
	memset((char *)p+iLen, 0, iMax-iLen);
}
*/
void _SetFile(char * p, CString strVer, CString strCall, CString strRes, CString strOem, CString strVersion)
{
	int iLen;

	iLen = 0;
	_SetStr((char *)(p+iLen), strVer, MAX_FLAG_PRODUCT_LEN);
	iLen += MAX_FLAG_PRODUCT_LEN;
	_SetStr((char *)(p+iLen), strCall, MAX_FLAG_PROTOCOL);
	iLen += MAX_FLAG_PROTOCOL;
	_SetStr((char *)(p+iLen), strRes, MAX_FLAG_COUNTRY);
	iLen += MAX_FLAG_COUNTRY;
	if (_IsOem(strOem))
	{
		_SetStr((char *)(p+iLen), strOem, MAX_FLAG_OEM_LEN);
	}
	else
	{
		memset((char *)(p+iLen), 0, MAX_FLAG_OEM_LEN);
	}
	iLen += MAX_FLAG_OEM_LEN;
	_SetStr((char *)(p+iLen), strVersion, MAX_FLAG_VERSION);
}

BOOL _WriteHttpPage(char * p, CStringList & listSrcName)
{
	CFile in;
	CString strSrcName;
	POSITION pos;
	int iIndex, iAddr, iLen;

	iIndex = 0;
	iAddr = 64;
	for (pos = listSrcName.GetHeadPosition(); pos != NULL;)
	{
		strSrcName = listSrcName.GetNext(pos);
		if (!in.Open(strSrcName, CFile::modeRead))
		{
			return FALSE;
		}
		iLen = (int)in.GetLength();
		p[iIndex] = (BYTE)(iAddr>>8);
		p[iIndex+1] = (BYTE)iAddr;
		p[iIndex+2] = (BYTE)(iLen >> 8);
		p[iIndex+3] = (BYTE)iLen;

		in.Read((char *)(p + iAddr), iLen);
		iAddr += iLen;
		iIndex += 4;
		in.Close();
	}

	return TRUE;

}

#define DSP_FILE_NUM	12
const TCHAR * _cDspFiles[DSP_FILE_NUM] = 
{_T("g711.dat"), _T("gsm.dat"), _T("g729.dat"), _T("ilbc.dat"), _T("speex.dat"), 
 _T("spx_tab4k.dat"), _T("spx_tab6k.dat"), _T("spx_tab8k.dat"), _T("spx_tab11k.dat"), _T("spx_tab15k.dat"),
 _T("spx_tab18k.dat"), _T("spx_tab25k.dat")
};

BOOL _MergeFile(CString strCurDir, CString strSrcName, CString strDstName, CString strVer, CString strCall, CString strRes, CString strOem, CString strVersion)
{
	CFile out;
	CFile in;
	CStringList listSrcName;
	CString strResDir;
	CString strFileName;
	int i, iLength, iWriteLen;
	unsigned int iOffset = 0xffd0;
	char * p;
	char * pCur;
	CString strTempFile;
	CString strTempVer;
	CFileStatus status;
	
	if (!in.Open(strSrcName, CFile::modeRead))
	{
		wprintf(_T("\nUnable to open %s"), strSrcName);
		return FALSE;
	}
	if (!out.Open(strDstName, CFile::modeCreate|CFile::modeWrite))
	{
		wprintf(_T("\nUnable to create %s"), strDstName);
		in.Close();
		return FALSE;
	}

	iLength = (int)in.GetLength();
	p = (char *)malloc(iLength);
	in.Read(p, iLength);
	in.Close();

	for (i = 0; i < iLength; i += FILE_FULL_PAGE_SIZE)
	{
		_SetFile((char *)(p + iOffset), strVer, strCall, strRes, strOem, strVersion);
		iOffset += 0x010000;
	}

	out.Write(p, iLength);

	if (iLength > FILE_FULL_PAGE_SIZE)
	{	
		// add http data for main upgrade binary file
		strResDir = strCurDir + _T("res\\");
		strTempVer = strVer;
		strTempVer.MakeLower();
		if (strOem == _T("bt2008") || strTempVer == _T("bt2008") || strTempVer == _T("bt2008n"))
		{
			strResDir += _T("bt2008\\");
		}
		else if (strOem == _T("innomedia"))
		{
			strResDir += _T("innomedia\\");
		}
		else if (strOem == _T("ip20"))
		{
			strResDir += _T("ip20\\");
		}
		else if (strOem ==  _T("roip"))
		{
			strResDir += _T("roip\\");
		}
		strResDir += strRes + _T("\\");

		strTempFile = strResDir + _T("login.htm"); 
		if (!CFile::GetStatus(strTempFile, status))
		{	// use us http page if speical language not found
			strResDir = strCurDir + _T("res\\us\\");
		}

		listSrcName.AddTail(strResDir + _T("login.htm"));
		if (strCall == _T("sip"))
		{
			listSrcName.AddTail(strResDir + _T("settings_sip.htm"));
		}
		else
		{
			listSrcName.AddTail(strResDir + _T("settings_iax2.htm"));
		}
		listSrcName.AddTail(strResDir + _T("phonebook_head.htm"));
		listSrcName.AddTail(strResDir + _T("phonebook_tail.htm"));
		listSrcName.AddTail(strResDir + _T("upgrade.htm"));
		listSrcName.AddTail(strResDir + _T("success.htm"));
		listSrcName.AddTail(strResDir + _T("failure.htm"));
		listSrcName.AddTail(strResDir + _T("abort.htm"));
		listSrcName.AddTail(strResDir + _T("image.jpg"));
		listSrcName.AddTail(strResDir + _T("digitmap_head.htm"));
		listSrcName.AddTail(strResDir + _T("digitmap_tail.htm"));

		memset(p, 0, FILE_FULL_PAGE_SIZE);
		if (!_WriteHttpPage(p, listSrcName))
		{
			free(p);
			out.Close();
			CFile::Remove(strDstName);
			return FALSE;
		}
		_SetFile((char *)(p + FILE_FLAG_PAGE_SIZE), strVer, strCall, strRes, strOem, strVersion);
		out.Write(p, FILE_FULL_PAGE_SIZE);

		// add all dsp data for main upgrade binary file
		free(p);
		p = (char *)malloc(FILE_FULL_PAGE_SIZE * SYSTEM_DSP_PAGE_NUM);
		memset(p, 0, FILE_FULL_PAGE_SIZE * SYSTEM_DSP_PAGE_NUM);
		pCur = p;
		iOffset = 0;
		for (i = 0; i < DSP_FILE_NUM; i ++)
		{
			strFileName = strCurDir + _T("\\res\\") + _cDspFiles[i];
			if (!in.Open(strFileName, CFile::modeRead))
			{
				free(p);
				out.Close();
				CFile::Remove(strDstName);
				return FALSE;
			}
			iLength = (int)in.GetLength();
			while (iLength)
			{
				if ((iLength + iOffset) < FILE_FLAG_PAGE_SIZE)
				{
					in.Read(pCur, iLength);
					iOffset += iLength;
					pCur += iLength;
					iLength = 0;
				}
				else
				{
					iWriteLen = FILE_FLAG_PAGE_SIZE - iOffset;
					in.Read(pCur, iWriteLen);
					iLength -= iWriteLen;
					pCur += iWriteLen + (FILE_FULL_PAGE_SIZE - FILE_FLAG_PAGE_SIZE);
					iOffset = 0;
				}
			}
			in.Close();
		}
	
		pCur = p;

		for (i = 0; i < SYSTEM_DSP_PAGE_NUM; i ++)
		{
			_SetFile((char *)(pCur + FILE_FLAG_PAGE_SIZE), strVer, strCall, strRes, strOem, strVersion);
			pCur += FILE_FULL_PAGE_SIZE;
		}
		out.Write(p, FILE_FULL_PAGE_SIZE * SYSTEM_DSP_PAGE_NUM);
	}

	free(p);
	out.Close();
	return TRUE;
}

BOOLEAN _WriteFile(CFile& out, CString strFileName, int iLength)
{
	char * p;
	CFile in;

	p = (char *)malloc(iLength);
	memset(p, 0, iLength);
	if (!in.Open(strFileName, CFile::modeRead))
	{
		wprintf(_T("Unable to open %s\n"), strFileName);
		return FALSE;
	}
	in.Read(p, iLength);
	in.Close();
	out.Write(p, iLength);
	free(p);
	return TRUE;
}

CString RenameBinary(CString strFileName, CString strVer, CString strCall, CString strRes, CString strOem)
{
	CString strDstName, strCurDir, strSrcName;
	CString strAllName, strSettingName, strDigitmapName, strPhonebookName, strPage0Name, strVersion, strRingtoneName, strHoldmusicName, strFontName, strUpgradeName;
	CFileStatus status;
	CFile out;
	CFile in;
	int i, iLength;
	char * p;
	TCHAR szCurDir[128];
	CString strCmdLine, strSettingsFile, strDigitmapFile;

	GetCurrentDirectory(128, szCurDir);
	strSrcName.Format(_T("%s\\%s"), szCurDir, strFileName);
	strCurDir.Format(_T("%s\\"), szCurDir);

	strDstName = strCurDir + strVer + _T("_") + strCall + _T("_") + strRes + _T("_");
	if (_IsOem(strOem))
	{
		strDstName += strOem + _T("_");
	}
	strVersion = _GetVersion(strCurDir);
	strDstName += strVersion;

	strAllName = strDstName;
	strAllName += _T("_all.bin");
	strDstName += _T(".bin");
	
	if (CFile::GetStatus(strDstName, status))
	{
		CFile::Remove(strDstName);
	}
	strCall.MakeLower();

	// built page0 .bin
	if (strCall == _T("none"))
	{
		_MergeFile (strCurDir, strSrcName, strDstName, strVer, strCall, strRes, strOem, strVersion);
		goto End;
	}

	// built upgrade .bin
	if (!_MergeFile (strCurDir, strSrcName, strDstName, strVer, strCall, strRes, strOem, strVersion))
	{
		goto End;
	}

	// built _all.bin
	if (!out.Open(strAllName, CFile::modeCreate|CFile::modeWrite))
	{
		goto End;
	}

	// built page0 name
	strPage0Name = strCurDir + strVer + _T("_") + _T("none") + _T("_") + strRes + _T("_");
	if (_IsOem(strOem))
	{
		strPage0Name += strOem + _T("_");
	}
	strPage0Name += strVersion;
	strPage0Name += _T(".bin");
	if (!in.Open(strPage0Name, CFile::modeRead))
	{
		goto Error;
	}
	iLength = (int)in.GetLength();
	p = (char *)malloc(iLength);
	in.Read(p, iLength);
	in.Close();
	out.Write(p, iLength);
	free(p);

	// write settings_sip.dat or settings_iax2.dat
	strSettingsFile = strCurDir + _T("\\settings\\") + strVer + _T("_") + strCall + _T("_") + strRes;
	if (_IsOem(strOem))
	{
		strSettingsFile +=  _T("_") + strOem + _T(".txt");
	}
	else
		strSettingsFile += _T(".txt");

	if (!CFile::GetStatus(strSettingsFile, status))
	{	// use default settings if speical .txt not found
		strSettingsFile = strCurDir + _T("\\settings\\") + _T("default") + _T("_") + strCall + _T(".txt");
	}

	strCmdLine.Format(_T("..\\bin\\convert -c -f %s settings.dat"), strSettingsFile);
	_tsystem(strCmdLine);

	strSettingName = strCurDir + _T("settings.dat");
	if (!in.Open(strSettingName, CFile::modeRead))
	{
		wprintf(_T("Unable to open %s\n"), strSettingName);
		goto Error;
	}
	iLength = (int)in.GetLength();
	p = (char *)malloc(iLength);
	in.Read(p, iLength);
	in.Close();

	// store settings contents
	_SetFlag((char *)(p + OPT_FLAG_PRODUCT), strVer, MAX_FLAG_PRODUCT_LEN);
	_SetFlag((char *)(p + OPT_FLAG_PROTOCOL), strCall, MAX_FLAG_PROTOCOL);
	_SetFlag((char *)(p + OPT_FLAG_COUNTRY), strRes, MAX_FLAG_COUNTRY);
	if (_IsOem(strOem))
	{
		_SetFlag((char *)(p + OPT_FLAG_OEM), strOem, MAX_FLAG_OEM_LEN);
	}
	else
	{
		memset((char *)(p + OPT_FLAG_OEM), 0, MAX_FLAG_OEM_LEN);
	}
	_SetFlag((char *)(p + OPT_FLAG_VERSION), strVersion, MAX_FLAG_VERSION);
	out.Write(p, iLength);
	free(p);

	// write default settings
	strSettingsFile = strCurDir + _T("\\settings\\") + _T("default") + _T("_") + strCall + _T(".txt");
	strCmdLine.Format(_T("..\\bin\\convert -c -f %s settings.dat"), strSettingsFile);
	_tsystem(strCmdLine);
	strSettingName = strCurDir + _T("settings.dat");
	if (!in.Open(strSettingName, CFile::modeRead))
	{
		wprintf(_T("Unable to open %s\n"), strSettingName);
		goto Error;
	}
	iLength = (int)in.GetLength();
	p = (char *)malloc(iLength);
	in.Read(p, iLength);
	in.Close();

	// store settings contents
	_SetFlag((char *)(p + OPT_FLAG_PRODUCT), strVer, MAX_FLAG_PRODUCT_LEN);
	_SetFlag((char *)(p + OPT_FLAG_PROTOCOL), strCall, MAX_FLAG_PROTOCOL);
	_SetFlag((char *)(p + OPT_FLAG_COUNTRY), strRes, MAX_FLAG_COUNTRY);
	if (_IsOem(strOem))
	{
		_SetFlag((char *)(p + OPT_FLAG_OEM), strOem, MAX_FLAG_OEM_LEN);
	}
	else
	{
		memset((char *)(p + OPT_FLAG_OEM), 0, MAX_FLAG_OEM_LEN);
	}
	_SetFlag((char *)(p + OPT_FLAG_VERSION), strVersion, MAX_FLAG_VERSION);
	out.Write(p, iLength);
	free(p);

	// Write phonebook.dat
	strPhonebookName = strCurDir + _T("\\res\\") + _T("phonebook.dat");
	if (!_WriteFile(out, strPhonebookName, PHONEBOOK_FILE_SIZE))
	{
		goto Error;
	}

	// write ringtone.dat
	strRingtoneName = strCurDir + _T("\\res\\") + _T("ringtone.dat");
	if (!_WriteFile(out, strRingtoneName, SYSTEM_RINGTONE_PAGE_NUM * FILE_FULL_PAGE_SIZE))
	{
		goto Error;
	}

	// write ringtone_hold.dat
	strHoldmusicName = strCurDir + _T("\\res\\") + _T("holdmusic.dat");
	if (!_WriteFile(out, strHoldmusicName, SYSTEM_HOLDMUSIC_PAGE_NUM * FILE_FULL_PAGE_SIZE))
	{
		goto Error;
	}

	// write font.dat or ivr_xx.dat
	if (strOem == _T("ivr"))
	{
		strFontName = strCurDir + _T("\\res\\") + _T("ivr_") + strRes + _T(".dat");
	}
	else
	{
		strFontName = strCurDir + _T("\\res\\") + _T("font.dat");
	}
	if (!_WriteFile(out, strFontName, SYSTEM_FONT_PAGE_NUM * FILE_FULL_PAGE_SIZE))
	{
		goto Error;
	}

	// write upgrade .bin
	for (i = 0; i < 2; i ++)
	{
		if (!_WriteFile(out, strDstName, SYSTEM_PROGRAM_PAGE_NUM * FILE_FULL_PAGE_SIZE))
		{
			goto Error;
		}
	}

	out.Close();
End:
	return strDstName;

Error:
	out.Close();
	CFile::Remove(strAllName);
	return strDstName;
}

void ReportCodeSize()
{
	CStdioFile f;
	CStringList list;
	POSITION pos;
	CString str;
	bool bFound;
	int iTotal;

	// read file
	if (!f.Open(_T("main.map"), CFile::modeRead|CFile::typeText))
	{
		printf("Can not read file main.map\n");
	}

// _CODE7                 00008000    000052FC =       21244. bytes (REL,CON)
	while (f.ReadString(str))
	{
		if ((str.Left(5) == _T("_CODE")) && (str.Right(9) == _T("(REL,CON)")))
		{
			bFound = false;
			for (pos = list.GetHeadPosition(); pos != NULL;)
			{
				if (str == list.GetNext(pos))
				{
					bFound = true;
					break;
				}
			}
			if (bFound == false)
			{
				list.AddTail(str);
			}
		}
	}

	iTotal = 0;
	for (pos = list.GetHeadPosition(); pos != NULL;)
	{
		str = list.GetNext(pos);
//		wprintf(_T("%s\n"), str);
		str = str.Right(22);
		str = str.Left(5);
		//wprintf(_T("%s\n"), str);
		iTotal += _wtoi(str);
	}
	printf("Total code size: %d bytes\n", iTotal);
	f.Close();
}

void ZipBin(CString strDstName)
{
	CString strZipName;
	CString strCmdLine;

	strZipName = strDstName.Left(strDstName.GetLength() - 3)  + _T("zip");

	// zip .bin file
	strCmdLine.Format(_T("..\\bin\\7z a %s %s"), strZipName, strDstName);
	_tsystem(strCmdLine);
}


#include "stdafx.h"
#include "Convert.h"
#include "CovertStr.h"

#include "..\\common\\common.h"

void OnBin2Cfg(CString strSrcFile, CString strDstFile, BOOL bForce, BOOL bInnomedia) 
{
	CFile in;
	CStdioFile out;
	CString	cStr;
	char * p;

	if (!PromptOverwriteFile(strDstFile, bForce))	return;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
		printf("Can not open source file %s", strSrcFile);
		return;
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
	{
		printf("Can not create destination file %s", strDstFile);
		in.Close();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	p = (char *)malloc(SETTINGS_FILE_SIZE);
	in.Read(p, SETTINGS_FILE_SIZE);

	if (bInnomedia)
	{
		AR168ConvertCfg_IM((char *)p, cStr);
	}
	else
	{
		AR168ConvertCfg((char *)p, cStr);
	}

	out.WriteString(cStr);

	AfxGetApp()->EndWaitCursor();

	free(p);
	in.Close();
	out.Close();
}

void OnCfg2Bin(CString strSrcFile, CString strDstFile, BOOL bForce) 
{
	CFile out;
	CStdioFile in;
	CString	strRead;
	char * p;
	BOOL bSettings = TRUE;
	int iItem;

	if (!PromptOverwriteFile(strDstFile, bForce))	return;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
		printf("Can not open source file %s", strSrcFile);
		return;
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite))
	{
		printf("Can not create destination file %s", strDstFile);
		in.Close();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	p = (char *)malloc(SETTINGS_FILE_SIZE);
	memset(p, 0, SETTINGS_FILE_SIZE);
	iItem = 0;

	while (in.ReadString(strRead))
	{
		if (!strRead.GetLength())
		{
			continue;
		}
		if (strRead.GetAt(0) == '[')
		{
			strRead.MakeLower();
			if (strRead.Left(10) == "[settings]")
			{
				bSettings = TRUE;
			}
			else if (strRead.Left(10) == "[digitmap]")
			{
				bSettings = FALSE;
			}
			continue;
		}
		else if (strRead.Left(12) == "ENDOFCONFIG=")
		{
			break;
		}
		else
		{
			if (bSettings)
			{
				AR168CompareCfg((char *)p, strRead);
			}
			else
			{
				if (iItem < DIGITMAP_ENTRY_NUM)
				{
					if (AR168CompareMap((char *)(p + OPT_DIGIT_MAP), strRead, iItem))
					{
						iItem++;					
					}
				}
			}
		}
	}

	out.Write(p, SETTINGS_FILE_SIZE);

	AfxGetApp()->EndWaitCursor();
	free(p);
	in.Close();
	out.Close();
}

void OnBin2H(CString strSrcFile, CString strDstFile, BOOL bForce, int iAddressOffset, int iConvertLength)
{
	CFile in;
	CStdioFile out;
	CString	strAnswer, strName;
	CString strOut, strItem;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	char * p;
	int iLength, i, j;

	if (!PromptOverwriteFile(strDstFile, bForce))	return;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
		printf("Can not open source file %s", strSrcFile);
		return;
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite))
	{
		printf("Can not create destination file %s", strDstFile);
		in.Close();
		return;
	}

	iLength = (int)in.GetLength();
	p = (char *)malloc(iLength);
	in.Read(p, iLength);
	if (iConvertLength == 0)
	{
		iConvertLength = iLength;
	}

	_wsplitpath(strDstFile, drive, dir, fname, ext);
	strName = fname;
	strName.MakeUpper();

	strOut.Format(_T("#define DATA_%s_SIZE\t\t%d\n"), strName, iConvertLength);
	out.WriteString(strOut);
	strOut.Format(_T("const unsigned char _c_%s[DATA_%s_SIZE] = {\n"), fname, strName);
	out.WriteString(strOut);

	for (i = iAddressOffset; i < iAddressOffset+iConvertLength; i += FRAME_LEN)
	{
		strOut = _T("");
		for (j = 0; j < FRAME_LEN; j ++)
		{
			if (i+j < iAddressOffset+iConvertLength)
			{
				strItem.Format(_T("0x%02x"), (unsigned char)p[i+j]);
				strOut += strItem;
			}
			if (i+j < ((iAddressOffset+iConvertLength) - 1))
			{
				strOut += _T(", ");
			}
		}
		strOut += _T("\n");
		out.WriteString(strOut);
	}

	strOut.Format(_T("};\n"));
	out.WriteString(strOut);
	
	free(p);
	in.Close();
	out.Close();
}

void OnRing2Bin(CString strSrcFile, CString strDstFile, BOOL bForce) 
{
	CFile in, out;
//	CStdioFile in;
	CString	strRead;
	char * pRead;
	char * pWrite;
	int i, iAddressOffset;

	if (!PromptOverwriteFile(strDstFile, bForce))	return;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
		printf("Can not open source file %s", strSrcFile);
		return;
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite))
	{
		printf("Can not create destination file %s", strDstFile);
		in.Close();
		return;
	}

	pRead = (char *)malloc(DATA_RING_SIZE);
	in.Read(pRead, DATA_RING_SIZE);

	pWrite = (char *)malloc(DATA_RING_SIZE);
	memset(pWrite, 0, DATA_RING_SIZE);

	iAddressOffset = 0;
	for (i = 0; i < DATA_RING_SIZE / (DATA_BLOCK_LEN + 128); i ++)
	{
		memcpy(pWrite + iAddressOffset, pRead + i * DATA_BLOCK_LEN, DATA_BLOCK_LEN);
		iAddressOffset += FILE_HALF_PAGE_SIZE;
	}

	out.Write(pWrite, DATA_RING_SIZE);

	free(pRead);
	free(pWrite);
	in.Close();
	out.Close();
	
}

void OnIvr2Bin(CString strSrcFile, CString strDstFile, BOOL bForce) 
{
	CFile in, out;
	CString	strRead;
	char * pRead;
	char * pWrite;
	int iOffset, iInLen, iOutLen, iLeftLen;

	if (!PromptOverwriteFile(strDstFile, bForce))	return;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
		printf("Can not open source file %s", strSrcFile);
		return;
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		printf("Can not create destination file %s", strDstFile);
		in.Close();
		return;
	}

	iInLen = (int)in.GetLength();
	pRead = (char *)malloc(iInLen);
	in.Read(pRead, iInLen);

	pWrite = (char *)malloc(IVR_FILE_SIZE);
	memset(pWrite, 0, IVR_FILE_SIZE);

	iOffset = 0;
	iOutLen = 0;
	while (iOffset < iInLen)
	{
		iLeftLen = iInLen - iOffset;
		if (!iLeftLen)	break;
		if (iLeftLen > IVR_PAGE_SIZE)
		{
			iLeftLen = IVR_PAGE_SIZE;
		}
		if ((iOutLen + iLeftLen) > IVR_FILE_SIZE)
		{
			break;
		}
		memcpy((char *)(pWrite + iOutLen), (char *)(pRead + iOffset), iLeftLen);
		iOutLen += FILE_HALF_PAGE_SIZE;
		iOffset += iLeftLen;
	}
	out.Write(pWrite, IVR_FILE_SIZE);

	free(pRead);
	free(pWrite);
	in.Close();
	out.Close();
}

void OnPB2Bin(CString strSrcFile, CString strDstFile, BOOL bForce) 
{
	CFile out;
	CStdioFile in;
	CString	strRead;
	char * p;

	if (!PromptOverwriteFile(strDstFile, bForce))	return;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
		printf("Can not open source file %s", strSrcFile);
		return;
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite))
	{
		printf("Can not create destination file %s", strDstFile);
		in.Close();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	p = (char *)malloc(PHONEBOOK_FILE_SIZE);
	memset(p, 0, PHONEBOOK_FILE_SIZE);

	while (in.ReadString(strRead))
	{
		if (!strRead.GetLength())
		{
			continue;
		}
		if (strRead.Left(11) == "[phonebook]")
		{
			continue;
		}
		AR168ComparePB((char *)p, strRead);
	}
	out.Write(p, PHONEBOOK_FILE_SIZE);

	AfxGetApp()->EndWaitCursor();
	free(p);
	in.Close();
	out.Close();
}

void OnBin2PB(CString strSrcFile, CString strDstFile, BOOL bForce) 
{
	CStdioFile in, out;
	CString	cStr;
	char * p;

	if (!PromptOverwriteFile(strDstFile, bForce))	return;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
		printf("Can not open source file %s", strSrcFile);
		return;
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
	{
		printf("Can not create destination file %s", strDstFile);
		in.Close();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	p = (char *)malloc(PHONEBOOK_FILE_SIZE);
	in.Read(p, PHONEBOOK_FILE_SIZE);

	AR168ConvertPB((char *)p, cStr);

	out.WriteString(cStr);

	AfxGetApp()->EndWaitCursor();

	free(p);
	in.Close();
	out.Close();
}

void OnDsp2H(CStringList& strFileList, CString strSrcDir, CString strDstDir, BOOL bForce)
{
	CString strSrcFile, strDstFile, strTemp, strVal, strOut;;
	CStdioFile out;
	CFileStatus status;
	POSITION pos;
	int iDot, iOffset, iPage, iLength;

	strDstFile = strDstDir + strFileList.RemoveTail();

	if (!PromptOverwriteFile(strDstFile, bForce))	return;
	CFile::GetStatus(strDstFile, status);

	iOffset = 0;
	iPage = 0;
	strOut = _T("");
	for (pos = strFileList.GetHeadPosition(); pos != NULL; )
	{
		strTemp = strFileList.GetNext(pos);
		strSrcFile = strSrcDir + strTemp;
		if (!CFile::GetStatus(strSrcFile, status))
		{
			printf("Source file %s doesn't exist", strSrcFile);
			return;
		}
		iLength = (int)status.m_size;
		iDot = strTemp.Find(_T('.'));
		strTemp = strTemp.Left(iDot);
		strTemp.MakeUpper();
		strVal.Format(_T("#define SYSTEM_%s_PAGE\t\t(SYSTEM_DSP_PAGE + %d)\n"), strTemp, iPage);
		strOut += strVal;
		strVal.Format(_T("#define SYSTEM_%s_OFFSET\t\t0x%x\n"), strTemp, iOffset);
		strOut += strVal;
		iOffset += iLength;
		while (iOffset >= FILE_FLAG_PAGE_SIZE)
		{
			iOffset -= FILE_FLAG_PAGE_SIZE;
			iPage ++;
			if (iPage >= SYSTEM_DSP_PAGE_NUM)
			{
				printf("DSP dat files too large!");
				return;
			}
		}
	}

	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
	{
		printf("Can not create destination file %s", strDstFile);
		return;
	}
	out.WriteString(strOut);
	out.Close();
}

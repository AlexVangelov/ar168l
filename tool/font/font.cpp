// font.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void MainProcess(int argc, TCHAR* argv[]);

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
#ifndef __GNUC__
CWinApp theApp;
#endif
using namespace std;

void PrintHelp()
{
	printf("font.exe fontFile [-a] [-c] [-d] [i] [-t5|6|7|8|9]\n");
	printf("fontFile\tfont file name\n");
	printf("-a\t indicat the font file is 8x16 ascii (16x16 chinese by default)\n");
	printf("-c\t convert font file to vertical format\n");
	printf("-d\t display font file\n");
	printf("-i\t generate ISO-8859 font file from array.txt\n");
	printf("-t\t convert text font file to array.txt, (2x16)5-5x8, (dot-matrix)6-5x8, 7-7x14, 8-8x13, 9-18x18\n");
}

#ifndef __GNUC__
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
#else
int main(int argc, char *argv[])
{
   int nRetCode = 0;
   {
#endif
		printf("Palmmicro AR1688 font utility %d.%d%d.%d%d%d\n", SOFT_VER_HIGH, SOFT_VER_LOW, SOFT_VER_BUILD, ENG_BUILD_HIGH, ENG_BUILD_MID, ENG_BUILD_LOW);
		if (argc < 2)
		{
			PrintHelp();
		}
		else
		{
			MainProcess(argc, argv);
		}
	}

	return nRetCode;
}

#define FONT_SIZE		32
#define FONT_PAGE		87
#define FONT_PAGE_SIZE	94

#define FONT_BIN_SIZE	262144

#define ASCII_FONT_SIZE			16
#define ASCII_FONT_PAGE			4
#define ASCII_FONT_PAGE_SIZE	64

#define ASCII_FONT_BIN_SIZE		4096


int iMask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void ConvertSingleFont(char * pSrc, char * pDst)
{
	int i, j;
	char pBuf[FONT_SIZE][8];

	for (i = 0; i < FONT_SIZE; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			pBuf[i][j] = (pSrc[i] & iMask[j]) ? 1 : 0;
		}
	}

	memset(pDst, 0, FONT_SIZE);

	for (i = 0; i < 8; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			if (pBuf[j*2][i])
			{
				pDst[i] += iMask[7-j];
			}
		}
	}

	for (i = 8; i < 16; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			if (pBuf[j*2+1][i-8])
			{
				pDst[i] += iMask[7-j];
			}
		}
	}

	for (i = 16; i < 24; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			if (pBuf[16+j*2][i-16])
			{
				pDst[i] += iMask[7-j];
			}
		}
	}

	for (i = 24; i < 32; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			if (pBuf[16+j*2+1][i-24])
			{
				pDst[i] += iMask[7-j];
			}
		}
	}

//	memcpy(pDst, pSrc, FONT_SIZE);
}

void ConvertFont(char * p)
{
	int i;
	char * pNew;
	CFile font;

	pNew = (char *)malloc(FONT_BIN_SIZE);
	if (pNew == NULL)
	{
		printf("malloc convert buffer failed");
		return;
	}

	memset(pNew, 0, FONT_BIN_SIZE);

	for (i = 0; i < FONT_PAGE*FONT_PAGE_SIZE*FONT_SIZE; i += FONT_SIZE)
	{
		ConvertSingleFont((char *)(p+i), (char *)(pNew+i));
	}

	if (!font.Open(_T("font.dat"), CFile::modeCreate|CFile::modeWrite))
	{
		printf("Can not create font file");
		free(pNew);
		return;
	}

	font.Write(pNew, FONT_BIN_SIZE);
	font.Close();

	free(pNew);
}

void ConvertSingle5x8AsciiFont(char * pSrc, char * pDst)
{
	int i, j;
	char pBuf[8][8];

	for (i = 0; i < 8; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			pBuf[i][j] = (pSrc[i] & iMask[j]) ? 1 : 0;
		}
	}

	memset(pDst, 0, 6);

	for (i = 0; i < 6; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			if (pBuf[j][i])
			{
				pDst[i] += iMask[7-j];
			}
		}
	}
}


void ConvertSingleAsciiFont(char * pSrc, char * pDst)
{
	int i, j;
	char pBuf[ASCII_FONT_SIZE][8];

	for (i = 0; i < ASCII_FONT_SIZE; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			pBuf[i][j] = (pSrc[i] & iMask[j]) ? 1 : 0;
		}
	}

	memset(pDst, 0, ASCII_FONT_SIZE);

	for (i = 0; i < 8; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			if (pBuf[j][i])
			{
				pDst[i] += iMask[7-j];
			}
		}
	}

	for (i = 8; i < 16; i ++)
	{
		for (j = 0; j < 8; j ++)
		{
			if (pBuf[8+j][i-8])
			{
				pDst[i] += iMask[7-j];
			}
		}
	}
}

void ConvertAsciiFont(char * p)
{
	int i;
	char * pNew;
	CFile font;

	pNew = (char *)malloc(ASCII_FONT_BIN_SIZE);
	if (pNew == NULL)
	{
		printf("malloc convert buffer failed");
		return;
	}

	memset(pNew, 0, ASCII_FONT_BIN_SIZE);

	for (i = 0; i < ASCII_FONT_PAGE*ASCII_FONT_PAGE_SIZE*ASCII_FONT_SIZE; i += ASCII_FONT_SIZE)
	{
		ConvertSingleAsciiFont((char *)(p+i), (char *)(pNew+i));
	}

	if (!font.Open(_T("ascii.dat"), CFile::modeCreate|CFile::modeWrite))
	{
		printf("Can not create font file");
		free(pNew);
		return;
	}

	font.Write(pNew, ASCII_FONT_BIN_SIZE);
	font.Close();

	free(pNew);
}


void ProcessTextFile(CString strSrcFile, CString strDstFile, int iTextFont)
{
	CStdioFile in, out;
	CFile bin;
	CString strLine, strChar, strEncoding;
	int i, iLength, iLong, iTotal;
	unsigned char iVal[32], iVal2[32];

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
#ifndef __GNUC__
		wprintf(_T("Can not open source file %s"), strSrcFile);
#else
		printf("Can not open source file %s", strSrcFile.c_str());
#endif
		return;
	}
#ifndef __GNUC__
	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite))
	{
		wprintf(_T("Can not create destination file %s"), strDstFile);
#else
	if (!out.Open(strDstFile,std::fstream::out|std::fstream::trunc))
	{
		printf("Can not create destination file %s", strDstFile.c_str());
#endif
		in.Close();
		return;
	}

	if (iTextFont == '9')		// 18x18 unicode
	{
		if (!bin.Open(_T("font.bin"), CFile::modeCreate|CFile::modeWrite))
		{
			printf("Can not create font.bin");
			in.Close();
			out.Close();
			return;
		}
		iTotal = 0;
	}

#ifndef __GNUC__
	while (in.ReadString(strLine))
	{
#else
	while (!in.eof())
	{
		std::getline(in, strLine);
#endif
		iLength = strLine.GetLength();
		if (iLength > 9 && strLine.Left(9) == "STARTCHAR")
		{
			strChar = strLine.Right(iLength - 10);
#ifndef __GNUC__
			if (!in.ReadString(strLine))	break;			// "ENCODING "
#else
			if (!std::getline(in, strLine)) break;
#endif
			strEncoding = strLine.Right(strLine.GetLength() - 9);
#ifndef __GNUC__
			if (!in.ReadString(strLine))	break;			// "SWIDTH "
			if (!in.ReadString(strLine))	break;			// "DWIDTH "
			if (!in.ReadString(strLine))	break;			// "BBX "
			if (!in.ReadString(strLine))	break;			// "BITMAP"
#else
			if (!std::getline(in, strLine)) break;
			if (!std::getline(in, strLine)) break;
			if (!std::getline(in, strLine)) break;
			if (!std::getline(in, strLine)) break;
#endif	
			if (iTextFont == '5')	// 5x8 ascii
			{
				for (i = 0; i < 8; i ++)
				{
#ifndef __GNUC__
					if (!in.ReadString(strLine))	break;
					swscanf_s(strLine, _T("%02x"), &iVal[i]);
#else
					if (!std::getline(in, strLine)) break;
					sscanf(strLine, _T("%02hhx"), &iVal[i]);
#endif
					iVal[i] >>= 3;
				}
#ifndef __GNUC__
				strLine.Format(_T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal[0], iVal[1], iVal[2], iVal[3], iVal[4], iVal[5], iVal[6], iVal[7], strEncoding, strChar);
#else
			 {
				  char buff[1023];
					sprintf(buff, _T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
						iVal[0], iVal[1], iVal[2], iVal[3], iVal[4], iVal[5], iVal[6], iVal[7], strEncoding.c_str(), strChar.c_str());
				  strLine = buff;
			 }
#endif
			}
			else if (iTextFont == '6')	// 5x8 ascii for dot-matrix display
			{
				for (i = 0; i < 8; i ++)
				{
#ifndef __GNUC__
					if (!in.ReadString(strLine))	break;
					swscanf_s(strLine, _T("%02x"), &iVal[i]);
#else
					if (!std::getline(in, strLine)) break;
					sscanf(strLine, _T("%02hhx"), &iVal[i]);
#endif
//					iVal[i] >>= 3;
				}
				ConvertSingle5x8AsciiFont((char *)iVal, (char *)iVal2);
#ifndef __GNUC__
				strLine.Format(_T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], strEncoding, strChar);
#else
			 {
				  char buff[1023];
					sprintf(buff, _T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
						iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], strEncoding.c_str(), strChar.c_str());
				  strLine = buff;
			 }
#endif
			}
			else if (iTextFont == '7')	// 7x14 ascii
			{
				iVal[0] = 0;
				iVal[15] = 0;
				for (i = 1; i < 15; i ++)
				{
#ifndef __GNUC__
					if (!in.ReadString(strLine))	break;
					swscanf_s(strLine, _T("%02x"), &iVal[i]);
#else
					if (!std::getline(in, strLine)) break;
					sscanf(strLine, _T("%02hhx"), &iVal[i]);
#endif
				}
				ConvertSingleAsciiFont((char *)iVal, (char *)iVal2);
#ifndef __GNUC__
				strLine.Format(_T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], iVal2[6], iVal2[7], iVal2[8], iVal2[9], iVal2[10], iVal2[11], iVal2[12], iVal2[13], iVal2[14], iVal2[15], strEncoding, strChar);
#else
			 {
				  char buff[1023];
					sprintf(buff, _T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], iVal2[6], iVal2[7], iVal2[8], iVal2[9], iVal2[10], iVal2[11], iVal2[12], iVal2[13], iVal2[14], iVal2[15], strEncoding.c_str(), strChar.c_str());
				  strLine = buff;
			 }
#endif
			}
			else if (iTextFont == '8')	// 8x13 ascii
			{
				iVal[0] = 0;
				iVal[14] = 0;
				iVal[15] = 0;
				for (i = 1; i < 14; i ++)
				{
#ifndef __GNUC__
					if (!in.ReadString(strLine))	break;
					swscanf_s(strLine, _T("%02x"), &iVal[i]);
#else
					if (!std::getline(in, strLine)) break;
					sscanf(strLine, _T("%02hhx"), &iVal[i]);
#endif
				}
				ConvertSingleAsciiFont((char *)iVal, (char *)iVal2);
#ifndef __GNUC__
				strLine.Format(_T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], iVal2[6], iVal2[7], iVal2[8], iVal2[9], iVal2[10], iVal2[11], iVal2[12], iVal2[13], iVal2[14], iVal2[15], strEncoding, strChar);
#else
			 {
				  char buff[1023];
					sprintf(buff, _T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], iVal2[6], iVal2[7], iVal2[8], iVal2[9], iVal2[10], iVal2[11], iVal2[12], iVal2[13], iVal2[14], iVal2[15], strEncoding.c_str(), strChar.c_str());
				  strLine = buff;
			 }
#endif
			}
			else if (iTextFont == '9')	// 18x18 unicode
			{
#ifndef __GNUC__
				if (!in.ReadString(strLine))	break;			// skip first line
#else
				if (!std::getline(in, strLine)) break;
#endif
				for (i = 0; i < 16; i ++)
				{
#ifndef __GNUC__
					if (!in.ReadString(strLine))	break;
#else
					if (!std::getline(in, strLine)) break;
#endif
					swscanf_s(strLine, _T("%06x"), &iLong);
					iLong >>= 7;
					iVal[i * 2] = (unsigned char)(iLong >> 8);
					iVal[i * 2 + 1] = (unsigned char)iLong;
				}
#ifndef __GNUC__
				if (!in.ReadString(strLine))	break;			// skip last line
#else
				if (!std::getline(in, strLine)) break;
#endif
				ConvertSingleFont((char *)iVal, (char *)iVal2);
#ifndef __GNUC__
				strLine.Format(_T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], iVal2[6], iVal2[7], iVal2[8], iVal2[9], iVal2[10], iVal2[11], iVal2[12], iVal2[13], iVal2[14], iVal2[15],
					iVal2[16], iVal2[17], iVal2[18], iVal2[19], iVal2[20], iVal2[21], iVal2[22], iVal2[23], iVal2[24], iVal2[25], iVal2[26], iVal2[27], iVal2[28], iVal2[29], iVal2[30], iVal2[31], strEncoding, strChar);
#else
			 {
				  char buff[1023];
					sprintf(buff, _T("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, //\t%s\t%s\n"), 
					iVal2[0], iVal2[1], iVal2[2], iVal2[3], iVal2[4], iVal2[5], iVal2[6], iVal2[7], iVal2[8], iVal2[9], iVal2[10], iVal2[11], iVal2[12], iVal2[13], iVal2[14], iVal2[15],
					iVal2[16], iVal2[17], iVal2[18], iVal2[19], iVal2[20], iVal2[21], iVal2[22], iVal2[23], iVal2[24], iVal2[25], iVal2[26], iVal2[27], iVal2[28], iVal2[29], iVal2[30], iVal2[31], strEncoding.c_str(), strChar.c_str());
				  strLine = buff;
			 }
#endif
			}
			out.WriteString(strLine);
			if (iTextFont == '9')		// 18x18 unicode
			{
				swscanf_s(strEncoding, _T("%d"), &iLong);
//				if (iLong >= 44032 && iTotal < 8192)
//				if (iLong >= 32 && iTotal < 8192)
				if (iLong  >= 0xac00 && (iLong & 0xff) >= 0xa1)
				{
					bin.Write((char *)iVal2, 32);
					iTotal ++;
				}
			}
#ifndef __GNUC__
			if (!in.ReadString(strLine))	break;			// "ENDCHAR"
#else
			if (!std::getline(in, strLine)) break;
#endif
		}
		else
		{
			continue;
		}
	}

	in.Close();
	out.Close();
	if (iTextFont == '9')		// 18x18 unicode
	{
		while (iTotal < 8192)
		{
			bin.Write((char *)iVal2, 32);
			iTotal ++;
		}
		bin.Close();
	}
}

void GenerateISO8859File(CString strISO8859, CString strSrcFile, CString strDstFile)
{
	CStdioFile in, out;
	CString strLine, str;
	CStringList list;
	POSITION pos;
	int iIndex, iPos, iStart, iNew;

	if (!in.Open(strSrcFile, CFile::modeRead))
	{
#ifndef __GNUC__
		wprintf(_T("Can not open source file %s"), strSrcFile);
#else
		printf("Can not open source file %s", strSrcFile.c_str());
#endif
		return;
	}
#ifndef __GNUC__
	while (in.ReadString(strLine))
	{
#else
   while (!in.eof())
   {
      std::getline(in, strLine);
#endif
		list.AddTail(strLine);
	}
	in.Close();

	if (!in.Open(strISO8859, CFile::modeRead))
	{
#ifndef __GNUC__
		wprintf(_T("Can not open ISO8859 file %s\n"), strISO8859);
#else
		printf("Can not open ISO8859 file %s\n", strISO8859.c_str());
#endif
		return;
	}

#ifndef __GNUC__
	if (!out.Open(strDstFile, CFile::modeCreate|CFile::modeWrite))
	{
		wprintf(_T("Can not create destination file %s\n"), strDstFile);
#else
	if (!out.Open(strDstFile,std::fstream::out|std::fstream::trunc))
	{
		printf("Can not create destination file %s\n", strDstFile.c_str());
#endif
		in.Close();
		return;
	}

#ifndef __GNUC__
	while (in.ReadString(strLine))
	{
#else
	while (!in.eof())
	{
		std::getline(in, strLine);
#endif
		if (strLine.GetAt(0) != '0')	continue;

		swscanf_s(strLine, _T("0x%02x\t0x%04x"), &iIndex, &iPos);
		if (iIndex < 0xa0)	continue;

#ifndef __GNUC__
		for (pos = list.GetHeadPosition(); pos != NULL;)
#else
		for (pos=list.begin(); pos != list.end(); ++pos)
#endif
		{
			strLine = list.GetNext(pos);
			iStart = strLine.Find(_T("//"), 0);
			str = strLine.Right(strLine.GetLength() - iStart);
			swscanf_s(str, _T("//\t%d"), &iNew);
			if (iNew == iPos)
			{
				out.WriteString(strLine + _T("\n"));
				break;
			}
		}

#ifndef __GNUC__
		if (pos == NULL)
		{
			for (pos = list.GetHeadPosition(); pos != NULL;)
#else
		if (pos == list.end())
		{
			for (pos=list.begin(); pos != list.end(); ++pos)
#endif
			{
				strLine = list.GetNext(pos);
				iStart = strLine.Find(_T("//"), 0);
				str = strLine.Right(strLine.GetLength() - iStart);
				swscanf_s(str, _T("//\t%d"), &iNew);
				if (iNew == iIndex)
				{
					out.WriteString(strLine + _T(" can not find\n"));
					break;
				}
			}
		}
	}

	in.Close();
	out.Close();
}

void MainProcess(int argc, TCHAR* argv[])
{
	CFile font;
	int i, j, k, iLength, iPos, iCur;
	CString	strInput;
	CString strFile;
	char * p;
	char * pCur;
	BOOL bDisplay = FALSE, bConvert = FALSE, bAscii = FALSE, bTextFile = FALSE, bISO8859 = FALSE;
	int iFontSize, iFontPageSize, iTextFont = '5';

	strFile = argv[1];
	for (i = 2; i < argc; i ++)
	{
		if (argv[i][0] == '-')
		{
			if		(argv[i][1] == 'a')	bAscii = TRUE;
			else if (argv[i][1] == 'c')	bConvert = TRUE;
			else if (argv[i][1] == 'd')	bDisplay = TRUE;
			else if (argv[i][1] == 'i')	bISO8859 = TRUE;
			else if (argv[i][1] == 't')
			{
				bTextFile = TRUE;
				iTextFont = argv[i][2];
			}
		}
	}

	if (bTextFile)
	{
		ProcessTextFile(strFile, _T("array.txt"), iTextFont);
		return;
	}

	if (bISO8859)
	{
		GenerateISO8859File(strFile, _T("array.txt"), _T("font.txt"));
		return;
	}

	if (!font.Open(strFile, CFile::modeRead))
	{
		printf("Can not open font file");
		return;
	}

	iLength = (int)font.GetLength();
	p = (char *)malloc(iLength);
	if (p == NULL)
	{
		printf("malloc font buffer failed");
		font.Close();
		return;
	}

	font.Read(p, iLength);
	font.Close();

	if (bConvert)
	{
		if (bAscii)
		{
			ConvertAsciiFont(p);
		}
		else
		{
			ConvertFont(p);
		}
	}

	if (bAscii)
	{
		iFontSize = ASCII_FONT_SIZE;
		iFontPageSize = ASCII_FONT_PAGE_SIZE;
	}
	else
	{
		iFontSize = FONT_SIZE;
		iFontPageSize = FONT_PAGE_SIZE;
	}

	if (bDisplay)
	{
		iPos = 0;
		while (1)
		{
			printf("n-next char, p-prev char, d-next page, u-prev page, press q to quite\n");
			printf("current pos: %d\n", iPos/iFontSize);

			pCur = (char *)(p+iPos);
			for (i = 0; i < 16; i ++)
			{
				for (j = 0; j < (bAscii ? 1 : 2); j ++)
				{
					iCur = *pCur ++;
					for (k = 0; k < 8; k ++)
					{
						if (iCur & iMask[k])	printf("&&");
						else					printf("  ");
					}
				}
				printf("\n");
			}
#ifndef __GNUC__		
			gets((char *)(LPCTSTR)strInput);
#else
			{
				char buff[1023];
      	fgets( buff, sizeof(buff), stdin );
				strInput = buff;
			}
#endif
			if (strInput == "q")	break;
			else if (strInput == "n")
			{
				iPos += iFontSize;
				if (iPos == iLength)	iPos = 0;
			}
			else if (strInput == "p")
			{
				if (iPos >= iFontSize)
				{
					iPos -= iFontSize;
				}
			}
			else if (strInput == "d")
			{
				iPos += iFontSize*iFontPageSize;
				if (iPos >= iLength)	iPos = 0;
			}
			else if (strInput == "u")
			{
				if (iPos >= iFontSize*iFontPageSize)
				{
					iPos -= iFontSize*iFontPageSize;
				}
			}
		}	
	}

	free(p);
}

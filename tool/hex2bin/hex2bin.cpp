// hex2bin.cpp : Defines the entry point for the console application.
//
#ifndef __GNUC__
#include "stdafx.h"
#else
#include "../common/mfc2std.h"
#endif

#include "HexFile.h"

#include "../../include/ar168.h"

void MainProcess(int argc, TCHAR* argv[]);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
#else
int main(int argc, char *argv[])
{
   int nRetCode = 0;
   {
#endif
		// TODO: code your application's behavior here.
		if (argc < 3)
		{
			printf("Usage: dst-file, [-f(0-255), -a(0-65535)], srcfile, [srcfile2, srcfile3 ...]\n\t-f -- fill char\n\t-a -- address offset");
		}
		else
		{
			MainProcess(argc, argv);
		}
	}

	return nRetCode;
}

void MainProcess(int argc, TCHAR* argv[])
{
	int i, k, iAddress, iNumber, iCurBank, iBankSize;
	int iFillChar = 0xff, iAddressOffset = 0, iExtraBanks = 0, iBankBegin = 0x8000, iBankEnd = 0xe000;
	char * p;
	char * pBanks;
	unsigned char * pData;
	CFile fileWrite;
	CHexFile hex;
	BOOL bCommonBank, bError;
	char pArgv[128];

	p = (char *)malloc(HEXFILE_SIZE);
	fileWrite.Open(argv[1], CFile::modeCreate|CFile::modeWrite);
	for (i = 2; i < argc; i ++)
	{
		if (argv[i][0] == _T('-'))
		{
#ifndef __GNUC__
			WideCharToMultiByte(CP_ACP, 0, argv[i], -1, pArgv, 128, NULL, NULL); 
#else
         memcpy(pArgv, argv[i], 128);
#endif
			switch (argv[i][1])
			{
			case _T('f'):
				iFillChar = atoi(pArgv+2);
				break;
			case _T('a'):
				iAddressOffset = atoi(pArgv+2);
				break;
			case _T('x'):
				iExtraBanks = atoi(pArgv+2);
				break;
			case _T('b'):
				iBankBegin = atoi(pArgv+2);
				break;
			case _T('e'):
				iBankEnd = atoi(pArgv+2);
				break;
			}
		}
		else
		{
			if (iExtraBanks != 0)
			{
				iBankSize = HEXFILE_SIZE-iBankBegin;
				pBanks = (char *)malloc(iBankSize*iExtraBanks);
				if (pBanks != NULL)
				{
					iCurBank = 0;
					memset(pBanks, iFillChar, iBankSize*iExtraBanks);
				}
			}
			memset(p, iFillChar, HEXFILE_SIZE);
			bCommonBank = FALSE;
			bError = FALSE;
#ifndef __GNUC__
			if (hex.Open(argv[i]))
#else
         CString tmp = argv[i];
         if (hex.Open(tmp, std::fstream::in))
#endif
			{
				while (hex.ReadLine())
				{
					iAddress = hex.GetAddress();
					iNumber = hex.GetNumber();
					pData = hex.GetData();
					if (iExtraBanks != 0 && pBanks != NULL)
					{
						if (iAddress < iBankBegin)
						{	// single copy
							if (bCommonBank)
							{
								printf("Error: banked code exceeded bank %d end (default 0xe000)", iCurBank);
								bError = TRUE;
								break;
							}
							if (iAddress + iNumber >= 0x2000)
							{
								if (iAddress + iNumber < 0x4000)
								{
									printf("Error: sram code exceeded code end (default 0x2000)");
									bError = TRUE;
									break;
								}
								else if (iAddress + iNumber >= 0x8000)
								{
									printf("Error: font code exceeded code end (default 0x8000)");
									bError = TRUE;
									break;
								}
							}
							memcpy(p + iAddress, pData, iNumber);		
						}
						else if (iAddress >= iBankEnd)
						{	// every bank a copy
							if (iAddress + iNumber >= FILE_FLAG_PAGE_SIZE)
							{
								printf("Error: common code exceeded common end (default 0xffd0)");
								bError = TRUE;
								break;
							}
							memcpy(p + iAddress, pData, iNumber);		
							for (k = 0; k < iExtraBanks; k ++)
							{
								memcpy(pBanks + k * iBankSize + iAddress - iBankBegin, pData, iNumber);
							}
							bCommonBank = TRUE;
						}
						else
						{	// single bank copy
							if (bCommonBank || iAddress + iNumber >= iBankEnd)
							{
								printf("Error: banked code exceeded bank %d end (default 0xe000)", iCurBank);
								bError = TRUE;
								break;
							}
							if (iAddress == iBankBegin)
							{
								iCurBank ++;
							}
							if (iCurBank == 1)
							{
								memcpy(p + iAddress, pData, iNumber);		
							}
							else
							{
								memcpy(pBanks + (iCurBank-2)*iBankSize + iAddress - iBankBegin, pData, iNumber);
							}
						}
					}
					else
					{
						if (iAddress >= iAddressOffset)
						{
							memcpy(p + iAddress - iAddressOffset, hex.GetData(), hex.GetNumber());		
						}
					}
				}
				hex.Close();
			}
			else
			{
				break;
			}
			fileWrite.Write(p, HEXFILE_SIZE);
			if (iExtraBanks != 0 && pBanks != NULL)
			{
				fileWrite.Write(pBanks, iBankSize*iExtraBanks);
				free(pBanks);
			}
		}
	}

	free(p);
	fileWrite.Close();

	if (bError)
	{
		CFile::Remove(argv[1]);
	}
}



// convert.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include "convert.h"

void PrintHelp();
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
#endif

using namespace std;

void PrintHelp()
{
	printf("%s\n", "Convert files from one format to another: (0.55.011)");
	printf("%s\n", "convert [[-c|--cfg2bin] | [-C|--bin2cfg] [--innomedia]| [-w|--web2web] | [-i|--web2info] |");
	printf("%s\n", "\t[[-h|--bin2h] [-a<0-65535>] [-l<0-65535>]] | [-r|--ring2bin] |");
	printf("%s\n", "\t[-v|--ivr2bin][-p|--pb2bin] | [-P|--bin2pb] | [-d|--dsp2h]]");
	printf("%s\n", "\t [-f|--force] [--sd srcDir] [--dd dstDir] srcFile [srcFile2...] dstFile");
	printf("\n");
	printf("%s\n", "-c|--cfg2bin\t\tConvert configuration text file to binary file");
	printf("%s\n", "-C|--bin2cfg\t\tConvert configuration binary file to text file");
	printf("%s\n", "--innomedia\t\tConvert config bin file to InnoMedia style text file");
	printf("%s\n", "-w|--web2web\t\tConvert html file to AR1688 acceptable html format");
	printf("%s\n", "-i|--web2info\t\tExtract configuration info file from html file");
	printf("%s\n", "-h|--bin2h\t\tConvert a file to a c headfile");
	printf("%s\n", "  -a<0-65535>\t\taddress offset");
	printf("%s\n", "  -l<0-65535>\t\tlength");
	printf("%s\n", "-r|--ring2bin\t\tConvert pcm ring file to binary file");
	printf("%s\n", "-v|--ivr2bin\t\tConvert pcm IVR file to binary file");
	printf("%s\n", "-p|--pb2bin\t\tConvert phone book text file to binary file");
	printf("%s\n", "-P|--bin2pb\t\tConvert phone book binary file to text file");
	printf("%s\n", "-f|--force\t\tForce to overwrite the destination file that exists");
	printf("%s\n", "--sd srcDir\t\tSpecifies the directory containing the source files.");
	printf("%s\n", "--dd dstDir\t\tSpecifies the directory containing the destination file.");
	printf("%s\n", "srcFile\t\t\tSpecifies the source file to be converted");
	printf("%s\n", "srcFile2...\t\t\tSpecifies the additional source files to be converted");
	printf("%s\n", "dstFile\t\t\tSpecifies the destination file");
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
		// TODO: code your application's behavior here.
		MainProcess(argc, argv);
	}

	return nRetCode;
}

#define FUNCTION_NONE		0
#define FUNTCION_CFG2BIN	1
#define FUNCTION_BIN2CFG	2
#define FUNCTION_WEB2WEB	3
#define FUNCTION_WEB2INFO	4
#define FUNCTION_BIN2H		5
#define FUNCTION_RING2BIN	6
#define FUNCTION_PB2BIN		7
#define FUNCTION_BIN2PB		8
#define FUNCTION_DSP2H		9
#define FUNCTION_IVR2BIN	10

void MainProcess(int argc, TCHAR* argv[])
{
	int i, iFunction;
	CString strSrcFile, strDstFile, strSrcDir, strDstDir;
	CStringList strFileList;
	BOOL bForce;
	BOOL bInnomedia;
	int iAddressOffset = 0, iConvertLength = 0;
//	char *pArg;
	char pArg[128];

	iFunction = FUNCTION_NONE;
	bForce = FALSE;
	bInnomedia = FALSE;
	strSrcFile = "";
	strDstFile = "";
	strSrcDir = "";
	strDstDir = "";
	for (i = 1; i < argc; i ++)
	{
//		pArg = argv[i];
#ifndef __GNUC__
		WideCharToMultiByte(CP_ACP, 0, argv[i], -1, pArg, 128, NULL, NULL); 
#else
		strncpy((char *)pArg, argv[i], 128);
#endif
		if (pArg[0] == '-')
		{
			if (!strcmp(pArg, "-c") || !strcmp(pArg, "--cfg2bin"))
			{
				iFunction = FUNTCION_CFG2BIN;
			}
			else if (!strcmp(pArg, "-C") || !strcmp(pArg, "--bin2cfg"))
			{
				iFunction = FUNCTION_BIN2CFG;
			}
			else if (!strcmp(pArg, "-w") || !strcmp(pArg, "--web2web"))
			{
				iFunction = FUNCTION_WEB2WEB;
			}
			else if (!strcmp(pArg, "-i") || !strcmp(pArg, "--web2info"))
			{
				iFunction = FUNCTION_WEB2INFO;
			}
			else if (!strcmp(pArg, "-h") || !strcmp(pArg, "--bin2h"))
			{
				iFunction = FUNCTION_BIN2H;
			}
			else if (!strcmp(pArg, "-r") || !strcmp(pArg, "--ring2bin"))
			{
				iFunction = FUNCTION_RING2BIN;
			}
			else if (!strcmp(pArg, "-v") || !strcmp(pArg, "--ivr2bin"))
			{
				iFunction = FUNCTION_IVR2BIN;
			}
			else if (!strcmp(pArg, "-p") || !strcmp(pArg, "--pb2bin"))
			{
				iFunction = FUNCTION_PB2BIN;
			}
			else if (!strcmp(pArg, "-P") || !strcmp(pArg, "--bin2pb"))
			{
				iFunction = FUNCTION_BIN2PB;
			}
			else if (!strcmp(pArg, "-d") || !strcmp(pArg, "--dsp2h"))
			{
				iFunction = FUNCTION_DSP2H;
			}
			else if (pArg[1] == 'a')
			{
				iAddressOffset = atoi(pArg+2);
			}
			else if (pArg[1] == 'l')
			{
				iConvertLength = atoi(pArg+2);
			}
			else if (!strcmp(pArg, "--innomedia"))
			{
				bInnomedia = TRUE;
			}
			else if (!strcmp(pArg, "-f") || !strcmp(pArg, "--force"))
			{
				bForce = TRUE;
			}
			else if (!strcmp(pArg, "--sd"))
			{
				strSrcDir = argv[++i];
			}
			else if (!strcmp(pArg, "--dd"))
			{
				strDstDir = argv[++i];
			}
			else
			{
				PrintHelp();
				return;
			}
		}
		else
		{
			strFileList.AddTail(pArg);
		}
	}

	if (iFunction == FUNCTION_NONE || strFileList.GetCount() < 2)
	{
		PrintHelp();
		return;
	}

	TCHAR ch;
	if (strSrcDir.GetLength())
	{
		ch = strSrcDir.GetAt(strSrcDir.GetLength()-1);
		if (ch != '\\' && ch != '/')
		{
			strSrcDir += '\\';
		}
	}
	if (strDstDir.GetLength())
	{
		ch = strSrcDir.GetAt(strDstDir.GetLength()-1);
		if (ch != '\\' && ch != '/')
		{
			strDstDir += '\\';
		}
	}
	strSrcFile = strSrcDir + strFileList.GetHead();
	strDstFile = strDstDir + strFileList.GetTail();
//	printf("src file: %s\n", strSrcFile);
//	printf("dst file: %s\n", strDstFile);
#ifdef __GNUC__
	printf("src file: %s\n", strSrcFile.c_str());
	printf("dst file: %s\n", strDstFile.c_str());
#endif
	switch (iFunction)
	{
	case FUNTCION_CFG2BIN:
		OnCfg2Bin(strSrcFile, strDstFile, bForce);
		break;

	case FUNCTION_BIN2CFG:
		OnBin2Cfg(strSrcFile, strDstFile, bForce, bInnomedia);
		break;

	case FUNCTION_WEB2WEB:
		OnWeb2Web(strSrcFile, strDstFile, bForce);
		break;

	case FUNCTION_WEB2INFO:
		OnWeb2Info(strSrcFile, strDstFile, bForce);
		break;

	case FUNCTION_BIN2H:
		OnBin2H(strSrcFile, strDstFile, bForce, iAddressOffset, iConvertLength);
		break;

	case FUNCTION_RING2BIN:
		OnRing2Bin(strSrcFile, strDstFile, bForce);
		break;

	case FUNCTION_IVR2BIN:
		OnIvr2Bin(strSrcFile, strDstFile, bForce);
		break;

	case FUNCTION_PB2BIN:
		OnPB2Bin(strSrcFile, strDstFile, bForce);
		break;

	case FUNCTION_BIN2PB:
		OnBin2PB(strSrcFile, strDstFile, bForce);
		break;

	case FUNCTION_DSP2H:
		OnDsp2H(strFileList, strSrcDir, strDstDir, bForce);
		break;
	}
}


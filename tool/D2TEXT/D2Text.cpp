// D2Text.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void MainProcess(int argc, TCHAR* argv[]);
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

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
		// TODO: code your application's behavior here.
		if (argc < 2)
		{
			printf("Usage (0.55.011): D2Text srcFileName\n");
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
	CString datFilename,txtFilename;
	CFile   datFile;
	CStdioFile txtFile;
	CString strLine, strLine2;
	int k, fnlen, Temp;
	int stage = 1;
	char Data;
	int DM_Mem[16*1024];

	datFilename = argv[1];
	if (!datFile.Open(datFilename,CFile::modeRead|CFile::typeBinary))
	{
		printf("Can not open dat file %s\n", datFilename);
		return;
	}

	fnlen = datFilename.GetLength();
	txtFilename= datFilename.Left(fnlen-4)+".txt";
	if (!txtFile.Open(txtFilename,CFile::modeCreate|CFile::modeWrite))
	{
		printf("Can not open txt file %s\n", txtFilename);
		datFile.Close();
		return;
	}

	for (k = 0; k< 16*1024; k++)
	{
		DM_Mem[k] = 0;
	}

	do 
	{
		for ( k = 0; k< 16*1024; k++)
		{
			datFile.Read(&Data, 1);
			Temp = Data & 0x000000ff;
			Temp = Temp <<((stage -1)*8);
			DM_Mem[k] = DM_Mem[k] | Temp;
		}
		stage++;
	}while (stage<4);

	for (k = 0; k< 16*1024; k++)
	{
		strLine.Format(_T("%08x"),DM_Mem[k]);
		strLine= strLine.Right(6);
		strLine+= "\n";
		strLine2.Format(_T("%04x:	"), k);
		strLine = strLine2 + strLine;
		txtFile.WriteString(strLine);
	}

	datFile.Close();
	txtFile.Close();
}


// mergebin.cpp : Defines the entry point for the console application.
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
		if (argc < 4)
		{
			printf("(0.55.011)Usage: mergebin dst_file, srcfile, srcfile2, [srcfile3 ...]");
		}
		else
		{
			MainProcess(argc, argv);
		}
	}

	return nRetCode;
}

#define BANK_SIZE	32768

void MainProcess(int argc, TCHAR* argv[])
{
	CFile in;
	CFile out;
	BYTE p[BANK_SIZE];
	int i, k, iLength;

	if (!out.Open(argv[1], CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox(_T("Can not create dest file"));
		return;
	}

	if (!in.Open(argv[2], CFile::modeRead))
	{
		AfxMessageBox(_T("Can not open source file 1"));
		out.Close();
		return;
	}

	iLength = (int)in.GetLength();
	if (iLength < BANK_SIZE)
	{
		AfxMessageBox(_T("file length error"));
		in.Close();
		out.Close();
		return;
	}

	in.Read(p, BANK_SIZE);
	in.Close();
	out.Write(p, BANK_SIZE);

	for (i = 3; i < argc; i ++)
	{
		if (!in.Open(argv[i], CFile::modeRead))
		{
			AfxMessageBox(_T("Can not open source file"));
			out.Close();
			return;
		}
		iLength = (int)in.GetLength();
		if (iLength < BANK_SIZE*2)
		{
			AfxMessageBox(_T("file length error"));
			in.Close();
			out.Close();
			return;
		}
		in.Seek(BANK_SIZE, CFile::begin);
		for (k = BANK_SIZE; k < iLength; k += BANK_SIZE)
		{
			in.Read(p, BANK_SIZE);
			out.Write(p, BANK_SIZE);
		}
		in.Close();
	}

	out.Close();
}

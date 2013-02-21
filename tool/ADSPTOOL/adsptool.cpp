// adsptool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\\common\\common.h"

#define big_endian(x)	((x & 0xff) << 8) + (x >> 8)

void MainProcess(int argc, TCHAR* argv[]);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

struct DM_Node
{
	CString Var_name;
	unsigned short Var_address;
	bool operator == (DM_Node &r){ return r.Var_address==Var_address;}
};

CList<DM_Node * , DM_Node *>  DM_List;


CStdioFile	sym_file;
CStdioFile	link_file;
CStdioFile	temp_file;
CStdioFile	exe_file;
CStdioFile	out_file;
CStdioFile	dat_file;

CString Filenames[6];

CString strCmdLine;

// Filename[0] === sym_file; r
// Filename[1] === link_file;r
// Filename[2] === temp_file; rw save the init @DA infomation
// Filename[3] === the first stage .exe file;
// Filename[4] === the second stage .exe file;
// Filename[5] ==== the output .dat file

void Usage();
int Dm_cov(CString Filenames[]);
int Exe_Merge(CString Filename[]);
int GetLineLen(char *p)
{
		int len=0;
		char *pLine = p;

		while((*pLine++)!=0x0a)
			len++;
			len++;
		return len;

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
		// TODO: code your application's behavior here.
		MainProcess(argc, argv);
	}

	return nRetCode;
}

#define FUNCTION_NONE		0
#define FUNTCION_EXE2DAT	1
#define FUNCTION_SYM2H		2


int CheckDspFileLen(char * pData, int * iDMLen, int * iPMLen, int* iDMMin)
{
	int iPMAddr, iDMAddr, iSpacePD, temp, iFileLen;
	bool bNewAera;
	char ch;

	iPMAddr = iDMAddr = 0;
	*iDMMin = 0x3fff;

	// OPEN INPUT FILE USING STANDARD DOS FUNCTIONS.
	if ((*pData ++) != '\x1b')	goto Error;
	if ((*pData ++) != '\x1b')	goto Error;
	if ((*pData ++) != 'i')		goto Error;

	while (1)
	{
		//  READ ALL THE SECTIONS
		//  Reading section header 
		ch = (*pData ++);
		if (ch == 0x0d && *pData == 0x0a)
		{
			pData ++;
			continue;
		}
		if (ch == '\x1b')
		{
			//  <ESC> signifies end of file. 
			OutputDebugString(_T("All kernels have been loaded\n"));
			break;
		}
		if (ch != '@')	goto Error;

		//  CHECK MEMORY SPACE THAT THE SECTION WILL BE LOADED INTO.
		switch ((*pData ++))
		{
		case 'P' :
		case 'B' :
			iSpacePD = 'p';
			break;

		case 'D' :
			iSpacePD = 'd';
			break;

		default :
			goto Error;
		}

		//  CHECK MEMORY TYPE THAT THE SECTION WILL BE LOADED INTO.
		switch ((*pData ++))
		{
		case 'O' :
		case 'A' :
			break;

	    default :
			goto Error;
		}

		//  READING SECTION ADDRESS
		if (*pData == 0x0d && (*(pData+1)) == 0x0a)	pData += 2;
		if (sscanf (pData, "%x", &temp) != 1)	goto Error;
		pData += 6;

		TRACE2("loading kernel to %s memory location %04x\n", (iSpacePD == 'd') ? "data" : "program", temp);

		//  Conuting kernel 
		if (iSpacePD == 'p')
		{
			if (iPMAddr <= temp)
			{
				iPMAddr = temp;
				bNewAera = true;
			}
			else 
			{
				bNewAera = false;
			}
		}
		else
		{
			if (*iDMMin > temp)
			{
				*iDMMin = temp;
			}

			if (iDMAddr <= temp)
			{
				iDMAddr = temp;
				bNewAera = true;
			}
			else 
			{
				bNewAera = false;
			}
		}

		//  Read data and load to PM and DM
		while (sscanf (pData, "%lx", &temp) == 1)
		{
			//  Conuting words 
			if (iSpacePD == 'p')
			{
				if (bNewAera)
				{
					iPMAddr++;
				}
				pData += 8;
			}
			else
			{
				pData+=GetLineLen(pData);

				if (bNewAera)
				{
					iDMAddr++;
				}
			}
        }

		//  Reading dummy value #dummy
		if (*pData != '#') goto Error;

		//  Skip dummy string 
		while (*pData != 0x0d || *(pData+1) != 0x0a)	pData ++;
	}
	
	if (*iDMMin == 0x3fff)
	{
		*iDMMin = 0;
	}
	iDMAddr -= *iDMMin;

	*iDMLen = 2 * ((int)(iDMAddr+1)/2);
	*iPMLen = 2 * ((int)(iPMAddr+1)/2);

	iFileLen = 3*(*iDMLen+*iPMLen) + 0x40;
	return iFileLen;
Error:
	OutputDebugString(_T("2181 exe format error!\n"));
	return false;
}

BOOL AR168ConvertDsp(char * pData, char * pWrite, int iDMLen, int iPMLen, int iDMMin)
{
	int iPMAddr, iDMAddr, iSpacePD, temp;
	int iStart, iLength;
	short *pCode;
	char ch;
	char *pDML;
	char *pDMM;
	char *pDMH;
	char *pPML;
	char *pPMM;
	char *pPMH;

	pDML = pWrite + 0x40;
	pDMM = pDML + iDMLen;
	pDMH = pDMM + iDMLen;
	pPML = pDMH + iDMLen;
	pPMM = pPML + iPMLen;
	pPMH = pPMM + iPMLen;

	*pWrite ++ = 'A';
	*pWrite ++ = 'D';
	*pWrite ++ = 'E';
	*pWrite ++ = 'F';
	pWrite += 10;
	
	pCode = (short*)pWrite;
	*pCode ++ = big_endian(iDMMin);
	pWrite = (char*)pCode;

	*pWrite ++ = 'D';
	*pWrite ++ = 'M';
	*pWrite ++ = 'L';
	*pWrite ++ = 'S';

	pCode = (short*)pWrite;
	iStart = 0x20;
	iLength = iDMLen/2;
	*pCode ++ = big_endian(iStart);
	*pCode ++ = big_endian(iLength);
	pWrite = (char*)pCode;

	*pWrite ++ = 'D';
	*pWrite ++ = 'M';
	*pWrite ++ = 'M';
	*pWrite ++ = 'S';

	pCode = (short*)pWrite;
	iStart += iLength;
	*pCode ++ = big_endian(iStart);
	*pCode ++ = big_endian(iLength);
	pWrite = (char*)pCode;

	*pWrite ++ = 'D';
	*pWrite ++ = 'M';
	*pWrite ++ = 'H';
	*pWrite ++ = 'S';

	pCode = (short*)pWrite;
	iStart += iLength;
	*pCode ++ = big_endian(iStart);
	*pCode ++ = big_endian(iLength);
	pWrite = (char*)pCode;

	*pWrite ++ = 'P';
	*pWrite ++ = 'M';
	*pWrite ++ = 'L';
	*pWrite ++ = 'S';

	pCode = (short*)pWrite;
	iStart += iLength;
	iLength = iPMLen/2;
	*pCode ++ = big_endian(iStart);
	*pCode ++ = big_endian(iLength);
	pWrite = (char*)pCode;

	*pWrite ++ = 'P';
	*pWrite ++ = 'M';
	*pWrite ++ = 'M';
	*pWrite ++ = 'S';

	pCode = (short*)pWrite;
	iStart += iLength;
	*pCode ++ = big_endian(iStart);
	*pCode ++ = big_endian(iLength);
	pWrite = (char*)pCode;

	*pWrite ++ = 'P';
	*pWrite ++ = 'M';
	*pWrite ++ = 'H';
	*pWrite ++ = 'S';

	pCode = (short*)pWrite;
	iStart += iLength;
	*pCode ++ = big_endian(iStart);
	*pCode ++ = big_endian(iLength);
	pWrite = (char*)pCode;

	//  reset some counters
	iPMAddr = iDMAddr = 0;

	// OPEN INPUT FILE USING STANDARD DOS FUNCTIONS.
	if ((*pData ++) != '\x1b')	goto Error;
	if ((*pData ++) != '\x1b')	goto Error;
	if ((*pData ++) != 'i')		goto Error;

	while (1)
	{
		//  READ ALL THE SECTIONS
		//  Reading section header 
		ch = (*pData ++);
		if (ch == 0x0d && *pData == 0x0a)
		{
			pData ++;
			continue;
		}
		if (ch == '\x1b')
		{
			//  <ESC> signifies end of file. 
			OutputDebugString(_T("All kernels have been loaded\n"));
			break;
		}
		if (ch != '@')	goto Error;

		//  CHECK MEMORY SPACE THAT THE SECTION WILL BE LOADED INTO.
		switch ((*pData ++))
		{
		case 'P' :
		case 'B' :
			iSpacePD = 'p';
			break;

		case 'D' :
			iSpacePD = 'd';
			break;

		default :
			goto Error;
		}

		//  CHECK MEMORY TYPE THAT THE SECTION WILL BE LOADED INTO.
		switch ((*pData ++))
		{
		case 'O' :
		case 'A' :
			break;

	    default :
			goto Error;
		}

		//  READING SECTION ADDRESS
		if (*pData == 0x0d && (*(pData+1)) == 0x0a)	pData += 2;
		if (sscanf(pData, "%x", &temp) != 1)	goto Error;
		pData += 6;

		TRACE2("loading kernel to %s memory location %04x\n", (iSpacePD == 'd') ? "data" : "program", temp);

		//  Conuting kernel 
		if (iSpacePD == 'p')
		{
			iPMAddr = temp;
		}
		else
		{
			iDMAddr = temp - iDMMin;
		}

		//  Read data and load to PM and DM
		while (sscanf(pData, "%lx", &temp) == 1)
		{
			//  Conuting words 
			if (iSpacePD == 'p')
			{
				pPML[iPMAddr] = (char)(temp & 0xff);
				pPMM[iPMAddr] = (char)((temp >> 8) & 0xff);
				pPMH[iPMAddr] = (char)(temp >> 16);
				iPMAddr ++;
				pData += 8;
			}
			else
			{
			
				pData += GetLineLen(pData);
				pDML[iDMAddr] = (char)(temp & 0xff);
				pDMM[iDMAddr] = (char)((temp >> 8) & 0xff);
				pDMH[iDMAddr] = (char)(temp >> 16);
				iDMAddr ++;
			}
        }

		//  Reading dummy value #dummy
		if (*pData != '#') goto Error;

		//  Skip dummy string 
		while (*pData != 0x0d || *(pData+1) != 0x0a)	pData ++;
	}

	OutputDebugString(_T("2181 exe load OK!\n"));
	return true;

Error:
	OutputDebugString(_T("2181 exe format error!\n"));
	return false;
}

int IntFromString(CString str)
{
	int iVal = 0x80000000;

	str.MakeLower();
	if (str.Find(_T("0x")) != -1)
	{
		swscanf_s(str, _T("0x%x"), &iVal);
	}
	else
	{
		swscanf_s(str, _T("%d"), &iVal);
	}
	return iVal;
}

void OnExe2Dat(CString strSrcFile, CString strDstFile, bool bForce) 
{
	CFile in, out;
	CString	cStr;
	char * pRead;
	char * pWrite;
	int iLength, iDMLen, iPMLen, iDMMin;

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
	pRead = (char *)malloc(iLength);
	in.Read(pRead, iLength);

	iLength = CheckDspFileLen(pRead, &iDMLen, &iPMLen, &iDMMin);
	if (iLength)
	{
		pWrite = (char *)malloc(iLength);
		memset(pWrite, 0, iLength);


		AR168ConvertDsp((char *)pRead, (char *)pWrite, iDMLen, iPMLen, iDMMin);
		out.Write(pWrite, iLength);
		free(pWrite);
	}
	else
	{
		printf("%s\n", "2181 dsp file format error");
	}

	free(pRead);
	in.Close();
	out.Close();
}

void OnSym2H(CString strSrcFile, CString strDstFile, bool bForce) 
{
	CStdioFile in, out;
	CString	strLine, strSymbol, str;
	int iAddress, iLength;

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

	while (in.ReadString(strLine))
	{
		iLength = strLine.GetLength();
		if (iLength < 7)	continue;
		if (strLine.GetAt(0) == '_')	continue;

		strSymbol = strLine.Left(iLength-6);
		str = _T("0x") + strLine.Mid(iLength-5, 4);
		str.MakeLower();
		iAddress = IntFromString(str);

		strLine.Format(_T(".const %s= 0x%04x;\n"), strSymbol, iAddress);
		out.WriteString(strLine);
	}

	in.Close();
	out.Close();
}

void MainProcess(int argc, TCHAR* argv[])
{
	int i, iFunction, iParamIndex;
	int fnlen;
	CString strSrcFile, strDstFile;
	bool bForce;

	iFunction = FUNCTION_NONE;
	iParamIndex = 0;
	bForce = FALSE;
	strSrcFile = "";
	strDstFile = "";

	
	for (i= 1; i< argc; i ++)
	{
			if (argv[i][0] == '-')
			{
				switch(argv[i][1])
				{
				case 's':
					{
						iFunction =FUNCTION_SYM2H;
						strSrcFile = argv[++i];
						strDstFile = argv[++i];
						
					}
					break;
				case 'f':
					{
						bForce = TRUE;
						break;

					}
				case 'e':
					{
						iFunction =FUNTCION_EXE2DAT;
						//Filenames[3] = argv[++i];

					}
					break;
				case 'x':
					{
					    Filenames[3] = argv[++i];
						fnlen=Filenames[3].GetLength();
						Filenames[4] = Filenames[3].Left(fnlen-1) +"_";
						break;
					}
					
				case 'm':
						Filenames[0] = argv[++i];
					break;
				case 'l':
						Filenames[1] = argv[++i];
					break;
			
					break;
				case 'd':
						Filenames[5]= argv[++i];
					break;
				default:
					break;
				}
			}
			else
			{
				Usage();
				return;
			}
	}

/*	Filenames[0] = "test.sym";
	Filenames[1] = "file_all.txt";
	Filenames[3] = "test.exe";
	Filenames[4] = "test.ex_";
	Filenames[5] = "g711.dat";
	iFunction =FUNTCION_EXE2DAT;
	bForce = TRUE;
*/	
		if (!iFunction)
		{
			Usage();
			return;
		}

		switch(iFunction)
		{
		case FUNTCION_EXE2DAT:
			Filenames[2] ="temp.dat";
			Dm_cov(Filenames);
			Exe_Merge(Filenames);
			OnExe2Dat(Filenames[4], Filenames[5],bForce);
			strCmdLine.Format(_T("del %s"), Filenames[3]);
			_tsystem(strCmdLine);
			strCmdLine.Format(_T("ren %s %s"), Filenames[4], Filenames[3]);
			_tsystem(strCmdLine);


			break;
		case FUNCTION_SYM2H:
				OnSym2H(strSrcFile, strDstFile, true);
			break;
		default:
			break;
		}

	}




int Dm_cov( CString Filenames[])
{
	
	CString strLine, subStr, strTemp;
	CStdioFile   int_file;
	CStdioFile   dat_file;	
	CString      int_file_name, dat_file_name;
	TCHAR  drive[8], dir[7], fname[20], ext[8];
	CString DM_addr;
	unsigned short DM_Var;
	DM_Node  *p_Node, *k_Node;
	int  StrLen, blank_pos;
	POSITION pos;

	if(NULL==temp_file.Open(Filenames[2],CFile::modeWrite|CFile::modeCreate|CFile::typeText))
		{
			printf("can't open temp file %s", Filenames[2]);
			exit(1);
		};

		if (NULL==link_file.Open(Filenames[1], CFile::modeRead|CFile::typeText))
		{
			printf("can't open link file %s", Filenames[1]);
			exit(1);

		};
		if(NULL==sym_file.Open(Filenames[0],CFile::modeRead|CFile::typeText))
		{
			printf("can't open sym file %s", Filenames[0]);
			exit(1);
		};




	sym_file.ReadString(strLine);

		while(sym_file.ReadString(strLine))
		{
			if (strLine.GetAt(0) == 0x1b)
				break;			
			StrLen = strLine.GetLength();
			if (strLine.GetAt(StrLen-1)=='d')
			{
				blank_pos=strLine.Find(' ');
				subStr=strLine.Left(blank_pos);
				p_Node = new DM_Node;
				p_Node->Var_name=subStr;
				subStr = strLine.Mid(blank_pos+1,4);
				swscanf_s(subStr, _T("%lx"), &DM_Var);
				p_Node->Var_address = DM_Var;
				//if (DM_List.Find(p_Node, NULL)==NULL)

				if (DM_List.IsEmpty())
					DM_List.AddTail(p_Node);
				else
				{
				
					for(pos= DM_List.GetHeadPosition(); pos != NULL;)
					{
						k_Node = DM_List.GetNext(pos);
						if (p_Node->Var_address==k_Node->Var_address)
						break;		
					}

					if (pos==NULL)
						DM_List.AddTail(p_Node);
				}
			}

		};

/*		for (pos = DM_List.GetHeadPosition(); pos != NULL;)
		{
			p_Node = DM_List.GetNext(pos);			
			strLine.Format("%04x:  %s", p_Node->Var_address, p_Node->Var_name);
		} */
		sym_file.Close();

		while(link_file.ReadString(strLine))
		{
			_wsplitpath(strLine, drive, dir, fname, ext);
			int_file_name.Format(_T("%s%s.int"),dir,fname);
			if (NULL==int_file.Open(int_file_name, CFile::modeRead|CFile::typeText))
				continue;
				

			while(int_file.ReadString(strLine))
			{
				if (strLine.Find(_T(",p"))== -1)
				{
					if (strLine.GetAt(0) == '$' && strLine.GetAt(1) =='i')
					{
						blank_pos=strLine.Find(' ');
						subStr =strLine.Mid(2,blank_pos-2);
						for(pos= DM_List.GetHeadPosition(); pos != NULL;)
						{
							p_Node = DM_List.GetNext(pos);
							if(p_Node->Var_name == subStr)
							{
								
								//strTemp.Format("@DA\n");
								temp_file.WriteString(_T("@DA\n"));
								strTemp.Format(_T("%04x\n"), p_Node->Var_address);
								strTemp.MakeUpper();
								temp_file.WriteString(strTemp);

								strLine=strLine.Right(strLine.GetLength()-blank_pos-1);

								if (strLine.GetAt(0)==','&&strLine.GetAt(1)=='f')
								{
									strLine=strLine.Right(strLine.GetLength()-2);
									dat_file_name=strLine.Left(strLine.GetLength()-1);
									if (dat_file.Open(dat_file_name,CFile::modeRead|CFile::typeText))
									{
										while(dat_file.ReadString(strLine))
										{
											strLine=strLine+"\n";
											temp_file.WriteString(strLine);
											
										}

										dat_file.Close();

									}
								}
								else
								{
									do
									{
										subStr.Format(_T("%s\n"),strLine.Left(6));
										temp_file.WriteString(subStr);
										strLine = strLine.Right(strLine.GetLength()-7);
									}while(strLine.GetLength()>0);

								}


								break;
							}
						}
					}
				}
			}


			int_file.Close();
		}

		link_file.Close();
		temp_file.Close();
	
		while (!DM_List.IsEmpty())
		{
			p_Node = DM_List.RemoveHead();
			delete p_Node;
		}



	return 1;
}


int Exe_Merge(CString Filenames[])
{

		CString strTemp;	
		CString   strLine;
		CString   DM_Addr;
	
	
		if (NULL==exe_file.Open(Filenames[3], CFile::modeRead|CFile::typeText))
		{
			printf("can't open exe file %s", Filenames[3]);
			exit(1);
		}

		if (NULL==temp_file.Open(Filenames[2], CFile::modeRead|CFile::typeText))
		{
			printf("can't open temp file %s", Filenames[2]);
			exit(1);
		}
		
		if (NULL==out_file.Open(Filenames[4], CFile::modeWrite|CFile::typeText|CFile::modeCreate))
		{
			printf("can't open out file %s", Filenames[4]);
			exit(1);
		}

		while(exe_file.ReadString(strLine))
		{
			if (strLine.GetAt(0)=='@' &&strLine.GetAt(1)=='D')
			{
				strLine+="\n";
				out_file.WriteString(strLine); //Write @DA

				exe_file.ReadString(strLine);  //Get Address
				DM_Addr = strLine;

				strLine+="\n";
				out_file.WriteString(strLine);


				while(temp_file.ReadString(strLine)) //search the temp_file
				{
					
					if (strLine.GetAt(0)=='@' &&strLine.GetAt(1)=='D')
					{
						temp_file.ReadString(strLine);
					//	strLine.MakeUpper();
						if (DM_Addr == strLine) // if finded ,write tha data, then break;
						{						
								
								while(temp_file.ReadString(strLine))
								{
									if (strLine.GetAt(0)!='@')
									{
										strLine+="\n";
										out_file.WriteString(strLine);
										exe_file.ReadString(strTemp);
									}
									else
									{
										exe_file.ReadString(strLine);
										strLine+="\n";					//Write "#..."
										out_file.WriteString(strLine);

										//exe_file.ReadString(strTemp);

										temp_file.SeekToBegin();
										break;
									}
								}
								break;
								

						}

					}
				}
				temp_file.SeekToBegin();

			
			}

			else
			{
				strLine+="\n";
				out_file.WriteString(strLine);
			}
		}

		exe_file.Close();
		temp_file.Close();
		out_file.Close();
	
	return 1;
}

void Usage()
{
	printf("Usage of adsptool 0.55.011\n");
	printf("you can use the adsptool in two ways:\n");
	printf("1:  covert a .Sym file to .h file\n");
	printf(" example:"); 
	printf("	adsptool -s a.sym a.h\n");
	printf("2:   covent a .exe file to a new .exe file which used for palmadsp tool\n");
	printf(" example: ");
	printf(" adsptool -e -m a.sym -l file_all.txt -x old.exe  -d new.dat");

	return;
}




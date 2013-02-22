
#include <stdafx.h>

BOOL PromptOverwriteFile(CString strFileName, BOOL bForce)
{
	char pAnswer[80];
	CString strAnswer;
#ifndef __GNUC__
	CFileStatus status;

	if (bForce)									return TRUE;
	if (!CFile::GetStatus(strFileName, status))	return TRUE;
#else
	struct stat status;

	if (bForce)									return TRUE;
   if (stat(strFileName.c_str(),&status)) return TRUE;
#endif
	while (1)
	{
		printf("The destination file exists. Overwrite the file?(Yes/No):");
#ifndef __GNUC__
		gets(pAnswer);
#else
      fgets( pAnswer, sizeof(pAnswer), stdin );
#endif
		strAnswer = pAnswer;
		if (!strAnswer.CompareNoCase(_T("no")) || !strAnswer.CompareNoCase(_T("n")))
		{
			printf("Exit Convert without writing destination file");
			return FALSE;
		}
		if (!strAnswer.CompareNoCase(_T("yes")) || !strAnswer.CompareNoCase(_T("ye")) || !strAnswer.CompareNoCase(_T("y")))
		{
			break;
		}
	}
	return TRUE;
}

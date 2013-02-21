#include <stdafx.h>

BOOL PromptOverwriteFile(CString strFileName, BOOL bForce)
{
	char pAnswer[80];
	CString strAnswer;
	CFileStatus status;

	if (bForce)									return TRUE;
	if (!CFile::GetStatus(strFileName, status))	return TRUE;

	while (1)
	{
		printf("The destination file exists. Overwrite the file?(Yes/No):");
		gets(pAnswer);
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

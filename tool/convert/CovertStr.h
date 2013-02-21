#define	IP_ALEN					4
#define HW_ALEN					6

#define AR168_PROTOCOL_UNKNOWN	0
#define AR168_PROTOCOL_IAX2		1
#define AR168_PROTOCOL_SIP		2

BOOL _GetName(CString strSrc, CString& strName);
void _GetVal(CString strSrc, CString& strVal);
void DisplayError(CString strName);

BOOL _SetPort(char * p, CString strName, CString strVal, char *pLabel);
BOOL _SetVal(char * p, CString strName, CString strVal, char *pLabel, int iMaxlen);
BOOL _SetString(char * p, CString strName, CString strVal, char *pLabel, int iMaxlen);
BOOL _SetIPAddr(char * p, CString strName, CString strVal, char *pLabel);
BOOL _SetMacAddr(char * p, CString strName, CString strVal, char *pLabel);

CString _WritePort(unsigned char * p, CString strName, BOOL bQuote);
CString _WriteString(char * p, CString strName, int iMaxLen, BOOL bQuote);
CString _WriteVal(char * p, CString strName, BOOL bQuote);
CString _WriteIPAddr(unsigned char * p, CString strName, BOOL bQuote);
CString _WriteMacAddr(char * pMac, CString strName, BOOL bQuote);

CString _WritePBIndex(int iIndex, bool bName);
CString _WritePB(char * p, int iMaxLen);
int _GetPBIndex(CString cStr);
void _SetPBFlag(char * p, int iFlag);
void _SetPB(char * p, CString strName, CString strVal, int iMaxLen);

void AR168CompareCfg(char * pSettings, CString str);
void AR168ConvertCfg(char * pSettings, CString& strwrite);
void AR168ConvertCfg_IM(char * pSettings, CString& strwrite);
void AR168ComparePB(char * pSettings, CString str);
void AR168ConvertPB(char * pSettings, CString& strwrite);
BOOL AR168CompareMap(char * pSettings, CString str, int iItem);
CString _AR168ConvertMap(char * p);

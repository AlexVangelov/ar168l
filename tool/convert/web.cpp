#include "stdafx.h"
#include "Convert.h"

#include "..\\common\\common.h"

#include "..\\..\\include\\type.h"
#define CALL_SIP
#define CALL_IAX2
#include "..\\..\\include\\gdiconst.h"

#define ITEM_TYPE_TEXT		0
#define ITEM_TYPE_CHECKBOX	1
#define ITEM_TYPE_PASSWORD	2
#define ITEM_TYPE_RADIO		3
#define ITEM_TYPE_UNKNOWN	0xff

int find_char(TCHAR * pStr, char cVal, int iStart)
{
	int i;
	TCHAR * pCur;

	pCur = (TCHAR *)(pStr + iStart);
	for (i = iStart; *pCur != 0; i ++)
	{
		if (*pCur == cVal)		return i;
		pCur ++;
	}
	return -1;
}


CString RemoveQuote(CString strIn)
{
	int iPos;
	CString strOut;

	iPos = strIn.Find('"');
	if (iPos == -1)		return strIn;
	
	iPos ++;
	strOut = strIn.Mid(iPos);
	iPos = strOut.Find('"');
	if (iPos == -1)		return strOut;

	strOut = strOut.Left(iPos);
	return strOut;
}

void WebGetOptionsList(CString strInput, CStringList& strList)
{
	POSITION pos;
	int iPos;
	CString strName, strItem;

	iPos = strInput.Find(' ');
	strItem = strInput.Mid(iPos + 6);
	iPos = strItem.Find(' ');
	if (iPos != -1)
	{
		strName = strItem.Left(iPos);
	}
	else
	{
		strName = strItem;
	}
	for (pos = strList.GetHeadPosition(); pos != NULL;)
	{
		strItem = strList.GetNext(pos);
		if (!strItem.Compare(strName))
		{
			return;
		}
	}
	strList.AddTail(strName);
}

void WebGetStringList(CString strInput, int iStartPos, CStringList& strList)
{
	CString strItem;
	int iPos;
	TCHAR cChar;
	BOOL bQuote;

	strItem = "";
	bQuote = FALSE;
	for (iPos = iStartPos; iPos < strInput.GetLength(); iPos ++)
	{
		cChar = strInput.GetAt(iPos);
		if (cChar == '>')
		{
			if (strItem.GetLength())
			{
				strList.AddTail(strItem);
			}
			break;
		}
		else if (cChar == ' ' || cChar == '\t' || cChar == 0x0d || cChar == 0x0a)
		{
			if (bQuote)
			{
				strItem += cChar;
			}
			else
			{
				if (strItem.GetLength())
				{
					strList.AddTail(strItem);
				}
				strItem = "";
			}
		}
		else 
		{
			if (cChar == '"')
			{
				if (bQuote)		bQuote = FALSE;
				else			bQuote = TRUE;
			}
			strItem += cChar;
		}
	}
}

CString WebUpdateInput(CString strInput)
{
	CString strOutput, strItem, strTemp;
	CStringList strList;
	POSITION pos1, pos2;
	int iType;

	WebGetStringList(strInput, 6, strList);

	strOutput = _T("<INPUT");
	iType = ITEM_TYPE_TEXT;
	for (pos1 = strList.GetHeadPosition(); pos1 != NULL;)
	{
		pos2 = pos1;
		strItem = strList.GetNext(pos1);
		strTemp = strItem.Left(5);
		if (!strTemp.CompareNoCase(_T("name=")))
		{
			strItem = strItem.Right(strItem.GetLength()-5);
			strItem = RemoveQuote(strItem);
			strOutput += _T(" name=");
			strOutput += strItem;
			strList.RemoveAt(pos2);
		}
		else if (!strTemp.CompareNoCase(_T("type=")))
		{
			strTemp = strItem.Right(strItem.GetLength()-5);
			strTemp = RemoveQuote(strTemp);
			if (!strTemp.CompareNoCase(_T("checkbox")))
			{
				iType = ITEM_TYPE_CHECKBOX;
			}
			else if (!strTemp.CompareNoCase(_T("text")))
			{
				iType = ITEM_TYPE_TEXT;
			}
			else if (!strTemp.CompareNoCase(_T("password")))
			{
				iType = ITEM_TYPE_PASSWORD;
			}
			else if (!strTemp.CompareNoCase(_T("radio")))
			{
				iType = ITEM_TYPE_RADIO;
			}
			else
			{
				iType = ITEM_TYPE_UNKNOWN;
			}
		}
		else if (!strItem.CompareNoCase(_T("checked")))
		{
			strList.RemoveAt(pos2);
		}
	}

	for (pos1 = strList.GetHeadPosition(); pos1 != NULL;)
	{
		pos2 = pos1;
		strItem = strList.GetNext(pos1);
		strTemp = strItem.Left(6);
		if (!strTemp.CompareNoCase(_T("value=")))
		{
			if (iType == ITEM_TYPE_CHECKBOX || iType == ITEM_TYPE_TEXT || iType == ITEM_TYPE_PASSWORD)
			{
				strList.RemoveAt(pos2);
			}
			else if (iType == ITEM_TYPE_RADIO)
			{
				strItem = strItem.Right(strItem.GetLength()-6);
				strItem = RemoveQuote(strItem);
				strOutput += _T(" value=");
				strOutput += strItem;
				strList.RemoveAt(pos2);
			}
		}
	}
	for (pos1 = strList.GetHeadPosition(); pos1 != NULL;)
	{
		strOutput += _T(' ');
		strItem = strList.GetNext(pos1);
		strOutput += strItem;
	}
	strOutput += _T('>');
	return strOutput;
}

CString WebUpdateSelect(CString strInput)
{
	CString strOutput, strItem, strTemp;
	CStringList strList;
	POSITION pos1, pos2;

	WebGetStringList(strInput, 7, strList);

	strOutput = _T("<SELECT");
	for (pos1 = strList.GetHeadPosition(); pos1 != NULL;)
	{
		pos2 = pos1;
		strItem = strList.GetNext(pos1);
		strTemp = strItem.Left(5);
		if (!strTemp.CompareNoCase(_T("name=")))
		{
			strItem = strItem.Right(strItem.GetLength()-5);
			strItem = RemoveQuote(strItem);
			strOutput += _T(" name=");
			strOutput += strItem;
			strList.RemoveAt(pos2);
		}
	}
	for (pos1 = strList.GetHeadPosition(); pos1 != NULL;)
	{
		strOutput += _T(' ');
		strItem = strList.GetNext(pos1);
		strOutput += strItem;
	}
	strOutput += _T('>');
	return strOutput;
}

CString WebUpdateOption(CString strInput)
{
	CString strOutput, strItem, strTemp;
	CStringList strList;
	POSITION pos1, pos2;

	WebGetStringList(strInput, 7, strList);

	strOutput = _T("<OPTION");
	for (pos1 = strList.GetHeadPosition(); pos1 != NULL;)
	{
		pos2 = pos1;
		strItem = strList.GetNext(pos1);
		strTemp = strItem.Left(6);
		if (!strTemp.CompareNoCase(_T("value=")))
		{
			strItem = strItem.Right(strItem.GetLength()-6);
			strItem = RemoveQuote(strItem);
			strOutput += _T(" value=");
			strOutput += strItem;
			strList.RemoveAt(pos2);
		}
		else if (!strItem.CompareNoCase(_T("selected")))
		{
			strList.RemoveAt(pos2);
		}
	}
	for (pos1 = strList.GetHeadPosition(); pos1 != NULL;)
	{
		strOutput += _T(' ');
		strItem = strList.GetNext(pos1);
		strOutput += strItem;
	}
	strOutput += _T('>');
	return strOutput;
}

void OnWeb2Web(CString strSrcFile, CString strDstFile, BOOL bForce)
{
	CStdioFile in, out;
	CString strInput, strOutput;
	CString strItem, strNew;
	int iBeginPos, iEndPos, iStartIndex;

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

	strInput = _T("");
	while (in.ReadString(strItem))
	{
		strItem.TrimLeft();
		strInput += strItem;
		strInput += _T("\n");
	}

	iStartIndex = 0;
	strOutput = _T("");
	while (iStartIndex < strInput.GetLength())
	{
		iBeginPos = find_char((TCHAR *)(LPCTSTR)strInput, '<', iStartIndex);
		iEndPos = find_char((TCHAR *)(LPCTSTR)strInput, '>', iStartIndex);
		if (iBeginPos == -1 || iEndPos == -1)	break;
		if (iEndPos - iBeginPos < 6)	
		{
			iStartIndex = iEndPos + 1;
			continue;
		}

		strItem = strInput.Mid(iBeginPos, 7);
		strItem.MakeUpper();
		if (!memcmp((LPCTSTR)strItem, "<INPUT", 6))
		{
			strItem = strInput.Mid(iBeginPos, (iEndPos + 1 - iBeginPos));
			strNew = WebUpdateInput(strItem);
		}
		else if (!strItem.Compare(_T("<SELECT")))
		{
			strItem = strInput.Mid(iBeginPos, (iEndPos + 1 - iBeginPos));
			strNew = WebUpdateSelect(strItem);
		}
		else if (!strItem.Compare(_T("<OPTION")))
		{
			strItem = strInput.Mid(iBeginPos, (iEndPos + 1 - iBeginPos));
			strNew = WebUpdateOption(strItem);
		}
		else
		{
			iStartIndex = iEndPos + 1;
			continue;
		}
		strOutput += strInput.Left(iBeginPos);
		strOutput += strNew;
		strInput = strInput.Right(strInput.GetLength() - iEndPos - 1);
		iStartIndex = 0;
	}

	strOutput += strInput;

	out.WriteString(strOutput);
	in.Close();
	out.Close();
}

void OnWeb2Info(CString strSrcFile, CString strDstFile, BOOL bForce)
{
	CStdioFile in, out;
	CString strInput, strOutput;
	CString strItem, strNew;
	CStringList listOptions;
	POSITION pos;
	int iBeginPos, iEndPos, iStartIndex;

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

	strInput = _T("");
	while (in.ReadString(strItem))
	{
		strItem.TrimLeft();
		strInput += strItem;
		strInput += _T("\n");
	}

	iStartIndex = 0;
	while (iStartIndex < strInput.GetLength())
	{
		iBeginPos = find_char((TCHAR *)(LPCTSTR)strInput, '<', iStartIndex);
		iEndPos = find_char((TCHAR *)(LPCTSTR)strInput, '>', iStartIndex);
		if (iBeginPos == -1 || iEndPos == -1)	break;
		if (iEndPos - iBeginPos < 6)	
		{
			iStartIndex = iEndPos + 1;
			continue;
		}

		strItem = strInput.Mid(iBeginPos, 7);
		strItem.MakeUpper();
		if (!memcmp((LPCTSTR)strItem, "<INPUT", 6))
		{
			strItem = strInput.Mid(iBeginPos, (iEndPos + 1 - iBeginPos));
			strNew = WebUpdateInput(strItem);
			WebGetOptionsList(strNew, listOptions);
		}
		else if (!strItem.Compare(_T("<SELECT")))
		{
			strItem = strInput.Mid(iBeginPos, (iEndPos + 1 - iBeginPos));
			strNew = WebUpdateSelect(strItem);
			WebGetOptionsList(strNew, listOptions);
		}
		else
		{
			iStartIndex = iEndPos + 1;
			continue;
		}
		strInput = strInput.Right(strInput.GetLength() - iEndPos - 1);
		iStartIndex = 0;
	}

	iStartIndex = 0;
	for (pos = listOptions.GetHeadPosition(); pos != NULL;)
	{
		strItem = listOptions.GetNext(pos);
		strItem += _T("=");
		if (strItem == _cIPtype)
		{
			strNew = "OPT_IP_TYPE";
		}
		else if (strItem == _cIPAddress)
		{
			strNew = "OPT_IP_ADDRESS";
		}
		else if (strItem == _cSubMask)
		{
			strNew = "OPT_SUBNET_MASK";
		}
		else if (strItem == _cRouterIP)
		{
			strNew = "OPT_ROUTER_IP";
		}
		else if (strItem == _cDnsType)
		{
			strNew = "OPT_DNS_TYPE";
		}
		else if (strItem == _cDnsIP)
		{
			strNew = "OPT_DNS_IP";
		}
		else if (strItem == _cDnsIP2)
		{
			strNew = "OPT_DNS_IP2";
		}
		else if (strItem == _cLay3Qos)
		{
			strNew = "OPT_LAY3_QOS";
		}
		else if (strItem == _cVlanTag)
		{
			strNew = "OPT_VLAN_TAG";
		}
		else if (strItem == _cQosPriority)
		{
			strNew = "OPT_QOS_PRIORITY";
		}
		else if (strItem == _cVlanTag2)
		{
			strNew = "OPT_VLAN_TAG2";
		}
		else if (strItem == _cQosPriority2)
		{
			strNew = "OPT_QOS_PRIORITY2";
		}
		else if (strItem == _cVlanTag3)
		{
			strNew = "OPT_VLAN_TAG3";
		}
		else if (strItem == _cQosPriority3)
		{
			strNew = "OPT_QOS_PRIORITY3";
		}
		else if (strItem == _cPPPoeID)
		{
			strNew = "OPT_PPPOE_ID";
		}
		else if (strItem == _cPPPoePIN)
		{
			strNew = "OPT_PPPOE_PIN";
		}
		else if (strItem == _cSerialMethod)
		{
			strNew = "OPT_SERIAL_METHOD";
		}
		else if (strItem == _cBaudRate)
		{
			strNew = "OPT_BAUD_RATE";
		}
		else if (strItem == _cAdminPIN)
		{
			strNew = "OPT_ADMIN_PIN";
		}
		else if (strItem == _cAdminUser)
		{
			strNew = "OPT_ADMIN_USER";
		}
		else if (strItem == _cVolInput)
		{
			strNew = "OPT_VOL_INPUT";
		}
		else if (strItem == _cVolOutput)
		{
			strNew = "OPT_VOL_OUTPUT";
		}
		else if (strItem == _cSpeakerVol)
		{
			strNew = "OPT_SPEAKER_VOL";
		}
		else if (strItem == _cProvisionServer)
		{ 
			strNew = "OPT_PROVISION_SERVER";
		}
		else if (strItem == _cProvisionTTL)
		{
			strNew = "OPT_PROVISION_TTL";
		}
		else if (strItem == _cProvisionFilename)
		{
			strNew = "OPT_PROV_FILENAME";
		}
		else if (strItem == _cProvisionPort)
		{
			strNew = "OPT_PROVISION_PORT";
		}
		else if (strItem == _cTimeZone)
		{
			strNew = "OPT_TIME_ZONE";
		}
		else if (strItem == _cDayLight)
		{
			strNew = "OPT_DAY_LIGHT";
		}
		else if (strItem == _cSntpDns)
		{
			strNew = "OPT_SNTP_DNS";
		}
		else if (strItem == _cVoiceCoder0)
		{
			strNew = "OPT_VOICE_CODER0";
		}
		else if (strItem == _cVoiceCoder1)
		{
			strNew = "OPT_VOICE_CODER1";
		}
		else if (strItem == _cVoiceCoder2)
		{
			strNew = "OPT_VOICE_CODER2";
		}
		else if (strItem == _cVoiceCoder3)
		{
			strNew = "OPT_VOICE_CODER3";
		}
		else if (strItem == _cVoiceCoder4)
		{
			strNew = "OPT_VOICE_CODER4";
		}
		else if (strItem == _cVoiceCoder5)
		{
			strNew = "OPT_VOICE_CODER5";
		}
		else if (strItem == _cVoiceCoder6)
		{
			strNew = "OPT_VOICE_CODER6";
		}
		else if (strItem == _cSpeexRate)
		{
			strNew = "OPT_SPEEX_RATE";
		}
		else if (strItem == _cVad)
		{
			strNew = "OPT_VAD";
		}
		else if (strItem == _cAgc)
		{
			strNew = "OPT_AGC";
		}
		else if (strItem == _cAec)
		{
			strNew = "OPT_AEC";
		}
		else if (strItem == _cIlbcFrame)
		{
			strNew = "OPT_ILBC_FRAME";
		}
		else if (strItem == _cFrameNumber0)
		{
			strNew = "OPT_FRAME_NUMBER0";
		}
		else if (strItem == _cFrameNumber1)
		{
			strNew = "OPT_FRAME_NUMBER1";
		}
		else if (strItem == _cFrameNumber2)
		{
			strNew = "OPT_FRAME_NUMBER2";
		}
		else if (strItem == _cFrameNumber3)
		{
			strNew = "OPT_FRAME_NUMBER3";
		}
		else if (strItem == _cFrameNumber4)
		{
			strNew = "OPT_FRAME_NUMBER4";
		}
		else if (strItem == _cFrameNumber5)
		{
			strNew = "OPT_FRAME_NUMBER5";
		}
		else if (strItem == _cFrameNumber6)
		{
			strNew = "OPT_FRAME_NUMBER6";
		}
		else if (strItem == _cFwdNumber)
		{
			strNew = "OPT_FWD_NUMBER";
		}
		else if (strItem == _cFwdAlways)
		{
			strNew = "OPT_FWD_ALWAYS";
		}
		else if (strItem == _cFwdBusy)
		{
			strNew = "OPT_FWD_BUSY";
		}
		else if (strItem == _cFwdNoAnswer)
		{
			strNew = "OPT_FWD_NOANSWER";
		}
		else if (strItem == _cNoAnswerTimeout)
		{
			strNew = "OPT_NOANSWER_TIMEOUT";
		}
		else if (strItem == _cCallWaiting)
		{
			strNew = "OPT_CALL_WAITING";
		}
		else if (strItem == _cAutoAnswer)
		{
			strNew = "OPT_AUTO_ANSWER";
		}
		else if (strItem == _cDialPrefix)
		{
			strNew = "OPT_DIAL_PREFIX";
		}
		else if (strItem == _cHotLineNumber)
		{
			strNew = "OPT_HOTLINE_NUMBER";
		}
		else if (strItem == _cUseDigitmap)
		{
			strNew = "OPT_USE_DIGITMAP";
		}
		else if (strItem == _cDigitmapTimeout)
		{
			strNew = "OPT_DIGITMAP_TIMEOUT";
		}
		else if (strItem == _cPoundAsCall)
		{
			strNew = "OPT_POUND_AS_CALL";
		}
		else if (strItem == _cRedialTimeout)
		{
			strNew = "OPT_REDIAL_TIMEOUT";
		}
		else if (strItem == _cCallLogTimeout)
		{
			strNew = "OPT_CALL_LOG_TIMEOUT";
		}
		else if (strItem == _cRinging)
		{
			strNew = "OPT_RINGING";
		}
		else if (strItem == _cGradualRinging)
		{
			strNew = "OPT_GRADUAL_RINGING";
		}
		else if (strItem == _cGetVoiceMsg)
		{
			strNew = "OPT_GET_VOICE_MSG";
		}
		else if (strItem == _cRegister)
		{
			strNew = "OPT_REGISTER";
		}
		else if (strItem == _cLocalPort)
		{
			strNew = "OPT_LOCAL_PORT";
		}
		else if (strItem == _cAuthID)
		{
			strNew = "OPT_AUTH_ID";
		}
		else if (strItem == _cAuthPIN)
		{
			strNew = "OPT_AUTH_PIN";
		}
		else if (strItem == _cUserName)
		{
			strNew = "OPT_USER_NAME";
		}
		else if (strItem == _cUserNumber)
		{
			strNew = "OPT_USER_NUMBER";
		}
		else if (strItem == _cSipProxy)
		{
			strNew = "OPT_SIP_PROXY";
		}
		else if (strItem == _cSipPort)
		{
			strNew = "OPT_SIP_PORT";
		}
		else if (strItem == _cSipDomain)
		{
			strNew = "OPT_SIP_DOMAIN";
		}
		else if (strItem == _cOutBoundProxy)
		{
			strNew = "OPT_OUTBOUND_PROXY";
		}
		else if (strItem == _cRtpPort)
		{
			strNew = "OPT_RTP_PORT";
		}
		else if (strItem == _cDtmfPayload)
		{
			strNew = "OPT_DTMF_PAYLOAD";
		}
		else if (strItem == _cIlbcPayload)
		{
			strNew = "OPT_ILBC_PAYLOAD";
		}
		else if (strItem == _cAdpcm32Payload)
		{
			strNew = "OPT_ADPCM32_PAYLOAD";
		}
		else if (strItem == _cSpeexPayload)
		{
			strNew = "OPT_SPEEX_PAYLOAD";
		}
		else if (strItem == _cDtmfType)
		{
			strNew = "OPT_DTMF_TYPE";
		}
		else if (strItem == _cDnsSrv)
		{
			strNew = "OPT_DNS_SRV";
		}
		else if (strItem == _cRegisterTTL)
		{
			strNew = "OPT_REGISTER_TTL";
		}
		else if (strItem == _cKeepAlive)
		{
			strNew = "OPT_KEEP_ALIVE";
		}
		else if (strItem == _cMessageNumber)
		{
			strNew = "OPT_MESSAGE_NUMBER";
		}
		else if (strItem == _cPrack)
		{
			strNew = "OPT_PRACK";
		}
		else if (strItem == _cSubscribeMWI)
		{
			strNew = "OPT_SUBSCRIBE_MWI";
		}
		else if (strItem == _cProxyRequire)
		{
			strNew = "OPT_PROXY_REQUIRE";
		}
		else if (strItem == _cNatTraversal)
		{
			strNew = "OPT_NAT_TRAVERSAL";
		}
		else if (strItem == _cNatIP)
		{
			strNew = "OPT_NAT_IP";
		}
		else if (strItem == _cStunServer)
		{
			strNew = "OPT_STUN_SERVER";
		}
		else if (strItem == _cStunPort)
		{
			strNew = "OPT_STUN_PORT";
		}
		else if (strItem == _cIax2Server)
		{
			strNew = "OPT_IAX2_SERVER";
		}
		else if (strItem == _cIax2Port)
		{
			strNew = "OPT_IAX2_PORT";
		}
		else if (strItem == _cDigitDialing)
		{
			strNew = "OPT_DIGIT_DIALING";
		}
/*		else if (strItem == _cUdpDebug)
		{
			strNew = "OPT_UDP_DEBUG";
		}
*/
		else if (strItem == _cSyslogIP)
		{
			strNew = "OPT_SYSLOG_IP";
		}
		else if (strItem == _cTimeOut)
		{
			strNew = "OPT_TIME_OUT";
		}
		else if (strItem == _cRingVolOutput)
		{
			strNew = "OPT_RING_VOL_OUTPUT";
		}
		else
		{
			continue;
		}
		if (iStartIndex)
		{
			strOutput += ", \n";
		}
		strOutput += strNew;
		iStartIndex ++;
	}

	strItem.Format(_T("#define WEB_INFO_NUM	%d\n\nconst unsigned short web_info_code[WEB_INFO_NUM] = {\n"), iStartIndex);
	strOutput = strItem + strOutput;
	strOutput += _T("\n};\n");
	out.WriteString(strOutput);

	in.Close();
	out.Close();
}

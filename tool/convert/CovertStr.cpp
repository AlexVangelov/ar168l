#include "stdafx.h"

#include "CovertStr.h"
#include "..//..//include//type.h"
#define CALL_SIP
#define CALL_IAX2
#include "..//..//include//gdiconst.h"
#include "..//..//include//ar168.h"

CString _OutputStr(char *pVal, CString strName, BOOL bQuote)
{
	CString strWrite;

	strWrite = strName;
	if (bQuote)
	{
		strWrite += "\"";
		strWrite += pVal;
		strWrite += "\"";
	}
	else
	{
		strWrite += pVal;
	}
	strWrite += "\n";
	return strWrite;
}

CString _AR168ConvertMap(char * p)
{
	char pTemp[DIGITMAP_ENTRY_LEN];
	int i;
	CString strWrite;
	char *pCur;

	strWrite = _T("[digitmap]");
	strWrite += _T("\n");

	pCur = p;
	i = 0;
	for (i = 0; i < DIGITMAP_ENTRY_NUM; i++)
	{
		memcpy (pTemp, pCur, DIGITMAP_ENTRY_LEN-1);
		pTemp[DIGITMAP_ENTRY_LEN-1] = 0;
		if (strlen(pTemp))
		{
			strWrite += pTemp;
			strWrite += _T("\n");
		}
		pCur += DIGITMAP_ENTRY_LEN;
	}

	return strWrite;
}

CString _WriteDigitMap(char * p)
{
	CString strWrite;
	char pTemp[DIGITMAP_ENTRY_LEN];
	int i;
	bool bFirst;

	bFirst = TRUE;
	strWrite = _cDigitmapString;
	strWrite += _T("\"");
	for (i = 0; i < DIGITMAP_ENTRY_NUM; i++)
	{
		memcpy (pTemp, p, DIGITMAP_ENTRY_LEN-1);
		pTemp[DIGITMAP_ENTRY_LEN-1] = 0;
		if (strlen(pTemp))
		{
			if (!bFirst)
			{
				strWrite += _T("|");
				strWrite += pTemp;
			}
			else
			{
				strWrite += pTemp;
				bFirst = FALSE;
			}
		}
		p += DIGITMAP_ENTRY_LEN;
	}
	strWrite += _T("\"\n");
	return strWrite;
}

CString _WritePort(unsigned char * p, CString strName, BOOL bQuote)
{
	char pTemp[6];

	itoa((unsigned int)((p[0] << 8) + p[1]), pTemp, 10);
	return _OutputStr(pTemp, strName, bQuote);
}

CString _WriteString(char * p, CString strName, int iMaxLen, BOOL bQuote)
{
	char pTemp[256];

	memcpy (pTemp, p, iMaxLen);
	pTemp[iMaxLen] = 0;
	return _OutputStr(pTemp, strName, bQuote);
}

CString _WriteVal(char * p, CString strName, BOOL bQuote)
{
	char pTemp[6];

	itoa((unsigned char)p[0], pTemp, 10);
	return _OutputStr(pTemp, strName, bQuote);
}

CString _WriteIPAddr(unsigned char * p, CString strName, BOOL bQuote)
{
	int i, iLen;
	char pBuf[20];

	pBuf[0] = 0;
	for (i = 0; i < IP_ALEN; i ++)
	{
		iLen = strlen(pBuf);
		itoa(p[i], pBuf+iLen, 10);
		if (i < IP_ALEN-1)
		{
			strcat(pBuf, ".");
		}
	}
	return _OutputStr(pBuf, strName, bQuote);
}

CString _WriteMacAddr(char * pMac, CString strName, BOOL bQuote)
{
	int i, iPos;
	unsigned char iVal;
	char pBuf[40];

	iPos = 0;
	pBuf[0] = 0;
	for (i = 0; i < HW_ALEN; i ++)
	{
		iVal = pMac[i];
		memcpy (pBuf+iPos, "0x", 2);
		iPos += 2;
		if (iVal < 0x10)
		{
			pBuf[iPos] = '0';
			iPos ++;
			itoa(iVal, pBuf+iPos, 16);
			iPos ++;
		}
		else
		{
			itoa(iVal, pBuf+iPos, 16);
			iPos += 2;
		}
		pBuf[iPos] = ',';
		iPos ++;
	}
	iPos --;
	pBuf[iPos] = 0;

	return _OutputStr(pBuf, strName, bQuote);
}

CString _WritePBIndex(int iIndex, bool bName)
{
	char pNum[4];
	char pIndex[4];
	int iLen;
	CString strWrite;

	strWrite = _T("");
	itoa(iIndex, pNum, 10);
	iLen = strlen(pNum);
	if (iLen > 3)	return strWrite;

	strcpy(pIndex, "000");
	memcpy((PCHAR)(pIndex+3-iLen), pNum, iLen);
	if (!bName)
	{
		strWrite += _T("n");
	}
	else
	{
		strWrite += _T("p");
	}
	strWrite += pIndex;
	strWrite += _T("=");
	return strWrite;
}

CString _WritePB(char * p, int iMaxLen)
{
	char *pTemp;
	CString strWrite;

	pTemp = (char *)malloc(iMaxLen + 1);
	memcpy (pTemp, p, iMaxLen);
	pTemp[iMaxLen] = 0;

	strWrite = pTemp;
	strWrite += _T("\n");

	free(pTemp);
	return strWrite;
}

void AR168ConvertCfg(char * pSettings, CString& strwrite)
{
	CString strOem, strProduct, strProtocol;

	strProduct = (char *)(pSettings + OPT_FLAG_PRODUCT);
	strProduct.MakeLower();
	strOem = (char *)(pSettings + OPT_FLAG_OEM);
	strOem.MakeLower();

	strwrite = _T("[settings]\n");

	strwrite += _WriteString((char *)(pSettings + OPT_CONFIG_ID), _cConfigID, MAX_CONFIG_ID, FALSE);
	strwrite += _WriteMacAddr((char *)(pSettings + OPT_MAC_ADDRESS), _cMacAddress, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SYSTEM_PAGE), _cSystemPage, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_PRODUCT), _cFlagProduct, MAX_FLAG_PRODUCT_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_PROTOCOL), _cFlagProtocol, MAX_FLAG_PROTOCOL, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_COUNTRY), _cFlagCountry, MAX_FLAG_COUNTRY, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_OEM), _cFlagOem, MAX_FLAG_OEM_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_VERSION), _cFlagVersion, MAX_FLAG_VERSION, FALSE);
	//Network settings
	strwrite += _WriteVal((char *)(pSettings + OPT_IP_TYPE), _cIPtype, FALSE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_IP_ADDRESS), _cIPAddress, FALSE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_SUBNET_MASK), _cSubMask, FALSE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_ROUTER_IP), _cRouterIP, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_PPPOE_ID), _cPPPoeID, MAX_PPPOE_ID_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_PPPOE_PIN), _cPPPoePIN, MAX_PPPOE_PIN_LEN, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DNS_TYPE), _cDnsType, FALSE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_DNS_IP), _cDnsIP, FALSE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_DNS_IP2), _cDnsIP2, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_LAY3_QOS), _cLay3Qos, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_VLAN_TAG), _cVlanTag, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_QOS_PRIORITY), _cQosPriority, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_VLAN_TAG2), _cVlanTag2, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_QOS_PRIORITY2), _cQosPriority2, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_VLAN_TAG3), _cVlanTag3, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_QOS_PRIORITY3), _cQosPriority3, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SERIAL_METHOD), _cSerialMethod, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_BAUD_RATE), _cBaudRate, FALSE);
	//System settings
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_SYSLOG_IP), _cSyslogIP, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_TIME_ZONE), _cTimeZone, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DAY_LIGHT), _cDayLight, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_SNTP_DNS), _cSntpDns, MAX_DNS_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_ADMIN_PIN), _cAdminPIN, MAX_ADMIN_PIN_LEN, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOL_INPUT), _cVolInput, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOL_OUTPUT), _cVolOutput, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SPEAKER_VOL), _cSpeakerVol, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_RING_VOL_OUTPUT), _cRingVolOutput, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_PROVISION_SERVER), _cProvisionServer, MAX_DNS_LEN, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_PROVISION_PORT), _cProvisionPort, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_PROVISION_TTL), _cProvisionTTL, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_PROV_FILENAME), _cProvisionFilename, MAX_DNS_LEN, FALSE);
	//Voice settings
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER0), _cVoiceCoder0, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER0), _cFrameNumber0, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER1), _cVoiceCoder1, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER1), _cFrameNumber1, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER2), _cVoiceCoder2, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER2), _cFrameNumber2, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER3), _cVoiceCoder3, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER3), _cFrameNumber3, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER4), _cVoiceCoder4, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER4), _cFrameNumber4, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER5), _cVoiceCoder5, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER5), _cFrameNumber5, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER6), _cVoiceCoder6, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER6), _cFrameNumber6, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SPEEX_RATE), _cSpeexRate, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VAD), _cVad, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_AGC), _cAgc, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_AEC), _cAec, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_ILBC_FRAME), _cIlbcFrame, FALSE);
	//Dialplan settings
	strwrite += _WriteString((char *)(pSettings + OPT_FWD_NUMBER), _cFwdNumber, MAX_USER_NUMBER_LEN, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FWD_ALWAYS), _cFwdAlways, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FWD_BUSY), _cFwdBusy, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FWD_NOANSWER), _cFwdNoAnswer, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_NOANSWER_TIMEOUT), _cNoAnswerTimeout, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_AUTO_ANSWER), _cAutoAnswer, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_CALL_WAITING), _cCallWaiting, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_DIAL_PREFIX), _cDialPrefix, MAX_DIAL_PREFIX_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_HOTLINE_NUMBER), _cHotLineNumber, MAX_USER_NUMBER_LEN, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_USE_DIGITMAP), _cUseDigitmap, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DIGITMAP_TIMEOUT), _cDigitmapTimeout, FALSE);
	strwrite += _WriteVal((char *)(pSettings + OPT_POUND_AS_CALL), _cPoundAsCall, FALSE);

	if (strProduct == "bt2008" || strProduct == "bt2008n" || strOem == "bt2008")
	{
		strwrite += _WriteVal((char *)(pSettings + OPT_REDIAL_TIMEOUT), _cRedialTimeout, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_CALL_LOG_TIMEOUT), _cCallLogTimeout, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_RINGING), _cRinging, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_GRADUAL_RINGING), _cGradualRinging, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_GET_VOICE_MSG), _cGetVoiceMsg, FALSE);
	}
	//Protocol settings
	strwrite += _WriteVal((char *)(pSettings + OPT_REGISTER), _cRegister, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_LOCAL_PORT), _cLocalPort, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_AUTH_ID), _cAuthID, MAX_AUTH_ID_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_AUTH_PIN), _cAuthPIN, MAX_AUTH_PIN_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_USER_NAME), _cUserName, MAX_USER_NAME_LEN, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_USER_NUMBER), _cUserNumber, MAX_USER_NUMBER_LEN, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_REGISTER_TTL), _cRegisterTTL, FALSE);
	strwrite += _WriteString((char *)(pSettings + OPT_MESSAGE_NUMBER), _cMessageNumber, MAX_USER_NUMBER_LEN, FALSE);
	
	strProtocol = (char *)(pSettings + OPT_FLAG_PROTOCOL);
	strProtocol.MakeLower();
	if (strProtocol == "iax2")
	{
		strwrite += _WriteString((char *)(pSettings + OPT_IAX2_SERVER), _cIax2Server, MAX_DNS_LEN, FALSE);
		strwrite += _WritePort((unsigned char *)(pSettings + OPT_IAX2_PORT), _cIax2Port, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_DIGIT_DIALING), _cDigitDialing, FALSE);
	}
	else if (strProtocol == "sip")
	{
		strwrite += _WriteString((char *)(pSettings + OPT_SIP_PROXY), _cSipProxy, MAX_DNS_LEN, FALSE);
		strwrite += _WritePort((unsigned char *)(pSettings + OPT_SIP_PORT), _cSipPort, FALSE);
		strwrite += _WriteString((char *)(pSettings + OPT_SIP_DOMAIN), _cSipDomain, MAX_DNS_LEN, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_OUTBOUND_PROXY), _cOutBoundProxy, FALSE);
		strwrite += _WritePort((unsigned char *)(pSettings + OPT_RTP_PORT), _cRtpPort, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_KEEP_ALIVE), _cKeepAlive, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_DNS_SRV), _cDnsSrv, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_PRACK), _cPrack, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_SUBSCRIBE_MWI), _cSubscribeMWI, FALSE);
		strwrite += _WriteString((char *)(pSettings + OPT_PROXY_REQUIRE), _cProxyRequire, MAX_DNS_LEN, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_NAT_TRAVERSAL), _cNatTraversal, FALSE);
		strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_NAT_IP), _cNatIP, FALSE);
		strwrite += _WriteString((char *)(pSettings + OPT_STUN_SERVER), _cStunServer, MAX_DNS_LEN, FALSE);
		strwrite += _WritePort((unsigned char *)(pSettings + OPT_STUN_PORT), _cStunPort, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_DTMF_TYPE), _cDtmfType, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_DTMF_PAYLOAD), _cDtmfPayload, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_ILBC_PAYLOAD), _cIlbcPayload, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_ADPCM32_PAYLOAD), _cAdpcm32Payload, FALSE);
		strwrite += _WriteVal((char *)(pSettings + OPT_SPEEX_PAYLOAD), _cSpeexPayload, FALSE);
	}

	strwrite += "\n";
	strwrite += _AR168ConvertMap((char *)(pSettings+OPT_DIGIT_MAP));

}

void AR168ConvertCfg_IM(char * pSettings, CString& strwrite)
{
	strwrite = _WriteMacAddr((char *)(pSettings + OPT_MAC_ADDRESS), _cMacAddress, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SYSTEM_PAGE), _cSystemPage, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_PRODUCT), _cFlagProduct, MAX_FLAG_PRODUCT_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_PROTOCOL), _cFlagProtocol, MAX_FLAG_PROTOCOL, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_COUNTRY), _cFlagCountry, MAX_FLAG_COUNTRY, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_OEM), _cFlagOem, MAX_FLAG_OEM_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_FLAG_VERSION), _cFlagVersion, MAX_FLAG_VERSION, TRUE);
	//Network settings
	strwrite += _WriteVal((char *)(pSettings + OPT_IP_TYPE), _cIPtype, TRUE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_IP_ADDRESS), _cIPAddress, TRUE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_SUBNET_MASK), _cSubMask, TRUE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_ROUTER_IP), _cRouterIP, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_PPPOE_ID), _cPPPoeID, MAX_PPPOE_ID_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_PPPOE_PIN), _cPPPoePIN, MAX_PPPOE_PIN_LEN, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DNS_TYPE), _cDnsType, TRUE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_DNS_IP), _cDnsIP, TRUE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_DNS_IP2), _cDnsIP2, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_LAY3_QOS), _cLay3Qos, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_VLAN_TAG), _cVlanTag, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_QOS_PRIORITY), _cQosPriority, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_VLAN_TAG2), _cVlanTag2, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_QOS_PRIORITY2), _cQosPriority2, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_VLAN_TAG3), _cVlanTag3, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_QOS_PRIORITY3), _cQosPriority3, TRUE);
	//System settings
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_SYSLOG_IP), _cSyslogIP, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_TIME_ZONE), _cTimeZone, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DAY_LIGHT), _cDayLight, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_SNTP_DNS), _cSntpDns, MAX_DNS_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_ADMIN_PIN), _cAdminPIN, MAX_ADMIN_PIN_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_ADMIN_USER), _cAdminUser, MAX_ADMIN_USER_LEN, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOL_INPUT), _cVolInput, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOL_OUTPUT), _cVolOutput, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SPEAKER_VOL), _cSpeakerVol, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_RING_VOL_OUTPUT), _cRingVolOutput, FALSE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_PROVISION_TTL), _cProvisionTTL, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_PROV_FILENAME), _cProvisionFilename, MAX_DNS_LEN, TRUE);
	//Voice settings
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER0), _cVoiceCoder0, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER0), _cFrameNumber0, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER1), _cVoiceCoder1, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER1), _cFrameNumber1, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER2), _cVoiceCoder2, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER2), _cFrameNumber2, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER3), _cVoiceCoder3, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER3), _cFrameNumber3, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER4), _cVoiceCoder4, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER4), _cFrameNumber4, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER5), _cVoiceCoder5, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER5), _cFrameNumber5, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VOICE_CODER6), _cVoiceCoder6, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FRAME_NUMBER6), _cFrameNumber6, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SPEEX_RATE), _cSpeexRate, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_VAD), _cVad, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_AGC), _cAgc, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_AEC), _cAec, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_ILBC_FRAME), _cIlbcFrame, TRUE);
	//Dialplan settings
	strwrite += _WriteString((char *)(pSettings + OPT_FWD_NUMBER), _cFwdNumber, MAX_USER_NUMBER_LEN, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FWD_ALWAYS), _cFwdAlways, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FWD_BUSY), _cFwdBusy, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_FWD_NOANSWER), _cFwdNoAnswer, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_NOANSWER_TIMEOUT), _cNoAnswerTimeout, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_AUTO_ANSWER), _cAutoAnswer, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_CALL_WAITING), _cCallWaiting, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_DIAL_PREFIX), _cDialPrefix, MAX_DIAL_PREFIX_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_HOTLINE_NUMBER), _cHotLineNumber, MAX_USER_NUMBER_LEN, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_USE_DIGITMAP), _cUseDigitmap, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DIGITMAP_TIMEOUT), _cDigitmapTimeout, TRUE);
	strwrite += _WriteDigitMap((char *)(pSettings+OPT_DIGIT_MAP));
	strwrite += _WriteVal((char *)(pSettings + OPT_POUND_AS_CALL), _cPoundAsCall, TRUE);

	//Protocol settings
	strwrite += _WriteVal((char *)(pSettings + OPT_REGISTER), _cRegister, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_LOCAL_PORT), _cLocalPort, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_AUTH_ID), _cAuthID, MAX_AUTH_ID_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_AUTH_PIN), _cAuthPIN, MAX_AUTH_PIN_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_USER_NAME), _cUserName, MAX_USER_NAME_LEN, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_USER_NUMBER), _cUserNumber, MAX_USER_NUMBER_LEN, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_REGISTER_TTL), _cRegisterTTL, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_MESSAGE_NUMBER), _cMessageNumber, MAX_USER_NUMBER_LEN, TRUE);
	
	strwrite += _WriteString((char *)(pSettings + OPT_SIP_PROXY), _cSipProxy, MAX_DNS_LEN, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_SIP_PORT), _cSipPort, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_SIP_DOMAIN), _cSipDomain, MAX_DNS_LEN, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_OUTBOUND_PROXY), _cOutBoundProxy, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_RTP_PORT), _cRtpPort, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_KEEP_ALIVE), _cKeepAlive, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DNS_SRV), _cDnsSrv, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_PRACK), _cPrack, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SUBSCRIBE_MWI), _cSubscribeMWI, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_PROXY_REQUIRE), _cProxyRequire, MAX_DNS_LEN, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_NAT_TRAVERSAL), _cNatTraversal, TRUE);
	strwrite += _WriteIPAddr((unsigned char *)(pSettings + OPT_NAT_IP), _cNatIP, TRUE);
	strwrite += _WriteString((char *)(pSettings + OPT_STUN_SERVER), _cStunServer, MAX_DNS_LEN, TRUE);
	strwrite += _WritePort((unsigned char *)(pSettings + OPT_STUN_PORT), _cStunPort, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DTMF_TYPE), _cDtmfType, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_DTMF_PAYLOAD), _cDtmfPayload, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_ILBC_PAYLOAD), _cIlbcPayload, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_ADPCM32_PAYLOAD), _cAdpcm32Payload, TRUE);
	strwrite += _WriteVal((char *)(pSettings + OPT_SPEEX_PAYLOAD), _cSpeexPayload, TRUE);
	strwrite += "ENDOFCONFIG=\"ENDOFCONFIG\"";

}

void DisplayError(CString strName)
{
	CString cError;
	
	if (strName.Find(_T('=')) != -1)
	{
		strName.Replace(_T('=') , _T(' '));
	}
	cError = strName + _T("is invalid! Continue loading?");
	AfxMessageBox(cError);

//	if (AfxMessageBox(cError, MB_ICONQUESTION|MB_YESNO) != IDYES)
//		return FALSE;
//	else
//		return TRUE;
}

BOOL _GetName(CString strSrc, CString& strName)
{
	int iPos;

	iPos = strSrc.Find('=');
	if (iPos == -1)
	{
		return FALSE;
	}
	strName = strSrc.Left(iPos);
	strName.TrimRight();
	strName.TrimLeft();
	if (strName.GetLength() == 0)
	{
		return FALSE;
	}
	strName += "=";
	return TRUE;
}

void _GetVal(CString strSrc, CString& strVal)
{
	int iPos;

	iPos = strSrc.Find('=');
	strVal = strSrc.Right(strSrc.GetLength()-(iPos+1));

	strVal.TrimRight();
	strVal.TrimLeft();
	strVal.TrimLeft('"');
	strVal.TrimRight('"');
}

BOOL IsDigitStr(CString strVal)
{
	int i;
	TCHAR ch;

	for (i = 0; i < strVal.GetLength(); i ++)
	{
		ch = strVal.GetAt(i);
		if (ch < '0' || ch > '9')
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL _SetPort(char * p, CString strName, CString strVal, char *pLabel)
{
	unsigned int iPort;
	CString strLabel = pLabel;

	if (strLabel != strName)
//	if (strcmp((char *)(LPCTSTR)strName, pLabel))
	{
		return FALSE;
	}
	if (strVal.IsEmpty())
	{
		return TRUE;
	}

	if (!IsDigitStr(strVal))
	{
		DisplayError(strName);
		return TRUE;
	}
	iPort = (unsigned int)_wtoi(strVal);

	if(iPort <= 65535)
	{
		p[0] = (BYTE)((iPort>>8)&0xff);
		p[1] = (BYTE)(iPort&0xff);
	}
	else
	{
		DisplayError(strName);
	}
	return TRUE;
}

BOOL _SetString(char * p, CString strName, CString strVal, char *pLabel, int iMaxlen)
{
	int iLen;
	CString strLabel = pLabel;

	if (strLabel == strName)
//	if (!strcmp((char *)(LPCTSTR)strName, pLabel))
	{
		if (strVal.IsEmpty())
		{
			return TRUE;
		}

		iLen = strVal.GetLength();
		if (iLen <	iMaxlen)
		{
//			memcpy((char *)p, strVal, iLen);
			WideCharToMultiByte(CP_ACP, 0, strVal, -1, p, iMaxlen, NULL, NULL); 
			return TRUE;
		}
		else
		{
			DisplayError(strName);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL _SetVal(char * p, CString strName, CString strVal, char *pLabel, int iMaxVal)
{
	int iVal;
	CString strLabel = pLabel;

	if (strLabel == strName)
//	if (!strcmp((char *)(LPCTSTR)strName, pLabel))
	{
		if (strVal.IsEmpty())
		{
			return TRUE;
		}
		if (!IsDigitStr(strVal))
		{
			DisplayError(strName);
			return TRUE;
		}
		iVal = _wtoi(strVal);
		if (iVal <= iMaxVal)
		{
			p[0] = (BYTE)iVal;
		}
		else
		{
			DisplayError(strName);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL _SetIPAddr(char * p, CString strName, CString strVal, char *pLabel)
{
	TCHAR iChar;
	TCHAR * pTemp;
	int iCount;
	unsigned short iTotal;
	CString strLabel = pLabel;

	if (strLabel == strName)
//	if (!strcmp((char *)(LPCTSTR)strName, pLabel))
	{
		if (strVal.IsEmpty())
		{
			return TRUE;
		}

		if (strVal.Find(',') != -1)
		{
			strVal.Replace(',' , '.');
		}
		pTemp = (TCHAR *)(LPCTSTR)strVal;
		iTotal = 0;
		iCount = 0;
		do
		{
			iChar = *pTemp;
			pTemp ++;
			if (iChar >= '0' && iChar <= '9')
			{
				iTotal *= 10;
				iTotal += iChar - '0';
				if (iTotal >= 256)
				{
					DisplayError(strName);
					return TRUE;
				}
			}
			else
			{
				p[iCount] = (UCHAR)iTotal;
				iTotal = 0;
				iCount ++;
				if (iCount < IP_ALEN)
				{
					if (iChar != '.')
					{
						DisplayError(strName);
						return TRUE;
					}
				}
				else
				{
					 return TRUE;
				}
			}
		}while (iChar);

		return TRUE;
	}

  return FALSE;
}

int atoi_16(char * pBuf, int iRadix)
{
	int iPos;
	unsigned short iTotal;
	UCHAR iVal;

	iTotal = 0;
	iPos = 0;
	do
	{
		iVal = pBuf[iPos];
		if (iVal >= '0' && iVal <= '9')
		{
			iVal -= '0';
		}
		else 
		{
			if (iRadix == 16)
			{
				if (iVal >= 'a' && iVal <= 'f')
				{
					iVal -= 'a' - 10;
				}
				else if (iVal >= 'A' && iVal <= 'F')
				{
					iVal -= 'A' - 10;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		iTotal *= iRadix;
		iTotal += iVal;
		iPos ++;
	} while (1);
	return iTotal;
}

BOOL IsHexStr(CString strVal)
{
	TCHAR ch;
	int i;

	for (i = 0; i < strVal.GetLength(); i ++)
	{
		ch = strVal.GetAt(i);
		if (!((ch >= '0' && ch <= '9') 
			|| (ch >= 'a' && ch <= 'f')
			|| (ch >= 'A' && ch <= 'F')))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL _SetMacAddr(char * p, CString strName, CString strVal, char *pLabel)
{
	int i, j, iLen;
	char pMac[3];
	int iPos;
	CString cTemp;
	CString strLabel = pLabel;

	if (strLabel == strName)
//	if (!strcmp((char *)(LPCTSTR)strName, pLabel))
	{
		if (strVal.IsEmpty())
		{
			return TRUE;
		}

		while (strVal.Find(_T(",")) != -1)
		{
			iPos = strVal.Find(_T(","));
			iLen = strVal.Delete(iPos, 1);
		}
		while (strVal.Find(_T("0x")) != -1)
		{
			iPos = strVal.Find(_T("0x"));
			iLen = strVal.Delete(iPos, 2);
		}

		iLen = strVal.GetLength();
		if (iLen == HW_ALEN * 2)
		{
			if (IsHexStr(strVal))
			{
				pMac[2] = 0;
				for (i = 0, j = 0; i < HW_ALEN; i ++)
				{
					pMac[0] = (char)strVal.GetAt(j++);
					pMac[1] = (char)strVal.GetAt(j++);
					p[i] = (UCHAR)atoi_16(pMac, 16);
				}
				return TRUE;
			}
			else
			{
				DisplayError(strName);
				return TRUE;
			}
		}
		else
		{
			DisplayError(strName);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL _SetDigitmap(char * p, CString strName, CString strVal, char *pLabel)
{
	int iLen, iPos;
	CString strTemp;
	CString strLabel = pLabel;

	if (strLabel != strName)
//	if (strcmp((char *)(LPCTSTR)strName, pLabel))
	{
		return FALSE;
	}

	if (strVal.IsEmpty())
	{
		return TRUE;
	}

	while (!strVal.IsEmpty())
	{
//		printf("%s\n", strVal);
		iPos = strVal.Find('|');
		if (iPos != -1)
		{
			strTemp = strVal.Left(iPos);
		}
		else
		{
			strTemp = strVal;
		}
		iLen = strTemp.GetLength();
		if (iLen && iLen < DIGITMAP_ENTRY_LEN)
		{
//			memcpy(p, strTemp, iLen);
			WideCharToMultiByte(CP_ACP, 0, strTemp, -1, p, iLen, NULL, NULL); 
			p += DIGITMAP_ENTRY_LEN;
		}

		if (iPos != -1)
		{
			strVal = strVal.Mid(iPos + 1);
		}
		else
		{
			break;
		}
	}
	return TRUE;
}

int _GetPBIndex(CString strSrc)
{
	TCHAR ch;
	int i, iLen, iStart, iCount;
	CString strTemp;

	ch = strSrc.GetAt(0);
	if (ch != 'n' && ch != 'p')
	{
		return 0;
	}
	iLen = strSrc.GetLength();
	for (i = 1; i < iLen; i ++)
	{
		ch = strSrc.GetAt(i);
		if (ch >= '1' && ch <= '9')
		{
			iStart = i;
			break;
		}
	}
	if (i == iLen)	return 0;

	iCount = 0;
	for (i = iStart; i < iLen; i ++)
	{
		ch = strSrc.GetAt(i);
		if (ch >= '0' && ch <= '9')
		{
			iCount ++;
		}
	}
	if (!iCount)
	{
		return 0;
	}
	else
	{
		strTemp = strSrc.Mid(iStart, iCount);
		return _wtoi(strTemp);
	}
}

void _SetPBFlag(char * p, int iFlag)
{
	p[0] = iFlag;
}

void _SetPB(char * p, CString strName, CString strVal, int iMaxLen)
{
	int iLen;

	iLen = strVal.GetLength();
	if (iLen < iMaxLen)
	{
//		memcpy((char *)p, strVal, iLen);
		WideCharToMultiByte(CP_ACP, 0, strVal, -1, p, iMaxLen, NULL, NULL); 
		memset((char *)p+iLen, 0, iMaxLen-iLen);
	}
	else
	{
		strName += " ";
		DisplayError(strName);
	}
}

void AR168CompareCfg(char * pSettings, CString str)
{
	int iPos;
	CString strName, strVal;

	iPos = str.Find(_T("//"));
	if (iPos == 0)
	{
		return;
	}
	else if (iPos != -1)
	{
		str = str.Left(iPos);
	}

	if (!_GetName(str, strName))
	{
		return;
	}
	_GetVal(str, strVal);

	if (_SetString((char *)(pSettings + OPT_CONFIG_ID), strName, strVal, (char *)_cConfigID, MAX_CONFIG_ID))
		return;

	if (_SetMacAddr((char *)(pSettings + OPT_MAC_ADDRESS), strName, strVal, (char *)_cMacAddress))
		return;

	if (_SetVal((char *)(pSettings + OPT_SYSTEM_PAGE), strName, strVal, (char *)_cSystemPage, 255))
		return;

	if (_SetString((char *)(pSettings + OPT_FLAG_PRODUCT), strName, strVal, (char *)_cFlagProduct, MAX_FLAG_PRODUCT_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_FLAG_PROTOCOL), strName, strVal, (char *)_cFlagProtocol, MAX_FLAG_PROTOCOL))
		return;

	if (_SetString((char *)(pSettings + OPT_FLAG_COUNTRY), strName, strVal, (char *)_cFlagCountry, MAX_FLAG_COUNTRY))
		return;

	if (_SetString((char *)(pSettings + OPT_FLAG_OEM), strName, strVal, (char *)_cFlagOem, MAX_FLAG_OEM_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_FLAG_VERSION), strName, strVal, (char *)_cFlagVersion, MAX_FLAG_VERSION))
		return;

	if (_SetIPAddr((char *)(pSettings + OPT_SYSLOG_IP), strName, strVal, (char *)_cSyslogIP))
		return;

	//Network settings
	if (_SetVal((char *)(pSettings + OPT_IP_TYPE), strName, strVal, (char *)_cIPtype, 255))
		return;

	if (_SetIPAddr((char *)(pSettings + OPT_IP_ADDRESS), strName, strVal, (char *)_cIPAddress))
		return;

	if (_SetIPAddr((char *)(pSettings + OPT_SUBNET_MASK), strName, strVal, (char *)_cSubMask))
		return;

	if (_SetIPAddr((char *)(pSettings + OPT_ROUTER_IP), strName, strVal, (char *)_cRouterIP))
		return;

	if (_SetVal((char *)(pSettings + OPT_DNS_TYPE), strName, strVal, (char *)_cDnsType, 255))
		return;

	if (_SetIPAddr((char *)(pSettings + OPT_DNS_IP), strName, strVal, (char *)_cDnsIP))
		return;

	if (_SetIPAddr((char *)(pSettings + OPT_DNS_IP2), strName, strVal, (char *)_cDnsIP2))
		return;

	if (_SetVal((char *)(pSettings + OPT_LAY3_QOS), strName, strVal, (char *)_cLay3Qos, 255))
		return;

	if (_SetPort((char *)(pSettings + OPT_VLAN_TAG), strName, strVal, (char *)_cVlanTag))
		return;

	if (_SetVal((char *)(pSettings + OPT_QOS_PRIORITY), strName, strVal, (char *)_cQosPriority, 255))
		return;

	if (_SetPort((char *)(pSettings + OPT_VLAN_TAG2), strName, strVal, (char *)_cVlanTag2))
		return;

	if (_SetVal((char *)(pSettings + OPT_QOS_PRIORITY2), strName, strVal, (char *)_cQosPriority2, 255))
		return;

	if (_SetPort((char *)(pSettings + OPT_VLAN_TAG3), strName, strVal, (char *)_cVlanTag3))
		return;

	if (_SetVal((char *)(pSettings + OPT_QOS_PRIORITY3), strName, strVal, (char *)_cQosPriority3, 255))
		return;

	if (_SetString((char *)(pSettings + OPT_PPPOE_ID), strName, strVal, (char *)_cPPPoeID, MAX_PPPOE_ID_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_PPPOE_PIN), strName, strVal, (char *)_cPPPoePIN, MAX_PPPOE_PIN_LEN))
		return;

	if (_SetVal((char *)(pSettings + OPT_SERIAL_METHOD), strName, strVal, (char *)_cSerialMethod, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_BAUD_RATE), strName, strVal, (char *)_cBaudRate, 255))
		return;

	//System settings
	if (_SetVal((char *)(pSettings + OPT_TIME_ZONE), strName, strVal, (char *)_cTimeZone, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_DAY_LIGHT), strName, strVal, (char *)_cDayLight, 255))
		return;

	if (_SetString((char *)(pSettings + OPT_SNTP_DNS), strName, strVal, (char *)_cSntpDns, MAX_DNS_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_ADMIN_PIN), strName, strVal, (char *)_cAdminPIN, MAX_ADMIN_PIN_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_ADMIN_USER), strName, strVal, (char *)_cAdminUser, MAX_ADMIN_USER_LEN))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOL_INPUT), strName, strVal, (char *)_cVolInput, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOL_OUTPUT), strName, strVal, (char *)_cVolOutput, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_SPEAKER_VOL), strName, strVal, (char *)_cSpeakerVol, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_RING_VOL_OUTPUT), strName, strVal, (char *)_cRingVolOutput, 255))
		return;

	if (_SetString((char *)(pSettings + OPT_PROVISION_SERVER), strName, strVal, (char *)_cProvisionServer, MAX_DNS_LEN))
		return;

	if (_SetPort((char *)(pSettings + OPT_PROVISION_TTL), strName, strVal, (char *)_cProvisionTTL))
		return;

	if (_SetPort((char *)(pSettings + OPT_PROVISION_PORT), strName, strVal, (char *)_cProvisionPort))
		return;

	if (_SetString((char *)(pSettings + OPT_PROV_FILENAME), strName, strVal, (char *)_cProvisionFilename, MAX_DNS_LEN))
		return;

	//Voice settings
	if (_SetVal((char *)(pSettings + OPT_VOICE_CODER0), strName, strVal, (char *)_cVoiceCoder0, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOICE_CODER1), strName, strVal, (char *)_cVoiceCoder1, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOICE_CODER2), strName, strVal, (char *)_cVoiceCoder2, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOICE_CODER3), strName, strVal, (char *)_cVoiceCoder3, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOICE_CODER4), strName, strVal, (char *)_cVoiceCoder4, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOICE_CODER5), strName, strVal, (char *)_cVoiceCoder5, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VOICE_CODER6), strName, strVal, (char *)_cVoiceCoder6, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_SPEEX_RATE), strName, strVal, (char *)_cSpeexRate, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_VAD), strName, strVal, (char *)_cVad, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_AGC), strName, strVal, (char *)_cAgc, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_AEC), strName, strVal, (char *)_cAec, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_ILBC_FRAME), strName, strVal, (char *)_cIlbcFrame, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FRAME_NUMBER0), strName, strVal, (char *)_cFrameNumber0, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FRAME_NUMBER1), strName, strVal, (char *)_cFrameNumber1, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FRAME_NUMBER2), strName, strVal, (char *)_cFrameNumber2, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FRAME_NUMBER3), strName, strVal, (char *)_cFrameNumber3, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FRAME_NUMBER4), strName, strVal, (char *)_cFrameNumber4, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FRAME_NUMBER5), strName, strVal, (char *)_cFrameNumber5, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FRAME_NUMBER6), strName, strVal, (char *)_cFrameNumber6, 255))
		return;

	//Dialplan settings
	if (_SetString((char *)(pSettings + OPT_FWD_NUMBER), strName, strVal, (char *)_cFwdNumber, MAX_USER_NUMBER_LEN))
		return;

	if (_SetVal((char *)(pSettings + OPT_FWD_ALWAYS), strName, strVal, (char *)_cFwdAlways, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FWD_BUSY), strName, strVal, (char *)_cFwdBusy, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_FWD_NOANSWER), strName, strVal, (char *)_cFwdNoAnswer, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_NOANSWER_TIMEOUT), strName, strVal, (char *)_cNoAnswerTimeout, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_CALL_WAITING), strName, strVal, (char *)_cCallWaiting, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_AUTO_ANSWER), strName, strVal, (char *)_cAutoAnswer, 255))
		return;

	if (_SetString((char *)(pSettings + OPT_DIAL_PREFIX), strName, strVal, (char *)_cDialPrefix, MAX_DIAL_PREFIX_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_HOTLINE_NUMBER), strName, strVal, (char *)_cHotLineNumber, MAX_USER_NUMBER_LEN))
		return;

	if (_SetVal((char *)(pSettings + OPT_USE_DIGITMAP), strName, strVal, (char *)_cUseDigitmap, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_DIGITMAP_TIMEOUT), strName, strVal, (char *)_cDigitmapTimeout, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_REDIAL_TIMEOUT), strName, strVal, (char *)_cRedialTimeout, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_CALL_LOG_TIMEOUT), strName, strVal, (char *)_cCallLogTimeout, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_RINGING), strName, strVal, (char *)_cRinging, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_GRADUAL_RINGING), strName, strVal, (char *)_cGradualRinging, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_GET_VOICE_MSG), strName, strVal, (char *)_cGetVoiceMsg, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_POUND_AS_CALL), strName, strVal, (char *)_cPoundAsCall, 255))
		return;

	//Protocol settings
	if (_SetVal((char *)(pSettings + OPT_REGISTER), strName, strVal, (char *)_cRegister, 255))
		return;

	if (_SetPort((char *)(pSettings + OPT_LOCAL_PORT), strName, strVal, (char *)_cLocalPort))
		return;

	if (_SetString((char *)(pSettings + OPT_AUTH_ID), strName, strVal, (char *)_cAuthID, MAX_AUTH_ID_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_AUTH_PIN), strName, strVal, (char *)_cAuthPIN, MAX_AUTH_PIN_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_USER_NAME), strName, strVal, (char *)_cUserName, MAX_USER_NAME_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_USER_NUMBER), strName, strVal, (char *)_cUserNumber, MAX_USER_NUMBER_LEN))
		return;

	if (_SetPort((char *)(pSettings + OPT_REGISTER_TTL), strName, strVal, (char *)_cRegisterTTL))
		return;

	if (_SetString((char *)(pSettings + OPT_MESSAGE_NUMBER), strName, strVal, (char *)_cMessageNumber, MAX_USER_NUMBER_LEN))
		return;

	if (_SetString((char *)(pSettings + OPT_IAX2_SERVER), strName, strVal, (char *)_cIax2Server, MAX_DNS_LEN))
		return;

	// Iax2 Settings
	if (_SetPort((char *)(pSettings + OPT_IAX2_PORT), strName, strVal, (char *)_cIax2Port))
		return;

	if (_SetVal((char *)(pSettings + OPT_DIGIT_DIALING), strName, strVal, (char *)_cDigitDialing, 255))
		return;

	if (_SetString((char *)(pSettings + OPT_SIP_PROXY), strName, strVal, (char *)_cSipProxy, MAX_DNS_LEN))
		return;

	// SIP settings
	if (_SetPort((char *)(pSettings + OPT_SIP_PORT), strName, strVal, (char *)_cSipPort))
		return;

	if (_SetString((char *)(pSettings + OPT_SIP_DOMAIN), strName, strVal, (char *)_cSipDomain, MAX_DNS_LEN))
		return;

	if (_SetVal((char *)(pSettings + OPT_OUTBOUND_PROXY), strName, strVal, (char *)_cOutBoundProxy, 255))
		return;

	if (_SetPort((char *)(pSettings + OPT_RTP_PORT), strName, strVal, (char *)_cRtpPort))
		return;

	if (_SetVal((char *)(pSettings + OPT_DTMF_PAYLOAD), strName, strVal, (char *)_cDtmfPayload, 65535))
		return;

	if (_SetVal((char *)(pSettings + OPT_DNS_SRV), strName, strVal, (char *)_cDnsSrv, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_PRACK), strName, strVal, (char *)_cPrack, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_SUBSCRIBE_MWI), strName, strVal, (char *)_cSubscribeMWI, 255))
		return;

	if (_SetString((char *)(pSettings + OPT_PROXY_REQUIRE), strName, strVal, (char *)_cProxyRequire, MAX_DNS_LEN))
		return;

	if (_SetVal((char *)(pSettings + OPT_NAT_TRAVERSAL), strName, strVal, (char *)_cNatTraversal, 255))
		return;

	if (_SetIPAddr((char *)(pSettings + OPT_NAT_IP), strName, strVal, (char *)_cNatIP))
		return;

	if (_SetString((char *)(pSettings + OPT_STUN_SERVER), strName, strVal, (char *)_cStunServer, MAX_DNS_LEN))
		return;

	if (_SetPort((char *)(pSettings + OPT_STUN_PORT), strName, strVal, (char *)_cStunPort))
		return;

	if (_SetVal((char *)(pSettings + OPT_DTMF_TYPE), strName, strVal, (char *)_cDtmfType, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_KEEP_ALIVE), strName, strVal, (char *)_cKeepAlive, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_ILBC_PAYLOAD), strName, strVal, (char *)_cIlbcPayload, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_ADPCM32_PAYLOAD), strName, strVal, (char *)_cAdpcm32Payload, 255))
		return;

	if (_SetVal((char *)(pSettings + OPT_SPEEX_PAYLOAD), strName, strVal, (char *)_cSpeexPayload, 255))
		return;

	if (_SetDigitmap((char *)(pSettings + OPT_DIGIT_MAP), strName, strVal, (char *)_cDigitmapString))
		return;
}


void AR168ComparePB(char * pSettings, CString str)
{
	CString strName, cError, strVal;
	int iIndex, iPos;

	iPos = str.Find(_T("//"));
	if (iPos == 0)
	{
		return;
	}
	else if (iPos != -1)
	{
		str = str.Left(iPos);
	}

	if (!_GetName(str, strName))
	{
		return;
	}

	_GetVal(str, strVal);
	if (strVal.IsEmpty())
	{
		return;
	}

	//Set Phone Book
	iIndex = _GetPBIndex(strName);
	if (iIndex > PHONEBOOK_ENTRY_NUM || iIndex == 0)
	{
		cError = strName + " is overflow! Continue loading?";
		AfxMessageBox(cError);
		return;
	}

	iIndex --;
	iIndex <<= 8;
	_SetPBFlag((char *)(pSettings + iIndex), 1);
	if (strName.Left(1) == "n")
	{
		iIndex += PHONEBOOK_DATA_NAME;
		_SetPB((char *)(pSettings + iIndex), strName, strVal, MAX_USER_NAME_LEN);
	}
	else if (strName.Left(1) == "p")
	{
		iIndex += PHONEBOOK_DATA_NUMBER;
		_SetPB((char *)(pSettings + iIndex), strName, strVal, MAX_USER_NUMBER_LEN);
	}
}

void AR168ConvertPB(char * pSettings, CString& strwrite)
{
	int iIndex, iPos;

//	strwrite = "";
	strwrite = "[phonebook]\n";
	//Write phone book
	for (iIndex = 0; iIndex < PHONEBOOK_ENTRY_NUM; iIndex++)
	{
		iPos = iIndex;
		iPos <<= 8;

		if (pSettings[iPos] == 1)
		{
			strwrite += _WritePBIndex(iIndex + 1, 0);
			strwrite += _WritePB((char *)(pSettings+iPos+PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
			strwrite += _WritePBIndex(iIndex + 1, 1);
			strwrite += _WritePB((char *)(pSettings+iPos+PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
		}
	}
}

BOOL AR168CompareMap(char * pSettings, CString str, int iItem)
{
	int iLen, iPos;
	char *pCur;

	if (str.Left(10) == _T("[digitmap]"))
		return FALSE;

	iPos = str.Find(_T("//"));
	if (iPos == 0)
		return FALSE;
	str.TrimRight();
	str.TrimLeft();

	iLen = str.GetLength();
	pCur = (char *)(pSettings + iItem * DIGITMAP_ENTRY_LEN);
	memset(pCur, 0, DIGITMAP_ENTRY_LEN);
	if (iLen && iLen <	DIGITMAP_ENTRY_LEN)
	{
//		memcpy(pCur, str, iLen);
		WideCharToMultiByte(CP_ACP, 0, str, -1, pCur, iLen, NULL, NULL); 
		return TRUE;
	}
	else
	{
		AfxMessageBox(_T("Digitmap entry MUST be less than or equal to 31 characters"));
		return FALSE;
	}
}


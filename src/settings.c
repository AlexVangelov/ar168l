/*-------------------------------------------------------------------------
   AR1688 Settings function copy right information

   Copyright (c) 2006-2012. Tang, Li      <tangli@palmmicro.com>
						    Li, Jing      <lijing@palmmicro.com>
                            Lin, Rongrong <woody@palmmicro.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Palmmicro nor the names of its contributors may be
	  used to endorse or promote products derived from this software without
	  specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL COPYRIGHT OWNER BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-------------------------------------------------------------------------*/

#include "version.h"
#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "bank4.h"

#include "gdiconst.h"

#ifdef CALL_SIP
#include "web_info_sip.h"
#elif defined CALL_IAX2
#include "web_info_iax2.h"
#endif

#define INPUT_TOKEN_LEN		7
const UCHAR _cInput[] = "<INPUT ";

#define SELECT_TOKEN_LEN	8
const UCHAR _cSelect[] = "<SELECT ";

#define OPTION_TOKEN_LEN	8
const UCHAR _cOption[] = "<OPTION ";

#define SELECTED_TOKEN_LEN	9
const UCHAR _cSelected[] = "selected ";

#define CHECKED_TOKEN_LEN	8
const UCHAR _cChecked[] = "checked ";

#define NAME_TOKEN_LEN		5
//const UCHAR _cName[] = "name=";

#define VALUE_TOKEN_LEN		6
//const UCHAR _cValue[] = "value=";

#ifdef OEM_IP20
#define PB_BODY_LEN		155
const UCHAR _cPbBody[] = "<tr><td width=60></td><td width=150><INPUT name=n value=\"\" size=16 maxlength=31></td><td width=250><INPUT name=p value=\"\" size=24 maxlength=31></td></tr>\r\n";
#else
#define PB_BODY_LEN		126
const UCHAR _cPbBody[] = "<tr><td></td><td><INPUT name=n value=\"\" size=32 maxlength=31></td><td><INPUT name=p value=\"\" size=32 maxlength=31></td></tr>\r\n";
#endif

#ifdef OEM_IP20
#define DM_BODY_LEN		155
const UCHAR _cDmBody[] = "<tr><td width=60><p></p><p></td><td width=150>&nbsp;</td><td width=150>&nbsp;</td><td width=60></td><td width=150><INPUT name=n value=\"\" size=16 maxlength=16></td><td width=150><INPUT name=p value=\"\" size=16 maxlength=16></td></tr>\r\n";
#else
#define DM_BODY_LEN		135
const UCHAR _cDmBody[] = "<tr><td></td><td><INPUT name=m value=\"\" size=32 maxlength=31></td><td></td><td><INPUT name=m value=\"\" size=32 maxlength=31></td></tr>\r\n";
#endif

UCHAR _iHttpSelectVal;

typedef struct _NAME_INDEX
{
	PCHAR pName;
	USHORT sIndex;
} NAME_INDEX;

#ifdef OEM_IP20
#define NAME_INDEX_NUM	88
#elif defined OEM_INNOMEDIA
#define NAME_INDEX_NUM	88
#else
#ifdef CALL_SIP
#ifdef OEM_BT2008
#define NAME_INDEX_NUM	95
#elif defined OEM_ROIP
#define NAME_INDEX_NUM	91
#else
#define NAME_INDEX_NUM	90
#endif
#else
#ifdef OEM_BT2008
#define NAME_INDEX_NUM	79
#else
#define NAME_INDEX_NUM	74
#endif
#endif
#endif

const NAME_INDEX _p_name_index[NAME_INDEX_NUM] = {
	{_cConfigID,	OPT_CONFIG_ID},
	{_cIPtype,		OPT_IP_TYPE},
	{_cIPAddress,	OPT_IP_ADDRESS},
	{_cSubMask,		OPT_SUBNET_MASK},
	{_cRouterIP,	OPT_ROUTER_IP},
	{_cDnsType,		OPT_DNS_TYPE},
	{_cDnsIP,		OPT_DNS_IP},
	{_cDnsIP2,		OPT_DNS_IP2},
	{_cLay3Qos,		OPT_LAY3_QOS},
	{_cVlanTag,		OPT_VLAN_TAG},

	{_cQosPriority,	OPT_QOS_PRIORITY},
	{_cVlanTag2,	OPT_VLAN_TAG2},
	{_cQosPriority2,	OPT_QOS_PRIORITY2},
	{_cVlanTag3,	OPT_VLAN_TAG3},
	{_cQosPriority3,	OPT_QOS_PRIORITY3},
	{_cPPPoeID,		OPT_PPPOE_ID},
	{_cPPPoePIN,	OPT_PPPOE_PIN},		// 17
#if	!defined OEM_IP20 && !defined OEM_INNOMEDIA
	{_cSerialMethod,	OPT_SERIAL_METHOD},
	{_cBaudRate,	OPT_BAUD_RATE},		// 19
#endif
	{_cTimeZone,	OPT_TIME_ZONE},
	{_cDayLight,	OPT_DAY_LIGHT},
	{_cSntpDns,		OPT_SNTP_DNS},
	{_cAdminPIN,	OPT_ADMIN_PIN},
	{_cSyslogIP,	OPT_SYSLOG_IP},
	{_cVolInput,	OPT_VOL_INPUT},
	{_cVolOutput,	OPT_VOL_OUTPUT},	// 26
#ifdef OEM_IP20
	{_cTimeOut,		OPT_TIME_OUT},
#endif
	{_cRingVolOutput,	OPT_RING_VOL_OUTPUT},
	{_cSpeakerVol,	OPT_SPEAKER_VOL},
#ifndef OEM_INNOMEDIA
	{_cProvisionServer,	OPT_PROVISION_SERVER},
	{_cProvisionPort,	OPT_PROVISION_PORT},
#endif
	{_cProvisionTTL,	OPT_PROVISION_TTL},
	{_cProvisionFilename,	OPT_PROV_FILENAME},
	{_cVoiceCoder0,	OPT_VOICE_CODER0},
	{_cVoiceCoder1,	OPT_VOICE_CODER1},
	{_cVoiceCoder2,	OPT_VOICE_CODER2},
	{_cVoiceCoder3,	OPT_VOICE_CODER3},
	{_cVoiceCoder4,	OPT_VOICE_CODER4},
	{_cVoiceCoder5,	OPT_VOICE_CODER5},
	{_cVoiceCoder6,	OPT_VOICE_CODER6},
	{_cSpeexRate,	OPT_SPEEX_RATE},
	{_cVad,			OPT_VAD},
	{_cAgc,			OPT_AGC},
	{_cAec,			OPT_AEC},
	{_cIlbcFrame,	OPT_ILBC_FRAME},
	{_cFrameNumber0,	OPT_FRAME_NUMBER0},
	{_cFrameNumber1,	OPT_FRAME_NUMBER1},
	{_cFrameNumber2,	OPT_FRAME_NUMBER2},
	{_cFrameNumber3,	OPT_FRAME_NUMBER3},
	{_cFrameNumber4,	OPT_FRAME_NUMBER4},
	{_cFrameNumber5,	OPT_FRAME_NUMBER5},
	{_cFrameNumber6,	OPT_FRAME_NUMBER6},
	{_cFwdNumber,	OPT_FWD_NUMBER},
	{_cFwdAlways,	OPT_FWD_ALWAYS},
	{_cFwdBusy,		OPT_FWD_BUSY},
	{_cFwdNoAnswer,	OPT_FWD_NOANSWER},
	{_cNoAnswerTimeout,	OPT_NOANSWER_TIMEOUT},
	{_cCallWaiting,	OPT_CALL_WAITING},
	{_cAutoAnswer,	OPT_AUTO_ANSWER},
	{_cDialPrefix,	OPT_DIAL_PREFIX},
	{_cHotLineNumber,	OPT_HOTLINE_NUMBER},
	{_cUseDigitmap,	OPT_USE_DIGITMAP},
	{_cDigitmapTimeout,	OPT_DIGITMAP_TIMEOUT},	// 64
#ifdef OEM_INNOMEDIA
	{_cDigitmapString,	OPT_DIGIT_MAP},
	{_cAdminUser, OPT_ADMIN_USER},
#endif
#ifndef OEM_IP20
	{_cPoundAsCall,	OPT_POUND_AS_CALL},			// 65
#endif
#ifdef OEM_BT2008
	{_cRedialTimeout,	OPT_REDIAL_TIMEOUT},
	{_cCallLogTimeout,	OPT_CALL_LOG_TIMEOUT},
	{_cRinging,		OPT_RINGING},
	{_cGradualRinging,	OPT_GRADUAL_RINGING},
	{_cGetVoiceMsg,	OPT_GET_VOICE_MSG},
#endif
#ifdef CALL_SIP
#ifdef OEM_ROIP
	{_cPttControl,	OPT_PTT_CONTROL},
#endif
#endif

	{_cRegister,	OPT_REGISTER},
	{_cLocalPort,	OPT_LOCAL_PORT},
	{_cAuthID,		OPT_AUTH_ID},
	{_cAuthPIN,		OPT_AUTH_PIN},
	{_cUserName,	OPT_USER_NAME},
	{_cUserNumber,	OPT_USER_NUMBER},
	{_cRegisterTTL,	OPT_REGISTER_TTL},
	{_cMessageNumber,	OPT_MESSAGE_NUMBER},	// 78
#ifdef CALL_SIP
	{_cDtmfType,	OPT_DTMF_TYPE},
	{_cKeepAlive,	OPT_KEEP_ALIVE},
	{_cSipProxy,	OPT_SIP_PROXY},
	{_cSipPort,		OPT_SIP_PORT},
	{_cSipDomain,	OPT_SIP_DOMAIN},	
	{_cOutBoundProxy,	OPT_OUTBOUND_PROXY},
	{_cRtpPort,		OPT_RTP_PORT},
	{_cDtmfPayload,	OPT_DTMF_PAYLOAD},
	{_cIlbcPayload,	OPT_ILBC_PAYLOAD},
	{_cAdpcm32Payload,	OPT_ADPCM32_PAYLOAD},
	{_cSpeexPayload,	OPT_SPEEX_PAYLOAD},
	{_cDnsSrv,		OPT_DNS_SRV},
	{_cPrack,		OPT_PRACK},
	{_cSubscribeMWI,	OPT_SUBSCRIBE_MWI},
	{_cProxyRequire,	OPT_PROXY_REQUIRE},
	{_cNatTraversal,	OPT_NAT_TRAVERSAL},
	{_cNatIP,		OPT_NAT_IP},
	{_cStunServer,	OPT_STUN_SERVER},
	{_cStunPort,	OPT_STUN_PORT},		// 97
#elif defined CALL_IAX2
	{_cIax2Server,	OPT_IAX2_SERVER},
	{_cIax2Port,	OPT_IAX2_PORT},
	{_cDigitDialing,	OPT_DIGIT_DIALING},	// 81
#endif
};

USHORT OptGetIndex(PCHAR pName)
{
	UCHAR i;

	for (i = 0; i < NAME_INDEX_NUM; i ++)
	{
		if (!strcmp(pName, _p_name_index[i].pName))
		{
			return _p_name_index[i].sIndex;
		}
	}
	return 0;
}

UCHAR OptGetType(USHORT sIndex)
{
	switch (sIndex)
	{
	// select box variable
	case OPT_IP_TYPE:
#ifndef OEM_IP20
	case OPT_SERIAL_METHOD:
	case OPT_BAUD_RATE:
#endif
	case OPT_VOICE_CODER0:
	case OPT_VOICE_CODER1:
	case OPT_VOICE_CODER2:
	case OPT_VOICE_CODER3:
	case OPT_VOICE_CODER4:
	case OPT_VOICE_CODER5:
	case OPT_VOICE_CODER6:
	case OPT_SPEEX_RATE:
	case OPT_TIME_ZONE:
#ifdef CALL_SIP
	case OPT_DTMF_TYPE:
	case OPT_NAT_TRAVERSAL:
#ifdef OEM_ROIP
	case OPT_PTT_CONTROL:
#endif
#endif
		return OPTION_TYPE_SELECT;

	// radio button variable
	case OPT_DNS_TYPE:
	case OPT_DAY_LIGHT:
	case OPT_VAD:
	case OPT_AGC:
	case OPT_AEC:
	case OPT_ILBC_FRAME:
	case OPT_FWD_NOANSWER:
	case OPT_FWD_ALWAYS:
	case OPT_FWD_BUSY:
	case OPT_AUTO_ANSWER:
	case OPT_CALL_WAITING:
	case OPT_REGISTER:
	case OPT_USE_DIGITMAP:
#ifndef OEM_IP20
	case OPT_POUND_AS_CALL:
#endif
#ifdef OEM_BT2008
	case OPT_RINGING:
	case OPT_GET_VOICE_MSG:
	case OPT_GRADUAL_RINGING:
#endif
#ifdef CALL_SIP
	case OPT_OUTBOUND_PROXY:
	case OPT_DNS_SRV:
	case OPT_PRACK:
	case OPT_SUBSCRIBE_MWI:
#elif defined CALL_IAX2
	case OPT_DIGIT_DIALING:
#endif
		return OPTION_TYPE_RADIO;

	// single byte variable
	case OPT_LAY3_QOS:
	case OPT_QOS_PRIORITY:
	case OPT_QOS_PRIORITY2:
	case OPT_QOS_PRIORITY3:
	case OPT_VOL_INPUT:
	case OPT_VOL_OUTPUT:
#ifdef OEM_IP20
	case OPT_TIME_OUT:
#endif
	case OPT_RING_VOL_OUTPUT:
	case OPT_SPEAKER_VOL:
	case OPT_FRAME_NUMBER0:
	case OPT_FRAME_NUMBER1:
	case OPT_FRAME_NUMBER2:
	case OPT_FRAME_NUMBER3:
	case OPT_FRAME_NUMBER4:
	case OPT_FRAME_NUMBER5:
	case OPT_FRAME_NUMBER6:
	case OPT_NOANSWER_TIMEOUT:
	case OPT_DIGITMAP_TIMEOUT:
#ifdef OEM_BT2008
	case OPT_REDIAL_TIMEOUT:
	case OPT_CALL_LOG_TIMEOUT:
#endif
#ifdef CALL_SIP
	case OPT_ILBC_PAYLOAD:
	case OPT_ADPCM32_PAYLOAD:
	case OPT_SPEEX_PAYLOAD:
	case OPT_KEEP_ALIVE:
	case OPT_DTMF_PAYLOAD:
#endif
		return OPTION_TYPE_BYTE;

	// IP address type variable
	case OPT_IP_ADDRESS:
	case OPT_SUBNET_MASK:
	case OPT_ROUTER_IP:
	case OPT_DNS_IP:
	case OPT_DNS_IP2:
	case OPT_SYSLOG_IP:
#ifdef CALL_SIP
	case OPT_NAT_IP:
#endif
		return OPTION_TYPE_IPADDR;

	// string type variable
	case OPT_CONFIG_ID:
	case OPT_SNTP_DNS:
	case OPT_ADMIN_PIN:
#ifdef OEM_INNOMEDIA
	case OPT_ADMIN_USER:
#endif
	case OPT_PROVISION_SERVER:
	case OPT_PROV_FILENAME:
	case OPT_FWD_NUMBER:
	case OPT_DIAL_PREFIX:
	case OPT_HOTLINE_NUMBER:
	case OPT_MESSAGE_NUMBER:
	case OPT_AUTH_ID:
	case OPT_AUTH_PIN:
	case OPT_USER_NAME:
	case OPT_USER_NUMBER:
	case OPT_PPPOE_ID:
	case OPT_PPPOE_PIN:
#ifdef CALL_SIP
	case OPT_SIP_PROXY:
	case OPT_SIP_DOMAIN:
	case OPT_PROXY_REQUIRE:
	case OPT_STUN_SERVER:
#elif defined CALL_IAX2
	case OPT_IAX2_SERVER:
#endif
		return OPTION_TYPE_STRING;

	// short type variable
	case OPT_VLAN_TAG:
	case OPT_VLAN_TAG2:
	case OPT_VLAN_TAG3:
	case OPT_PROVISION_TTL:
	case OPT_PROVISION_PORT:
	case OPT_REGISTER_TTL:
	case OPT_LOCAL_PORT:
#ifdef CALL_SIP
	case OPT_SIP_PORT:
	case OPT_RTP_PORT:
	case OPT_STUN_PORT:
#elif defined CALL_IAX2
	case OPT_IAX2_PORT:
#endif
		return OPTION_TYPE_SHORT;
	}

	return OPTION_TYPE_UNKNOWN;
}

UCHAR OptGetMaxlen(USHORT sIndex)
{
	switch (sIndex)
	{
	case OPT_CONFIG_ID:
		return MAX_CONFIG_ID;

	case OPT_SNTP_DNS:
	case OPT_PROVISION_SERVER:
	case OPT_PROV_FILENAME:
#ifdef CALL_SIP
	case OPT_SIP_PROXY:
	case OPT_SIP_DOMAIN:
	case OPT_PROXY_REQUIRE:
	case OPT_STUN_SERVER:
#elif defined CALL_IAX2
	case OPT_IAX2_SERVER:
#endif
		return MAX_DNS_LEN;

	case OPT_ADMIN_PIN:
		return MAX_ADMIN_PIN_LEN;

#ifdef OEM_INNOMEDIA
	case OPT_ADMIN_USER:
		return MAX_ADMIN_USER_LEN;
#endif

	case OPT_FWD_NUMBER:
	case OPT_HOTLINE_NUMBER:
	case OPT_MESSAGE_NUMBER:
	case OPT_USER_NUMBER:
		return MAX_USER_NUMBER_LEN;

	case OPT_DIAL_PREFIX:
		return MAX_DIAL_PREFIX_LEN;

	case OPT_AUTH_ID:
		return MAX_AUTH_ID_LEN;

	case OPT_AUTH_PIN:
		return MAX_AUTH_PIN_LEN;

	case OPT_USER_NAME:
		return MAX_USER_NAME_LEN;

	case OPT_PPPOE_ID:
		return MAX_PPPOE_ID_LEN;

	case OPT_PPPOE_PIN:
		return MAX_PPPOE_PIN_LEN;
	}

	return 0;
}

void OptWriteStr(PCHAR pString, USHORT sIndex, UCHAR iMaxLen)
{
	UCHAR pBuf[255];
	USHORT sLen;

	memset(pBuf, 0, iMaxLen);
	sLen = strlen(pString);
	if (sLen)
	{
		if (sLen < iMaxLen)
		{
			strcpy(pBuf, pString);
		}
		else
		{
			memcpy(pBuf, pString, iMaxLen-1);
		}
	}
	BufSeek(sIndex);
	BufWrite(pBuf, iMaxLen);
}

void OptGetIP(USHORT sIndex, PCHAR pIPAddr)
{
	switch (sIndex)
	{
	case OPT_IP_ADDRESS:
		memcpy4(pIPAddr, Sys_pIpAddress);
		return;

	case OPT_SUBNET_MASK:
		memcpy4(pIPAddr, Sys_pSubnetMask);
		return;

	case OPT_ROUTER_IP:
		memcpy4(pIPAddr, Sys_pRouterIp);
		return;

	case OPT_DNS_IP:
		memcpy4(pIPAddr, Sys_pDnsIp);
		return;

	case OPT_DNS_IP2:
		memcpy4(pIPAddr, Sys_pDnsIp2);
		return;

	case OPT_NAT_IP:
		OptionsGetBuf(pIPAddr, sIndex, IP_ALEN);
		return;

	case OPT_SYSLOG_IP:
		memcpy4(pIPAddr, Sys_pLogIp);
		return;
	}
}

USHORT http_get_settings_len()
{
	UCHAR i, iType, iLen;
	USHORT sLen, sIndex, sVal;
	UCHAR pStr[255];
	UCHAR pIPAddr[IP_ALEN];

	sLen = 0;
	for (i = 0; i < WEB_INFO_NUM; i ++)
	{
		sIndex = web_info_code[i]; 
		iType = OptGetType(sIndex);
		if (iType == OPTION_TYPE_UNKNOWN)
		{
			continue;
		}
		if (iType == OPTION_TYPE_SELECT)
		{
			// select box, insert "selected "
			sLen += SELECTED_TOKEN_LEN;
		}
		else if (iType == OPTION_TYPE_RADIO)
		{
			// radio button, insert "checked "
			sLen += CHECKED_TOKEN_LEN;
		}
		else
		{
			pStr[0] = 0;
			if (iType == OPTION_TYPE_BYTE)
			{
				sVal = (USHORT)OptionsGetByte(sIndex);
				itoa(sVal, pStr, 10);
			}
			else if (iType == OPTION_TYPE_IPADDR)
			{
				OptGetIP(sIndex, pIPAddr);
				ip2str(pIPAddr, pStr);
			}
			else if (iType == OPTION_TYPE_STRING)
			{
				iLen = OptGetMaxlen(sIndex);
				OptionsGetString(pStr, sIndex, iLen);
			}
			else if (iType == OPTION_TYPE_SHORT)
			{
				sVal = OptionsGetShort(sIndex);
				itoa(sVal, pStr, 10);
			}
			sLen += strlen(pStr);
			// length of "value="" " is 9
			sLen += 9;
		}
	}

	return sLen;
}

USHORT http_get_pb_len()
{
	UCHAR i;
	USHORT sIndex, sLen;
	UCHAR pBuf[MAX_USER_NUMBER_LEN];

	sIndex = OPT_PHONE_BOOK;
	sLen = 0;
	for (i = 0; i < PHONEBOOK_ENTRY_NUM; i ++, sIndex += PHONEBOOK_ENTRY_LEN)
	{
		sLen += PB_BODY_LEN + 9; // 9 = three indexes' length
		if (OptionsGetByte(sIndex))
		{
			OptionsGetString(pBuf, (USHORT)(sIndex + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
			sLen += strlen(pBuf);
			OptionsGetString(pBuf, (USHORT)(sIndex + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
			sLen += strlen(pBuf);
		}
	}
	return sLen;
}

#ifdef OEM_IP20

USHORT http_get_dm_len()
{
	UCHAR i;
	USHORT sIndex, sLen;
	UCHAR pBuf[MAX_USER_NUMBER_LEN];

	sIndex = OPT_PHONE_BOOK;
	sLen = 0;
	for (i = 0; i < 20; i ++, sIndex += PHONEBOOK_ENTRY_LEN)	//PHN
	{
		sLen += PB_BODY_LEN + 9; // 9 = three indexes' length
		if (OptionsGetByte(sIndex))
		{
			OptionsGetString(pBuf, (USHORT)(sIndex + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
			sLen += strlen(pBuf);
			OptionsGetString(pBuf, (USHORT)(sIndex + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
			sLen += strlen(pBuf);
		}
	}
	return sLen;
}

#else

USHORT http_get_dm_len()
{
	UCHAR i;
	USHORT sIndex, sLen;
	UCHAR pBuf[DIGITMAP_ENTRY_LEN];

	sIndex = OPT_DIGIT_MAP;
	sLen = 0;
	for (i = 0; i < DIGITMAP_ENTRY_NUM; i ++)
	{
		sLen += DM_BODY_LEN + 12; // 12 = four indexes' length
		OptionsGetString(pBuf, sIndex, DIGITMAP_ENTRY_LEN);
		sLen += strlen(pBuf);
		sIndex += DIGITMAP_ENTRY_LEN;
		OptionsGetString(pBuf, sIndex, DIGITMAP_ENTRY_LEN);
		sLen += strlen(pBuf);
		sIndex += DIGITMAP_ENTRY_LEN;
	}
	return sLen;
}
#endif

UCHAR http_get_token(PCHAR pToken, PCHAR pInput)
{
	PCHAR pCur;
	UCHAR iLen;

	iLen = 0;
	pCur = pInput;
	while (*pCur && *pCur != (UCHAR)' ' && *pCur != (UCHAR)'=' && *pCur != (UCHAR)'>')
	{
		pToken[iLen] = *pCur;
		pCur ++;
		iLen ++;
		if (iLen >= 30)	
		{
			pToken[0] = 0;
			return 0;
		}
	}

	pToken[iLen] = '=';
	iLen ++;
	pToken[iLen] = 0;
	return iLen;
}

void http_update_info(PCHAR pInput, USHORT sOffset, UCHAR iMaxLen)
{
	UCHAR pToken[32];
	UCHAR iLen;

	memset(pInput, ' ', iMaxLen);
	OptionsGetString(pToken, sOffset, iMaxLen);
	iLen = strlen(pToken);
	if (iLen)
	{
		memcpy(pInput, pToken, iLen);
	}
	pInput[iMaxLen] = 0;
}

void http_update_settings(PCHAR pInput)
{
	PCHAR pCur;
	UCHAR pIPAddr[IP_ALEN];
	UCHAR pToken[32];
	UCHAR pBuf[255];
	UCHAR iVal, iLen, iType, iMaxLen;
	USHORT sIndex;

#if !defined OEM_IP20 && !defined OEM_INNOMEDIA
	if (!strcmp(pInput, "<!PRODUCT_XXXXX>"))
	{

#ifdef OEM_BIT2NET
		strcpy(pInput, "b2N1000         ");
#else
		http_update_info(pInput, OPT_FLAG_PRODUCT, MAX_FLAG_PRODUCT_LEN);
#endif
		return;
	}
	if (!strcmp(pInput, "<!OEM_FLAG_XXXX>"))
	{
		http_update_info(pInput, OPT_FLAG_OEM, MAX_FLAG_OEM_LEN);
		return;
	}
#endif

	if (!strcmp(pInput, "<!MAC_ADDRESS_XX>"))
	{
		mac2str(Sys_pMacAddress, pInput);
		return;
	}
#ifdef OEM_INNOMEDIA
	if (!strcmp(pInput, "<!VER_X>"))
	{
		TaskLoadString(IDS_VERSION, pToken);
		strcpy(pInput, (PCHAR)(pToken + 5));
		return;
	}
#else
	if (!strcmp(pInput, "<!VER_>"))
	{
#ifdef OEM_IP20
		strcpy(pInput, "AAT");
#else

		http_update_info(pInput, OPT_FLAG_VERSION, MAX_FLAG_VERSION);
#endif
		return;
	}
#endif
	if (strlen(pInput) <= (INPUT_TOKEN_LEN + NAME_TOKEN_LEN))	
	{
		return;
	}

	line_start(pBuf);
	if (!memcmp_str(pInput, _cInput))
	{
		pCur = (PCHAR)(pInput + INPUT_TOKEN_LEN + NAME_TOKEN_LEN);
		iLen = http_get_token(pToken, pCur);
		if (!iLen)	return;
		sIndex = OptGetIndex(pToken);
		if (sIndex)	
		{
			iType = OptGetType(sIndex);
			if (iType == OPTION_TYPE_SELECT || iType == OPTION_TYPE_UNKNOWN)
			{
				return;
			}
			if (iType == OPTION_TYPE_RADIO)
			{
				pCur += iLen + VALUE_TOKEN_LEN;
				iVal = (UCHAR)atoi(pCur, 10);
				if (iVal == OptionsGetByte(sIndex))
				{
					line_add_data(pInput, INPUT_TOKEN_LEN);
					line_add_str(_cChecked);
					line_add_str((PCHAR)(pInput + INPUT_TOKEN_LEN));
					strcpy(pInput, pBuf);
				}
			}
			else
			{
				line_add_data(pInput, INPUT_TOKEN_LEN);
				line_add_str("value=\"");
				if (iType == OPTION_TYPE_BYTE)
				{
					line_add_short((USHORT)OptionsGetByte(sIndex));
				}
				else if (iType == OPTION_TYPE_IPADDR)
				{
					OptGetIP(sIndex, pIPAddr);
					line_add_ip(pIPAddr);
				}
				else if (iType == OPTION_TYPE_STRING)
				{
					iMaxLen = OptGetMaxlen(sIndex);
//					OptionsGetString(line_get_buf(), sIndex, iMaxLen);
//					line_update_len();
					line_add_options_str(sIndex, iMaxLen);
				}
				else if (iType == OPTION_TYPE_SHORT)
				{
					line_add_short(OptionsGetShort(sIndex));
				}
				line_add_str("\" ");
				line_add_str((PCHAR)(pInput + INPUT_TOKEN_LEN));
				strcpy(pInput, pBuf);
			}
		}
	}
	else if (!memcmp_str(pInput, _cSelect))
	{
		iLen = http_get_token(pToken, (PCHAR)(pInput+SELECT_TOKEN_LEN+NAME_TOKEN_LEN));
		if (!iLen)
		{
			_iHttpSelectVal = 255;
		}
		else
		{
			sIndex = OptGetIndex(pToken);
			_iHttpSelectVal = (sIndex) ? OptionsGetByte(sIndex) : 255;
		}
	}
	else if (!memcmp_str(pInput, _cOption))
	{
		iVal = (UCHAR)atoi((PCHAR)(pInput+OPTION_TOKEN_LEN+VALUE_TOKEN_LEN), 10);
		if (_iHttpSelectVal == iVal)
		{
			line_add_data(pInput, OPTION_TOKEN_LEN);
			line_add_str(_cSelected);
			line_add_str((PCHAR)(pInput + OPTION_TOKEN_LEN));
			strcpy(pInput, pBuf);
		}
	}
}

void fix_itoa(USHORT sIndex, PCHAR pDst, UCHAR iFixLen)
{
	UCHAR iLen;
	UCHAR pBuf[8];

	itoa(sIndex, pBuf, 10);
	iLen = (UCHAR)strlen(pBuf);
	memset(pDst, '0', iFixLen);
	strcpy((PCHAR)(pDst + iFixLen - iLen), pBuf);
}

void http_update_pb(PCHAR pBuf, UCHAR iIndex)
{
	USHORT sIndex;
	UCHAR pIndex[4];
	UCHAR pName[MAX_USER_NAME_LEN];
	UCHAR pNumber[MAX_USER_NUMBER_LEN];

	sIndex = OPT_PHONE_BOOK + (iIndex << PHONEBOOK_ENTRY_SHIFT);
	fix_itoa((USHORT)(iIndex + 1), pIndex, 3);

	if (OptionsGetByte(sIndex))
	{
		OptionsGetString(pName, (USHORT)(sIndex + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
		OptionsGetString(pNumber, (USHORT)(sIndex + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
	}
	else
	{
		pName[0] = 0;
		pNumber[0] = 0;
	}
#ifdef OEM_IP20
	line_start(pBuf);
	line_add_data(_cPbBody, 17);
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cPbBody + 17), (UCHAR)(49-17));
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cPbBody + 49), (UCHAR)(57-49));
	line_add_str(pName);
	line_add_data((PCHAR)(_cPbBody + 57), (UCHAR)(112-57));
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cPbBody + 112), (UCHAR)(120-112));
	line_add_str(pNumber);
	line_add_data((PCHAR)(_cPbBody + 120), (UCHAR)(PB_BODY_LEN-120));
#else
	line_start(pBuf);
	line_add_data(_cPbBody, 8);
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cPbBody + 8), (UCHAR)(30-8));
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cPbBody + 30), (UCHAR)(38-30));
	line_add_str(pName);
	line_add_data((PCHAR)(_cPbBody + 38), (UCHAR)(83-38));
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cPbBody + 83), (UCHAR)(91-83));
	line_add_str(pNumber);
	line_add_data((PCHAR)(_cPbBody + 91), (UCHAR)(PB_BODY_LEN-91));
#endif
}

#ifdef OEM_IP20

#define PB_BODY_LEN_MOD		155
const UCHAR _cPbBodyMod[] = "<tr><td width=60></td><td width=150><INPUT name=n value=\"\" size=16 maxlength=31></td><td width=150><INPUT name=p value=\"\" size=24 maxlength=31></td></tr>\r\n";
//const UCHAR _cDmBodyMod[] = "<tr><td width=60><p></p><p></td><td width=150>&nbsp;</td><td width=150>&nbsp;</td><td width=60></td><td width=150><INPUT name=n value=\"\" size=16 maxlength=16></td><td width=150><INPUT name=p value=\"\" size=16 maxlength=16></td></tr>\r\n";
const UCHAR _cDmBodyMod[] = "<tr><td width=60><p></p><p></td><td width=150>&nbsp;</td><td width=150>&nbsp;</td><td width=60></td><td width=150><INPUT name=n value=\"\" size=16 maxlength=31></td><td width=150><INPUT name=p value=\"\" size=24 maxlength=31></td></tr>\r\n";
//const UCHAR _cMsgBodyMod[]= "<tr><td width=60></td><td>Mesaj Servis Numaras?</td><td width=150><INPUT name=p value=\"\" size=24 maxlength=31></td></tr>\r\n";
const UCHAR _cMsgBodyMod[]= "<tr><td width=60></td><td>Mesaj Servis No:</td><td width=150><INPUT name=p value=\"\" size=24 maxlength=31></td></tr>\r\n";

void fix_itoa_mod(USHORT sIndex, PCHAR pDst, UCHAR iFixLen)
{
	UCHAR iLen;
	UCHAR pBuf[8];

	itoa(sIndex, pBuf, 10);
	iLen = (UCHAR)strlen(pBuf);
	memset(pDst, '0', iFixLen);
	strcpy((PCHAR)(pDst + iFixLen - iLen), pBuf);
}

void http_update_dm(PCHAR pBuf, UCHAR iIndex)
{
	USHORT sIndex;
	UCHAR pIndex[4];
	UCHAR pName[MAX_USER_NAME_LEN];
	UCHAR pNumber[MAX_USER_NUMBER_LEN];
	UCHAR pMessage[MAX_USER_NUMBER_LEN];
	UCHAR pMessageName[MAX_USER_NAME_LEN];

	sIndex = OPT_PHONE_BOOK + (iIndex << PHONEBOOK_ENTRY_SHIFT);
	fix_itoa_mod((USHORT)(iIndex + 1), pIndex, 3);
	
	strcpy(pMessageName, "Mesaj Servis No");
	OptionsGetString(pMessage, OPT_MESSAGE_NUMBER, MAX_USER_NUMBER_LEN);

	if (OptionsGetByte(sIndex))
	{
		OptionsGetString(pName, (USHORT)(sIndex + PHONEBOOK_DATA_NAME), MAX_USER_NUMBER_LEN);
		OptionsGetString(pNumber, (USHORT)(sIndex + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
	}
	else
	{
		pName[0] = 0;
		pNumber[0] = 0;
	}
	//UdpDebugVal(sIndex , 10) ;
	
	if (!memcmp_str(pIndex, "008")||!memcmp_str(pIndex, "009")||!memcmp_str(pIndex, "010")||!memcmp_str(pIndex, "011")||!memcmp_str(pIndex, "012")||!memcmp_str(pIndex, "013")||!memcmp_str(pIndex, "014")||!memcmp_str(pIndex, "015"))
	{	
		line_start(pBuf);	
		line_add_data(_cDmBodyMod, 95);
		line_add_str(pIndex);
		line_add_data((PCHAR)(_cDmBodyMod + 95), (UCHAR)(127-95));
		line_add_str(pIndex);
		line_add_data((PCHAR)(_cDmBodyMod + 127), (UCHAR)(135-127));
		line_add_str(pName);
		line_add_data((PCHAR)(_cDmBodyMod + 135), (UCHAR)(190-135));
		line_add_str(pIndex);
		line_add_data((PCHAR)(_cDmBodyMod + 190), (UCHAR)(198-190));
		line_add_str(pNumber);
		line_add_data((PCHAR)(_cDmBodyMod + 198), (UCHAR)(300-198));
	}	
	else if (!memcmp_str(pIndex, "120"))
	{	
		line_start(pBuf);
		line_add_data(_cMsgBodyMod, 17);
		//line_add_str(pIndex);
		line_add_data((PCHAR)(_cMsgBodyMod + 17), (UCHAR)(49-17));
		//line_add_str(pIndex);
		line_add_data((PCHAR)(_cMsgBodyMod + 49), (UCHAR)(57-49));
		//line_add_str(pName);
		//line_add_str(pMessageName);
		line_add_data((PCHAR)(_cMsgBodyMod + 57), (UCHAR)(70-57));
		//line_add_str(pIndex);
		line_add_data((PCHAR)(_cMsgBodyMod + 70), (UCHAR)(82-70));
		//line_add_str(pNumber);
		line_add_str(pMessage);
		line_add_data((PCHAR)(_cMsgBodyMod + 82), (UCHAR)(120-82));
	}
	else 
	{	
		line_start(pBuf);
		line_add_data(_cPbBodyMod, 17);
		line_add_str(pIndex);
		line_add_data((PCHAR)(_cPbBodyMod + 17), (UCHAR)(49-17));
		line_add_str(pIndex);
		line_add_data((PCHAR)(_cPbBodyMod + 49), (UCHAR)(57-49));
		line_add_str(pName);
		line_add_data((PCHAR)(_cPbBodyMod + 57), (UCHAR)(112-57));
		line_add_str(pIndex);
		line_add_data((PCHAR)(_cPbBodyMod + 112), (UCHAR)(120-112));
		line_add_str(pNumber);
		line_add_data((PCHAR)(_cPbBodyMod + 120), (UCHAR)(PB_BODY_LEN_MOD-120));
	}
}

#else
void http_update_dm(PCHAR pBuf, UCHAR iIndex)
{
	USHORT sIndex;
	UCHAR pIndex[4];
	UCHAR pMap[DIGITMAP_ENTRY_LEN];

	sIndex = OPT_DIGIT_MAP + (iIndex << DIGITMAP_ENTRY_SHIFT);
	OptionsGetString(pMap, sIndex, DIGITMAP_ENTRY_LEN);
	iIndex ++;
	fix_itoa((USHORT)iIndex, pIndex, 3);

	line_start(pBuf);
	line_add_data(_cDmBody, 8);
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cDmBody + 8), (UCHAR)(30-8));
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cDmBody + 30), (UCHAR)(38-30));
	line_add_str(pMap);
	line_add_data((PCHAR)(_cDmBody + 38), (UCHAR)(70-38));

	iIndex ++;
	fix_itoa((USHORT)iIndex, pIndex, 3);
	sIndex += DIGITMAP_ENTRY_LEN;
	OptionsGetString(pMap, sIndex, DIGITMAP_ENTRY_LEN);
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cDmBody + 70), (UCHAR)(92-70));
	line_add_str(pIndex);
	line_add_data((PCHAR)(_cDmBody + 92), (UCHAR)(100-92));
	line_add_str(pMap);
	line_add_data((PCHAR)(_cDmBody + 100), (UCHAR)(DM_BODY_LEN-100));
}
#endif

#ifdef OEM_INNOMEDIA
void ap_write_dm(PCHAR pBuf)
{
	UCHAR pMap[DIGITMAP_ENTRY_LEN];
	PCHAR p;
	UCHAR i, iLen;
	USHORT sIndex;

	i = 0;
	iLen = 0;
	p = pBuf;
	sIndex = OPT_DIGIT_MAP;
	while (1)
	{
		if (*p == (UCHAR)'|' || *p == 0)
		{
			if (iLen)
			{
				pMap[iLen] = 0;
				OptWriteStr(pMap, sIndex, DIGITMAP_ENTRY_LEN);
				sIndex += DIGITMAP_ENTRY_LEN;
				iLen = 0;
				i ++;
			}
			if (*p)
			{
				p ++;
			}
			else
			{
				break;
			}
		}
		else
		{
			if (iLen < DIGITMAP_ENTRY_LEN-1)
			{
				pMap[iLen] = *p;
				iLen ++;
			}
			p ++;
		}
	}
	pMap[0] = 0;
	for (;i < DIGITMAP_ENTRY_NUM; i ++)
	{
		OptWriteStr(pMap, sIndex, DIGITMAP_ENTRY_LEN);
		sIndex += DIGITMAP_ENTRY_LEN;
	}
}
#endif

void http_write_settings(PCHAR pBuf)
{
	PCHAR pCur;
	USHORT sIndex;
	UCHAR iLen, iType, iMaxLen;
	UCHAR pToken[32];
	UCHAR pVal[IP_ALEN];

	iLen = http_get_token(pToken, pBuf);
	if (!iLen)	return;

	sIndex = OptGetIndex(pToken);
	if (sIndex)
	{
		pCur = (PCHAR)(pBuf + iLen);
#ifdef OEM_INNOMEDIA
		if (sIndex == OPT_DIGIT_MAP)
		{
			ap_write_dm(pCur);
			return;
		}
#endif
		iType = OptGetType(sIndex);
		if (iType == OPTION_TYPE_UNKNOWN)		return;

		if (iType <= OPTION_TYPE_BYTE)
		{
#ifdef OEM_BT2008
			if ((sIndex == OPT_VOL_OUTPUT) && (atoi(pCur, 10) > MAX_VOLOUT))
			{
				OptionsWriteByte(sIndex, MAX_VOLOUT);
			}
			else
			{
				OptionsWriteByte(sIndex, (UCHAR)atoi(pCur, 10));
			}
#else
			OptionsWriteByte(sIndex, (UCHAR)atoi(pCur, 10));
#endif
		}
		else if (iType == OPTION_TYPE_IPADDR)
		{
			if (!str2ip(pCur, pVal))
			{
				memset(pVal, 0, IP_ALEN);
			}
			OptionsWriteBuf(pVal, sIndex, IP_ALEN);
		}
		else if (iType == OPTION_TYPE_STRING)
		{
			iMaxLen = OptGetMaxlen(sIndex);
			if (iMaxLen)
			{
				OptWriteStr(pCur, sIndex, iMaxLen);
			}
		}
		else if (iType == OPTION_TYPE_SHORT)
		{
			USHORT2PCHAR(atoi(pCur, 10), pVal);
			OptionsWriteBuf(pVal, sIndex, 2);
		}
	}
/*	else
	{
		UdpDebugString("Option unknown");
		UdpDebugString(pToken);
	}
*/
}

void http_write_pb(PCHAR pBuf)
{
	UCHAR iVal;
	USHORT sIndex;
	PCHAR pVal;

	iVal = pBuf[0];
	pVal = (PCHAR)(pBuf + 5);
	sIndex = atoi((PCHAR)(pBuf + 1), 10) - 1;
	if (sIndex >= PHONEBOOK_ENTRY_NUM)	return;
	sIndex <<= PHONEBOOK_ENTRY_SHIFT;
	sIndex += OPT_PHONE_BOOK;

	// first clear this entry
	if (iVal == (UCHAR)'n')
	{
		OptionsWriteByte(sIndex, 0);
	}
	if (strlen(pVal))
	{
		OptionsWriteByte(sIndex, 1);
	}
	if (iVal == (UCHAR)'n')
	{
		OptWriteStr(pVal, (USHORT)(sIndex + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
	}
	else
	{
		OptWriteStr(pVal, (USHORT)(sIndex + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
	}
}

void http_write_dm(PCHAR pBuf)
{
	USHORT sIndex;

	sIndex = atoi((PCHAR)(pBuf + 1), 10) - 1;
	if (sIndex >= DIGITMAP_ENTRY_NUM)	return;
	sIndex <<= DIGITMAP_ENTRY_SHIFT;
	sIndex += OPT_DIGIT_MAP;
	OptWriteStr((PCHAR)(pBuf + 5), sIndex, DIGITMAP_ENTRY_LEN);
}

USHORT HttpHandleEvent(UCHAR iEvent, USHORT sParam)
{
	USHORT sVal;
	PCHAR pParam;

	pParam = (PCHAR)sParam;
	switch(iEvent)
	{
	case HTTP_GET_SETTINGS_LEN:
		sVal = http_get_settings_len();
		return sVal;

	case HTTP_GET_PB_LEN:
		sVal = http_get_pb_len();
		return sVal;

	case HTTP_GET_DM_LEN:
		sVal = http_get_dm_len();
		return sVal;

	case HTTP_UPDATE_SETTINGS:
		http_update_settings(pParam);
		break;

	case HTTP_UPDATE_PB:
		http_update_pb(pParam, pParam[0]);
		break;

	case HTTP_UPDATE_DM:
		http_update_dm(pParam, pParam[0]);
		break;

	case HTTP_WRITE_SETTINGS:
		http_write_settings(pParam);
		break;

	case HTTP_WRITE_PB:
		http_write_pb(pParam);
		break;

	case HTTP_WRITE_DM:
		http_write_dm(pParam);
		break;

	case HTTP_WRITE_APDM:
		sVal = OPT_DIGIT_MAP + (pParam[DIGITMAP_ENTRY_LEN] << DIGITMAP_ENTRY_SHIFT);
		OptWriteStr(pParam, sVal, DIGITMAP_ENTRY_LEN);
		break;
	}

	return 0;
}

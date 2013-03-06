/*-------------------------------------------------------------------------
   AR1688 Menu function copy right information

   Copyright (c) 2006-2013. Tang, Li <tangli@palmmicro.com>
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

#ifdef SYS_LCD

const USHORT _cNetowrkOptions[NETWORK_MENU_MAX] = {
OPT_IP_TYPE,
OPT_IP_ADDRESS,
OPT_SUBNET_MASK,
OPT_ROUTER_IP,
OPT_PPPOE_ID,
OPT_PPPOE_PIN,
OPT_DNS_TYPE,
OPT_DNS_IP,
OPT_DNS_IP2,
OPT_LAY3_QOS,
OPT_VLAN_TAG,
OPT_QOS_PRIORITY,
};

const USHORT _cVoiceOptions[VOICE_MENU_MAX] = {
OPT_VOICE_CODER0,
OPT_FRAME_NUMBER0,
OPT_VOICE_CODER1,
OPT_FRAME_NUMBER1,
OPT_VOICE_CODER2,
OPT_FRAME_NUMBER2,
OPT_VOICE_CODER3,
OPT_FRAME_NUMBER3,
OPT_VOICE_CODER4,
OPT_FRAME_NUMBER4,
OPT_VOICE_CODER5,
OPT_FRAME_NUMBER5,
OPT_VOICE_CODER6,
OPT_FRAME_NUMBER6,
OPT_ILBC_FRAME,
OPT_SPEEX_RATE,
OPT_VAD,	
OPT_AGC,		
OPT_AEC,
};

#ifdef CALL_IAX2
const USHORT _cProtocolOptions[PROTOCOL_MENU_MAX] = {
OPT_REGISTER,
OPT_IAX2_SERVER,
OPT_IAX2_PORT,
OPT_USER_NUMBER,
OPT_AUTH_ID,
OPT_AUTH_PIN,
OPT_USER_NAME,
OPT_LOCAL_PORT,
OPT_REGISTER_TTL,
OPT_MESSAGE_NUMBER,
OPT_DIGIT_DIALING,
};
#elif defined CALL_SIP
const USHORT _cProtocolOptions[PROTOCOL_MENU_MAX] = {
OPT_REGISTER,
OPT_SIP_PROXY,
OPT_SIP_PORT,
OPT_SIP_DOMAIN,
OPT_OUTBOUND_PROXY,
OPT_DNS_SRV,
OPT_USER_NUMBER,
OPT_AUTH_ID,
OPT_AUTH_PIN,
OPT_USER_NAME,
OPT_LOCAL_PORT,
OPT_RTP_PORT,
OPT_REGISTER_TTL,
OPT_KEEP_ALIVE,
OPT_DTMF_TYPE,
OPT_DTMF_PAYLOAD,
OPT_ADPCM32_PAYLOAD,
OPT_ILBC_PAYLOAD,
OPT_SPEEX_PAYLOAD,
OPT_MESSAGE_NUMBER,
OPT_PRACK,
OPT_SUBSCRIBE_MWI,
OPT_PROXY_REQUIRE,
OPT_NAT_TRAVERSAL,
OPT_NAT_IP,
OPT_STUN_SERVER,
OPT_STUN_PORT,
};
#endif

const USHORT _cDialplanOptions[DIALPLAN_MENU_MAX] = {
OPT_FWD_NUMBER,
OPT_FWD_ALWAYS,
OPT_FWD_BUSY,
OPT_FWD_NOANSWER,
OPT_NOANSWER_TIMEOUT,
OPT_AUTO_ANSWER,
OPT_CALL_WAITING,
OPT_DIAL_PREFIX,
OPT_HOTLINE_NUMBER,
#ifdef OEM_IP20
OPT_TIME_OUT,
#endif
#ifndef OEM_IP20
OPT_USE_DIGITMAP,
OPT_DIGITMAP_TIMEOUT,
OPT_POUND_AS_CALL,
#endif
#ifdef OEM_BT2008
OPT_REDIAL_TIMEOUT,
OPT_CALL_LOG_TIMEOUT,
OPT_RINGING,
OPT_GRADUAL_RINGING,
OPT_GET_VOICE_MSG,
#endif
};

const USHORT _cSystemOptions[SYSTEM_MENU_MAX] = {
#ifdef OEM_INNOMEDIA
OPT_ADMIN_USER,
#endif
OPT_ADMIN_PIN,
OPT_SYSLOG_IP,
OPT_VOL_INPUT,
OPT_VOL_OUTPUT,
OPT_SPEAKER_VOL,
OPT_RING_VOL_OUTPUT,
#ifndef OEM_INNOMEDIA
OPT_PROVISION_SERVER,
OPT_PROVISION_PORT,
#endif
OPT_PROVISION_TTL,
OPT_SNTP_DNS,
OPT_TIME_ZONE,
OPT_DAY_LIGHT,
};

const UCHAR _cSettingsSubMenuMax[SETTINGS_MENU_SYSTEM+1] = {
NETWORK_MENU_MAX,
VOICE_MENU_MAX,
PROTOCOL_MENU_MAX,
DIALPLAN_MENU_MAX,
SYSTEM_MENU_MAX,
};

#define INPUT_TYPE_DIGIT_STR	0	// Include '0'-'9', '*', '#'
#define INPUT_TYPE_CHAR_STR		1	// Include all supported chars
#define INPUT_TYPE_IP_STR		2	// Include '0'-'9' and '*', '*' is seen as '.'
#define INPUT_TYPE_INT_STR		3	// Include '0'-'9'
#define INPUT_TYPE_STRING		3	// All type <= this value are string type
#define INPUT_TYPE_SELECT		4

#define SCROLL_TIMEOUT	3
#define MENU_TIMEOUT	60

#define INPUT_BUF_MAX	MAX_DNS_LEN
UCHAR _pInputBuf[INPUT_BUF_MAX];
UCHAR _iInputPos;
UCHAR _iInputMax;
UCHAR _iPrevKey;
UCHAR _iMapPos;
UCHAR _iInputType;
UCHAR _iInputVal;

UCHAR _iMenuType;
UCHAR _iMainMenuItem;
UCHAR _iSub1MenuItem;
UCHAR _iSub2MenuItem;
UCHAR _iSub3MenuItem;

UCHAR _iOldSub1Item;

UCHAR _iScrollPos;
UCHAR _iScrollTimer;
UCHAR _iMenuTimer;
BOOLEAN _bPhonebookChanged;
BOOLEAN _bSettingsChanged;
BOOLEAN _bScrollDisplay;

#ifdef OEM_IP20
extern UCHAR _pTotalRecords[3];
//extern UCHAR voice_message ;
extern void sampleExec();
#endif

/*******************************************************************
**********************   SRAM Handler     *************************
 *******************************************************************
*/

UCHAR SramGetByte(USHORT sOffset)
{
	UCHAR iVal;

	BufSeek(sOffset);
	BufRead(&iVal, 1);
	return iVal;
}

USHORT SramGetShort(USHORT sOffset)
{
	UCHAR pBuf[2];

	BufSeek(sOffset);
	BufRead(pBuf, 2);
	return PCHAR2USHORT(pBuf);
}

void SramGetBuf(PCHAR pBuf, USHORT sOffset, UCHAR iLen)
{
	BufSeek(sOffset);
	BufRead(pBuf, iLen);
}

/*******************************************************************
 ********************* Misc Handler     *************************
 *******************************************************************
*/

UCHAR ScrollUp(UCHAR iIndex, UCHAR iMax)
{
	if (!iIndex || iIndex >= iMax)
	{
		iIndex = iMax - 1;
	}
	else
	{
		iIndex --;
	}
	return iIndex;
}

UCHAR ScrollDown(UCHAR iIndex, UCHAR iMax)
{
	iIndex ++;
	if (iIndex >= iMax)
	{
		iIndex = 0;
	}
	return iIndex;
}

void ClearInputBuf()
{
	memset(_pInputBuf, 0, INPUT_BUF_MAX);
	_iInputPos = 0;
}

void FixedDisplay(PCHAR pString)
{
	_bScrollDisplay = FALSE;
	LcdDisplay(pString, 1);
}

#ifdef OEM_IP20

void FixedDispTwo(PCHAR pString)
{
	_bScrollDisplay = FALSE;
	LcdDisplay(pString, 2);
}

void FixedDispThree(PCHAR pString)
{
	_bScrollDisplay = FALSE;
	LcdDisplay(pString, 3);
}

#define RANGE_LINE	2

#else
#define RANGE_LINE	1
#endif

void RangeDisplay()
{
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

	memcpy(pBuf, (PCHAR)(_pInputBuf + _iScrollPos), DISPLAY_MAX_CHAR);
	pBuf[DISPLAY_MAX_CHAR] = 0;
	LcdDisplay(pBuf, RANGE_LINE);
}

void ScrollDisplay()
{
	USHORT sLen;

	sLen = strlen(_pInputBuf);
	_bScrollDisplay = FALSE;
	if (sLen > DISPLAY_MAX_CHAR)
	{
#ifdef UI_4_LINES
		LcdDisplay(_pInputBuf, RANGE_LINE);
		LcdDisplay(_pInputBuf + DISPLAY_MAX_CHAR, RANGE_LINE + 1);
#else
		_bScrollDisplay = TRUE;
		_iScrollTimer = 0;
		_iScrollPos = 0;
		RangeDisplay();
#endif
	}
	else
	{
		if (sLen)
		{
			LcdDisplay(_pInputBuf, RANGE_LINE);
		}
		else
		{
			LcdDisplay(_cEmpty, RANGE_LINE);
		}
#ifdef UI_4_LINES
		LcdDisplay("", RANGE_LINE + 1);
#endif
	}
}

void ScrollDisplayTimer()
{
	_iScrollTimer ++;
	if (_iScrollTimer == SCROLL_TIMEOUT)
	{
		_iScrollTimer = 0;
		_iScrollPos += DISPLAY_MAX_CHAR;
		if (_iScrollPos >= strlen(_pInputBuf))
		{
			_iScrollPos = 0;
		}
		RangeDisplay();
	}
}

void InputDisplay()
{
	UCHAR iPos;
#ifdef UI_4_LINES
	UCHAR pString[DISPLAY_MAX_CHAR+1];
#endif

	if (_iInputPos >= DISPLAY_MAX_CHAR)
	{
		FixedDisplay((PCHAR)(_pInputBuf + _iInputPos - DISPLAY_MAX_CHAR + 1));
		iPos = DISPLAY_MAX_CHAR-1;
	}
	else
	{
		FixedDisplay(_pInputBuf);
		iPos = _iInputPos;
	}
#ifdef UI_4_LINES
	memset(pString, ' ', DISPLAY_MAX_CHAR);
	pString[iPos] = '^';
	pString[iPos+1] = 0;
	LcdDisplay(pString, 2);
#else
	LcdCursor(1, iPos, TRUE);
#endif
}

void WriteSram2Flash()
{
	LcdDisplay(_cWriting, 0);
	FixedDisplay("");
	TaskSystemHandler(SYS_MODIFY_PAGE, SYSTEM_SETTINGS_PAGE);
	FlashWritePage(SYSTEM_SETTINGS_PAGE);
}

/*******************************************************************
 ********************* Phonebook Handler   *************************
 *******************************************************************
*/

USHORT PbIndex2Offset(UCHAR iIndex)
{
	return (USHORT)(OPT_PHONE_BOOK + ((USHORT)iIndex << PHONEBOOK_ENTRY_SHIFT));
}

BOOLEAN PbIsValid(UCHAR iIndex)
{
	USHORT sOffset;

	sOffset = PbIndex2Offset(iIndex);
	return SramGetByte(sOffset) ? TRUE : FALSE;
}

UCHAR PbGetNum()
{
	UCHAR i, iNum;

	iNum = 0;
	for (i = 0; i < PHONEBOOK_ENTRY_NUM; i ++)
	{
		if (PbIsValid(i))
		{
			iNum ++;
		}
	}
	return iNum;
}

UCHAR PbGetNew()
{
	UCHAR i;

	for (i = 0; i < PHONEBOOK_ENTRY_NUM; i ++)
	{
		if (!PbIsValid(i))	return i;
	}

	return PHONEBOOK_ENTRY_NUM;
}

UCHAR PbGetNext(UCHAR iStart)
{
	UCHAR i;

	i = iStart + 1;
	while (i < PHONEBOOK_ENTRY_NUM)
	{
		if (PbIsValid(i))	return i;
		i ++;
	}

	return PHONEBOOK_MENU_MAX;
}

UCHAR PbGetPrev(UCHAR iStart)
{
	UCHAR i;

	if (!iStart)
	{
		return PHONEBOOK_MENU_MAX;
	}

	i = iStart;
	do
	{
		i --;
		if (PbIsValid(i))	return i;
	}while (i);

	return PHONEBOOK_MENU_MAX;
}

UCHAR PbGetHead()
{
	return PbGetNext(255);
}

UCHAR PbGetTail()
{
	return PbGetPrev(PHONEBOOK_ENTRY_NUM);
}

void PbGetName(PCHAR pName)
{
	USHORT sOffset;

	sOffset = PbIndex2Offset(_iSub1MenuItem);
	if (SramGetByte(sOffset))
	{
		SramGetBuf(pName, (USHORT)(sOffset + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
	}
	else
	{
		pName[0] = 0;
	}
}

void PbGetNumber(PCHAR pNumber)
{
	USHORT sOffset;

	sOffset = PbIndex2Offset(_iSub1MenuItem);
	if (SramGetByte(sOffset))
	{
		SramGetBuf(pNumber, (USHORT)(sOffset + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
	}
	else
	{
		pNumber[0] = 0;
	}
}

void PbCheckValid(USHORT sOffset)
{
	UCHAR pName[MAX_USER_NAME_LEN];
	UCHAR pNumber[MAX_USER_NUMBER_LEN];
	UCHAR iValid;

	iValid = 0;
	SramGetBuf(pName, (USHORT)(sOffset + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
	if (strlen(pName))
	{
		iValid = 1;
	}
	SramGetBuf(pNumber, (USHORT)(sOffset + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
	if (strlen(pNumber))
	{
		iValid = 1;
	}
	OptionsWriteByte(sOffset, iValid);
}

void PbSaveName(UCHAR iIndex)
{
	USHORT sOffset;

	sOffset = PbIndex2Offset(iIndex);
	OptWriteStr(_pInputBuf, (USHORT)(sOffset + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
	PbCheckValid(sOffset);
	_bPhonebookChanged = TRUE;
}

void PbSaveNumber(UCHAR iIndex)
{
	USHORT sOffset;

	sOffset = PbIndex2Offset(iIndex);
	OptWriteStr(_pInputBuf, (USHORT)(sOffset + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
	PbCheckValid(sOffset);
	_bPhonebookChanged = TRUE;
}

void PbDelete(UCHAR iIndex)
{
	USHORT sOffset;
	UCHAR pNumber[PHONEBOOK_DATA_LEN];

	sOffset = PbIndex2Offset(iIndex);
	memset(pNumber, 0, PHONEBOOK_DATA_LEN);
	OptionsWriteBuf(pNumber, sOffset, PHONEBOOK_DATA_LEN);
	_bPhonebookChanged = TRUE;
}

void PbDeleteAll()
{
	UCHAR i;

	for (i = 0; i < PHONEBOOK_ENTRY_NUM; i ++)
	{
		PbDelete(i);
	}
}
/*******************************************************************
 ********************* Settings Handler   *************************
 *******************************************************************
*/
void MarkValString(const PCHAR pMark, UCHAR iVal, PCHAR pBuf)
{
	pBuf[0] = '[';
	itoa(iVal, (PCHAR)(pBuf + 1), 10);
	strcat_char(pBuf, ' ');
	strcat(pBuf, pMark);
	strcat_char(pBuf, ']');
}

USHORT SettingsGetIndex()
{
	switch(_iSub1MenuItem)
	{
	case SETTINGS_MENU_NETWORK:
		if (_iSub2MenuItem < NETWORK_MENU_MAX)
		{
			return _cNetowrkOptions[_iSub2MenuItem];
		}
		break;

	case SETTINGS_MENU_VOICE:
		if (_iSub2MenuItem < VOICE_MENU_MAX)
		{
			return _cVoiceOptions[_iSub2MenuItem];
		}
		break;

	case SETTINGS_MENU_PROTOCOL:
		if (_iSub2MenuItem < PROTOCOL_MENU_MAX)
		{
			return _cProtocolOptions[_iSub2MenuItem];
		}
		break;

	case SETTINGS_MENU_DIALPLAN:
		if (_iSub2MenuItem < DIALPLAN_MENU_MAX)
		{
			return _cDialplanOptions[_iSub2MenuItem];
		}
		break;

	case SETTINGS_MENU_SYSTEM:
		if (_iSub2MenuItem < SYSTEM_MENU_MAX)
		{
			return _cSystemOptions[_iSub2MenuItem];
		}
		break;
	}

	return 0;
}

const PCHAR SettingsGetMark(USHORT sIndex, UCHAR iVal)
{
	const PCHAR pMark;
#ifdef OEM_IP20
	pMark = _cSpaces; 
#else
	pMark = _cError;
#endif
	switch (sIndex)
	{
	case OPT_IP_TYPE:
		pMark = _cIPTypeMark[iVal];
		_iInputMax = 3;
		break;

 	case OPT_DNS_TYPE:
		pMark = _cDnsTypeMark[iVal];
		_iInputMax = 2;
		break;

	case OPT_VOICE_CODER0:
	case OPT_VOICE_CODER1:
	case OPT_VOICE_CODER2:
	case OPT_VOICE_CODER3:
	case OPT_VOICE_CODER4:
	case OPT_VOICE_CODER5:
	case OPT_VOICE_CODER6:
		pMark = _cCodecMark[iVal];
		_iInputMax = VOICE_CODER_NUM+1;
		break;

 	case OPT_SPEEX_RATE:
		pMark = _cSpeexRateMark[iVal];
		_iInputMax = 8;
		break;

/*	case OPT_G723_RATE:
		pMark = _cG723RateMark[iVal];
		_iInputMax = 2;
		break;
*/
	case OPT_ILBC_FRAME:
		pMark = _cIlbcFrameMark[iVal];
		_iInputMax = 2;
		break;

	case OPT_VAD:
	case OPT_AGC:
	case OPT_AEC:
	case OPT_REGISTER:
	case OPT_FWD_ALWAYS:
	case OPT_FWD_BUSY:
	case OPT_FWD_NOANSWER:
	case OPT_CALL_WAITING:
	case OPT_AUTO_ANSWER:
	case OPT_DAY_LIGHT:
//	case OPT_UDP_DEBUG:
	case OPT_USE_DIGITMAP:
#ifndef OEM_IP20
	case OPT_POUND_AS_CALL:
#endif
//	case OPT_RINGING:
//	case OPT_GRADUAL_RINGING:
#ifdef CALL_IAX2
	case OPT_DIGIT_DIALING:
#elif defined CALL_SIP
	case OPT_OUTBOUND_PROXY:
	case OPT_DNS_SRV:
	case OPT_PRACK:
	case OPT_SUBSCRIBE_MWI:
#endif
		pMark = _cYesNoMark[iVal];
		_iInputMax = 2;
		break;

#ifdef OEM_BT2008
	case OPT_RINGING:
	case OPT_GRADUAL_RINGING:
		pMark = _cOnOffMark[iVal];
		_iInputMax = 2;
		break;

	case OPT_GET_VOICE_MSG:
		pMark = _cGetVoiceMsgMark[iVal];
		_iInputMax = 2;
		break;
#endif

#ifdef CALL_SIP
	case OPT_DTMF_TYPE:
		pMark = _cDtmfTypeMark[iVal];
		_iInputMax = 3;
		break;

	case OPT_NAT_TRAVERSAL:
		pMark = _cNatTraversalMark[iVal];
		_iInputMax = 3;
		break;
#endif
	}

	return pMark;
}

void SettingsGetVal(USHORT sIndex)
{
	UCHAR pBuf[IP_ALEN];
	UCHAR iMaxLen, iType;
	const PCHAR pMark;

	ClearInputBuf();
	iType = (sIndex == OPT_TIME_ZONE) ? OPTION_TYPE_BYTE : OptGetType(sIndex);
	if (iType == OPTION_TYPE_UNKNOWN)	return;

	switch (iType)
	{
	case OPTION_TYPE_RADIO:
	case OPTION_TYPE_SELECT:
		_iInputVal = SramGetByte(sIndex);
		pMark = SettingsGetMark(sIndex, _iInputVal);
		MarkValString(pMark, _iInputVal, _pInputBuf);
		break;

	case OPTION_TYPE_BYTE:
		itoa(SramGetByte(sIndex), _pInputBuf, 10);
		break;

	case OPTION_TYPE_IPADDR:
		SramGetBuf(pBuf, sIndex, IP_ALEN);
		ip2str(pBuf, _pInputBuf);
		break;

	case OPTION_TYPE_SHORT:
		itoa(SramGetShort(sIndex), _pInputBuf, 10);
		break;

	case OPTION_TYPE_STRING:
		if (sIndex == OPT_ADMIN_PIN || sIndex == OPT_AUTH_PIN || sIndex == OPT_PPPOE_PIN)
		{
			strcpy(_pInputBuf, "*****");
		}
		else
		{
			iMaxLen = OptGetMaxlen(sIndex);
			SramGetBuf(_pInputBuf, sIndex, iMaxLen);
		}
		break;
	}
}

void OnEditSettings()
{
	USHORT sIndex;
	UCHAR iType;

	sIndex = SettingsGetIndex();
	if (!sIndex)	return;

	iType = (sIndex == OPT_TIME_ZONE) ? OPTION_TYPE_BYTE : OptGetType(sIndex);
	if (iType == OPTION_TYPE_UNKNOWN)	return;

	switch (iType)
	{
	case OPTION_TYPE_RADIO:
	case OPTION_TYPE_SELECT:
		_iInputType = INPUT_TYPE_SELECT;
		_iInputPos = 1;
		break;

	case OPTION_TYPE_BYTE:
		_iInputType = INPUT_TYPE_INT_STR;
		_iInputPos = strlen(_pInputBuf);
		_iInputMax = 3;
		break;

	case OPTION_TYPE_IPADDR:
		_iInputType = INPUT_TYPE_IP_STR;
		_iInputPos = strlen(_pInputBuf);
		_iInputMax = 15;
		break;

	case OPTION_TYPE_SHORT:
		_iInputType = INPUT_TYPE_INT_STR;
		_iInputPos = strlen(_pInputBuf);
		_iInputMax = 5;
		break;

	case OPTION_TYPE_STRING:
		_iInputType = INPUT_TYPE_CHAR_STR;
		_iInputMax = OptGetMaxlen(sIndex) - 1;
		if (sIndex == OPT_ADMIN_PIN || sIndex == OPT_AUTH_PIN || sIndex == OPT_PPPOE_PIN)
		{
			ClearInputBuf();
		}
		else
		{
			_iInputPos = strlen(_pInputBuf);
		}
		break;
	}
}

void OnEditSettingsOK()
{
	USHORT sIndex, sVal;
	UCHAR pBuf[IP_ALEN];
	UCHAR iMaxLen, iType;

	sIndex = SettingsGetIndex();
	if (!sIndex)	return;

	iType = (sIndex == OPT_TIME_ZONE) ? OPTION_TYPE_BYTE : OptGetType(sIndex);
	if (iType == OPTION_TYPE_UNKNOWN)	return;

	switch (iType)
	{
	case OPTION_TYPE_RADIO:
	case OPTION_TYPE_SELECT:
		OptionsWriteByte(sIndex, _iInputVal);
		break;

	case OPTION_TYPE_BYTE:
		_iInputVal = atoi(_pInputBuf, 10);
#ifdef OEM_BT2008
		if (sIndex == OPT_VOL_OUTPUT)
		{
			if (_iInputVal > MAX_VOLOUT)
			{
				_iInputVal = MAX_VOLOUT;
			}
		}
#endif		
		OptionsWriteByte(sIndex, _iInputVal);
		break;

	case OPTION_TYPE_IPADDR:
		if (str2ip(_pInputBuf, pBuf))
		{
			OptionsWriteBuf(pBuf, sIndex, IP_ALEN);
		}
		break;

	case OPTION_TYPE_SHORT:
		sVal = atoi(_pInputBuf, 10);
		USHORT2PCHAR(sVal, pBuf);
		OptionsWriteBuf(pBuf, sIndex, 2);
		break;

	case OPTION_TYPE_STRING:
		iMaxLen = OptGetMaxlen(sIndex);
		if (iMaxLen)
		{
			OptWriteStr(_pInputBuf, sIndex, iMaxLen);
		}
		break;
	}

}

UCHAR ScrollSelect(UCHAR iIndex, UCHAR iMax, UCHAR iType)
{
	if (iType == UI_EVENT_UP)
	{
		return ScrollUp(iIndex, iMax);
	}
	else if (iType == UI_EVENT_DOWN)
	{
		return ScrollDown(iIndex, iMax);
	}
	else
	{
		iType -= UI_EVENT_DIGIT0;
		return (iType < iMax) ? iType : iIndex;
	}
}

// Settings that are selectable, items number should be LESS THAN 10!!
void OnEditSettingsSelect(UCHAR iType)
{
	USHORT sIndex;
	const PCHAR pMark;

	sIndex = SettingsGetIndex();
	if (!sIndex)	return;

	_iInputVal = ScrollSelect(_iInputVal, _iInputMax, iType);
	pMark = SettingsGetMark(sIndex, _iInputVal);
	MarkValString(pMark, _iInputVal, _pInputBuf);
	_iInputPos = 1;
	InputDisplay();
}

/*******************************************************************
 *********************   Display Handler   *************************
 *******************************************************************
*/
void DisplayMainMenu()
{
	LcdDisplay(_cMainMenu[_iMainMenuItem], 0);
#ifdef OEM_IP20
	FixedDisplay(_cYesOrCancel);
	FixedDispThree(_cCancelOK);
	//LcdDisplay("",1);
	FixedDispTwo(_cYesOrCancel);
#else
	FixedDisplay(_cOKToEnter);
#endif
}

#ifdef SYS_TEST
void DisplayTestMenu()
{
 	LcdDisplay(_cTestMenu[_iSub1MenuItem], 0);
#ifndef OEM_IP20
	FixedDisplay(_cOKToEnter);
#endif
}
#endif

void DisplayCallMenu()
{
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

	Sys_iMissedCalls = 0;
	MarkValString(_cRecords, CallLogGetNum(_iSub1MenuItem), pBuf);
#ifdef OEM_IP20
	LcdDisplay(_cMainMenu[0], 0);
	LcdDisplay(_cCallMenu[_iSub1MenuItem], 1);
	FixedDispTwo(pBuf);
	FixedDispThree(_cGeriSec);
#else
	LcdDisplay(_cCallMenu[_iSub1MenuItem], 0);
	FixedDisplay(pBuf);
#endif
}

void DisplaySettingsMenu()
{
#ifdef OEM_IP20
	LcdDisplay(_cMainMenu[1], 0);
 	LcdDisplay(_cSettingsMenu[_iSub1MenuItem], 1);
	FixedDispTwo(_cYesOrCancel);
	FixedDispThree(_cGeriSec);
#else
 	LcdDisplay(_cSettingsMenu[_iSub1MenuItem], 0);
	FixedDisplay(_cOKToEnter);
#endif
}

void DisplayInfoMenu()
{
#ifdef OEM_IP20
	LcdDisplay(_cMainMenu[3], 0);	
	LcdDisplay(_cInfoMenu[_iSub1MenuItem], 1);
	FixedDispThree(_cCancel);
#else
	LcdDisplay(_cInfoMenu[_iSub1MenuItem], 0);
#endif
	switch (_iSub1MenuItem)
	{
	case INFO_MENU_IP:
		ip2str(Sys_pIpAddress, _pInputBuf);
		break;

	case INFO_MENU_NUMBER:
		strcpy(_pInputBuf, Sys_pUserNumber);
		break;

	case INFO_MENU_MAC:
		mac2str(Sys_pMacAddress, _pInputBuf);
		break;

	case INFO_MENU_MODEL:
		LoadString(IDS_DISPLAY_MODEL, _pInputBuf);
		break;

	case INFO_MENU_PROTOCOL:
		OptionsGetString(_pInputBuf, OPT_FLAG_PROTOCOL, MAX_FLAG_PROTOCOL);
		break;

	case INFO_MENU_LANGUAGE:
#ifdef OEM_IP20
//		strcpy(_pInputBuf, "Türkçe");
		TaskLoadString(IDS_T_K, _pInputBuf);
#else
		OptionsGetString(_pInputBuf, OPT_FLAG_COUNTRY, MAX_FLAG_COUNTRY);
#endif
		break;

	case INFO_MENU_OEM:
		OptionsGetString(_pInputBuf, OPT_FLAG_OEM, MAX_FLAG_OEM_LEN);
		break;

	case INFO_MENU_VERSION:
#ifdef OEM_IP20
//		strcpy(_pInputBuf, "IP20_v_AAT");
		TaskLoadString(IDS_IP20_V_AAT, _pInputBuf);
#else
		strcpy(_pInputBuf, (PCHAR)(_cIDS[IDS_VERSION] + 5));
#endif
		break;
	}

	ScrollDisplay();
}

void DisplayPhonebook()
{
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

	if (_iSub1MenuItem < PHONEBOOK_MENU_MAX)
	{
		PbGetName(_pInputBuf);

#ifdef OEM_IP20
		LcdDisplay(_cMainMenu[2], 0);
		LcdDisplay(_pInputBuf, 1);
		FixedDispThree(_cGeriSec);
#else
		LcdDisplay(_pInputBuf, 0);
#endif
		PbGetNumber(_pInputBuf);
		ScrollDisplay();
	}
	else
	{
		MarkValString(_cContacts, PbGetNum(), pBuf);
#ifdef OEM_IP20
		LcdDisplay(_cMainMenu[2], 0);
		LcdDisplay(_cEndOfList, 1);
		FixedDispTwo(pBuf);	
		FixedDispThree(_cGeriSec);
#else
		LcdDisplay(_cEndOfList, 0);
		FixedDisplay(pBuf);
#endif
	}
}

void DisplayPbActionMenu()
{
	UCHAR iNum;
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

#ifdef OEM_IP20
	LcdDisplay(_cMainMenu[2], 0);
	LcdDisplay(_cPbActionMenu[_iSub2MenuItem], 1);
	FixedDispThree(_cGeriSec);
#else
	LcdDisplay(_cPbActionMenu[_iSub2MenuItem], 0);
#endif

	if (_iSub2MenuItem == PB_ACTION_MENU_CALL || _iSub2MenuItem == PB_ACTION_MENU_EDIT || _iSub2MenuItem == PB_ACTION_MENU_DELETE)
	{
		PbGetName(_pInputBuf);
		if (!strlen(_pInputBuf))
		{
			PbGetNumber(_pInputBuf);
		}
		ScrollDisplay();
	}
	else
	{
		iNum = PbGetNum();
		if (_iSub2MenuItem == PB_ACTION_MENU_ADD)
		{
			MarkValString(_cSpaces, PHONEBOOK_ENTRY_NUM - iNum, pBuf);
		}
		else
		{
			MarkValString(_cContacts, iNum, pBuf);
		}
#ifdef OEM_IP20
		FixedDispTwo(pBuf);
#else
		FixedDisplay(pBuf);
#endif
	}
}

void DisplayOptionsItem()
{

#ifdef OEM_IP20
	if(_iSub1MenuItem == SETTINGS_ALARM)
	{
		LcdCenterDisplay(_cSettingsMenu[7], 0);
//		LcdDisplay("Alarm Aç\Kapat", 1);
		TaskUIHandler(UI_CALL_DISPLAY_IDS1, IDS_ALARM_KAPAT);
	}
	else {
#endif

	USHORT sIndex;

	sIndex = SettingsGetIndex();
	if (!sIndex)	return;

	switch(_iSub1MenuItem)
	{
	case SETTINGS_MENU_NETWORK:
#ifdef OEM_IP20
		LcdCenterDisplay(_cSettingsMenu[0],0);
		LcdDisplay(_cNetworkMenu[_iSub2MenuItem], 1);
#else
		LcdDisplay(_cNetworkMenu[_iSub2MenuItem], 0);
#endif
		break;

	case SETTINGS_MENU_VOICE:
#ifdef OEM_IP20
		LcdCenterDisplay(_cSettingsMenu[1],0);
		LcdDisplay(_cVoiceMenu[_iSub2MenuItem], 1);
#else
		LcdDisplay(_cVoiceMenu[_iSub2MenuItem], 0);
#endif
		break;

	case SETTINGS_MENU_PROTOCOL:
#ifdef OEM_IP20
		LcdCenterDisplay(_cSettingsMenu[2],0);
		LcdDisplay(_cProtocolMenu[_iSub2MenuItem], 1);
#else
		LcdDisplay(_cProtocolMenu[_iSub2MenuItem], 0);
#endif
		break;

	case SETTINGS_MENU_DIALPLAN:
#ifdef OEM_IP20
		LcdCenterDisplay(_cSettingsMenu[3],0);
		LcdDisplay(_cDialplanMenu[_iSub2MenuItem], 1);
#else
		LcdDisplay(_cDialplanMenu[_iSub2MenuItem], 0);
#endif
		break;

	case SETTINGS_MENU_SYSTEM:
#ifdef OEM_IP20
		LcdCenterDisplay(_cSettingsMenu[4],0);
		LcdDisplay(_cSystemMenu[_iSub2MenuItem], 1);
#else
		LcdDisplay(_cSystemMenu[_iSub2MenuItem], 0);
#endif
		break;
	}
	SettingsGetVal(sIndex);
	ScrollDisplay();
#ifdef OEM_IP20
	}
	FixedDispThree(_cGeriSec);
#endif
}

/*******************************************************************
 *******************   Call Log Handler    *********************
 *******************************************************************
*/

void DisplayCallLogItem()
{
	CALL_RECORD * p;
	PCHAR pNumber;
#ifdef OEM_IP20
	UCHAR pNamedisp[32];
	UCHAR iLengname;
#endif

	p = CallLogRead(_iSub1MenuItem, _iSub2MenuItem);
	if (p)
	{
#ifdef OEM_IP20
		LcdCenterDisplay(_cCallMenu[_iSub1MenuItem], 0);
#endif
		TaskUIHandler(UI_MENU_DISPLAY_TIME, (USHORT)(&p->lDatetime));
		pNumber = &p->iMem;
#ifdef OEM_IP20
		strcpy(pNamedisp, (PCHAR)(pNumber + 1 + strlen(pNumber)));	
		iLengname = strlen(pNamedisp) ;
#endif		
		strcpy(_pInputBuf, (PCHAR)(pNumber + 1 + strlen(pNumber)));
		strcat(_pInputBuf, " <");
		strcat(_pInputBuf, pNumber);
		strcat_char(_pInputBuf, '>');
#ifdef OEM_IP20
		strcat_char(_pInputBuf, ' ');
		strcat_char(_pInputBuf, '('); 
		itoa(_iSub2MenuItem+1, (PCHAR)(_pInputBuf + strlen(pNumber)+ iLengname + 5), 10);
		strcat_char(_pInputBuf, '/');
		if(_iSub2MenuItem + 1 == 10)
		{
			itoa(_pTotalRecords[_iSub1MenuItem], (PCHAR)(_pInputBuf + strlen(pNumber) + iLengname + 8), 10);
		}
		else
		{
			itoa(_pTotalRecords[_iSub1MenuItem], (PCHAR)(_pInputBuf + strlen(pNumber) + iLengname + 7), 10);
		}
		strcat_char(_pInputBuf, ')');
#endif

		ScrollDisplay();
	}
	else
	{
		_iMenuType = MENU_TYPE_SUB1;
		DisplayCallMenu();
	}
}

void DisplayCallLogActionMenu()
{
	CALL_RECORD * p;

	p = CallLogRead(_iSub1MenuItem, _iSub2MenuItem);
	if (p)
	{
		_iMenuType = MENU_TYPE_SUB3;
		_iSub3MenuItem = 0;
#ifdef OEM_IP20
		LcdCenterDisplay(_cCallMenu[_iSub1MenuItem], 0);
		LcdDisplay(_cChActionMenu[_iSub3MenuItem], 1);
#else
		LcdDisplay(_cChActionMenu[_iSub3MenuItem], 0);
#endif		
		strcpy(_pInputBuf, &p->iMem);
		ScrollDisplay();
	}
}

void CallLogCall()
{
	CALL_RECORD * p;

	p = CallLogRead(_iSub1MenuItem, _iSub2MenuItem);
	if (p)
	{
		strcpy(_pInputBuf, &p->iMem);
		TaskUIHandler(UI_MENU_CALL, (USHORT)_pInputBuf);
	}
}

void CallLogAddToPb()
{
	UCHAR iPbIndex;
	CALL_RECORD * p;
	PCHAR pNumber;

	iPbIndex = PbGetNew();
	if (iPbIndex < PHONEBOOK_ENTRY_NUM)
	{
		p = CallLogRead(_iSub1MenuItem, _iSub2MenuItem);
		if (p)
		{
			pNumber = &p->iMem;
			strcpy(_pInputBuf, (PCHAR)(pNumber + 1 + strlen(pNumber)));
			PbSaveName(iPbIndex);
			strcpy(_pInputBuf, pNumber);
			PbSaveNumber(iPbIndex);
		}
	}

	_iMenuType = MENU_TYPE_SUB2;
	DisplayCallLogItem();
}

/*******************************************************************
 *********************   Edit Handler      *************************
 *******************************************************************
*/
#if defined RES_RO
#include "res/ro/inputmap.h"
#elif defined RES_RU
#include "res/ru/inputmap.h"
#elif defined RES_TR && defined OEM_IP20
#include "res/ip20/tr/inputmap.h"
#elif defined RES_VN
#include "res/vn/inputmap.h"
#else // RES_US and others
#include "res/us/inputmap.h"
#endif

void InputNext()
{
	_iPrevKey = 0;
	_iMapPos = 0;
}

// iKey = 0-11
void OnInputCharStr(UCHAR iType)
{
	UCHAR iKeyIndex;

	if (_iInputPos >= _iInputMax)	return;

	iKeyIndex = iType - UI_EVENT_DIGIT0;

	if (!_iPrevKey)
	{
		_iMapPos = 0;
	}
	else
	{
		if (iType == _iPrevKey)
		{
			_iMapPos ++;
			if (!_cLetterMap[iKeyIndex][_iMapPos])	_iMapPos = 0;
		}
		else
		{
			_iInputPos ++;
			if (_iInputPos >= _iInputMax)	return;
			_iMapPos = 0;

		}
	}
	_pInputBuf[_iInputPos] = _cLetterMap[iKeyIndex][_iMapPos];
	_iPrevKey = iType;
	InputDisplay();
}

void InputDigit(UCHAR iKey)
{
	if (_iInputPos >= _iInputMax)	return;
	_pInputBuf[_iInputPos] = iKey;
	_iInputPos ++;
	InputDisplay();
}

void OnInputDigitStr(UCHAR iType)
{
	InputDigit(_cLetterMap[iType-UI_EVENT_DIGIT0][0]);
}

void OnInputIPStr(UCHAR iType)
{
	UCHAR iKey;

	if (iType == UI_EVENT_POUND)	return;
	iKey = (iType == UI_EVENT_STAR) ? '.' : _cLetterMap[iType-UI_EVENT_DIGIT0][0];
	InputDigit(iKey);
}

void OnInputIntStr(UCHAR iType)
{
	if (iType <= UI_EVENT_DIGIT9)
	{
		InputDigit(_cLetterMap[iType-UI_EVENT_DIGIT0][0]);
	}
}

void OnEditCancel()
{
	if (!_pInputBuf[_iInputPos] && _iInputPos)	
	{
		_iInputPos --;
	}
	_pInputBuf[_iInputPos] = 0;
	InputNext();
	InputDisplay();
}

void OnCursorRight()
{
	if (_iInputPos >= _iInputMax)	return;
	if (!_pInputBuf[_iInputPos])
	{
		_pInputBuf[_iInputPos] = ' ';
	}

	_iInputPos ++;
	if (_iInputPos >= _iInputMax)	return;
	InputNext();
	InputDisplay();
}

void PbEditName()
{
	LcdDisplay(_cEnterName, 0);
	InputDisplay();
	InputNext();
	_iInputType = INPUT_TYPE_CHAR_STR;
	_iInputMax = MAX_USER_NAME_LEN - 1;
}

void PbEditNumber()
{
	LcdDisplay(_cEnterNumber, 0);
	InputDisplay();
	InputNext();
	_iInputType = INPUT_TYPE_CHAR_STR;
	_iInputMax = MAX_USER_NUMBER_LEN - 1;
}

void OnPbAddName()
{
	_iSub3MenuItem = EDIT_PB_ADD_NAME;
	ClearInputBuf();
	PbEditName();
}

void OnPbAddNumber()
{
	_iSub3MenuItem = EDIT_PB_ADD_NUMBER;
	ClearInputBuf();
	PbEditNumber();
}

void OnPbEditName()
{
	_iSub3MenuItem = EDIT_PB_EDIT_NAME;
	PbGetName(_pInputBuf);
	_iInputPos = strlen(_pInputBuf);
	PbEditName();
}

void OnPbEditNumber()
{
	_iSub3MenuItem = EDIT_PB_EDIT_NUMBER;
	PbGetNumber(_pInputBuf);
	_iInputPos = strlen(_pInputBuf);
	PbEditNumber();
}

/*******************************************************************
 *********************   UI Event Handler  *************************
 *******************************************************************
*/

BOOLEAN IsDigitInput(UCHAR iType)
{
	if (iType >= UI_EVENT_DIGIT0 && iType <= UI_EVENT_POUND)
	{
		return TRUE;
	}
	return FALSE;
}

void PopupSavePb()
{
	_iMenuType = MENU_TYPE_SAVEPB;
	LcdDisplay(_cSavePhonebook, 0);
	FixedDisplay(_cYesOrCancel);
#ifdef OEM_IP20
	FixedDispTwo(_cYesOrCancel);
	FixedDispThree(_cCiKaydet);
#endif
}

void PopupSaveSettings()
{
	_iMenuType = MENU_TYPE_SAVECFG;
	LcdDisplay(_cSaveSettings, 0);
	FixedDisplay(_cYesOrCancel);
#ifdef OEM_IP20
	FixedDispTwo(_cYesOrCancel);
	FixedDispThree(_cCiKaydet);
#endif
}

void MenuHandleTimer()
{
	if (_bScrollDisplay)
	{
		ScrollDisplayTimer();
	}
	_iMenuTimer ++;
	if (_iMenuTimer >= MENU_TIMEOUT)
	{
		TaskUIHandler(UI_MENU_EXIT, 0);			
	}
}

void MenuHandler(UCHAR iType)
{
	if (iType == UI_EVENT_TIMER)
	{
#ifdef SYS_TEST
		if (_iMenuType == MENU_TYPE_TEST)
		{
			TaskUIHandler(UI_TEST_PHONE, TEST_ITEM_TIMER);
		}
		else
#endif
		{
			MenuHandleTimer();
		}
		return;
	}
#ifdef SYS_TEST
	if (iType == UI_EVENT_TESTDONE)
	{
		if (_iMenuType == MENU_TYPE_TEST)
		{
			_iMenuType = MENU_TYPE_SUB1;
			DisplayTestMenu();
		}
		return;
	}
#endif	
	if (iType == UI_EVENT_MENU)
	{
		_bPhonebookChanged = FALSE;
		_bSettingsChanged = FALSE;
		_iMenuTimer = 0;
		_iMainMenuItem = 0;
		_iMenuType = MENU_TYPE_MAIN;
		DisplayMainMenu();
		return;
	}

	if (iType == UI_EVENT_CALL)
	{
		_bPhonebookChanged = FALSE;
		_bSettingsChanged = FALSE;
		_iMenuTimer = 0;
		_iMenuType = MENU_TYPE_SUB1;
		_iSub1MenuItem = 0;
		_iMainMenuItem = MAIN_MENU_CALL;
		ReadFlashPage(SYSTEM_SETTINGS_PAGE);
		DisplayCallMenu();
		return;
	}

	_iMenuTimer = 0;
	if (_iMenuType == MENU_TYPE_SAVEPB)
	{
		if (iType == UI_EVENT_OK || iType == UI_EVENT_CANCEL)
		{
			if (iType == UI_EVENT_OK)
			{
				WriteSram2Flash();
			}
			_bPhonebookChanged = FALSE;
			_iMenuType = MENU_TYPE_MAIN;
			DisplayMainMenu();
		}
		return;
	}

	if (_iMenuType == MENU_TYPE_SAVECFG)
	{
		if (iType == UI_EVENT_OK)
		{
			WriteSram2Flash();
			UdpDebugString("rebooting ...");
			FlashReboot();
		}
		else if (iType == UI_EVENT_CANCEL)
		{
			_bSettingsChanged = FALSE;
			_iMenuType = MENU_TYPE_MAIN;
			DisplayMainMenu();
		}
		return;
	}

#ifdef OEM_IP20
	if (_iMenuType == MENU_TYPE_ALR)
	{
		if (iType == UI_EVENT_OK)
		{
			//	LcdDisplay("all",2);
			sampleExec();
		}
		else if (iType == UI_EVENT_CANCEL)
		{
			_iMenuType = MENU_TYPE_SUB1;
			DisplaySettingsMenu();
		}
		return;
	}	
#endif

	if (_iMenuType == MENU_TYPE_LOADDFT || _iMenuType == MENU_TYPE_STOREDFT)
	{
		if (iType == UI_EVENT_CANCEL)
		{
			_iMenuType = MENU_TYPE_SUB1;
			DisplaySettingsMenu();
		}
		else if (iType == UI_EVENT_OK)
		{
			TaskUIHandler((_iMenuType == MENU_TYPE_LOADDFT) ? UI_LOAD_DEFAULTS : UI_STORE_DEFAULTS, 0);
		}
		return;
	}

	if (_iMenuType == MENU_TYPE_EDIT)
	{
		if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_OK)
		{
			LcdCursor(1, 0, FALSE);
#ifdef UI_4_LINES
			LcdDisplay("", 2);
#endif
			switch (_iSub3MenuItem)
			{
			case EDIT_PB_ADD_NAME:
				if (iType == UI_EVENT_CANCEL)
				{
					_iMenuType = MENU_TYPE_SUB1;
					DisplayPhonebook();
				}
				else
				{
					_iOldSub1Item = _iSub1MenuItem;
					_iSub1MenuItem = PbGetNew();
					PbSaveName(_iSub1MenuItem);
					OnPbAddNumber();
				}
				break;

			case EDIT_PB_EDIT_NAME:
				if (iType == UI_EVENT_CANCEL)
				{
					_iMenuType = MENU_TYPE_SUB1;
					DisplayPhonebook();
				}
				else
				{
					PbSaveName(_iSub1MenuItem);
					OnPbEditNumber();
				}
				break;

			case EDIT_PB_ADD_NUMBER:
				if (iType == UI_EVENT_OK)
				{
					PbSaveNumber(_iSub1MenuItem);
				}
				_iMenuType = MENU_TYPE_SUB1;
				if (!PbIsValid(_iSub1MenuItem))
				{
					_iSub1MenuItem = _iOldSub1Item;
				}
				DisplayPhonebook();
				break;

			case EDIT_PB_EDIT_NUMBER:
				if (iType == UI_EVENT_OK)
				{
					PbSaveNumber(_iSub1MenuItem);
				}
				if (!PbIsValid(_iSub1MenuItem))
				{
					_iSub1MenuItem = PbGetNext(_iSub1MenuItem);
				}
				_iMenuType = MENU_TYPE_SUB1;
				DisplayPhonebook();
				break;

			case EDIT_PHONE_SETTINGS:
				if (iType == UI_EVENT_OK)
				{
					_bSettingsChanged = TRUE;
					OnEditSettingsOK();
				}
				_iMenuType = MENU_TYPE_SUB2;
				DisplayOptionsItem();
				break;
			}
			return;
		}

		if (IsDigitInput(iType))
		{
			if (_iInputType == INPUT_TYPE_DIGIT_STR)
			{
				OnInputDigitStr(iType);
			}
			else if (_iInputType == INPUT_TYPE_CHAR_STR)
			{
				OnInputCharStr(iType);
			}
			else if (_iInputType == INPUT_TYPE_IP_STR)
			{
				OnInputIPStr(iType);
			}
			else if (_iInputType == INPUT_TYPE_INT_STR)
			{
				OnInputIntStr(iType);
			}
			else if (_iInputType == INPUT_TYPE_SELECT)
			{
				OnEditSettingsSelect(iType);
			}
			return;
		}
		if (iType == UI_EVENT_LEFT)
		{
			if (_iInputType <= INPUT_TYPE_STRING)
			{
				OnEditCancel();
			}
			return;
		}
		if (iType == UI_EVENT_RIGHT)
		{
			if (_iInputType <= INPUT_TYPE_STRING)
			{
				OnCursorRight();
			}
			return;
		}
		if (iType == UI_EVENT_UP)
		{
			if (_iInputType == INPUT_TYPE_SELECT)
			{
				OnEditSettingsSelect(iType);
			}
#if defined VER_AR168F || defined OEM_IP20 // 7x2 ADC key, up as cancel
			else if (_iInputType <= INPUT_TYPE_STRING)
			{
				OnEditCancel();
			}
#endif
			return;
		}
		if (iType == UI_EVENT_DOWN)
		{
			if (_iInputType == INPUT_TYPE_SELECT)
			{
				OnEditSettingsSelect(iType);
			}
#if defined VER_AR168F || defined OEM_IP20	// 7x2 ADC key, up down right
			else if (_iInputType <= INPUT_TYPE_STRING)
			{
				OnCursorRight();
			}
#endif
			return;
		}
		return;
	}

	if (IsDigitInput(iType))	return;

	if (_iMenuType == MENU_TYPE_MAIN)
	{
		if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
		{
			TaskUIHandler(UI_MENU_EXIT, 0);			
			return;
		}
		if (iType == UI_EVENT_DOWN)
		{
			_iMainMenuItem = ScrollDown(_iMainMenuItem, MAIN_MENU_MAX);
			DisplayMainMenu();
			return;
		}
		if (iType == UI_EVENT_UP)
		{
			_iMainMenuItem = ScrollUp(_iMainMenuItem, MAIN_MENU_MAX);
			DisplayMainMenu();
			return;
		}
		if (iType == UI_EVENT_OK)
		{
			_iMenuType = MENU_TYPE_SUB1;
			_iSub1MenuItem = 0;
			switch (_iMainMenuItem)
			{
			case MAIN_MENU_PHONEBOOK:
				ReadFlashPage(SYSTEM_SETTINGS_PAGE);
				_iSub1MenuItem = PbGetHead();
				DisplayPhonebook();
				break;

			case MAIN_MENU_CALL:
				ReadFlashPage(SYSTEM_SETTINGS_PAGE);
				DisplayCallMenu();
				break;

			case MAIN_MENU_SETTINGS:
				ReadFlashPage(SYSTEM_SETTINGS_PAGE);
				TaskSystemHandler(SYS_OPTIONS_SET_NETWORK, 0);
				DisplaySettingsMenu();
				break;

			case MAIN_MENU_INFO:
				DisplayInfoMenu();
				break;
#ifdef SYS_TEST
			case MAIN_MENU_TEST:
				DisplayTestMenu();
				break;
#endif
			}
			return;
		}
		return;
	}

	if (_iMenuType == MENU_TYPE_SUB1)
	{
		switch (_iMainMenuItem)
		{
		case MAIN_MENU_PHONEBOOK:
			if (iType == UI_EVENT_UP)
			{
				_iSub1MenuItem = (_iSub1MenuItem < PHONEBOOK_MENU_MAX) ? PbGetPrev(_iSub1MenuItem) : PbGetTail();
				DisplayPhonebook();
			}
			else if (iType == UI_EVENT_DOWN)
			{
				_iSub1MenuItem = (_iSub1MenuItem < PHONEBOOK_MENU_MAX) ? PbGetNext(_iSub1MenuItem) : PbGetHead();
				DisplayPhonebook();
			}
			else if (iType == UI_EVENT_OK)
			{
				_iMenuType = MENU_TYPE_SUB2;
				_iSub2MenuItem = (_iSub1MenuItem < PHONEBOOK_MENU_MAX) ? PB_ACTION_MENU_CALL : PB_ACTION_MENU_ADD;
				DisplayPbActionMenu();
			}
			else if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				if (_bPhonebookChanged)
				{
					PopupSavePb();
				}
				else
				{
					_iMenuType = MENU_TYPE_MAIN;
					DisplayMainMenu();
				}
			}
			break;

		case MAIN_MENU_CALL:
			if (iType == UI_EVENT_UP)
			{
				_iSub1MenuItem = ScrollUp(_iSub1MenuItem, CALL_MENU_MAX);
				DisplayCallMenu();
			}
			else if (iType == UI_EVENT_DOWN)
			{
				_iSub1MenuItem = ScrollDown(_iSub1MenuItem, CALL_MENU_MAX);
				DisplayCallMenu();
			}
			else if (iType == UI_EVENT_OK)
			{
				_iMenuType = MENU_TYPE_SUB2;
				_iSub2MenuItem = 0;
				DisplayCallLogItem();
			}
			else if (iType == UI_EVENT_CANCEL  || iType == UI_EVENT_LEFT)
			{
				if (_bPhonebookChanged)
				{
					PopupSavePb();
				}
				else
				{
					_iMenuType = MENU_TYPE_MAIN;
					DisplayMainMenu();
				}
			}
			break;

		case MAIN_MENU_SETTINGS:
			if (iType == UI_EVENT_UP)
			{
				_iSub1MenuItem = ScrollUp(_iSub1MenuItem, SETTINGS_MENU_MAX);
				DisplaySettingsMenu();
			}
			else if (iType == UI_EVENT_DOWN)
			{
				_iSub1MenuItem = ScrollDown(_iSub1MenuItem, SETTINGS_MENU_MAX);
				DisplaySettingsMenu();
			}
			else if (iType == UI_EVENT_OK)
			{
				if (_iSub1MenuItem < SETTINGS_MENU_LOADDFT)
				{
					_iMenuType = MENU_TYPE_SUB2;
					_iSub2MenuItem = 0;
					DisplayOptionsItem();
				}
				else
				{
#ifdef OEM_IP20
					if (_iSub1MenuItem == SETTINGS_MENU_LOADDFT)
					{
						_iMenuType = MENU_TYPE_LOADDFT;
					}
					else if (_iSub1MenuItem == SETTINGS_MENU_STOREDFT)
					{
						_iMenuType = MENU_TYPE_STOREDFT;
					}
					else
					{
						_iMenuType = MENU_TYPE_ALR;
						DisplayOptionsItem();
						break;
					}
					LcdDisplay(_cConfirm, 0);
					FixedDisplay(_cYesOrCancel);
					FixedDispThree(_cYesNo);

#else
					_iMenuType = (_iSub1MenuItem == SETTINGS_MENU_LOADDFT) ? MENU_TYPE_LOADDFT : MENU_TYPE_STOREDFT;
					LcdDisplay(_cConfirm, 0);
					FixedDisplay(_cYesOrCancel);
#endif
				}
			}
			else if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				if (_bSettingsChanged)
				{
					PopupSaveSettings();
				}
				else
				{
					_iMenuType = MENU_TYPE_MAIN;
					DisplayMainMenu();
				}
			}
			break;

		case MAIN_MENU_INFO:
			if (iType == UI_EVENT_UP)
			{
				_iSub1MenuItem = ScrollUp(_iSub1MenuItem, INFO_MENU_MAX);
				DisplayInfoMenu();
			}
			else if (iType == UI_EVENT_DOWN)
			{
				_iSub1MenuItem = ScrollDown(_iSub1MenuItem, INFO_MENU_MAX);
				DisplayInfoMenu();
			}
			else if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				_iMenuType = MENU_TYPE_MAIN;
				DisplayMainMenu();
			}
			break;

#ifdef SYS_TEST
		case MAIN_MENU_TEST:
			if (iType == UI_EVENT_UP)
			{
				_iSub1MenuItem = ScrollUp(_iSub1MenuItem, TEST_MENU_MAX);
				DisplayTestMenu();
			}
			else if (iType == UI_EVENT_DOWN)
			{
				_iSub1MenuItem = ScrollDown(_iSub1MenuItem, TEST_MENU_MAX);
				DisplayTestMenu();
			}
			else if (iType == UI_EVENT_OK)
			{
				_iMenuType = MENU_TYPE_TEST;
				TaskUIHandler(UI_TEST_PHONE, _iSub1MenuItem);
			}
			else if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				_iMenuType = MENU_TYPE_MAIN;
				DisplayMainMenu();
			}
			break;
#endif
		}
		return;
	}

	if (_iMenuType == MENU_TYPE_SUB2)
	{
		switch (_iMainMenuItem)
		{
		case MAIN_MENU_PHONEBOOK:
			if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				_iMenuType = MENU_TYPE_SUB1;
				DisplayPhonebook();
				return;
			}
			switch (_iSub2MenuItem)
			{
			case PB_ACTION_MENU_CALL:
				if (iType == UI_EVENT_UP)
				{
					_iSub2MenuItem = PB_ACTION_MENU_DELALL;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_DOWN)
				{
					_iSub2MenuItem = PB_ACTION_MENU_EDIT;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_OK)
				{
					PbGetNumber(_pInputBuf);
					TaskUIHandler(UI_MENU_CALL, (USHORT)_pInputBuf);
				}
				break;

			case PB_ACTION_MENU_EDIT:
				if (iType == UI_EVENT_UP)
				{
					_iSub2MenuItem = PB_ACTION_MENU_CALL;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_DOWN)
				{
					_iSub2MenuItem = PB_ACTION_MENU_DELETE;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_OK)
				{
					_iMenuType = MENU_TYPE_EDIT;
					OnPbEditName();
				}
				break;

			case PB_ACTION_MENU_DELETE:
				if (iType == UI_EVENT_UP)
				{
					_iSub2MenuItem = PB_ACTION_MENU_EDIT;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_DOWN)
				{
					_iSub2MenuItem = PB_ACTION_MENU_ADD;
					DisplayPbActionMenu();
				}
				if (iType == UI_EVENT_OK)
				{
					PbDelete(_iSub1MenuItem);
					_iMenuType = MENU_TYPE_SUB1;
					_iSub1MenuItem = PbGetNext(_iSub1MenuItem);
					DisplayPhonebook();
				}
				break;

			case PB_ACTION_MENU_ADD:
				if (iType == UI_EVENT_UP)
				{
					_iSub2MenuItem = (_iSub1MenuItem == PHONEBOOK_MENU_MAX) ? PB_ACTION_MENU_DELALL : PB_ACTION_MENU_DELETE;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_DOWN)
				{
					_iSub2MenuItem = PB_ACTION_MENU_DELALL;
					DisplayPbActionMenu();
				}
				if (iType == UI_EVENT_OK)
				{
					if (PbGetNum() < PHONEBOOK_ENTRY_NUM)
					{
						_iMenuType = MENU_TYPE_EDIT;
						OnPbAddName();
					}
				}
				break;

			case PB_ACTION_MENU_DELALL:
				if (iType == UI_EVENT_UP)
				{
					_iSub2MenuItem = PB_ACTION_MENU_ADD;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_DOWN)
				{
					_iSub2MenuItem = (_iSub1MenuItem == PHONEBOOK_MENU_MAX) ? PB_ACTION_MENU_ADD : PB_ACTION_MENU_CALL;
					DisplayPbActionMenu();
				}
				else if (iType == UI_EVENT_OK)
				{
					PbDeleteAll();
					_iMenuType = MENU_TYPE_SUB1;
					_iSub1MenuItem = PHONEBOOK_MENU_MAX;
					DisplayPhonebook();
				}
				break;
			}
			break;

		case MAIN_MENU_CALL:
			if (iType == UI_EVENT_UP)
			{
				if (_iSub2MenuItem)
				{
					_iSub2MenuItem --;
				}
				else
				{
					_iSub2MenuItem = CallLogGetNum(_iSub1MenuItem) - 1;
				}
				DisplayCallLogItem();
			}
			else if (iType == UI_EVENT_DOWN)
			{
				_iSub2MenuItem ++;
				if (_iSub2MenuItem == CallLogGetNum(_iSub1MenuItem))
				{
					_iSub2MenuItem = 0;
				}
				DisplayCallLogItem();
			}
			else if (iType == UI_EVENT_OK)
			{
				DisplayCallLogActionMenu();
			}
			else if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				_iMenuType = MENU_TYPE_SUB1;
				DisplayCallMenu();
			}
			break;

		case MAIN_MENU_SETTINGS:
			if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				_iMenuType = MENU_TYPE_SUB1;
				DisplaySettingsMenu();
				return;
			}
			if (iType == UI_EVENT_OK)
			{

#ifdef OEM_IP20
				if(_iSub1MenuItem == SETTINGS_ALARM)
				{
					sampleExec();
					//LcdDisplay("alarm1",1);
				}				
				else				
				{	
					_iMenuType = MENU_TYPE_EDIT;
					_iSub3MenuItem = EDIT_PHONE_SETTINGS;
					OnEditSettings();
					InputNext();
					InputDisplay();
  				}
#else
				_iMenuType = MENU_TYPE_EDIT;
				_iSub3MenuItem = EDIT_PHONE_SETTINGS;
				OnEditSettings();
				InputNext();
				InputDisplay();
#endif
				return;
			}
			if (iType == UI_EVENT_UP)
			{
				if (_iSub1MenuItem <= SETTINGS_MENU_SYSTEM)
				{
					_iSub2MenuItem = ScrollUp(_iSub2MenuItem, _cSettingsSubMenuMax[_iSub1MenuItem]);
					DisplayOptionsItem();
				}
				return;
			}
			if (iType == UI_EVENT_DOWN)
			{
				if (_iSub1MenuItem <= SETTINGS_MENU_SYSTEM)
				{
					_iSub2MenuItem = ScrollDown(_iSub2MenuItem, _cSettingsSubMenuMax[_iSub1MenuItem]);
					DisplayOptionsItem();
				}
				return;
			}
			break;
		}
		return;
	}

	if (_iMenuType == MENU_TYPE_SUB3)
	{
		if (_iMainMenuItem == MAIN_MENU_CALL)
		{
			if (iType == UI_EVENT_UP)
			{
				_iSub3MenuItem = ScrollUp(_iSub3MenuItem, CH_ACTION_MENU_MAX);
#ifdef OEM_IP20
				LcdDisplay(_cChActionMenu[_iSub3MenuItem], 1);
#else
				LcdDisplay(_cChActionMenu[_iSub3MenuItem], 0);
#endif
			}
			else if (iType == UI_EVENT_DOWN)
			{
				_iSub3MenuItem = ScrollDown(_iSub3MenuItem, CH_ACTION_MENU_MAX);
#ifdef OEM_IP20
				LcdDisplay(_cChActionMenu[_iSub3MenuItem], 1);
#else
				LcdDisplay(_cChActionMenu[_iSub3MenuItem], 0);
#endif
			}
			else if (iType == UI_EVENT_OK)
			{
				switch (_iSub3MenuItem)
				{
				case CH_ACTION_MENU_CALL:
					CallLogCall();
					break;
		
				case CH_ACTION_MENU_ADD:
					CallLogAddToPb();
					break;

				case CH_ACTION_MENU_DELALL:
					CallLogDeleteAll(_iSub1MenuItem);
					_iMenuType = MENU_TYPE_SUB1;
					DisplayCallMenu();
					break;
				}
			}
			else if (iType == UI_EVENT_CANCEL || iType == UI_EVENT_LEFT)
			{
				_iMenuType = MENU_TYPE_SUB2;
				DisplayCallLogItem();
			}
		}
	}
}

#endif	// SYS_LCD

/*-------------------------------------------------------------------------
   AR1688 User Interface function copy right information

   Copyright (c) 2006-2013. Tang, Li      <tangli@palmmicro.com>
                            Lin, Rongrong <woody@palmmicro.com>
						    Li, Yajing    <liyajing@palmmicro.com>
                            Alex Gradinar <Alex.Gradinar@cartel.md>
                            Pedram J. Katebi <pedram@itechfrontiers.com>

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
#include "sfr.h"
#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"
//#include "tcpip.h"
#include "bank1.h"

#define MAX_KEYBUF_LEN	(DISPLAY_MAX_CHAR*2)
#define DISPLAY_TTL		3
#define DISPLAY_TTL_CALL_MSG		20 /* Added additional TTL Display for duration of the text message received during a call */

UCHAR Sys_iCurMode;
UCHAR Sys_iMissedCalls;
BOOLEAN Sys_bDnd;

UCHAR _pKeyBuf[MAX_KEYBUF_LEN+1];
UCHAR _pRedialBuf[MAX_KEYBUF_LEN+1];

BOOLEAN _bAutoAnswer;

BOOLEAN _bSntpOK;
BOOLEAN _bLogoned;
UCHAR _iNewMsg;
UCHAR _iOldMsg;

BOOLEAN _bHandsetUp;
BOOLEAN _bSpeaker;

BOOLEAN _bMute;
UCHAR _iDisplayTimer;
UCHAR _iKeyTimer;

#ifdef OEM_BT2008
#define ALARM_STATE_IDLE		0
#define ALARM_STATE_ENTER_TIME	1
#define ALARM_STATE_RINGING		2

UCHAR _iRedialTimer;
UCHAR _iRingVol;
UCHAR _iLastRecordTimer;
UCHAR _pLastRecord[MAX_USER_NUMBER_LEN];
UCHAR _iAlarmHour;
UCHAR _iAlarmMin;
UCHAR _iAlarmState;
UCHAR _pAlarmBuf[6];
UCHAR _iAlarmPos;
UCHAR _iAlarmTimer;
#endif

#ifdef OEM_IP20
#define ALARM_STATE_IDLE		0
#define ALARM_STATE_ENTER_TIME	1
#define ALARM_STATE_SAY_TIME	2
#define ALARM_STATE_RINGING		3

UCHAR _iAlarmHour;
UCHAR _iAlarmMin;
UCHAR _iAlarmState;
UCHAR _pAlarmBuf[6];
UCHAR _iAlarmPos;
UCHAR _iAlarmTimer;
#endif

#ifdef SYS_TEST
UCHAR _iTestItem;
UCHAR _iTestStep;
#endif

#ifdef VER_FWV2800
BOOLEAN _bOnBattery;
#endif
#ifdef OEM_IP20
UCHAR hotline;
UCHAR Reddet;
UCHAR Sessiz;
UCHAR Kapat;

void _display_ascii(UCHAR iChar, UCHAR iPos, UCHAR iLine);
UCHAR refresh_sym;

BOOLEAN Not_refresh;

UCHAR _pAutoCallBuf;
UCHAR call_stat;
UCHAR _pKeyBufy[MAX_KEYBUF_LEN+1];
BOOLEAN Interdigit;

BOOLEAN Test;
BOOLEAN Test1;
BOOLEAN Test2;
UCHAR Test_Result;
UCHAR sum1;
UCHAR sum2;

extern UCHAR TransferFlag;
extern UCHAR TransFlagResp;

UCHAR voice_message;

extern UCHAR atLine3;
#endif

UCHAR Sys_pMacAddress[HW_ALEN];			// mac address, 6 bytes
UCHAR Sys_iIpType;						// ip type, 1 byte 0[static] 1[dhcp]
UCHAR Sys_pIpAddress[IP_ALEN];			// ip address, 4 bytes
UCHAR Sys_pSubnetMask[IP_ALEN];			// subnet mask, 4 bytes
UCHAR Sys_pRouterIp[IP_ALEN];			// router ip, 4 bytes
UCHAR Sys_pDnsIp[IP_ALEN];				// dns ip, 4 bytes
UCHAR Sys_pDnsIp2[IP_ALEN];				// dns ip 2, 4 bytes
UCHAR Sys_pLogIp[IP_ALEN];				// sys log ip, 4 bytes
#ifndef CALL_NONE
BOOLEAN Sys_bRegister;
UCHAR Sys_iTimeZone;
BOOLEAN Sys_bDayLight;
UCHAR Sys_iDtmfType;
UCHAR Sys_iDigitmapTimeout;
BOOLEAN Sys_bUseDigitmap;
BOOLEAN Sys_bFwdAlways;
BOOLEAN Sys_bFwdBusy;
BOOLEAN Sys_bFwdNoanswer;
BOOLEAN Sys_bCallWaiting;
BOOLEAN Sys_bAutoAnswer;
UCHAR Sys_iNoanswerTimeout;
#ifdef CALL_SIP
BOOLEAN Sys_bOutboundProxy;
BOOLEAN Sys_bPrack;
UCHAR Sys_iDtmfPayload;
UCHAR Sys_pSipDomain[MAX_DNS_LEN];
UCHAR Sys_pSipProxy[MAX_DNS_LEN];
UCHAR Sys_pProxyRequire[MAX_DNS_LEN];
#endif
USHORT Sys_sRegisterTtl;
USHORT Sys_sLocalPort;
USHORT Sys_sRtpPort;
UCHAR Sys_pAuthID[MAX_AUTH_ID_LEN];
UCHAR Sys_pAuthPin[MAX_AUTH_PIN_LEN];
UCHAR Sys_pUserNumber[MAX_USER_NUMBER_LEN];
UCHAR Sys_pUserName[MAX_USER_NAME_LEN];
#endif

BOOLEAN Sys_bOptVad;
UCHAR Sys_iSpeexRate;
UCHAR Sys_iIlbcFrame;
UCHAR Sys_iRingVolOutput;
UCHAR Sys_iSpeakerVol;
UCHAR Sys_iHandsetVol;
UCHAR Sys_pVoiceCoders[VOICE_CODER_NUM];

#ifdef OEM_ROIP
UCHAR Sys_iPttControl;
#endif

UCHAR Rtp_pPayLoad[VOICE_CODER_NUM + 1];
const UCHAR _cPayLoad[VOICE_CODER_NUM + 1] = {RTP_NONE, RTP_ULAW, RTP_ALAW, RTP_G726_32, RTP_GSM, RTP_ILBC, RTP_SPEEX, RTP_G729};

void _LoadOptions()
{
	UCHAR i;

#ifndef CALL_NONE
	OptionsGetBuf(Sys_pMacAddress, OPT_MAC_ADDRESS, HW_ALEN);
	Sys_iIpType = OptionsGetByte(OPT_IP_TYPE);
	OptionsGetBuf(Sys_pIpAddress, OPT_IP_ADDRESS, IP_ALEN);
	OptionsGetBuf(Sys_pSubnetMask, OPT_SUBNET_MASK, IP_ALEN);
	OptionsGetBuf(Sys_pRouterIp, OPT_ROUTER_IP, IP_ALEN);
	OptionsGetBuf(Sys_pDnsIp, OPT_DNS_IP, IP_ALEN);
	OptionsGetBuf(Sys_pDnsIp2, OPT_DNS_IP2, IP_ALEN);
	OptionsGetBuf(Sys_pLogIp, OPT_SYSLOG_IP, IP_ALEN);
	Sys_bRegister = OptionsGetByte(OPT_REGISTER) ? TRUE : FALSE;
	Sys_iTimeZone = OptionsGetByte(OPT_TIME_ZONE);
	Sys_bDayLight = OptionsGetByte(OPT_DAY_LIGHT) ? TRUE : FALSE;
	Sys_iDtmfType = OptionsGetByte(OPT_DTMF_TYPE);
	Sys_iDigitmapTimeout = OptionsGetByte(OPT_DIGITMAP_TIMEOUT);
	Sys_bUseDigitmap = OptionsGetByte(OPT_USE_DIGITMAP) ? TRUE : FALSE;
	Sys_bFwdAlways = OptionsGetByte(OPT_FWD_ALWAYS) ? TRUE : FALSE;
	Sys_bFwdBusy = OptionsGetByte(OPT_FWD_BUSY) ? TRUE : FALSE;
	Sys_bFwdNoanswer = OptionsGetByte(OPT_FWD_NOANSWER) ? TRUE : FALSE;
	Sys_bCallWaiting = OptionsGetByte(OPT_CALL_WAITING) ? TRUE : FALSE;
	Sys_bAutoAnswer = OptionsGetByte(OPT_AUTO_ANSWER) ? TRUE : FALSE;
	Sys_iNoanswerTimeout = OptionsGetByte(OPT_NOANSWER_TIMEOUT);
#ifdef CALL_SIP
	Sys_bOutboundProxy = OptionsGetByte(OPT_OUTBOUND_PROXY) ? TRUE : FALSE;
	Sys_bPrack = OptionsGetByte(OPT_PRACK) ? TRUE : FALSE;
	Sys_iDtmfPayload = OptionsGetByte(OPT_DTMF_PAYLOAD);
	OptionsGetString(Sys_pSipDomain, OPT_SIP_DOMAIN, MAX_DNS_LEN);
	OptionsGetString(Sys_pSipProxy, OPT_SIP_PROXY, MAX_DNS_LEN);
	OptionsGetString(Sys_pProxyRequire, OPT_PROXY_REQUIRE, MAX_DNS_LEN);
#endif
	Sys_sRegisterTtl = OptionsGetShort(OPT_REGISTER_TTL);
	Sys_sLocalPort = OptionsGetShort(OPT_LOCAL_PORT);
	Sys_sRtpPort = OptionsGetShort(OPT_RTP_PORT);

	OptionsGetString(Sys_pAuthID, OPT_AUTH_ID, MAX_AUTH_ID_LEN);
	OptionsGetString(Sys_pAuthPin, OPT_AUTH_PIN, MAX_AUTH_PIN_LEN);
	OptionsGetString(Sys_pUserNumber, OPT_USER_NUMBER, MAX_USER_NUMBER_LEN);
	OptionsGetString(Sys_pUserName, OPT_USER_NAME, MAX_USER_NAME_LEN);
#endif

	Sys_bOptVad = OptionsGetByte(OPT_VAD) ? TRUE : FALSE;
	Sys_iSpeexRate = OptionsGetByte(OPT_SPEEX_RATE);
	Sys_iIlbcFrame = OptionsGetByte(OPT_ILBC_FRAME) ? 1 : 0;
	Sys_iRingVolOutput = OptionsGetByte(OPT_RING_VOL_OUTPUT);
	if (Sys_iRingVolOutput < 10)	Sys_iRingVolOutput = 10;
	Sys_iSpeakerVol = OptionsGetByte(OPT_SPEAKER_VOL);
	Sys_iHandsetVol = OptionsGetByte(OPT_VOL_OUTPUT);
	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		Sys_pVoiceCoders[i] = OptionsGetByte(OPT_VOICE_CODER0 + i);
	}

#ifdef OEM_ROIP
	Sys_iPttControl = OptionsGetByte(OPT_PTT_CONTROL);
#endif

	memcpy(Rtp_pPayLoad, _cPayLoad, VOICE_CODER_NUM + 1);
#ifdef CALL_SIP
	i = OptionsGetByte(OPT_ADPCM32_PAYLOAD);
	if (i > RTP_REDUNDENCE && i < RTP_LAST)		Rtp_pPayLoad[VOICE_CODER_G726_32] = i;
	i = OptionsGetByte(OPT_ILBC_PAYLOAD);
	if (i > RTP_REDUNDENCE && i < RTP_LAST)		Rtp_pPayLoad[VOICE_CODER_ILBC] = i;
	i = OptionsGetByte(OPT_SPEEX_PAYLOAD);
	if (i > RTP_REDUNDENCE && i < RTP_LAST)		Rtp_pPayLoad[VOICE_CODER_SPEEX] = i;
#endif
}

#ifdef OEM_IP20
UCHAR displayOnlineMissed(PCHAR ptr);

void DisplayLogStandby()
{
	UCHAR pStr[32];
	
	if (displayOnlineMissed(&pStr[0]))
	{
		LcdDisplay(pStr, 2);
	}
	else
	{
		DisplayClear(1);	
	}
}
#endif

void KeyClear()
{
	_pKeyBuf[0] = 0;
}

#ifdef VER_AR168R

void SpeakerPhone(BOOLEAN bHandfree)
{
	bHandfree;
	CodecMicMute(_bMute);
}

#else

void SpeakerPhone(BOOLEAN bHandfree)
{
	if (bHandfree)
	{
		SpeakerRing();
#ifdef OEM_BT2008
		CodecSetVol(OptionsGetByte(OPT_SPEAKER_VOL));
#else
		CodecSetVol(Sys_iSpeakerVol);	// set speaker vol
#endif
		_bSpeaker = TRUE;
	}
	else
	{
		SpeakerOff();

#ifdef OEM_BT2008
		CodecSetVol(OptionsGetByte(OPT_VOL_OUTPUT));	// set handset vol
#else
		CodecSetVol(Sys_iHandsetVol);	// set handset vol
#endif
		_bSpeaker = FALSE;
	}

	CodecMicMute(_bMute);
}
#endif

#if defined OEM_BT2008 || defined OEM_IP20
void UI_AlarmInit()
{
	_iAlarmState = ALARM_STATE_IDLE;
	_iAlarmHour = 25;
	_iAlarmMin = 61;
}

void UI_StopAlarm()
{
#if !defined SYS_PROGRAMMER && !defined CALL_NONE
	if (_iAlarmState == ALARM_STATE_ENTER_TIME)
	{
		_iAlarmState = ALARM_STATE_IDLE;
	}
	else if (_iAlarmState == ALARM_STATE_RINGING)
	{
		SpeakerPhone(FALSE);
		UI_AlarmInit();
	}
#endif
}
#endif

void UI_Init()
{
#ifdef SYS_CHIP_PLUS
//	SetDSP(31);	// 93MIPS
	SetDSP(24);	// 72MIPS
#else
//	SetDSP(2);	// 36MIPS
	SetDSP(3);	// 48MIPS
//	SetDSP(4);	// 60MIPS
#endif

#ifdef CALL_NONE
	SetNormalSpeed();
#else

	// try add your own hardware version here, see if it can run high speed or not

	// some old chip may not run high speed, try how lucky your device is here
//#ifdef VER_GP1266
//#if defined VER_AR168G || defined VER_YWH201 || defined VER_GF302 || defined VER_AR168J	
//	SetHighSpeed();
//#endif

//		SetZ80Speed(7);		// 48Mhz

/*
#ifdef SYS_CHIP_PLUS
#ifndef MT28F016S5
	if (FlashQueryData(0, QUERY_MANUFACTURER_ID) == SST_ID)
	{
#if defined VER_AR168P || defined VER_AR168KM
		SetZ80Speed(5);		// 42Mhz, KSZ8842 interface max at 50Mhz
//#elif defined VER_GP2266 || defined VER_BT2008N
//		SetZ80Speed(11);	// 60Mhz
#else
		SetZ80Speed(7);		// 48Mhz
#endif
	}
	else
	{
		SetZ80Speed(2);		// 33Mhz
	}
#endif
#endif	// SYS_CHIP_PLUS
*/

#endif	// !CALL_NONE

#ifdef VER_AR168M
	// Use GPIO_B0 as Handfree mic control
	rGPIO_B_Config = 0;
#ifdef OEM_BT2008	// all B0/B2/B4/B5 output
	rGPIO_B_Output_Enable |= 0x35;	
	rGPIO_B_Input_Enable &= ~0x35;
#else
	rGPIO_B_Output_Enable |= 0x01;
	rGPIO_B_Input_Enable &= 0xfe;
#endif
#endif

#ifdef SERIAL_CARD_READER
	rGPIO_A |= 0x04; //A2 cardreader off
#endif

#ifdef VER_FWV2800
	// GPIO_D5 as POWER_DET input
	rGPIO_D_Input_Enable |= 0x20;	// GPIO_D5 input enable
	rGPIO_D &= ~0x20;	// high indicate to use battery
	// GPIO_C0 as BAT_L input
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xfe) | 0x10;		// GPIO_C0 input enable, 11111110, 00010000
	rGPIO_C &= ~0x01;	// low indicate battery low
	_bOnBattery = FALSE;
#endif

	_LoadOptions();

#ifndef CALL_NONE
#ifdef OEM_INNOMEDIA
//	Sys_pProvUrl = heap_save_str("192.168.1.250:80/config.txt"); //heap_save_str("http://172.16.1.92:8802/Provisioning/config.txt");
	Sys_pProvUrl = NULL;
#endif

	LED_Init();

	SetDog(255);		// disable watchdog for all device
/*
#ifdef VER_AR168W
	SetDog(3);			// enable watchdog, 5.6s
#else
	SetDog(255);		// disable watchdog
#endif
*/
#endif

	CodecSetInputVol(OptionsGetByte(OPT_VOL_INPUT));
	CodecSetVol(Sys_iHandsetVol);

#if defined VER_GF302 || defined VER_YWH201
	SideToneEnable(FALSE);
#else
	SideToneEnable(TRUE);
#endif

	_bSntpOK = FALSE;
	_bLogoned = FALSE;

	_pRedialBuf[0] = 0;

	_bSpeaker = FALSE;
	_bHandsetUp = FALSE;

	_bAutoAnswer = FALSE;
	_bMute = FALSE;

	_iNewMsg = 0;
	_iOldMsg = 0;

#ifdef OEM_BT2008
	_iRedialTimer = 0;
	_iRingVol = OptionsGetByte(OPT_GRADUAL_RINGING)? 1:0;
	_iLastRecordTimer = 0;
	_pLastRecord[0]=0;
	UI_AlarmInit();
#endif

#ifdef OEM_IP20
	//hotline = 1;
	Reddet = 0;
	Sessiz = 0;
	Kapat = 0;

	_pAutoCallBuf = OptionsGetByte(OPT_TIME_OUT);
	
	if (_pAutoCallBuf < 3) _pAutoCallBuf = 3;

	voice_message = 0;
#endif

	Sys_iCurMode = SYS_MODE_STANDBY;
	Sys_iMissedCalls = 0;
	Sys_bDnd = FALSE;

	_iDisplayTimer = 0;
	_iKeyTimer = 0;

	Sys_lCurrentTime = 0;
//	ConvertTime(Sys_lCurrentTime, &Sys_tmLocal);
	memset((void *)&Sys_tmLocal, 1, sizeof(Sys_tmLocal));
}

#ifndef CALL_NONE
void _ClearCallStatus()
{
	LED_Off(LED_HOLD);	
	LED_Off(LED_TRANSFER);	
	LED_Off(LED_MUTE);
	LED_Off(LED_RINGING);
}
#endif

void UI_Start()
{
	KeyStart();

#ifdef KEY_5X6
	Key5x6_Start();
#endif

#ifdef KEY_8x4
	Key8x4_Start();
#endif

#ifdef KEY_8X5
	Key8x5_Start();
#endif

#ifdef KEY_7X8
	Key7x8_Start();
#endif

#ifdef KEY_C2
	KeyC2_Start();
#endif

#ifdef HOOK_GPIO
	// GPIO C2 as hook
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xfb) | 0x40;	// 11111011, 01000000
	HookStart();
#endif	// HOOK_GPIO

#ifndef CALL_NONE

#ifdef VER_AR168R
	RoIP_Start();
#endif

	LcdStart();
#ifdef OEM_IP20
	DisplayString(IDS_KAREL, 1);	
#else
	DisplayString(IDS_VERSION, 0);
#endif

	_ClearCallStatus();
	LED_On(LED_MESSAGE);	
	LED_Off(LED_ACTIVE);
	LED_Off(LED_LOGON);
#if defined OEM_2KLABS || defined OEM_WEBNEED
	LED_On(LED_LCD);
#else
	LED_Off(LED_LCD);
#endif

#endif	// !CALL_NONE

	InterruptInit();
	DspInit();

#ifdef OEM_IP20
	IP20_LED_Off();
	Not_refresh = 1;
#endif
	SpeakerPhone(FALSE);
}


void UI_StoreDefaults()
{
	USHORT sAddr;
	UCHAR pBuf[256];

	UdpDebugString("Copy current settings to default settings");
	ReadFlashPage(SYSTEM_SETTINGS_PAGE);
	for (sAddr = 0; sAddr < SETTINGS_FILE_SIZE; sAddr += 256)
	{
		BufSeek(sAddr);
		BufRead(pBuf, 256);
		BufSeek(sAddr+OPT_DEFAULT_SETTINGS);
		BufWrite(pBuf, 256);
	}
	FlashWritePage(SYSTEM_SETTINGS_PAGE);
	FlashReboot();
}

void UI_LoadDefaults()
{
	USHORT sAddr;
	UCHAR pBuf[256];

	UdpDebugString("Restore default settings");
	ReadFlashPage(SYSTEM_SETTINGS_PAGE);
	for (sAddr = 0; sAddr < SETTINGS_FILE_SIZE; sAddr += 256)
	{
		BufSeek(sAddr+OPT_DEFAULT_SETTINGS);
		BufRead(pBuf, 256);
		BufSeek(sAddr);
		BufWrite(pBuf, 256);
	}
	SystemModifyPage(SYSTEM_SETTINGS_PAGE);
	FlashWritePage(SYSTEM_SETTINGS_PAGE);
	FlashReboot();
}

void UI_Timer()
{
	USHORT sVal;

#if !defined SYS_PROGRAMMER && !defined CALL_NONE
	UCHAR iState;
	UCHAR iDialTO, iKeyLen;

#ifdef VER_AR168R
	RoIP_Timer();
#endif

#ifdef VER_AR168W
	LED_Blink(LED_RINGING);
/*	if (rGPIO_G & 0x01)
	{
		UdpDebugString("GPIO_G0 HIGH");
	}
	else
	{
		UdpDebugString("GPIO_G0 LOW");
	}
*/
#endif

	if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_TIMER);
	}
	else
	{
#ifdef OEM_IP20
		if (Sys_iCurMode == SYS_MODE_ALARM)
		{
			if (_iAlarmState == ALARM_STATE_SAY_TIME || _iAlarmState == ALARM_STATE_ENTER_TIME)
			{
				_iAlarmTimer ++;
				if (_iAlarmTimer >= 10)
				{
					OnWakeOK();	
				}
			}
		}
#endif
		if (Sys_iCurMode == SYS_MODE_STANDBY)
		{
			if (_iNewMsg)
			{
				LED_Blink(LED_MESSAGE);	
			}
			else
			{
				LED_Off(LED_MESSAGE);
			}
#ifdef OEM_IP20
			if (call_stat > _pAutoCallBuf && _pKeyBuf[0]!=0 && Sys_iCurMode == SYS_MODE_STANDBY && Interdigit == 1)
			{
				strcpy (_pKeyBufy,_pKeyBuf);	
				//UI_PrepareCall();		
				UI_ChangeMode(SYS_MODE_CALL);
				TaskHandleEvent(UI_EVENT_HANDSET_UP, 1);
				SpeakerPhone(TRUE);
			
				strcpy(_pKeyBuf, _pKeyBufy);	
				TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
				call_stat = 0;
				KeyClear();
			}
			else if (call_stat < 32)
			{
				call_stat ++;
			}	
			else
			{
				call_stat = 0;
			}	

			if (Not_refresh == 0)
			{
				if (refresh_sym)
				{
					_display_ascii(' ', 17, 0);
					refresh_sym = 0;
				}
				else
				{
					 _display_ascii(':', 17, 0);
					refresh_sym = 1;		
				}
			}
#endif	// OEM_IP20
#ifdef OEM_BT2008
			if (_iAlarmState == ALARM_STATE_ENTER_TIME)
			{
				_iAlarmTimer ++;
				if (_iAlarmTimer >= 10)
				{
					OnWake();
				}
			}
#endif
			if (_iDisplayTimer)
			{
				_iDisplayTimer --;
				if (!_iDisplayTimer)
				{
					UI_StandbyDisplay();
				}
			}
		}
		else if (Sys_iCurMode == SYS_MODE_CALL)
		{
			iState = TaskGetCallInfo(CALL_INFO_STATE);
			if (iState == CALL_STATE_RINGING)
			{
				LED_Blink(LED_RINGING);
#ifdef OEM_BT2008
				LED_Blink(LED_MESSAGE);	
				if (_iRingVol)
				{
					if (_iRingVol < Sys_iRingVolOutput)
					{
						_iRingVol ++;
						CodecSetVol(_iRingVol);
					}
				}
#endif
			}
			else if (iState == CALL_STATE_DIALING)
			{
				_iKeyTimer ++;
				iKeyLen = strlen(_pKeyBuf);
				if (iKeyLen)
				{
					iDialTO = Sys_bUseDigitmap ? Sys_iDigitmapTimeout : DIAL_TIMEOUT_END;
					if (_iKeyTimer > iDialTO)
					{
						_iKeyTimer = 0;
						_pKeyBuf[iKeyLen] = 'T';
						_pKeyBuf[iKeyLen+1] = 0;
						if (TaskMapMatch(_pKeyBuf))
						{
							_pKeyBuf[iKeyLen] = 0;
							UI_BuildNumber(_pKeyBuf);
							TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
						}
					}
				}
				else
				{
					if (_iKeyTimer >= DIAL_TIMEOUT_START)
					{
						_iKeyTimer = 0;
						TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
					}
				}
			}
			if (_iDisplayTimer)
			{
				_iDisplayTimer --;
				if (!_iDisplayTimer)
				{
					UI_CallDisplay(iState);
				}
			}
		}
	}


#ifdef VER_FWV2800
	if (rGPIO_D & 0x20)		// GPIO_D5 as POWER_DET input, high indicate to use battery
	{
		// GPIO_C0 as BAT_L input, low indicate battery low
		DisplayString((rGPIO_C & 0x01) ? IDS_ON_BATTERY : IDS_BATTERY_LOW, 3);
		_bOnBattery = TRUE;
	}
	else
	{
		if (_bOnBattery)
		{
			DisplayClear(3);
			if (Sys_iCurMode == SYS_MODE_STANDBY)
			{
				UI_StandbyDisplay();
			}
			else if (Sys_iCurMode == SYS_MODE_CALL)
			{
				iState = TaskGetCallInfo(CALL_INFO_STATE);
				UI_CallDisplay(iState);
			}
			_bOnBattery = FALSE;
		}
	}
#endif		// VER_FWV2800

#endif

	if (Sys_sDebug)
	{
		DI;
		sVal = Sys_sDebug;
		Sys_sDebug = 0;
		EI;
		UdpDebugVal(sVal, 10);
	}
}

void UI_Run()
{
#ifdef KEY_ADC
	ADC_KeyRun();
#endif

#ifdef HOOK_GPIO
	HookRun();
#endif

#ifdef KEY_4X4
	Key4x4_Run();
#endif

#ifdef KEY_5X5
	Key5x5_Run();
#endif

#ifdef KEY_5X6
	Key5x6_Run();
#endif

#ifdef KEY_8x4
	Key8x4_Run();
#endif

#ifdef KEY_8X5
	Key8x5_Run();
#endif

#ifdef KEY_7X8
	Key7x8_Run();
#endif

#ifdef KEY_C2
	KeyC2_Run();
#endif

	KickDog();

#ifndef CALL_NONE

#ifdef VER_AR168R
	RoIP_Run();
#endif

	DspRun(FALSE);
#endif

	if (Sys_iDebugVal)
	{
		UdpDebugVal(Sys_iDebugVal, 16);
		Sys_iDebugVal = 0;
	}
}

void _DisplayKey()
{
	UCHAR iLen;
#ifdef OEM_IP20
	UCHAR iState;
#endif

	iLen = strlen(_pKeyBuf);
#ifdef OEM_IP20
	LcdDisplay((iLen > DISPLAY_MAX_CHAR) ? (PCHAR)(_pKeyBuf+iLen-DISPLAY_MAX_CHAR) : _pKeyBuf, 2);
	iState = TaskGetCallInfo(CALL_INFO_STATE);
	if (iState != CALL_STATE_TALKING)	DisplayString(IDS_SIL_ARA, 3);
#else
	LcdDisplay((iLen > DISPLAY_MAX_CHAR) ? (PCHAR)(_pKeyBuf+iLen-DISPLAY_MAX_CHAR) : _pKeyBuf, 1);
#endif

	UdpDebugString(_pKeyBuf);
}

#ifndef CALL_NONE

void UI_HandsetDown()
{
	if (Sys_iCurMode != SYS_MODE_STANDBY && Sys_iCurMode != SYS_MODE_CALL)
	{
		return;	
	}
	TaskHandleEvent(UI_EVENT_HANDSET_DOWN, 0);
	LED_Off(LED_ACTIVE);
}

void UI_StartCodec(UCHAR iCodec, BOOLEAN bVad, UCHAR iOperation)
{
	UCHAR iCurType, iType;

	if (iCodec == VOICE_CODER_NONE)
	{
		return;
	}
	iType = Rtp_pPayLoad[iCodec];
	if (IsCodecRunning())
	{
		iCurType = DspGetType();
		DspStop();
		if (iCurType != iType)
		{
			DspLoad(iType);
		}
		DspStart(iType, bVad, iOperation);
	}
	else
	{
//		CodecStart(iType);
		CodecStart();
		DspLoad(iType);
		DspStart(iType, bVad, iOperation);
	}
	UdpDebugString("Codec Type");
	UdpDebugVal(iType, 10);
}

void UI_StopCodec()
{
	if (IsCodecRunning())
	{
		DspStop();
		CodecStop();
	}
}

void _MiniRun()
{
	if (_bTimer)
	{
		_bTimer = FALSE;
	}
	KickDog();
}

void UI_PlayKeys(PCHAR pKeys)
{
	UCHAR i;

	for (i = 0; i < strlen(pKeys); i ++)
	{
		DtmfPlayKey(pKeys[i]);
		while (Dsp_sOpDuration);
		_MiniRun();
		DtmfPlayKey(0);
		while (Dsp_sOpDuration);
		_MiniRun();
	}
}

void UI_PlayKey(UCHAR iKey, UCHAR iState)
{
	UCHAR iDspOp;

	DtmfPlayKey(iKey);
	if (iState == CALL_STATE_TALKING || iState == CALL_STATE_CALLING2)
	{
		iDspOp = DspGetOperation();
#ifdef CALL_IAX2
		DspSetOperation(DSP_OP_DTMFPLAY_ENCODE, iDspOp);		// outband
#else
		if (Sys_iDtmfType)
		{
			DspSetOperation(DSP_OP_DTMFPLAY_ENCODE, iDspOp);		// outband
		}
		else
		{
			DspSetOperation(DSP_OP_DTMFPLAY_DTMFENCODE, iDspOp);	// inband audio
		}
#endif
	}
	else
	{
		DspSetOperation(DSP_OP_DTMFPLAY, DSP_OP_DTMFPLAY);
	}
}

void UI_PlayBusyTone()
{
	UI_StartCodec(VOICE_CODER_ULAW, FALSE, DSP_OP_DTMFPLAY);	// alway use G.711 mu law for busy tone generation
	DtmfPlayTone(DTMF_SIGNAL_BUSY);
}

void UI_PlayDialTone()
{
	UI_StartCodec(VOICE_CODER_ULAW, FALSE, DSP_OP_DTMFPLAY);	// alway use G.711 mu law for dial tone generation);
	if (!_bLogoned)
	{
		DtmfPlayTone(DTMF_SIGNAL_CONGESTION);
	}
	else
	{
		if (_iNewMsg)
		{
			DtmfPlayStutter();
		}
		else
		{
			DtmfPlayTone(DTMF_SIGNAL_DIAL);
		}
	}
}

void UI_ChangeMode(UCHAR iMode)
{
	if (iMode == SYS_MODE_STANDBY)
	{
		if (UI_IsCallMode())
		{
			Sys_iCurMode = SYS_MODE_CALL;
			LED_On(LED_LCD);
			TaskHandleEvent(UI_EVENT_HANDSET_UP, 0);
		}
		else
		{
			Sys_iCurMode = SYS_MODE_STANDBY;
			LED_Off(LED_LCD);
			UI_StandbyDisplay();
		}
	}
	else
	{
		Sys_iCurMode = iMode;
		LED_On(LED_LCD);
		UI_ClearDisplay(1);
		switch (iMode)
		{
		case SYS_MODE_CALL:
			LED_Off(LED_MUTE);
			break;
		case SYS_MODE_TFTPD:
			DisplayString(IDS_TFTPD_MODE, 0);
			break;
		case SYS_MODE_HTTPD:
			DisplayString(IDS_HTTPD_MODE, 0);
			break;
		case SYS_MODE_AUTOPROV:
			DisplayString(IDS_AUTOPROV_MODE, 0);
			break;
		}
	}
}

void UI_BuildNumber(PCHAR pOldNumber)
{
	UCHAR pNumber[MAX_USER_NUMBER_LEN];
	UCHAR iLen;

	OptionsGetString(pNumber, OPT_DIAL_PREFIX, MAX_USER_NUMBER_LEN);
	iLen = strlen(pNumber);
	if (!iLen)	return;		// no dial prefix
	if (!memcmp_str(pOldNumber, pNumber))		// dialed number already has prefix
	{
		return;
	}
	iLen += strlen(pOldNumber);
	if (iLen < MAX_USER_NUMBER_LEN)
	{
		strcat(pNumber, pOldNumber);
		strcpy(pOldNumber, pNumber);
	}
}

UCHAR CodecIncVol()
{
	UCHAR iVol;

	iVol = CodecGetVol();
	if (iVol < 0x1f)
	{
		iVol ++;
#ifdef OEM_BT2008
		if (iVol > MAX_VOLOUT && _bSpeaker == FALSE)
		{
			iVol = MAX_VOLOUT;
		}
#endif		
		CodecSetVol(iVol);
	}
	return iVol;
}

UCHAR CodecDecVol()
{
	UCHAR iVol;

	iVol = CodecGetVol();
	if (iVol)
	{
		iVol --;
		CodecSetVol(iVol);
	}
	return iVol;
}

BOOLEAN GetPhoneBookNumber(UCHAR iIndex, PCHAR pBuf)
{
	USHORT sOffset;

	pBuf[0] = 0;
	if (iIndex)
	{
		iIndex --;
		sOffset = (USHORT)(OPT_PHONE_BOOK + ((USHORT)iIndex << PHONEBOOK_ENTRY_SHIFT));
		if (OptionsGetByte(sOffset))
		{
			OptionsGetString(pBuf, (USHORT)(sOffset + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef OEM_IP20
const UCHAR _cCodecMarkSys[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cDtmfTypeMarkSys[3][9] = {"Inband", "RFC2833", "SIP Info"};
const UCHAR _cNatTraversalMarkSys[3][9] = {"Kapat", "STUN", "NAT IP"};

void line_add_bool(BOOLEAN bYes)
{
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

	TaskLoadString((bYes ? IDS_TRUE : IDS_FALSE), pBuf);
	line_add_str(pBuf);	
}

void ManagerReport()
{
	UCHAR i;
	UCHAR pBuf[800];		// CALCLATE necessary size of buffer here, too large buffer will cause stack overflow!
	UCHAR _pMACBuf[18];
	UCHAR pIPSys[IP_ALEN];
	//UCHAR Sip_pNatIPSys[4];
	
	line_start(pBuf);
	line_add_str("IP20 Manager:");
	line_add_str_with_char("IP20_V_AAT", ',');
		
	//Network Settings
	mac2str(Sys_pMacAddress, _pMACBuf);
	line_add_str_with_char(_pMACBuf, ',');
	line_add_str(OptionsGetByte(OPT_IP_TYPE) ? "DHCP" : "STATIC");
		
	line_add_char(',');
	line_add_ip(Sys_pIpAddress);
	line_add_char(',');
	line_add_ip(Sys_pSubnetMask);
	line_add_char(',');
	line_add_ip(Sys_pRouterIp);
	line_add_char(',');
	line_add_str(OptionsGetByte(OPT_DNS_TYPE) ? "AUTO" : "DEFINED");
	line_add_char(',');
	line_add_ip(Sys_pDnsIp);
	line_add_char(',');
	line_add_ip(Sys_pDnsIp2);
	line_add_char(',');
	
	//Voice Codecs
	for (i = 0; i < VOICE_CODER_NUM; i ++)
	{
		line_add_str_with_char(_cCodecMarkSys[Sys_pVoiceCoders[i]], ',');
	}

	//Protocol Settings
	line_add_bool(Sys_bRegister);
	line_add_char(',');
	
	line_add_str_with_char(Sys_pSipProxy, ',');
	
	line_add_short(OptionsGetShort(OPT_SIP_PORT));	//Port
	line_add_char(',');
	
	line_add_str_with_char(Sys_pSipDomain, ',');
	
	line_add_bool(Sys_bOutboundProxy);
	line_add_char(',');
	
	line_add_bool((BOOLEAN)OptionsGetByte(OPT_DNS_SRV));	//DNS Server
	line_add_char(',');
	
	line_add_str_with_char(Sys_pUserNumber, ',');
	line_add_str_with_char(Sys_pAuthID, ',');
	line_add_str_with_char(Sys_pAuthPin, ',');
	line_add_str_with_char(Sys_pUserName, ',');
	
	line_add_short(Sys_sLocalPort);	//Yerel SIP Port
	line_add_char(',');
	
	line_add_short(Sys_sRtpPort);	//Yerel RTP Port
	line_add_char(',');
	
	line_add_short(Sys_sRegisterTtl);	//Kayýt Sonu
	line_add_char(',');
	
	line_add_str(_cDtmfTypeMarkSys[Sys_iDtmfType]);	//DTMF Type
	line_add_char(',');
	
//	OptionsGetString(line_get_buf(), OPT_MESSAGE_NUMBER, 32) ;
//	line_update_len();
	line_add_options_str(OPT_MESSAGE_NUMBER, 32);
	line_add_char(',');
	
	line_add_bool(Sys_bPrack);
	line_add_char(',');
	
	line_add_bool((BOOLEAN)OptionsGetByte(OPT_SUBSCRIBE_MWI));
	line_add_char(',');
	
	line_add_str_with_char(Sys_pProxyRequire, ',');
	line_add_str_with_char(_cNatTraversalMarkSys[OptionsGetByte(OPT_NAT_TRAVERSAL)], ',');	//NAT Geçiþi
	
	/*
	OptionsGetBuf(Sip_pNatIPSys, OPT_NAT_IP, IP_ALEN);		//NAT IP
	line_add_ip(Sip_pNatIpSys);
	line_add_char(',');	
	*/
	
//	OptionsGetString(line_get_buf(), OPT_STUN_SERVER, MAX_DNS_LEN);	//STUN Sunucu
//	line_update_len();
	line_add_options_str(OPT_STUN_SERVER, MAX_DNS_LEN);
	line_add_char(',');
	
	line_add_short(OptionsGetShort(OPT_STUN_PORT));	//STUN Sunucu Port
	line_add_char(',');
	
	//DialPlan Settings
	
//	OptionsGetString(line_get_buf(), OPT_FWD_NUMBER, MAX_DNS_LEN);	//Yönlendirme Numaras?
//	line_update_len();
	line_add_options_str(OPT_FWD_NUMBER, MAX_DNS_LEN);
	line_add_char(',');
	
	line_add_bool(Sys_bFwdAlways);
	line_add_char(',');
	
	line_add_bool(Sys_bFwdBusy);
	line_add_char(',');
	
	line_add_bool(Sys_bFwdNoanswer);
	line_add_char(',');
	
	line_add_bool(Sys_bAutoAnswer);
	line_add_char(',');
	
	line_add_bool(Sys_bCallWaiting);
	line_add_char(',');
	
	//System Settings
	
//	OptionsGetString(line_get_buf(), OPT_ADMIN_PIN, MAX_ADMIN_PIN_LEN); //Yönetici Þifresi	
//	line_update_len();
	line_add_options_str(OPT_ADMIN_PIN, MAX_ADMIN_PIN_LEN);
	line_add_char(',');

	OptionsGetBuf(pIPSys, OPT_SYSLOG_IP, IP_ALEN);	//syslog IP
	line_add_ip(pIPSys);
	line_add_char(',');
	
//	OptionsGetString(line_get_buf(), OPT_SNTP_DNS, MAX_DNS_LEN);	//SNTP Sunucusu
//	line_update_len();
	line_add_options_str(OPT_SNTP_DNS, MAX_DNS_LEN);
	line_add_char(',');
	
	switch(Sys_iCurMode)
	{
	case 0:
			line_add_str("STANDBY");
			break;
	case 1:
			line_add_str("CALL");
			break;
	case 2:
			line_add_str("MENU");
			break;
	case 3:
			line_add_str("TFTP");
			break;
	case 4:
			line_add_str("HTTP");
			break;
	case 5:
			line_add_str("AUTOPRO");
			break;
	case 6:
			line_add_str("TEST");
			break;
	case 7:
			line_add_str("ALARM");
			break;
	}
	
	line_add_char(',');

//	OptionsGetString(line_get_buf(), OPT_FLAG_PRODUCT, MAX_FLAG_PRODUCT_LEN);
//	line_update_len();	
	line_add_options_str(OPT_FLAG_PRODUCT, MAX_FLAG_PRODUCT_LEN);

	Syslog(SYSLOG_PRI_NOTICE, pBuf);
}

#else
void ManagerReport()
{
	UCHAR pBuf[100];

	line_start(pBuf);
	line_add_str("ar1688_manager:");
	line_add_ip(Sys_pIpAddress);
	line_add_char(',');
	line_add_str_with_char(Sys_pUserNumber, ',');
	line_add_str((Sys_iCurMode == SYS_MODE_STANDBY) ? "idle," : "busy,");
//	OptionsGetString(line_get_buf(), OPT_FLAG_VERSION, MAX_FLAG_VERSION);
//	line_update_len();
	line_add_options_str(OPT_FLAG_VERSION, MAX_FLAG_VERSION);
	line_add_char(',');
//	OptionsGetString(line_get_buf(), OPT_FLAG_PRODUCT, MAX_FLAG_PRODUCT_LEN);
//	line_update_len();
	line_add_options_str(OPT_FLAG_PRODUCT, MAX_FLAG_PRODUCT_LEN);

	Syslog(SYSLOG_PRI_NOTICE, pBuf);
}
#endif

void UI_MinuteTimer()
{
#ifndef SYS_PROGRAMMER

#ifdef OEM_BT2008
	UCHAR iTimeout;

	iTimeout = OptionsGetByte(OPT_REDIAL_TIMEOUT);
	if (iTimeout)
	{
		_iRedialTimer ++;
		if (_iRedialTimer >= iTimeout)
		{
			_iRedialTimer = 0;
			_pRedialBuf[0] = 0;
		}
	}
#endif	// OEM_BT2008

	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		UI_StandbyDisplay();
#if defined OEM_BT2008 || defined OEM_IP20
		if (Sys_tmLocal.iHour == _iAlarmHour && Sys_tmLocal.iMin == _iAlarmMin)
		{
			SpeakerPhone(TRUE);
			UI_StartCodec(VOICE_CODER_ULAW, FALSE, DSP_OP_RINGPLAY_RINGENCODE);
			_iAlarmState = ALARM_STATE_RINGING;
		}
		else
		{
			if (_iAlarmState == ALARM_STATE_RINGING)
			{
				SpeakerPhone(FALSE);
				UI_AlarmInit();
			}
		}
#endif	// OEM_BT2008
	}
	else
	{
		ConvertTime(Sys_lCurrentTime, &Sys_tmLocal);
	}

#endif	// SYS_PROGRAMMER

	ManagerReport();
#ifdef VER_AR168W
//	heap_debug();
#endif

	if (Sys_sMinuteDebug)
	{
		UdpDebugVal(Sys_sMinuteDebug, 10);
		Sys_sMinuteDebug = 0;
	}
/*
#ifdef VER_AR168R
	RoIP_MinuteTimer();
#endif
*/
}

#ifdef SYS_TEST

#define MAX_KEY_NUM		25
const UCHAR _cKeyId[MAX_KEY_NUM] = {
	'o', 'm', 'n', 'l',
	'g', 'h', 'i', 'j', 'k',
	'f', 'e', 'd', 'b',
	'a', 'c', 'p', 'q', 
	'r', 's', 't', 'u', 
	'v', 'w', 'x', 'y',
};


const UCHAR _cKeyName[MAX_KEY_NUM][9] = 
{
	"Message", "Hold", "Transfer", "Mute",
	"Menu", "Up", "Down", "Left", "Right",
	"OK", "Cancel", "Flash", "Redial",
	"Call", "Speaker", "M1", "M2",
	"M3", "M4", "M5", "M6",
	"M7", "M8", "M9", "M10",
};

void UI_TestInit()
{
	_iTestItem = TEST_ITEM_NONE;
	_iTestStep = 0;
}

void UI_TestKeys(UCHAR iKey)
{
	if (iKey == _cKeyId[_iTestStep])
	{
		_iTestStep ++;
		if (_iTestStep < MAX_KEY_NUM)
		{
			LcdDisplay(_cKeyName[_iTestStep], 0);
		}
		else
		{
			_iTestItem = TEST_ITEM_DONE;
//			LcdDisplay("Key OK!", 0);
			DisplayString(IDS_KEY_OK, 0);
		}
	}
}

void UI_TestTimer()
{
#ifdef LCD_ST7565
	UCHAR i;
#endif

	if (_iTestItem == TEST_ITEM_LCD)
	{
#ifdef LCD_ST7565
		if (_iTestStep < 128)
		{
			for (i = 0; i < 8; i ++)
			{
				LcdLight(_iTestStep, 0xff);
				_iTestStep ++;
			}
		}
		else
#elif defined LCD_HY1602
		if (_iTestStep < 16)
		{
			DisplayProgress(_iTestStep, 0);
			DisplayProgress(_iTestStep, 1);
			_iTestStep ++;
		}
		else
#endif		
		{
			_iTestItem = TEST_ITEM_DONE;
			LED_Off(LED_LCD);
			LcdStart();
//			LcdDisplay("LCD OK!", 0);
			DisplayString(IDS_LCD_OK, 0);
		}
	}
	else if (_iTestItem == TEST_ITEM_LED)
	{
		switch (_iTestStep)
		{
		case 0:
			LED_On(LED_MESSAGE);	
			break;

		case 1:
			LED_On(LED_HOLD);	
			break;

		case 2:
			LED_On(LED_TRANSFER);	
			break;

		case 3:
			LED_On(LED_MUTE);
			break;

		case 4:
			LED_Off(LED_MESSAGE);
			break;

		case 5:
			LED_Off(LED_HOLD);	
			break;

		case 6:
			LED_Off(LED_TRANSFER);	
			break;

		case 7:
			LED_Off(LED_MUTE);
			break;

		case 8:
			_iTestItem = TEST_ITEM_DONE;
//			LcdDisplay("LED OK!", 0);
			DisplayString(IDS_LED_OK, 0);
			break;

		}
		_iTestStep ++;
	}
	else if (_iTestItem == TEST_ITEM_DONE)
	{
		_iTestItem = TEST_ITEM_NONE;
		TaskMenuEvent(UI_EVENT_TESTDONE);
	}
}

void UI_TestPhone(UCHAR iItem)
{
	if (iItem <= TEST_ITEM_KEY)
	{
		_iTestItem = iItem;
		_iTestStep = 0;
		UI_ClearDisplay(1);
		switch (iItem)
		{
		case TEST_ITEM_LCD:
			LED_On(LED_LCD);
			LcdStart();
			break;

		case TEST_ITEM_LED:
			LED_Off(LED_MESSAGE);
			_ClearCallStatus();
			break;

		case TEST_ITEM_KEY:
			LcdDisplay(_cKeyName[0], 0);
			break;
		}
	}
	else if (iItem == TEST_ITEM_TIMER)
	{
		UI_TestTimer();
	}
}
#endif //	SYS_TEST

BOOLEAN UI_IsCallMode()
{
	if (_bHandsetUp)		return TRUE;
	if (_bSpeaker)			return TRUE;
	return FALSE;
}

void UI_ClearDisplay(UCHAR iStartLine)
{
	UCHAR i;

	if (iStartLine >= DISPLAY_MAX_LINE)	return;
	for (i = iStartLine; i < DISPLAY_MAX_LINE; i ++)
	{
		DisplayClear(i);
	}
}

void UI_DisplayIP(UCHAR iLine)
{
	UCHAR pString[16];

	ip2str(Sys_pIpAddress, pString);
	LcdDisplay(pString, iLine);
}

void _MissedCallString(PCHAR pString)
{
	itoa(Sys_iMissedCalls, pString, 10);
	strcat_char(pString, ' ');
	TaskLoadString(IDS_MISSED_CALLS, (PCHAR)(pString + strlen(pString)));
}

#ifdef UI_4_LINES
void UI_StandbyDisplay()
{
	UCHAR pString[64];

	if (Sys_iCurMode != SYS_MODE_STANDBY || _iDisplayTimer)		return;
	LED_SetState(LED_MUTE, Sys_bDnd);

#ifdef OEM_IP20
	IP20_LED_Off();
	Not_refresh = 0;
	Interdigit = 1;

	Test = FALSE;
	Test1 = FALSE;
	Test2 = FALSE;
	Test_Result = 0; 
	sum1 = 0x00;
	sum2 = 0x00;

	TransferFlag = 0;
	TransFlagResp = 0;

	atLine3 = 0;	
#endif

#ifndef OEM_IP20
	//Line0: Product ID or "Do Not Disturb" or "Not Logon"
	if (!Net_bConnected)
	{
		if (Sys_iIpType == IP_TYPE_DHCP)
		{
			TaskLoadString(IDS_DHCP_FAILED, pString);
		}
		else if (Sys_iIpType == IP_TYPE_PPPOE)
		{
			TaskLoadString(IDS_PPPOE_FAILED, pString);
		}
		LcdCenterDisplay(pString, 0);
		UI_ClearDisplay(1);
		return;
	}
	if (!_bLogoned)
	{
		TaskLoadString(IDS_NOT_LOGON, pString);
		goto Line0;
	}
	TaskLoadString(Sys_bDnd ? IDS_DO_NOT_DISTURB : IDS_DISPLAY_MODEL, pString);
Line0:
	LcdCenterDisplay(pString, 0);
#endif

	//Line1: User Name or New Missed Calls
	if (Sys_iMissedCalls)
	{
#ifdef OEM_IP20
		if (voice_message == 0)
		{	
			DisplayLogStandby();
			pString[0] = '(';
			pString[1] = '1';
			pString[2] = '/';	
			itoa(Sys_iMissedCalls, (PCHAR)(pString + 3), 10);
			strcat_char(pString, ')');
			TaskLoadString(IDS_MISSED_CALLS, (PCHAR)(pString + strlen(pString)));
			DisplayString(IDS_DETAY, 3);
			if(_iNewMsg) 	_display_ascii('>', 11, 3);
		}
		else if(_iNewMsg && voice_message == 1)
		{
			DisplayString(IDS_DINLE_1, 3);
		}
#else
		_MissedCallString(pString);
#endif				
	}
	else
	{
		strcpy(pString, Sys_pUserName);
	}
#ifdef OEM_IP20
//Line2:
	DisplayString(IDS_KAREL, 1);
	LcdDisplay(pString, (Sys_iMissedCalls) ? 1 : 2);
#else
	LcdCenterDisplay(pString, 1);
#endif

	//Line2: User Number or Local IP Address or New Voice Msg
	if (Sys_bRegister)
	{
#ifdef OEM_IP20
		if (_iNewMsg && voice_message==1)
		{
			//TaskLoadString(IDS_NEW_MESSAGE, pString);
			if (_iNewMsg == 0xff && _iOldMsg == 0xff)
			{
				TaskLoadString(IDS_VAR, (PCHAR)(pString + strlen(pString)));
			}
			else
			{
				pString[0]= '(' ;
				itoa(_iNewMsg, (PCHAR)(pString+1), 10);
				strcat_char(pString, '/');
				itoa(_iOldMsg, (PCHAR)(pString+strlen(pString)), 10);
				strcat_char(pString, ')');
				TaskLoadString(IDS_SESLI_MESAJ, (PCHAR)(pString + strlen(pString)));
			}
			LcdDisplay(pString, 1);
			UI_ClearDisplay(2);
			DisplayString(IDS_DINLE_2, 3);
			if (Sys_iMissedCalls) 	_display_ascii('>', 11, 3);
		}
#else
		if (_iNewMsg || _iOldMsg)
		{
			TaskLoadString(IDS_NEW_MESSAGE, pString);
			if (_iNewMsg == 0xff && _iOldMsg == 0xff)
			{
				strcat(pString, "Yes");
			}
			else
			{
				itoa(_iNewMsg, (PCHAR)(pString+strlen(pString)), 10);
				strcat_char(pString, '/');
				itoa(_iOldMsg, (PCHAR)(pString+strlen(pString)), 10);
			}
		}
#endif
		else
		{
#ifdef OEM_IP20	
			strcpy(pString, Sys_pUserName);
#else
			strcpy(pString, Sys_pUserNumber);
#endif
		}
	}
	else
	{
		ip2str(Sys_pIpAddress, pString);
	}
#ifndef OEM_IP20 
	LcdCenterDisplay(pString, 2);		//IP20 Not Shown Yet
#endif

	//Line3: Date & Time
	ConvertTime(Sys_lCurrentTime, &Sys_tmLocal);
#ifdef OEM_IP20
	DisplayTime(&Sys_tmLocal, 0);		//IP20 Line 0 & 3
	if (!Sys_iMissedCalls && !_iNewMsg)
	{
		DisplayString(IDS_OK, 3);
	}
#else
	DisplayTime(&Sys_tmLocal, 3);
#endif

#ifdef OEM_IP20
	if (!Net_bConnected)
	{
		if (Sys_iIpType == IP_TYPE_DHCP)
		{
			TaskLoadString(IDS_DHCP_FAILED, pString);
		}
		else if (Sys_iIpType == IP_TYPE_PPPOE)
		{
			TaskLoadString(IDS_PPPOE_FAILED, pString);
		}

		LcdDisplay(pString, 2);
	}
	if (!_bLogoned)
	{
		TaskLoadString(IDS_NOT_LOGON, pString);
		LcdDisplay(pString, 2);
	}
	if (Sys_bDnd)
	{
		TaskLoadString(IDS_DO_NOT_DISTURB, pString);
		LcdDisplay(pString, 2);
	}
#endif
}

#else
void UI_StandbyDisplay()
{
	UCHAR pString[64];

	if (Sys_iCurMode != SYS_MODE_STANDBY || _iDisplayTimer)	return;
	LED_SetState(LED_MUTE, Sys_bDnd);

	pString[0] = 0;
	if (!Net_bConnected)
	{
		if (Sys_iIpType == IP_TYPE_DHCP)
		{
			TaskLoadString(IDS_DHCP_START, pString);
		}
		else if (Sys_iIpType == IP_TYPE_PPPOE)
		{
			TaskLoadString(IDS_PPPOE_START, pString);
		}
		goto Line0;
	}
	if (!_bLogoned)
	{
		TaskLoadString(IDS_NOT_LOGON, pString);
		goto Line0;
	}
#ifdef OEM_BT2008
	if (strlen(_pLastRecord))
	{
		strcpy(pString, _pLastRecord);
		goto Line0;
	}
	if (Sys_bDnd)
	{
		TaskLoadString(IDS_DO_NOT_DISTURB, pString);
		goto Line0;
	}
#else
	if (Sys_bDnd || Sys_iMissedCalls)
	{
		if (Sys_iMissedCalls)
		{
			_MissedCallString(pString);
		}
		if (Sys_bDnd)
		{
			if (Sys_iMissedCalls)
			{
				strcat_char(pString, '/');
			}
			strcat(pString, "DND");
		}
		goto Line0;
	}
#endif
	if (_iNewMsg || _iOldMsg)
	{
		TaskLoadString(IDS_NEW_MESSAGE, pString);
		if (_iNewMsg == 0xff && _iOldMsg == 0xff)
		{
			strcat(pString, "Yes");
		}
		else
		{
			itoa(_iNewMsg, (PCHAR)(pString+strlen(pString)), 10);
			strcat_char(pString, '/');
			itoa(_iOldMsg, (PCHAR)(pString+strlen(pString)), 10);
		}
		goto Line0;
	}

	if (Sys_bRegister)
	{
		strcpy(pString, Sys_pUserNumber);
		if (!strlen(pString))
		{
			TaskLoadString(IDS_PHONE_READY, pString);
		}
	}
	else
	{
		ip2str(Sys_pIpAddress, pString);
	}

Line0:
	LcdDisplay(pString, 0);

	ConvertTime(Sys_lCurrentTime, &Sys_tmLocal);
	DisplayTime(&Sys_tmLocal, 1);
}
#endif

void UI_CallDisplay(UCHAR iState)
{
#ifndef OEM_IP20
	PCHAR pNumber;
#ifdef UI_4_LINES
	PCHAR pName;
#endif
#endif
	if (iState == CALL_STATE_DIALING)
	{
#ifdef OEM_IP20
		DisplayString(IDS_ENTER_NUMBER, 1);
#else
		DisplayString(IDS_ENTER_NUMBER, 0);
#endif
		_DisplayKey();
	}
#ifndef OEM_2KLABS
	else if (iState == CALL_STATE_CALLING || iState == CALL_STATE_CALLING2)
	{
#ifdef OEM_IP20
		ConvertTime(Sys_lCurrentTime, &Sys_tmLocal);
		DisplayTime(&Sys_tmLocal, 0);	
		DisplayString(IDS_CALLING, 1);
#else
		DisplayString(IDS_CALLING, 0);
#endif
	}
	else if (iState == CALL_STATE_TALKING)
	{
#ifdef OEM_IP20
		DisplayString(IDS_CONNECTED, 0);
		DisplayString(IDS_KAPAT, 3);
#else
		DisplayCallTime((CALL_TIME *)TaskGetCallInfo(CALL_INFO_CALL_TIME), 0);
#endif
	}
#endif
	else if (iState == CALL_STATE_RINGING)
	{
#ifdef OEM_IP20
		ConvertTime(Sys_lCurrentTime, &Sys_tmLocal);
		DisplayTime(&Sys_tmLocal, 0);		
		DisplayString(IDS_RINGING, 1);		
#else
		DisplayString(IDS_RINGING, 0);
#endif
	}
	else
	{
		return;
	}
#ifndef OEM_IP20
	pNumber = (PCHAR)TaskGetCallInfo(CALL_INFO_PEER_NUMBER);
	LcdDisplay(pNumber, 1);
	UI_ClearDisplay(2);
#ifdef UI_4_LINES
	pName = (PCHAR)TaskGetCallInfo(CALL_INFO_PEER_NAME);
	LcdDisplay(pName, 2);	
#endif
#endif
#ifdef OEM_IP20
	Not_refresh = 0;
#endif
}

void UI_MemCall(PCHAR pNameNumber)
{
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
#ifdef OEM_BT2008
		if (OptionsGetByte(OPT_GET_VOICE_MSG))
#endif
		{
			UI_ChangeMode(SYS_MODE_CALL);
			TaskHandleEvent(UI_EVENT_HANDSET_UP, 1);
			SpeakerPhone(TRUE);
		}
	}
	if (Sys_iCurMode != SYS_MODE_CALL)	return;

	UI_BuildNumber(pNameNumber);
	TaskHandleEvent(UI_EVENT_MEM_CALL, (USHORT)pNameNumber);
}

void UI_EndCall()
{
#ifdef VER_AR168R
	RoIP_EndCall();
#endif

	if (_bAutoAnswer && _bSpeaker)	// auto answer, auto silent speakphone
	{
#ifndef SYS_PROGRAMMER
#if defined VER_AR168M && defined OEM_BT2008
		OnSpeaker(FALSE);
#else
		OnSpeaker();
#endif
#endif
		return;
	}

#ifdef OEM_IP20
	Reddet = 0;
	Sessiz = 0;
	Kapat = 0;
#endif

	_bAutoAnswer = FALSE;
	_bMute = FALSE;
	_ClearCallStatus();
	if (UI_IsCallMode())
	{
		UI_PlayBusyTone();
#ifdef VER_AR168R
		OnHandsetDown();
#endif
	}
	else
	{
		UI_ChangeMode(SYS_MODE_STANDBY);
		UI_StopCodec();
		SpeakerPhone(FALSE);
	}
	KeyClear();
}

void GetPhoneBookName(UCHAR iIndex, PCHAR pBuf)
{
	USHORT sOffset;

	pBuf[0] = 0;
	if (iIndex)
	{
		iIndex --;
		sOffset = (USHORT)(OPT_PHONE_BOOK + ((USHORT)iIndex << PHONEBOOK_ENTRY_SHIFT));
		if (OptionsGetByte(sOffset))
		{
			OptionsGetString(pBuf, (USHORT)(sOffset + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
		}
	}
}

void UI_EventHandler(UCHAR iType, USHORT sParam)
{
	UCHAR pString[32];
	TIME_T tm;
	PCHAR pParam;
	UCHAR iTemp;
	PCHAR *pLine;
	PCHAR pCur;
	UCHAR i, iLine, iLen;

	pParam = (PCHAR)sParam;

#ifdef SERIAL_UI
	SerialUI_Event(iType);
#endif

	switch (iType)
	{
	case UI_PPP_DEBUG:
		DisplayVal("PPP: ", sParam, 0);
		break;

	case UI_NET_CONNECTED:
		ip2str(Sys_pIpAddress, pString);
		LcdDisplay(pString, 1);
//		UdpDebugString(pString);
		TaskStartProv();
		TaskStartApp();
#ifdef SST39VF1681
		SystemCheckSecurityID();
#endif
#ifdef SYS_PROGRAMMER
		ProgrammerDebugMac();
#endif
		break;

	case UI_SNTP_DONE:
		_bSntpOK = TRUE;
		UI_StandbyDisplay();
		break;

	case UI_SNTP_FAILED:
		_bSntpOK = FALSE;
		break;

#ifdef OEM_INNOMEDIA
	case UI_PROV_DONE:
		UI_ChangeMode(SYS_MODE_STANDBY);
		if (sParam)
		{
			Sys_iIlbcFrame = OptionsGetByte(OPT_ILBC_FRAME)? 1:0;
			Sys_iSpeakerVol = OptionsGetByte(OPT_SPEAKER_VOL);
			Sys_iHandsetVol = OptionsGetByte(OPT_VOL_OUTPUT);
			OptionsGetBuf(Sys_pLogIp, OPT_SYSLOG_IP, IP_ALEN);
			SyslogClose();
			SyslogOpen();
			TaskStartApp();
		}
		else
		{
			DisplayString(IDS_PROVISION_FAILED, 0);
		}	
		break;
#endif

#ifdef CALL_IAX2
	case UI_IAX2_TIME:
		if (!_bSntpOK)
		{
			Sys_lCurrentTime = CalcIax2Time((PCHAR)sParam);
			UI_StandbyDisplay();
		}
		break;
#endif

	case UI_CHANGE_MODE:
#ifdef OEM_BT2008
		UI_StopAlarm();
#endif
		UI_ChangeMode((UCHAR)sParam);
		break;

	case UI_UPDATE_PROGRESS:
		UI_UpdateProgress((UCHAR)sParam);
		break;

	case UI_MENU_EXIT:
		UI_ChangeMode(SYS_MODE_STANDBY);
		break;

	case UI_MENU_CALL:
		Sys_iCurMode = SYS_MODE_STANDBY;
		UI_MemCall((PCHAR)sParam);
		break;

	case UI_MENU_DISPLAY_TIME:
		ConvertTime(PCHAR2ULONG_L((PCHAR)sParam), &tm);
#ifdef OEM_IP20
		DisplayTime(&tm, 1);
#else
		DisplayTime(&tm, 0);
#endif
		break;

	case UI_LOGON_OK:
		if (!_bLogoned)
		{
			UdpDebugIDS(IDS_LOGON_OK);
			_bLogoned = TRUE;
			UI_StandbyDisplay();
			LED_On(LED_LOGON);
#ifdef VER_AR168R
			RoIP_LogonOk();
#endif
		}
		break;

	case UI_LOGON_FAILED:
		UdpDebugIDS(IDS_LOGON_FAILED);
		_bLogoned = FALSE;
		UI_StandbyDisplay();
		LED_Off(LED_LOGON);
		break;

	case UI_LOAD_DEFAULTS:
		UI_LoadDefaults();
		break;

	case UI_STORE_DEFAULTS:
		UI_StoreDefaults();
		break;

#ifdef	SYS_TEST
	case UI_TEST_PHONE:
		UI_TestPhone((UCHAR)sParam);
		break;
#endif

	case UI_STBY_DISPLAY_LINE0:
	case UI_STBY_DISPLAY_LINE1:
#ifdef UI_4_LINES
	case UI_STBY_DISPLAY_LINE2:
	case UI_STBY_DISPLAY_LINE3:
#endif
//		UdpDebugString(pParam);
		if (Sys_iCurMode == SYS_MODE_STANDBY)
		{
			LcdDisplay(pParam, iType - UI_STBY_DISPLAY_LINE0);
		}
		break;

	case UI_STBY_DISPLAY_IDS0:
	case UI_STBY_DISPLAY_IDS1:
#ifdef UI_4_LINES
	case UI_STBY_DISPLAY_IDS2:
	case UI_STBY_DISPLAY_IDS3:
#endif
		UdpDebugIDS((UCHAR)sParam);
		if (Sys_iCurMode == SYS_MODE_STANDBY)
		{
			DisplayString((UCHAR)sParam, iType - UI_STBY_DISPLAY_IDS0);
		}
		break;

	case UI_CALL_DISPLAY_LINE0:
	case UI_CALL_DISPLAY_LINE1:
#ifdef UI_4_LINES
	case UI_CALL_DISPLAY_LINE2:
	case UI_CALL_DISPLAY_LINE3:
#endif
//		UdpDebugString(pParam);
		LcdDisplay(pParam, iType - UI_CALL_DISPLAY_LINE0);
#ifdef OEM_IP20
		if (iType == UI_CALL_DISPLAY_LINE2)
		{
			IP20_LED_On(pParam);
		}
#endif
		break;

	case UI_CALL_DISPLAY_IDS0:
	case UI_CALL_DISPLAY_IDS1:
#ifdef UI_4_LINES
	case UI_CALL_DISPLAY_IDS2:
	case UI_CALL_DISPLAY_IDS3:
#endif
		UdpDebugIDS((UCHAR)sParam);
		DisplayString((UCHAR)sParam, iType - UI_CALL_DISPLAY_IDS0);
		break;

#ifdef OEM_IP20
	case UI_CALL_DIAL:
		ConvertTime(Sys_lCurrentTime, &Sys_tmLocal);
		DisplayTime(&Sys_tmLocal, 0);			
				
		if(_pKeyBuf[0]!=0 )
		{			
			UCHAR _pKeyBufz[MAX_KEYBUF_LEN+1];			
			
			hotline = 0;						
			UI_StartCodec(VOICE_CODER_ULAW, FALSE, DSP_OP_DTMFPLAY);
			DtmfPlayTone(DTMF_SIGNAL_WAITING);
			strcpy (_pKeyBufz,_pKeyBuf);	
			strcpy (_pKeyBuf,_pKeyBufz);
			DisplayString(IDS_SIL_ARA, 3);
			OnCall();			
		}
		else
		{
			hotline = 1;	
			DisplayString(IDS_ENTER_NUMBER, 1);
			UdpDebugIDS(IDS_ENTER_NUMBER);
			UI_ClearDisplay(2);
			UI_PlayDialTone();
			KeyClear();
		}
		break;			
#else
	case UI_CALL_DIAL:
#ifdef VER_AR168R
		RoIP_Dial();
#endif
		DisplayString(IDS_ENTER_NUMBER, 0);
		UdpDebugIDS(IDS_ENTER_NUMBER);
		UI_ClearDisplay(1);
		UI_PlayDialTone();
		KeyClear();
		break;
#endif

	case UI_CALL_CONNECTED:
		_iDisplayTimer = 0; /* Set display timer to make sure received text messages during call and previous displayed text are gone in examle when returning back from hold*/
		DisplayString(IDS_CONNECTED, 0);
		UdpDebugIDS(IDS_CONNECTED);
#ifdef OEM_IP20
		Reddet = 0;
		Sessiz = 0;
		Kapat = 1;
		DisplayString(IDS_KAPAT, 3);
#endif
		LED_On(LED_RINGING);
		UI_StartCodec(pParam[0], pParam[1], DSP_OP_DECODEPLAY_ENCODE);
#ifdef VER_AR168R
		RoIP_Connected();
#endif
		break;

	case UI_CALL_RINGING:
#ifdef VER_AR168R
		RoIP_Ringing();
#endif
#ifdef OEM_IP20
		Reddet = 1;
		Sessiz = 1;	
		DisplayString(IDS_RINGING, 1);
		DisplayString(IDS_SESSIZ_REDDET, 3);
#else
		DisplayString(IDS_RINGING, 0);
		UdpDebugIDS(IDS_RINGING);
#endif
		
		UI_ChangeMode(SYS_MODE_CALL);
#ifdef OEM_BT2008
		LED_Blink(LED_MESSAGE);	
		if (OptionsGetByte(OPT_RINGING))
		{
			SpeakerRing();
			if (_iRingVol)
			{
				_iRingVol = 1;
				CodecSetVol(_iRingVol);
			}
			else
			{
				CodecSetVol(Sys_iRingVolOutput);
			}
		}
#elif !defined VER_AR168R
		SpeakerRing();
		CodecSetVol(Sys_iRingVolOutput);
#endif

		iTemp = (UCHAR)sParam;
		if (iTemp == VOICE_CODER_NONE)
		{
			iTemp = VOICE_CODER_ULAW; 
		}
		UI_StartCodec(iTemp, FALSE, DSP_OP_RINGPLAY_RINGENCODE);
		break;

	case UI_CALL_DECODE_ENCODE:
		UI_StartCodec(pParam[0], pParam[1], DSP_OP_DECODEPLAY_ENCODE);
		break;

	case UI_CALL_RINGBACK_REMOTE:
		UI_StartCodec((UCHAR)sParam, FALSE, DSP_OP_DECODEPLAY_ENCODE);
		break;

	case UI_CALL_UPDATE_CODEC:
		iTemp = DspGetOperation();
		UI_StartCodec(pParam[0], pParam[1], iTemp);
		break;

	case UI_CALL_RINGBACK:
		UI_StartCodec((UCHAR)sParam, FALSE, DSP_OP_DTMFPLAY_ENCODE);
		DtmfPlayTone(DTMF_SIGNAL_RING);
		break;

	case UI_CALL_HOLD:
		DisplayString(IDS_CALL_ONHOLD, 0);
		LED_On(LED_HOLD);	
		UI_StartCodec(pParam[0], pParam[1], DSP_OP_VADPLAY_HOLDENCODE);
		break;

	case UI_CALL_AUTOANSWER:
		_bAutoAnswer = TRUE;
		UI_ChangeMode(SYS_MODE_CALL);
		SpeakerPhone(TRUE);
#ifdef VER_AR168R
		_bHandsetUp = TRUE;
#endif
		break;

	case UI_CALL_FINISHED:
		_iDisplayTimer = 0; /* Set display timer to make sure received text messages during call and previously displayed texts are gone */
		UdpDebugIDS(IDS_PLEASE_HANGUP);
#ifndef OEM_2KLABS
		if (sParam != 0) 
		{
			DisplayString(IDS_PLEASE_HANGUP, 0);
			LcdDisplay(pParam, 1);
			UI_ClearDisplay(2);
		}
#endif
		UI_EndCall();
		break;

	case UI_CALL_ENDED:
		_iDisplayTimer = 0; /* Set display timer to make sure received text messages during call and previously displayed texts are gone */
		UdpDebugIDS(IDS_PLEASE_HANGUP);
		if (sParam <= CALL_END_XFER_DONE)
		{
			sParam += IDS_UNKNOWN_REASON;
		}
		else
		{
			sParam = IDS_UNKNOWN_REASON;
		}
		UdpDebugIDS((UCHAR)sParam);
#ifndef OEM_2KLABS
		DisplayString(IDS_PLEASE_HANGUP, 0);
#ifdef OEM_IP20
		if (sParam != 25) 				//IDS_PEER_HANGUP (25) show talking time at the end
			DisplayString((UCHAR)sParam, 1);
		UI_ClearDisplay(3);	
#else
		DisplayString((UCHAR)sParam, 1);
		UI_ClearDisplay(2);
#endif
#endif
		UI_EndCall();
#ifdef OEM_IP20
		if (_bSpeaker )	
		{
			if (sParam == 25 && sParam != 20)		OnSpeaker();
		}
#endif
		break;

	case UI_CALL_XFERFAIL:
		UdpDebugIDS(IDS_XFER_FAILED);
		DisplayString(IDS_XFER_FAILED, 0);
		break;

	case UI_CALL_CALLING:
#ifndef OEM_2KLABS
#ifdef OEM_IP20
		DisplayString(IDS_CALLING, 1);
		LcdDisplay(pParam, 2);
		UI_ClearDisplay(3);
#else
		DisplayString(IDS_CALLING, 0);
		LcdDisplay(pParam, 1);
#endif
	
#endif
		UdpDebugIDS(IDS_CALLING);
		UdpDebugString(pParam);
		if (strlen(pParam))
		{
#ifdef OEM_BT2008
			_iRedialTimer = 0;
#endif
			strcpy(_pRedialBuf, pParam);
#ifndef OEM_IP20
			TaskCallLogAdd(pParam, pParam, CALL_LOG_TYPE_DIALED);
#endif
#ifdef OEM_IP20
			IP20_LED_On(pParam);
#endif
		}
		break;

	case UI_CALL_PLAYKEYS:
		LcdDisplay(pParam, 1);
		DspSetOperation(DSP_OP_DTMFPLAY, DSP_OP_DTMFPLAY);
		UI_PlayKeys(pParam);
		break;

	case UI_CALL_PLAYKEY:
#if defined VER_AR168R && defined OEM_ROIP
#else
		UI_PlayKey((UCHAR)sParam, TaskGetCallInfo(CALL_INFO_STATE));
#endif
		break;

	case UI_CALL_INCOMING_DTMF:
//		UdpDebugChar((UCHAR)sParam);
#if defined VER_AR168R && defined OEM_ROIP
		RoIP_Key((UCHAR)sParam);
#else
		DtmfPlayKey((UCHAR)sParam);
		DspSetOperation(DSP_OP_DTMFPLAY_ENCODE, DSP_OP_DECODEPLAY_ENCODE);
#endif
		break;

	case UI_CALL_TIMER:
#ifndef OEM_2KLABS
		if (Sys_iCurMode == SYS_MODE_CALL)
		{
			if (!_bMute && !_iDisplayTimer)
			{
#ifdef OEM_IP20
				DisplayCallTime((CALL_TIME *)sParam, 1);
#else
				DisplayCallTime((CALL_TIME *)sParam, 0);
#endif
			}
		}
#endif
		break;

	case UI_CALL_HANGUP:
		UI_StopCodec();
		if (_bSpeaker)
		{
			SpeakerPhone(FALSE);
		}
		Sys_iCurMode = SYS_MODE_STANDBY;
		LED_Off(LED_LCD);
		_bAutoAnswer = FALSE;
		_bMute = FALSE;
		_ClearCallStatus();
		UI_StandbyDisplay();
		break;

	case UI_VOICE_MESSAGE:
		UdpDebugIDS(IDS_NEW_MESSAGE);
		_iNewMsg = pParam[1];
		_iOldMsg = pParam[0];
#ifdef OEM_IP20
		voice_message = (_iNewMsg) ? 1 : 0;
#endif
		UI_StandbyDisplay();
		break;

	case UI_CALL_DISPLAY_MSG:
		pLine = (PCHAR *)sParam;
		pCur = pLine[0];
#ifdef SERIAL_UI
		SerialUI_DisplayMsg(pCur);
#endif
		pString[DISPLAY_MAX_CHAR] = 0;
		i = 0;
		iLine = 0;
		while (pCur && iLine < DISPLAY_MAX_LINE)
		{
			iLen = strlen(pCur);
			if (iLen)
			{
				if (iLen > DISPLAY_MAX_CHAR)
				{
					memcpy(pString, pCur, DISPLAY_MAX_CHAR);
					pCur += DISPLAY_MAX_CHAR;
				}
				else
				{
					strcpy(pString, pCur);
					i ++;
					pCur = pLine[i];
				}
				LcdDisplay(pString, iLine);
				iLine ++;
			}
			else
			{
				i ++;
				pCur = pLine[i];
			}
		}
		if (iLine)
		{
			UI_ClearDisplay(iLine);
		}
		_iDisplayTimer = DISPLAY_TTL_CALL_MSG;	/* added due to escalating text message display during call */
		break;

	case UI_CALL_WAITING:
		_iDisplayTimer = 0; /* Set display timer to make sure received text messages during call are gone */
		DisplayString(IDS_CALL_WAITING, 0);
		iTemp = DspGetOperation();
		strcpy(pString, "##");
		DspSetOperation(DSP_OP_DTMFPLAY_ENCODE, DSP_OP_DTMFPLAY_ENCODE);
		UI_PlayKeys(pString);
		DspSetOperation(iTemp, iTemp);
		break;
	}
}

void OnHandsetDown()
{
	if (_bHandsetUp)
	{
		_bHandsetUp = FALSE;
		UdpDebugString("Handset down");
		if (!_bSpeaker)
		{
			UI_HandsetDown();
		}
	}
}

#endif	// CALL_NONE

#if !defined CALL_NONE && !defined SYS_PROGRAMMER

#ifdef OEM_IP20
void AllDispChar()
{
	UCHAR pBuf[DISPLAY_MAX_CHAR];
	UCHAR i;

//	memset(pBuf, 0, DISPLAY_MAX_CHAR);
	memset(pBuf, LCD_STATUS_CHAR, DISPLAY_MAX_CHAR);
	for (i = 0; i < DISPLAY_MAX_LINE; i ++)
	{
		LcdDisplay(pBuf, i);
	}
	Test2 = TRUE;
}

void LedTest(UCHAR chn1, UCHAR chn2)
{
	sum1 = (sum1) | (~chn1);
	sum2 = (sum2) | (~chn2);
		
	IP20_LedControl(~sum1,~sum2);
		
	if (sum1 == 0xFF && sum2 == 0x7F)
	{
		sum1 = 0x00;
		sum2 = 0x00;	
		LcdStart();
		DisplayString(IDS_LEDLER, 1);
		_display_ascii('T', 0, 1);
		DisplayString(IDS_YAN_OR, 2);
		DisplayString(IDS_EVET, 3);
		Test = TRUE ;
	}
}

void UI_TestFunctionKeys(UCHAR iKey)
{
	UCHAR i;

	if (Test2 == 0)
	{
		switch (iKey)
		{
		case 'e':
			if (Test == 1)
			{
				LcdStart();
				DisplayString(IDS_EKRANDAKI, 0);
				DisplayString(IDS_KARAKTERLER_DOLU, 1);
				DisplayString(IDS_1E_DE, 2);
				DisplayString(IDS_ILERI, 3);
				Test1 = TRUE;
				Test = FALSE;
				Test_Result ++; 
			}
			else
			{
				for (i = 2; i <= 3; i ++)
				{
					_display_ascii(' ', i, 3);
				}
			}
			break;

		case 'k':
			for (i = 6; i <= 7; i ++)
			{
				_display_ascii(' ', i, 3);
			}
			break;

		case 'j':
			if (Test1 == 1)
			{
				LcdStart();
				AllDispChar();
			}
			else
			{
				for (i = 11; i <= 12; i ++)
				{
					_display_ascii(' ', i, 3);
				}
			}
			break;

		case 'f':
			if (Test == 1)
			{
				LcdStart();
				DisplayString(IDS_EKRANDAKI, 0);
				DisplayString(IDS_KARAKTERLER_DOLU, 1);
				DisplayString(IDS_1E_DE, 2);
				DisplayString(IDS_ILERI, 3);

				Test1 = TRUE;
				Test = FALSE;
			}
			else
			{
				for (i = 16; i <= 17; i ++)
				{
					_display_ascii(' ', i, 3);
				}
			}
			break;

		case 'm':
			for (i = 0; i <= 3; i ++)
			{
				_display_ascii(' ', i, 0);
			}
			break;

		case 'a':
			for (i = 5; i <= 9; i ++)
			{
				_display_ascii(' ', i, 0);
			}
			break;

		case 'g':
			for (i = 11; i <= 14; i ++)
			{
				_display_ascii(' ', i, 0);
			}
			break;

		case 'd':
			for (i = 16; i <= 19; i ++)
			{
				_display_ascii(' ', i, 0);
			}
			break;

		case 'n':
			for (i = 0; i <= 4; i ++)
			{
				_display_ascii(' ', i, 1);
			}
			break;

		case 'b':
			for (i = 6; i <= 11; i ++)
			{
				_display_ascii(' ', i, 1);
			}
			break;

		case 'c':
			for (i = 13; i <= 17; i ++)
			{
				_display_ascii(' ', i, 1);
			}
			SpeakerPhone(TRUE);
			break;

		case 'i':
			for (i = 0; i <= 6; i ++)
			{
				_display_ascii(' ', i, 2);
			}
			break;

		case 'h':
			for (i = 8; i <= 12; i ++)
			{
				_display_ascii(' ', i, 2);
			}
			break;

		case 'l':
			for (i = 14; i <= 17; i ++)
			{
				_display_ascii(' ', i, 2);
			}
			LED_On(LED_MUTE);
			break;

		case 'o':
			LED_On(LED_MESSAGE);
			LedTest(~0x00,~0x00);
			break;

		case 'D':
			LedTest(ledMap[0][0],ledMap[0][1]);
			break;

		case 'A':
			LedTest(ledMap[1][0],ledMap[1][1]);
			break;

		case 'B':
			LedTest(ledMap[2][0],ledMap[2][1]);
			break;

		case 'p':
			LedTest(ledMap[3][0],ledMap[3][1]);
			break;

		case 'q':
			LedTest(ledMap[4][0],ledMap[4][1]);
			break;

		case 'r':
			LedTest(ledMap[5][0],ledMap[5][1]);
			break;

		case 's':
			LedTest(ledMap[6][0],ledMap[6][1]);
			break;

		case 't':
			LedTest(ledMap[7][0],ledMap[7][1]);
			break;

		case 'u':
			LedTest(ledMap[8][0],ledMap[8][1]);
			break;

		case 'v':
			LedTest(ledMap[9][0],ledMap[9][1]);
			break;

		case 'w':
			LedTest(ledMap[10][0],ledMap[10][1]);
			break;

		case 'x':
			LedTest(ledMap[11][0],ledMap[11][1]);
			break;

		case 'y':
			LedTest(ledMap[12][0],ledMap[12][1]);
			break;

		case 'P':
			LedTest(ledMap[13][0],ledMap[13][1]);
			break;

		case 'Q':
			LedTest(ledMap[14][0],ledMap[14][1]);
			break;
		}	//switch
	}	//if (Test2 == 0)
}
#endif

void UI_HandsetUp(BOOLEAN bSpeaker)
{
	if (Sys_iCurMode == SYS_MODE_MENU || Sys_iCurMode == SYS_MODE_STANDBY)
	{
		// Exit Menu
		UI_ChangeMode(SYS_MODE_CALL);
	}

	if (Sys_iCurMode != SYS_MODE_CALL)		return;

	LED_On(LED_ACTIVE);
	LED_On(LED_RINGING);

	if (bSpeaker)
	{
		if (!_bHandsetUp)
		{
			TaskHandleEvent(UI_EVENT_HANDSET_UP, 0);
		}

#if defined VER_YWH201 || defined VER_BT2008	// avoid hardware handfree noise
		Delay(450);
#endif
	}
	else
	{
		if (_bSpeaker)
		{
			TaskHandleEvent(UI_EVENT_HOLD, 1);
		}
		else
		{
			TaskHandleEvent(UI_EVENT_HANDSET_UP, 0);
		}
	}
	SpeakerPhone(bSpeaker);
}

// 4x4 keys
#ifdef OEM_IP20

void OnCall()
{
	if (Sys_iCurMode == SYS_MODE_MENU)			//MENU Mode (Select)
	{
		OnOK();
	}
	else if (Sys_iCurMode == SYS_MODE_CALL) 	//Handset up or speaker on (Call)
	{
		if (Sys_iCurMode != SYS_MODE_CALL)	return;
		
		if (Kapat)
		{
			if(_bSpeaker)	OnSpeaker();
			OnHandsetDown();
			Kapat = 0;					
		}	
		else if (Reddet)
		{
			OnMute();
			Reddet = 0;
		}	
		else
		{
			UI_BuildNumber(_pKeyBuf);
			//UI_PlayDialTone(); //Added for dialtone for none registered client call
			TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
			KeyClear();		
		}
	}
	else if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		if (_pKeyBuf[0] != 0)
		{								//STNDBY Mode and number entered (Call)
			UCHAR _pKeyBufx[MAX_KEYBUF_LEN+1];		
			strcpy (_pKeyBufx,_pKeyBuf);	
						
			UI_ChangeMode(SYS_MODE_CALL);			//UI_PrepareCall();
			TaskHandleEvent(UI_EVENT_HANDSET_UP, 1);
			SpeakerPhone(TRUE);
			
			strcpy (_pKeyBuf,_pKeyBufx);	
			TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
			KeyClear();
		}
		else
		{											//STNDBY Mode and number is not entered (Menu On)
			OnMenu();
		}
	}
	else if (Sys_iCurMode == SYS_MODE_ALARM)				//Menu>Settings>Alarm (Set Alarm)
	{		
		_iAlarmTimer = 9 ;
	}
}

#else

void OnCall()
{
	UdpDebugString("Call");

	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		KeyClear();
		UI_ChangeMode(SYS_MODE_MENU);
		TaskMenuEvent(UI_EVENT_CALL);
	}
	else if (Sys_iCurMode == SYS_MODE_MENU)
	{
		OnOK();
	}
	else if (Sys_iCurMode == SYS_MODE_CALL)
	{
		UI_BuildNumber(_pKeyBuf);
		TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
	}
}

#endif

void OnFlash()
{
#ifndef OEM_2KLABS
	UdpDebugString("Flash");
	if (Sys_iCurMode != SYS_MODE_CALL)	return;
#ifdef OEM_IP20
	Kapat = 0;
	KeyClear();
#endif

	TaskHandleEvent(UI_EVENT_FLASH, 0);
#endif
}

void OnRedial()
{
	UdpDebugString("Redial");
	UI_MemCall(_pRedialBuf);
}

#if defined VER_AR168M && defined OEM_BT2008
void OnSpeaker(BOOLEAN bSpk)
{
	UdpDebugString("Speaker");
	UI_StopAlarm();

	if (bSpk)
	{
		UI_HandsetUp(TRUE);
	}
	else
	{
		if (!_bHandsetUp)
		{	// hang up phone
			UI_StopCodec();
			SpeakerPhone(FALSE);
			UI_HandsetDown();
		}
		else
		{
			SpeakerPhone(FALSE);
		}
	}
}
#else
void OnSpeaker()
{
	UdpDebugString("Speaker");
#if defined OEM_BT2008 || defined OEM_IP20
	UI_StopAlarm();
#endif
	if (_bSpeaker)
	{
		if (!_bHandsetUp)
		{	// hang up phone
			UI_StopCodec();
			SpeakerPhone(FALSE);
			UI_HandsetDown();
		}
		else
		{
			SpeakerPhone(FALSE);
		}
	}
	else
	{
		UI_HandsetUp(TRUE);
	}
}
#endif

// Other keys
void OnHook()
{
	if (!_bHandsetUp)
	{
		_bHandsetUp = TRUE;
		UdpDebugString("Handset up");
#ifdef OEM_BT2008
		UI_StopAlarm();
#endif
		UI_HandsetUp(FALSE);
	}
}

void OnHold()
{
#ifndef OEM_2KLABS
	UdpDebugString("Hold");
#ifdef VER_FWV2800
	if (Sys_iCurMode == SYS_MODE_CALL)
	{
		TaskHandleEvent(UI_EVENT_FLASH, 0);
	}
	else if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_LEFT);
	}
#else
	if (Sys_iCurMode == SYS_MODE_CALL)
	{
		TaskHandleEvent(UI_EVENT_HOLD, 0);
	}
#endif
#endif
}

void OnMute()
{
#ifndef OEM_2KLABS
#ifdef OEM_BT2008
#ifndef OEM_IVR
	UCHAR pString[16];
#endif
#endif

	UdpDebugString("Mute");
	if (Sys_iCurMode == SYS_MODE_CALL)
	{
		if (TaskGetCallInfo(CALL_INFO_STATE) == CALL_STATE_TALKING)
		{
			_bMute = 1 - _bMute;
			DisplayString(_bMute ? IDS_CALL_MUTED : IDS_CONNECTED, 0);
			CodecMicMute(_bMute);
			LED_SetState(LED_MUTE, _bMute);
		}
#ifndef OEM_BT2008
		else
		{
			TaskHandleEvent(UI_EVENT_MUTE_DND, 0);
		}
#endif
	}
#ifndef OEM_BT2008
	else 
	{
		OnDND();
	}
#endif

#ifdef OEM_BT2008
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
#ifdef OEM_IVR
		if (_bSpeaker)
		{
			SpeakerPhone(FALSE);
			UI_StopCodec();
		}
		else
		{
			SpeakerPhone(TRUE);
			PrepareIVR(IVR_TYPE_IP);
			UI_StartCodec(VOICE_CODER_ULAW, FALSE, DSP_OP_IVRPLAY);
		}
#else
		ip2str(Sys_pIpAddress, pString);
		LcdDisplay(pString, 0);
#endif
	}
#endif
#endif
}

#ifdef IP_KEY
void OnIpKey()
{
	UCHAR pString[16];

	ip2str(Sys_pIpAddress, pString);
	UdpDebugString(pString);

	PrepareIVR(IVR_TYPE_IP);
	UI_StartCodec(VOICE_CODER_ULAW, FALSE, DSP_OP_IVRPLAY);
}
#endif


void OnMenu()
{
#ifndef OEM_2KLABS
	UdpDebugString("Menu");
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		KeyClear();
#ifdef SYS_TEST
		UI_TestInit();
#endif
		UI_ChangeMode(SYS_MODE_MENU);
		TaskMenuEvent(UI_EVENT_MENU);
	}
	else if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_OK);
	}
#endif
}

void _DisplayVol(UCHAR iVol)
{
	if (_bSpeaker)
	{
		Sys_iSpeakerVol = iVol;
	}
	else
	{
		Sys_iHandsetVol = iVol;
	}
#ifdef OEM_IP20
	Not_refresh = 1;
#endif
	iVol ++;
	iVol >>= 1;
	DisplayProgress(iVol, 1);
}

void OnVolDown()
{
	UdpDebugString("Vol-/Down");
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_DOWN);
	}
	else if (Sys_iCurMode == SYS_MODE_STANDBY || Sys_iCurMode == SYS_MODE_CALL)
	{
		_iDisplayTimer = DISPLAY_TTL;
		DisplayString(IDS_VOLUME_DEC, 0);
		_DisplayVol(CodecDecVol());
#ifndef OEM_IP20
		UI_ClearDisplay(2);
#endif
	}
}

void OnVolUp()
{
	UdpDebugString("Vol+/Up");
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_UP);
	}
	else if (Sys_iCurMode == SYS_MODE_STANDBY || Sys_iCurMode == SYS_MODE_CALL)
	{
		_iDisplayTimer = DISPLAY_TTL;
		DisplayString(IDS_VOLUME_INC, 0);
		_DisplayVol(CodecIncVol());
#ifndef OEM_IP20
		UI_ClearDisplay(2);
#endif
	}
}

void _BackSpace()
{
	UCHAR iLen;

	iLen = strlen(_pKeyBuf);
	if (iLen)
	{
		iLen --;
		_pKeyBuf[iLen] = 0;
		_DisplayKey();
	}
	_iKeyTimer = 0;
}

#ifdef OEM_IP20
void OnCancel()
{
	UCHAR pDispName[32];
	
	UdpDebugString("Cancel");
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_CANCEL);
	}
	else if (Sys_iCurMode == SYS_MODE_CALL)
	{						
		if (Sessiz)
		{
			CodecSetVol(0);
			Sessiz = 0;
		}			
		else if (_pKeyBuf[1] != 0)
		{
			_BackSpace();
		}
		else if (_pKeyBuf[0] != 0)
		{
			_BackSpace();
			DisplayClear(3);
		}
	}
	else if (Sys_iCurMode == SYS_MODE_STANDBY) 
	{
		_display_ascii(' ', 11, 3);			

		if (voice_message == 1 && _iNewMsg)
		{
 			OnMessage();
		}	
		else if (Sys_iMissedCalls && _pKeyBuf[0] == 0)
		{			
			OnMenu();
			TaskMenuEvent(UI_EVENT_OK);
			TaskMenuEvent(UI_EVENT_OK);
			voice_message = 1;
		}
		else if (_pKeyBuf[0] == 0)
		{		
			OnMenu();
			TaskMenuEvent(UI_EVENT_UP);
			TaskMenuEvent(UI_EVENT_UP);
			TaskMenuEvent(UI_EVENT_OK);				
		}
		else if (_pKeyBuf[1] != 0)
		{
			_BackSpace();
		}
		else if (_pKeyBuf[0] != 0)
		{
			_BackSpace();
			if (Sys_iMissedCalls)
			{
				DisplayLogStandby();
				DisplayString(IDS_DETAY, 3);
			}
			else
			{
				strcpy(pDispName, Sys_pUserName);
				LcdDisplay(pDispName, 2);
				DisplayString(IDS_OK, 3);
			}
		}
		call_stat = 0;
	}
	else
	{
		_iAlarmPos = 0 ;
		_iAlarmTimer = 9 ; 	
	}
}

#else
void OnCancel()
{
	UdpDebugString("Cancel");
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_CANCEL);
	}
	else
	{
		_BackSpace();
		if (Sys_iCurMode == SYS_MODE_CALL)
		{
			TaskHandleEvent(UI_EVENT_MUTE_DND, 0);
		}
	}
}
#endif


void OnRight()
{
	UdpDebugString("Right");
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
#ifdef OEM_IP20
		TaskMenuEvent(UI_EVENT_UP);
#else
		TaskMenuEvent(UI_EVENT_RIGHT);
#endif
	}
}

void OnOK()
{
	UdpDebugString("OK");
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_OK);
	}
}

void OnLeft()
{
	UdpDebugString("Left");
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
#ifdef OEM_IP20
		TaskMenuEvent(UI_EVENT_DOWN);
#else
		TaskMenuEvent(UI_EVENT_LEFT);
#endif
	}
}

void OnMessage()
{
#ifndef OEM_2KLABS
	UCHAR pNumberName[MAX_USER_NUMBER_LEN + MAX_USER_NUMBER_LEN];

	UdpDebugString("Message");
	OptionsGetString(pNumberName, OPT_MESSAGE_NUMBER, MAX_USER_NUMBER_LEN);
	UI_MemCall(pNumberName);
#endif
}

void OnTransfer()
{
#ifndef OEM_2KLABS
	UdpDebugString("Transfer");

	if (Sys_iCurMode == SYS_MODE_CALL)
	{
#ifdef OEM_IP20
		TaskHandleEvent(UI_EVENT_TRANSFER, (USHORT)_pKeyBuf);
#else
		TaskHandleEvent(UI_EVENT_TRANSFER, 0);
#endif
	}
#ifdef VER_FWV2800
	else if (Sys_iCurMode == SYS_MODE_MENU)
	{
		TaskMenuEvent(UI_EVENT_RIGHT);
	}
#endif
#endif
}

void OnConf()
{
	UdpDebugString("Conf");
}

#ifdef OEM_BT2008
void OnWake()
{
	UCHAR pString[16];

	UdpDebugString("Wake");

	if (_iAlarmState == ALARM_STATE_IDLE)
	{
		_iAlarmState = ALARM_STATE_ENTER_TIME;
		_iAlarmPos = 0;
		_iAlarmTimer = 0;
		TaskLoadString(IDS_ALARM_BUF, _pAlarmBuf);
		DisplayString(IDS_ALARM_TIME, 0);
		LcdDisplay(_pAlarmBuf, 1);
		LcdCursor(1, 0, TRUE);
	}
	else
	{
		LcdCursor(1, 0, FALSE);
		_iAlarmHour = atoi(_pAlarmBuf, 10);
		_iAlarmMin = atoi((PCHAR)(_pAlarmBuf+3), 10);
		if (_iAlarmPos == 0 || _iAlarmHour > 24 || _iAlarmMin >= 60)
		{
			TaskLoadString(IDS_ALARM_CANCELLED, pString);
			UI_AlarmInit();
		}
		else
		{
			TaskLoadString(IDS_ALARM_AT, pString);
			strcat(pString, _pAlarmBuf);
			_iAlarmState = ALARM_STATE_IDLE;
		}
		LcdDisplay(pString, 0);
		DisplayClear(1);
		Delay(4000);
		LED_Off(LED_LCD);
		UI_StandbyDisplay();
	}
}
#endif

#ifdef OEM_IP20
void OnWake()
{
    if (Sys_iCurMode == SYS_MODE_MENU)	
	{
		Sys_iCurMode = SYS_MODE_ALARM;
		_iAlarmState = ALARM_STATE_ENTER_TIME;
		_iAlarmPos = 0;
		_iAlarmTimer = 0;
		TaskLoadString(IDS_ALARM_BUF, _pAlarmBuf);
		DisplayString(IDS_ALARM_ZAMAN, 1);
		LcdDisplay(_pAlarmBuf, 2);
		LcdCursor(2, 0, TRUE);
			
	}
}

void OnWakeOK()
{
	UCHAR pString[16];
		
	LcdCursor(2, 0, FALSE);
	_iAlarmHour = atoi(_pAlarmBuf, 10);
	_iAlarmMin = atoi((PCHAR)(_pAlarmBuf+3), 10);
	if (_iAlarmPos == 0 || _iAlarmHour > 24 || _iAlarmMin >= 60)
	{
		TaskLoadString(IDS_ALARM_CANCELLED, pString);
		UI_AlarmInit();
	}
	else
	{
		TaskLoadString(IDS_ALARM_AT, pString);
		strcat(pString, _pAlarmBuf);
		_iAlarmState = ALARM_STATE_IDLE;
	}
	LcdDisplay(pString, 1);
	DisplayClear(2);
	DisplayClear(3);
	Delay(2000);
	UI_ChangeMode(SYS_MODE_STANDBY);
}
#endif

void OnDND()
{
	UdpDebugString("DND");
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		Sys_bDnd = 1 - Sys_bDnd;
		LED_SetState(LED_MUTE, Sys_bDnd);
		UI_StandbyDisplay();
	}
#ifdef OEM_BT2008
	else if (Sys_iCurMode == SYS_MODE_CALL)
	{
		TaskHandleEvent(UI_EVENT_MUTE_DND, 0);
	}
#endif
}

#ifdef OEM_IP20
void DisplayIDSVal(USHORT sIDS, USHORT sVal, UCHAR iLine) 
{
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

	TaskLoadString(sIDS, pBuf);
	itoa(sVal, (PCHAR)(pBuf+strlen(pBuf)), 10);
	LcdDisplay(pBuf, iLine);
}
#endif

void OnMem(UCHAR iIndex)
{
#ifdef OEM_2KLABS
	iIndex;
	return;
#else
	UCHAR pNumberName[MAX_USER_NUMBER_LEN + MAX_USER_NAME_LEN];
#ifdef OEM_BT2008		// if press speeddial, dial the number directly
	USHORT i,j;
#endif

#ifdef OEM_IP20
	Interdigit = 0;
#endif

#ifdef VER_YWH201
	if (iIndex > 2)		return;
#endif

/*
#ifdef VER_GF302
	if (iIndex > 3)		return;
#endif
*/
	iIndex ++;
	UdpDebugString("Memory Dial");
	UdpDebugVal(iIndex, 10);

	GetPhoneBookNumber(iIndex, pNumberName);
	GetPhoneBookName(iIndex, (PCHAR)(pNumberName+MAX_USER_NUMBER_LEN));
#ifdef OEM_IP20
	Not_refresh = 1;
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		DisplayIDSVal(IDS_MEMORY_DIAL, iIndex, 0);
//		LcdDisplay((PCHAR)(pNumberName+MAX_USER_NUMBER_LEN), 1);
//		LcdDisplay(pNumberName, 2);
		if (strlen((PCHAR)(pNumberName+MAX_USER_NUMBER_LEN)))
		{
			LcdDisplay((PCHAR)(pNumberName+MAX_USER_NUMBER_LEN), 1);
		}
		else
		{
			DisplayString(IDS_EMPTY_STRING, 1);
		}

		if (strlen(pNumberName))
		{
			LcdDisplay(pNumberName, 2);
		}
		else
		{
			DisplayString(IDS_EMPTY_STRING, 2);
		}

		strcpy(_pKeyBuf,pNumberName);

		if(_pKeyBuf[0] != 0)
		{
			DisplayString(IDS_SIL_ARA, 3);
		}
		else
		{
			DisplayString(IDS_EMPTY_NUMBER, 1);
			UI_ClearDisplay(3);				
		}
	}
	else
	{
		UI_MemCall(pNumberName);
	}
#else
#ifdef OEM_BT2008		// if press speeddial, dial the number directly
	OnSpeaker();
	for (i = 0; i < 20000; i ++)
	{
		for(j = 0; j < 50; j ++)
		{
		}
	}
#endif
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
//		LcdDisplay((PCHAR)(pNumberName+MAX_USER_NUMBER_LEN), 0);
//		LcdDisplay(pNumberName, 1);
		if (strlen((PCHAR)(pNumberName+MAX_USER_NUMBER_LEN)))
		{
			LcdDisplay((PCHAR)(pNumberName+MAX_USER_NUMBER_LEN), 0);
		}
		else
		{
			DisplayString(IDS_EMPTY_STRING, 0);
		}
		if (strlen(pNumberName))
		{
			LcdDisplay(pNumberName, 1);
		}
		else
		{
			DisplayString(IDS_EMPTY_STRING, 1);
		}
	}
	else
	{
		UI_MemCall(pNumberName);
	}
#endif
#endif
}

void _FunctionKeys(UCHAR iKey)
{
#ifdef OEM_IP20
	if (Sys_iCurMode == SYS_MODE_TEST)
	{
		UI_TestFunctionKeys(iKey);
		return;
	}
#endif

#ifdef SYS_TEST
	if (Sys_iCurMode == SYS_MODE_MENU && _iTestItem == TEST_ITEM_KEY)
	{
		UI_TestKeys(iKey);
		return;
	}
#endif

#ifdef OEM_IP20
	switch (iKey)
	{
	case 'a':
		if (Sys_iCurMode == SYS_MODE_STANDBY)	//OnCall();
		{
			OnMenu();
			TaskMenuEvent(UI_EVENT_UP);
			TaskMenuEvent(UI_EVENT_UP);
			TaskMenuEvent(UI_EVENT_OK);	
		}
		break;

	case 'b':
		OnRedial();
		break;
		
	case 'c':
		OnSpeaker();
		break;

	case 'd':
		OnFlash();
		break;

	case 'e':		
	 	OnCancel();		
		break;

	case 'f':
		OnCall();
		break;

	case 'g':
		OnMenu();
		break;

	case 'h':
		OnVolUp();
		break;

	case 'i':
		OnVolDown();
		break;

	case 'j':
		OnLeft();
		if (voice_message == 0 && Sys_iCurMode == SYS_MODE_STANDBY)
		{
			voice_message = 1;
			UI_StandbyDisplay();
		}
		else if (voice_message == 1 && Sys_iCurMode == SYS_MODE_STANDBY)
		{
			voice_message=0 ;
			UI_StandbyDisplay();
		}
		break;

	case 'k':
		OnRight();
		break;

	case 'l':
		OnMute();
		break;

	case 'm':
		OnHold();
		break;

	case 'n':
		OnTransfer();
		break;

	case 'o':
		OnMessage();
		break;

	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
		OnMem(iKey - 'p'+3);
		break;

	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
		OnMem(iKey - 'P' + 13);
		break;

	case 'z':
		OnHook();
		break;

	case 'A':
		//OnConf();
		OnMem(iKey - 'A' + 1);
		break;

	case 'B':
		//OnWake();
		OnMem(iKey - 'B' + 2);
		break;

	case 'D':
		//OnDND();
		OnMem(iKey - 'D');
		break;

	case 'Z':
		OnHandsetDown();
		break;
	}

#else

	switch (iKey)
	{
	case 'a':
		OnCall();
		break;

	case 'b':
		OnRedial();
		break;
		
	case 'c':
#if defined VER_AR168M && defined OEM_BT2008		
		OnSpeaker(FALSE);
#else
		OnSpeaker();
#endif
		break;

#if defined VER_AR168M && defined OEM_BT2008		
	case 'C':
		OnSpeaker(TRUE);
		break;
#endif
	
	case 'd':
		OnFlash();
		break;

#ifdef KEY_ADC_2X7
	case 'e':
		OnMenu();
		break;

	case 'f':
		OnVolUp();
		break;

	case 'g':
		OnVolDown();
		break;

	case 'h':
		OnHold();
		break;

	case 'i':
		OnTransfer();
		break;

	case 'j':
		OnCancel();
		break;

	case 'k':
#ifdef VER_GF302
		OnMenu();
#else
		OnRight();
#endif
		break;

	case 'p':
		OnMessage();
		break;

	case 'q':
		OnMute();
		break;

	case 'r':
	case 's':
	case 't':
#ifndef VER_YWH201
	case 'u':
#endif
		OnMem(iKey - 'r');
		break;

#ifdef VER_YWH201
	case 'u':
	case 'v':
		OnHook();
		break;
#endif

/*	case 'v':
		OnHook();
		break;
*/
#else
	case 'e':
		OnCancel();
		break;

	case 'f':
		OnOK();
		break;

	case 'g':
		OnMenu();
		break;

	case 'h':
		OnVolUp();
		break;

	case 'i':
		OnVolDown();
		break;

	case 'j':
		OnLeft();
		break;

	case 'k':
		OnRight();
		break;

	case 'l':
		OnMute();
		break;

	case 'm':
		OnHold();
		break;

	case 'n':
		OnTransfer();
		break;

	case 'o':
		OnMessage();
		break;

	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
		OnMem(iKey - 'p');
		break;

	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
//	case 'M':
//	case 'N':
		OnMem(iKey - 'E' + 10);
		break;

	case 'z':
		OnHook();
		break;

	case 'A':
		OnConf();
		break;

#ifdef OEM_BT2008
	case 'B':
		OnWake();
		break;
#endif

	case 'D':
		OnDND();
		break;

#ifdef KEY_C2
	case 'Y':
#ifdef IP_KEY
		OnIpKey();
#else
		OnToggleCall();
#endif		// !IP_KEY
		break;
#endif

	case 'Z':
		OnHandsetDown();
		break;
#endif	// !KEY_ADC_2X7
	}
#endif	// !OEM_IP20
}

#endif	// !CALL_NONE && !SYS_PROGRAMMER

void _DigitKeys(UCHAR iKey)
{
	UCHAR iLen;
#if !defined CALL_NONE && !defined SYS_PROGRAMMER
	UCHAR iState;
#endif
#ifdef OEM_IP20
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];	
#endif

#ifdef SYS_PROGRAMMER
	if (iKey == (UCHAR)'#')
	{
//		UdpDebugChar(iKey);
		if (!strcmp(_pKeyBuf, "***"))
		{
#ifdef CALL_NONE
			DspTest(RTP_G729);
#else
			RunProgrammer();
#endif
		}
#ifdef CALL_NONE
		else if (!strcmp(_pKeyBuf, "*****"))
		{
			DspTestEcho();
		}
		else if (!strcmp(_pKeyBuf, "****"))
		{
			DspTest(RTP_SPEEX);
		}
		else if (!strcmp(_pKeyBuf, "**"))
		{
			DspTest(RTP_GSM);
		}
		else if (!strcmp(_pKeyBuf, "*"))
		{
			DspTest(RTP_G726_32);
		}
#endif
		KeyClear();
		return;
	}
#endif

#if !defined CALL_NONE && !defined SYS_PROGRAMMER
	if (Sys_iCurMode == SYS_MODE_MENU)
	{
		if (iKey >= (UCHAR)'0' && iKey <= (UCHAR)'9')
		{
			TaskMenuEvent(UI_EVENT_DIGIT0 + iKey - '0');
		}
		else if (iKey == (UCHAR)'*')
		{
			TaskMenuEvent(UI_EVENT_STAR);
		}
		else if (iKey == (UCHAR)'#')
		{
			TaskMenuEvent(UI_EVENT_POUND);
		}
		return;
	}
#ifdef OEM_IP20
	if (Sys_iCurMode == SYS_MODE_ALARM)
	{
		if (_iAlarmState == ALARM_STATE_ENTER_TIME)
		{
			_pAlarmBuf[_iAlarmPos] = iKey;
			_iAlarmPos ++;
			if (_iAlarmPos == 2)
			{
				_iAlarmPos ++;
			}
			LcdDisplay(_pAlarmBuf, 2);
			LcdCursor(2, _iAlarmPos, TRUE);
			if (_iAlarmPos == 5)
			{
				//play out alarm time
				_iAlarmState = ALARM_STATE_SAY_TIME;
			}
			_iAlarmTimer = 0;
		}
		return;
	}
#endif
#ifdef OEM_BT2008
	else if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		if (_iAlarmState == ALARM_STATE_ENTER_TIME)
		{
			_pAlarmBuf[_iAlarmPos] = iKey;
			_iAlarmPos ++;
			if (_iAlarmPos == 2)
			{
				_iAlarmPos ++;
			}
			LcdDisplay(_pAlarmBuf, 1);
			LcdCursor(1, _iAlarmPos, TRUE);
			if (_iAlarmPos == 5)
			{
				OnWake();
			}
			_iAlarmTimer = 0;
			return;
		}
	}
#endif
#endif

	// Save key to buffer
	iLen = strlen(_pKeyBuf);
	if (iLen == MAX_KEYBUF_LEN)
	{
		iLen = 0;
	}
	_pKeyBuf[iLen] = iKey;
	iLen ++;
	_pKeyBuf[iLen] = 0;
	_iKeyTimer = 0;

#if !defined CALL_NONE && !defined SYS_PROGRAMMER
	if (Sys_iCurMode == SYS_MODE_CALL)
	{
		iState = TaskGetCallInfo(CALL_INFO_STATE);
#ifndef OEM_IP20
		if (iKey == (UCHAR)'#' && OptionsGetByte(OPT_POUND_AS_CALL) && iState == CALL_STATE_DIALING)
		{
			UI_PlayKey(iKey, iState);
			iLen --;
			_pKeyBuf[iLen] = 0;		//escape '#'
			UI_BuildNumber(_pKeyBuf);
			TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
			return;
		}
#endif
		if (iState == CALL_STATE_TALKING || iState == CALL_STATE_CALLING2)
		{
			TaskHandleEvent(UI_EVENT_DIGIT, (USHORT)iKey);
		}
		else
		{
			UI_PlayKey(iKey, iState);
			if (iState == CALL_STATE_DIALING)
			{
				if (Sys_bUseDigitmap)
				{
					if (TaskMapMatch(_pKeyBuf))
					{
						UI_BuildNumber(_pKeyBuf);
						TaskHandleEvent(UI_EVENT_CALL, (USHORT)_pKeyBuf);
					}
				}
			}
		}
	}
#endif

#ifdef OEM_IP20
	if ((iKey == (UCHAR)'#'|| iKey == (UCHAR)'*'|| (iKey >= (UCHAR)'2' && iKey <= (UCHAR)'9')) && Sys_iCurMode == SYS_MODE_TEST)
	{
		Sys_iCurMode = SYS_MODE_STANDBY;
		_display_ascii(' ', 6, 3);
		_display_ascii(' ', 7, 3);
		_display_ascii(' ', 11, 3);
		_display_ascii(' ', 12, 3);
		UI_StopCodec();
		SpeakerPhone(FALSE);
		UI_StandbyDisplay();
	}
	else if ((iKey == (UCHAR)'0') && Sys_iCurMode == SYS_MODE_TEST)
	{
		LcdStart();	
		itoa(Test_Result, (PCHAR)(pBuf), 10);
		TaskLoadString(IDS_BA_TEST, (PCHAR)(pBuf + strlen(pBuf)));
		LcdDisplay(pBuf, 2);
	}
	else if ((iKey == (UCHAR)'1') && Sys_iCurMode == SYS_MODE_TEST)
	{
		Test_Result ++; 
		LcdStart();
		itoa(Test_Result, (PCHAR)(pBuf), 10);
		TaskLoadString(IDS_BA_TEST, (PCHAR)(pBuf + strlen(pBuf)));
		LcdDisplay(pBuf, 2);
	}
	else 
	{
		_DisplayKey();
	}
#else
	_DisplayKey();
#endif

#ifdef OEM_IP20
	call_stat = 0;
#endif

#ifndef SYS_PROGRAMMER
#ifdef CALL_NONE
	if (iLen >= 4)
	{
		if (_pKeyBuf[iLen-4] == (UCHAR)'#' && _pKeyBuf[iLen-2] == (UCHAR)'*')
		{
			UI_Test(_pKeyBuf[iLen-3], _pKeyBuf[iLen-1]);
		}
	}

#endif	// CALL_NONE
#ifdef OEM_IP20
	if (Sys_iCurMode == SYS_MODE_STANDBY)
	{
		if (iLen >= 12)
		{
			if (_pKeyBuf[iLen-12] == (UCHAR)'1' && _pKeyBuf[iLen-11] == (UCHAR)'2' && _pKeyBuf[iLen-10] == (UCHAR)'3' && _pKeyBuf[iLen-9] == (UCHAR)'4' && _pKeyBuf[iLen-8] == (UCHAR)'5' && _pKeyBuf[iLen-7] == (UCHAR)'6' && _pKeyBuf[iLen-6] == (UCHAR)'7' && _pKeyBuf[iLen-5] == (UCHAR)'8' && _pKeyBuf[iLen-4] == (UCHAR)'9' && _pKeyBuf[iLen-3] == (UCHAR)'#' && _pKeyBuf[iLen-2] == (UCHAR)'0' && _pKeyBuf[iLen-1] == (UCHAR)'*')
			{
				DisplayString(IDS_PARK_PBOOK, 0);
				DisplayString(IDS_TRANS_REDIAL, 1);
				DisplayString(IDS_VOLDOWN_VOLUP, 2);
				DisplayClear(3);
				_display_ascii(LCD_STATUS_CHAR, 2, 3);
				_display_ascii(LCD_STATUS_CHAR, 3, 3);
				_display_ascii(LCD_STATUS_CHAR, 6, 3);
				_display_ascii(LCD_STATUS_CHAR, 7, 3);
				_display_ascii(LCD_STATUS_CHAR, 11, 3);
				_display_ascii(LCD_STATUS_CHAR, 12, 3);
				_display_ascii(LCD_STATUS_CHAR, 16, 3);
				_display_ascii(LCD_STATUS_CHAR, 17, 3);
				Sys_iCurMode = SYS_MODE_TEST;
				KeyClear();
			}
		}
	}

#endif
#endif	// !SYS_PROGRAMMER
}

void UI_HandlePeerKeys(UCHAR iKey)
{
	UdpDebugString("Peer Dtmf Key");
	UdpDebugVal(iKey, 10);
}

void UI_HandleKeys(UCHAR iKey)
{
	if (!iKey)	return;

	if ((iKey >= (UCHAR)'a' && iKey <= (UCHAR)'z') || (iKey >= (UCHAR)'A' && iKey <= (UCHAR)'Z'))
	{
#if !defined CALL_NONE && !defined SYS_PROGRAMMER
		if (BootFlag(TRUE))
		{
			_FunctionKeys(iKey);
		}
#endif
	}
	else
	{
		_DigitKeys(iKey);
	}
}

void DisplayVal(PCHAR pPrefix, USHORT iVal, UCHAR iLine)
{
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

	strcpy(pBuf, pPrefix);
	itoa(iVal, (PCHAR)(pBuf+strlen(pBuf)), 16);
	LcdDisplay(pBuf, iLine);
}

void DisplayProgress(UCHAR iLen, UCHAR iLine)
{
	UCHAR pBuf[DISPLAY_MAX_CHAR + 1];

	memset(pBuf, 0, DISPLAY_MAX_CHAR + 1);
	if (iLen && iLen <= DISPLAY_MAX_CHAR)
	{
		memset(pBuf, LCD_STATUS_CHAR, iLen);
	}
	LcdDisplay(pBuf, iLine);
}

void DisplayString(UCHAR iIDS, UCHAR iLine)
{
	UCHAR pBuf[DISPLAY_MAX_CHAR + 1];

	TaskLoadString(iIDS, pBuf);
	LcdDisplay(pBuf, iLine);
}

void DisplayClear(UCHAR iLine)
{
	LcdDisplay("", iLine);	
}

void UI_UpdateProgress(UCHAR iPage)
{
	UCHAR iLen;

	UdpDebugString("Updating ...");

	if (iPage < SYSTEM_RINGTONE_PAGE)
	{
		iLen = 0;
	}
	else if (iPage >= SYSTEM_RINGTONE_PAGE && iPage < SYSTEM_HOLDMUSIC_PAGE)
	{
		iLen = iPage - SYSTEM_RINGTONE_PAGE;
	}
	else if (iPage >= SYSTEM_HOLDMUSIC_PAGE && iPage < SYSTEM_FONT_PAGE)
	{
		iLen = iPage - SYSTEM_HOLDMUSIC_PAGE;
	}
	else if (iPage >= SYSTEM_1ST_PAGE && iPage < SYSTEM_2ND_PAGE)
	{
		iLen = iPage - SYSTEM_1ST_PAGE;
	}
	else if (iPage >= SYSTEM_2ND_PAGE && iPage < SYSTEM_PROGRAMMER_PAGE)
	{
		iLen = iPage - SYSTEM_2ND_PAGE;
	}
#ifdef SYS_PROGRAMMER
	else if (iPage >= SYSTEM_PROGRAMMER_PAGE && iPage < SYSTEM_INVALID_PAGE)
	{
		iLen = mod_16x8((iPage + 1), DISPLAY_MAX_CHAR);
	}
#endif
	else	// iPage == SYSTEM_INVALID_PAGE
	{
		iLen = mod_16x8(rand(), DISPLAY_MAX_CHAR);
	}

	DisplayProgress(iLen + 1, 1);
}


void SpeakerControlOn()
{
#ifdef SYS_CHIP_PLUS
#if defined VER_AR168M
	rGPIO_B |= 0x01;	// GPIO_B0 (LED_KEYPAD) high
#else
	rGPIO_A |= 0x01;
#endif
#else
	rGPIO_A &= 0xfe;							// GPIO_A0 control handfree mic
#endif
}

void SpeakerControlOff()
{
#ifdef SYS_CHIP_PLUS
#if defined VER_AR168M
		rGPIO_B &= ~0x01;	// GPIO_B0 (LED_KEYPAD) low
#else
		rGPIO_A &= 0xfe;
#endif
#else
		rGPIO_A |= 0x01;							// GPIO_A0 control handfree mic
#endif
}

UCHAR GetDspPreprocess()
{
	UCHAR iPreProcess;

	iPreProcess = (OptionsGetByte(OPT_AGC)) ? 0x02 : 0;

#ifdef SYS_CHIP_PLUS
	iPreProcess |= 0x80;
#endif

	return iPreProcess;
}

#ifdef KEY_C2
void OnToggleCall()
{
	if (_bHandsetUp)
	{
		OnHandsetDown();
	}
	else
	{
		OnHook();
	}
}
#endif

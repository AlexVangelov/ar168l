/*-------------------------------------------------------------------------
   AR1688 main function copy right information

   Copyright (c) 2006-2013. Lin, Rongrong <woody@palmmicro.com>
                            Tang, Li      <tangli@palmmicro.com>
						    Li, Jing      <lijing@palmmicro.com>

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
#include "apps.h"
#include "core.h"
#include "tcpip.h"
#ifdef CALL_SIP
#include "sip.h"
#endif
#ifdef CALL_IAX2
#include "iax2.h"
#endif
#ifdef OEM_INNOMEDIA
#include "sha1.h"
#endif
#include "bank1.h"
#include "bank2.h"
#include "bank3.h"
#include "bank4.h"
#include "bank5.h"
#include "bank6.h"
#include "bank7.h"

#include "dsp.h"

#ifdef CALL_NONE
const UCHAR _cVersion[14] = {'V', 'e', 'r', ':', ' ', SOFT_VER_HIGH+'0', '.', SOFT_VER_LOW+'0', SOFT_VER_BUILD+'0', '.', ENG_BUILD_HIGH+'0', ENG_BUILD_MID+'0', ENG_BUILD_LOW+'0', 0};
#endif

TIME_T Sys_tmLocal;

#ifdef SYS_IVR_G729
#define IVR_STOP	15360					// font_alice.dat file length	
#define IVR_START	((IVR_STOP * 2) / 5)	// font_alice.dat is a PA1688 ringtone, first 2/5 is G.723.1, rest 3/5 is G.729 format
USHORT _sIVR_Pos;
extern UCHAR _iFramesPerTX;					// in dsp_bank1.c
#endif

#ifdef VER_AR168R
extern UCHAR _iOldCOR;						// in gpio_key.c
#endif

// main function MUST be the first function in this module
void main()
{
	// Init orders are important, do NOT change! FlashInit need to be the first function called
	FlashInit();
	ChipInit();
	heap_init();	// call heap_init() right after FlashInit() and ChipInit;

#ifdef CALL_NONE
	KeyInit();		// detect * key for safe mode below, key.c is always in bank1!
#ifndef SYS_CHIP_PLUS
	rGPIO_A &= 0xfe;	//	GPIO_A0 On
#endif
	// do NOT change this! ar168d/ar168ds use GPIO_A2 as flash CS, need to be 0 to boot from primary flash
	rGPIO_A &= 0xfb;	// GPIO_A2 On
#ifdef RTL8019AS
	Ne2000InitStep1();
#endif
	LcdInit();
	LcdStart();
	LcdDisplay(_cVersion, 0);
#ifdef OEM_LCDTEST
	LcdFactoryTest();
#endif
#ifdef RTL8019AS
	if (DetectSafeRecoveryMode(200)) goto SafeMode;
	Ne2000InitStep2();
	if (DetectSafeRecoveryMode(2250)) goto SafeMode;		// wait 2 seconds for NE2000 cold boot
#elif defined KSZ8842
	if (DetectSafeRecoveryMode(1)) goto SafeMode;
#endif
#ifndef SYS_CHIP_PLUS
	rGPIO_A |= 0x01;	// GPIO_A0 Off
#endif
	JumpMain();	// * key is not pressed, goto main program
SafeMode:
#ifdef RTL8019AS
	Delay(300);
	Ne2000InitStep2();
	Delay(3000);
#endif
	OptionsInit();
#endif
	UI_Init();

	FlashSetBankOffset(NET_BANK_OFFSET);
	NetInit();
	TftpInit();

#ifndef CALL_NONE
	srand(12);
	SntpInit();
	DnsInit();
#ifdef OEM_AUTOTEST
	DhcpdInit();
#endif

#ifdef CALL_SIP
	StunInit();
#endif

	FlashSetBankOffset(TCP_BANK_OFFSET);
	TcpInit();
//	FlashSetBankOffset(HTTP_BANK_OFFSET);
	HttpInit();
#ifdef SYS_WEBCALL
	WebcallInit();
#endif

	FlashSetBankOffset(MENU_BANK_OFFSET);
	CallLogInit();

	FlashSetBankOffset(PROTOCOL_BANK_OFFSET);
#ifdef CALL_SIP
	SipInit();
	FlashSetBankOffset(SIPR_BANK_OFFSET);
	SiprInit();
#elif defined CALL_IAX2
	Iax2Init();
#endif	
	
	if (Sys_iIpType)
	{
		FlashSetBankOffset(NET_BANK_OFFSET);
		NetDiscover();
	}
	else
	{
		TaskStartProv();
		TaskStartApp();
	}
#endif	// CALL_NONE
	
	FlashSetBankOffset(UI_BANK_OFFSET);
#if defined SYS_UART  || defined SYS_UART_POLL
	SerialInit();	// Call SerialInit() before interrupt enabled in UI_Start() 
#endif
	UI_Start();

#ifdef SYS_IVR_G729
	_sIVR_Pos = IVR_START;
#endif

	do
	{
		if (_bTimer)	// 1 second timer
		{
			_bTimer = FALSE;

			FlashSetBankOffset(NET_BANK_OFFSET);
			NetTimer();
			
#ifndef CALL_NONE
			if (Net_bConnected)
			{
				FlashSetBankOffset(TCP_BANK_OFFSET);
				TcpTimer();
//				FlashSetBankOffset(HTTP_BANK_OFFSET);
//				HttpTimer();
#ifdef OEM_INNOMEDIA
				HttpcTimer();
#endif
				HttpdTimer();

				FlashSetBankOffset(PROTOCOL_BANK_OFFSET);
#ifdef CALL_IAX2
				Iax2Timer();
#elif defined CALL_SIP
				SipTimer();
#endif
			}
#endif

			FlashSetBankOffset(UI_BANK_OFFSET);
			UI_Timer();

#ifndef CALL_NONE
			Sys_tmLocal.iSec ++;
			if (Sys_tmLocal.iSec == 60) 
			{
				if (Net_bConnected)
				{
					FlashSetBankOffset(HTTP_BANK_OFFSET);
					HttpcMinuteTimer();
				}

#ifdef OEM_BT2008
				FlashSetBankOffset(MENU_BANK_OFFSET);
				CallLogTimer();
#endif

				FlashSetBankOffset(UI_BANK_OFFSET);
				UI_MinuteTimer();
			}
#endif
		}

		FlashSetBankOffset(UI_BANK_OFFSET);
#if defined SYS_UART  || defined SYS_UART_POLL
		SerialRun();
#endif
		UI_Run();

		FlashSetBankOffset(NET_BANK_OFFSET);
#ifdef RTL8019AS
		Ne2000Run();	// ReadPacket() in BANK2
#elif defined KSZ8842
		Ksz8842Run();	// HandlePacket() in BANK2
#endif

	} while (1);
}

// The following 2 functions need to be in SRAM because FlashInit() calls OptionsGetByte()
// (sOffset, sOffset+iLen) shouldn't cross 0x8000 boundary
void OptionsGetBuf(PCHAR pBuf, USHORT sOffset, UCHAR iLen)
{
	ReadFlashData((USHORT)pBuf, sOffset, iLen, SYSTEM_SETTINGS_PAGE);
}

UCHAR OptionsGetByte(USHORT sOffset)
{
	UCHAR iVal;

	OptionsGetBuf(&iVal, sOffset, 1);
	return iVal;
}


#ifndef CALL_NONE

void TaskFrameTimer()
{
	UCHAR iOrg;

	// remember to set page here before call call-back
	iOrg = FlashSetBankOffset(PROTOCOL_BANK_OFFSET);
#ifdef CALL_IAX2
	Iax2FrameTimer();
#elif defined CALL_SIP
	SipFrameTimer();
#endif
	FlashSetBank(iOrg);
}

void TaskOutgoingData(UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2)
{
	UCHAR iOrg;

#ifdef VER_AR168R
	if (_iOldCOR && Sys_bOptVad)	return;
#endif

//	Sys_sMinuteDebug ++;

#ifdef SYS_IVR_G729
	UCHAR i;

	iOrg = GetExtPage();

	// get g.729 data
	SetExtPage((SYSTEM_IVR_PAGE<<1));
	pBuf1 = pBuf2;
	for (i = 0; i < _iFramesPerTX; i ++)
	{
		memcpy((PCHAR)(pBuf2 + 1), (PCHAR)(BANK_BASE + _sIVR_Pos), G729_FRAME_LEN);
//		*pBuf2 = *((PCHAR)(BANK_BASE + _sIVR_Pos + G729_FRAME_LEN + 1));	// Copy of length is not necessary when VAD is off.
		_sIVR_Pos += G729_FRAME_LEN + 2;	// PA1688 ringtone has extra 2 bytes of length for each G.729 frame.
		pBuf2 += G729_FRAME_LEN + 1;		// AR1688 dsp will add 1 byte of length at the start of each G.729 frame, 
	}
	SetExtPage(iOrg);

//	UdpDebugVals(pBuf1, G729_FRAME_LEN * 2);

	if (_sIVR_Pos >= IVR_STOP)	_sIVR_Pos = IVR_START;	// play it again

	iFrame1 = _iFramesPerTX;
	iFrame2 = 0;
#endif

	// remember to set page here before call call-back
	iOrg = FlashSetBankOffset(PROTOCOL_BANK_OFFSET);
#ifdef CALL_IAX2
	Iax2SendVoice(iFrame1, pBuf1, iFrame2, pBuf2);
#elif defined CALL_SIP
	SipSendVoice(iFrame1, pBuf1, iFrame2, pBuf2);
#endif
	FlashSetBank(iOrg);
}

void TaskIncomingData(USHORT sLen, PCHAR pBuf)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(UI_BANK_OFFSET);

#ifdef VER_AR168R
	RoIP_IncomingData(sLen);
#endif

	DspHandleData(sLen, pBuf);
	FlashSetBank(iOrg);
}

void TaskMiniRun()
{
#ifdef RTL8019AS
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(UI_BANK_OFFSET);
	DspRun(TRUE);
	FlashSetBankOffset(NET_BANK_OFFSET);
	Ne2000MiniRun();
	FlashSetBank(iOrg);
#endif
}

#ifdef OEM_INNOMEDIA
void TaskRC4Init(PCHAR pKey, UCHAR iKeyLen)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(MD5_BANK_OFFSET);
	RC4Init(pKey, iKeyLen);
	FlashSetBank(iOrg);
}

void TaskRC4Calc(PCHAR pDst, PCHAR pSrc, USHORT sLen)
{
	UCHAR iOrg;
	iOrg = FlashSetBankOffset(MD5_BANK_OFFSET);
	RC4Calc(pDst, pSrc, sLen);
	FlashSetBank(iOrg);
}

void TaskGenKey(PCHAR P, PCHAR S, USHORT c, PCHAR DK, UCHAR dkLen)
{
	UCHAR iOrg;
	iOrg = FlashSetBankOffset(MD5_BANK_OFFSET);
	pbkdf2_sha1(P, S, c, DK, dkLen);
	FlashSetBank(iOrg);
}
#endif

void TaskMD5GenValue(PCHAR pDst, PCHAR pSrc, UCHAR iLen)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(MD5_BANK_OFFSET);
	MD5GenValue(pDst, pSrc, iLen);
	FlashSetBank(iOrg);
}

BOOLEAN TaskMapMatch(PCHAR pNumber)
{
	UCHAR iOrg;
	BOOLEAN bRet;

	iOrg = FlashSetBankOffset(DIGITMAP_BANK_OFFSET);
	bRet = MapMatch(pNumber);
	FlashSetBank(iOrg);

	return bRet;
}

// may need to call line_backup() and line_restore before and after calling this function
void TaskHttpDigest(PCHAR pDst, PCHAR pAuthData, PCHAR pUri, PCHAR pAccount, PCHAR pPassword, PCHAR pMethod)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(AUTH_BANK_OFFSET);
	http_digest(pDst, pAuthData, pUri, pAccount, pPassword, pMethod);
	FlashSetBank(iOrg);
}

void TaskStartProv()
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(HTTP_BANK_OFFSET);
	HttpcStart();
	FlashSetBank(iOrg);
}

void TaskStartApp()
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(NET_BANK_OFFSET);
	SntpStart(TRUE);
	FlashSetBank(iOrg);
}

#ifdef CALL_SIP
void TaskStunMap(UCHAR iTask, UDP_SOCKET pSocket)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(NET_BANK_OFFSET);
	StunMap(iTask, pSocket);
	FlashSetBank(iOrg);
}

void TaskStunDone(PCHAR pMappedAddr, USHORT sMappedPort, UCHAR iTask)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(PROTOCOL_BANK_OFFSET);
	SipStunDone(pMappedAddr, sMappedPort, iTask);
	FlashSetBankOffset(NET_BANK_OFFSET);
	StunInit();
	FlashSetBank(iOrg);
}

BOOLEAN TaskStunRun(UDP_SOCKET pUcb)
{
	BOOLEAN bVal;
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(NET_BANK_OFFSET);
	bVal = StunRun(pUcb);
	FlashSetBank(iOrg);

	return bVal;
}
#endif

void TaskHandleEvent(UCHAR iEvent, USHORT sParam)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(PROTOCOL_BANK_OFFSET);
#ifdef CALL_IAX2
	Iax2HandleEvent(iEvent, sParam);
#elif defined CALL_SIP
	SipHandleEvent(iEvent, sParam);
#else
	iEvent;
	sParam;
#endif
	FlashSetBank(iOrg);
}

USHORT TaskGetCallInfo(UCHAR iType)
{
	UCHAR iOrg;
	USHORT sVal;

	iOrg = FlashSetBankOffset(PROTOCOL_BANK_OFFSET);
#ifdef CALL_IAX2
	sVal = Iax2GetCallInfo(iType);
#elif defined CALL_SIP
	sVal = SipGetCallInfo(iType);
#else
	sVal = 0;
#endif
	FlashSetBank(iOrg);

	return sVal;
}

USHORT TaskHttpEvent(UCHAR iEvent, USHORT sParam)
{
	UCHAR iOrg;
	USHORT sVal;

	iOrg = FlashSetBankOffset(MENU_BANK_OFFSET);
	sVal = HttpHandleEvent(iEvent, sParam);
	FlashSetBank(iOrg);

	return sVal;
}

void TaskMenuEvent(UCHAR iEvent)
{
#ifdef SYS_LCD
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(MENU_BANK_OFFSET);
	MenuHandler(iEvent);
	FlashSetBank(iOrg);
#else
	iEvent;
#endif
}

void TaskCallLogAdd(PCHAR pName, PCHAR pNumber, UCHAR iType)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(MENU_BANK_OFFSET);
	CallLogAdd(pName, pNumber, iType);
	FlashSetBank(iOrg);
}

#ifdef SERIAL_UI
void TaskSerialSendString(PCHAR pStr)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(SERIAL_BANK_OFFSET);
	SerialSendString(pStr);
	FlashSetBank(iOrg);
}
#endif

typedef void (*DnsDone)(PCHAR pIP);

// The function calling TaskDnsQuery must be in the same bank as the call back function
void TaskDnsQuery(PCHAR pDomain, UCHAR iType, UCHAR iBankOffset, USHORT sCallBackAddr)
{
	UCHAR iOrg;
	// Can't do any preprocess here. It will cause loop call.
/*	UCHAR pDstIP[IP_ALEN];

	if (strlen(pDomain))
	{
		if (str2ip(pDomain, pDstIP))
		{	// domain name is a string of IP address
			TaskDnsDone(pDstIP, sCallBackAddr, iBankOffset);
		}
		else
		{
			iOrg = FlashSetBankOffset(NET_BANK_OFFSET);
			DnsQuery(pDomain, iType, iBankOffset, sCallBackAddr);
			FlashSetBank(iOrg);
		}
	}
	else
	{
		TaskDnsDone(NULL, sCallBackAddr, iBankOffset);
	}
*/

	iOrg = FlashSetBankOffset(NET_BANK_OFFSET);
	DnsQuery(pDomain, iType, iBankOffset, sCallBackAddr);
	FlashSetBank(iOrg);
}

void TaskDnsDone(PCHAR pIP, USHORT sAddr, UCHAR iBank)
{
	UCHAR iOrg;

	// remember to set page here before call call-back
	iOrg = FlashSetBankOffset(iBank);
	(*(DnsDone)sAddr)(pIP);
	FlashSetBank(iOrg);
}

typedef void (*TcpRunData)(TCP_HANDLE h, USHORT sLen, PCHAR pBuf);

void TaskRunTcp(TCP_HANDLE h, USHORT sLen, PCHAR pBuf, USHORT sAddr, UCHAR iBank)
{
	UCHAR iOrg;

	// remember to set page here before call call-back
	iOrg = FlashSetBankOffset(iBank);
	(*(TcpRunData)sAddr)(h, sLen, pBuf);
	FlashSetBank(iOrg);
}

void TaskTcpRun(PACKET_LIST * p)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(TCP_BANK_OFFSET);
	TcpRun(p);
	FlashSetBank(iOrg);
}

#endif

typedef void (*UdpRunData)(UDP_SOCKET pUcb);

void TaskUdpRunData(UDP_SOCKET pUcb)
{
	UCHAR iOrg;

	// remember to set page here before call call-back
	iOrg = FlashSetBankOffset(pUcb->iBankOffset);
	(*(UdpRunData)pUcb->sCallBack)(pUcb);
	FlashSetBank(iOrg);
}

void TaskIpSendData(USHORT sLen, PCHAR pDstIP, UCHAR iProtocol)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(NET_BANK_OFFSET);
	IpSendData(sLen, pDstIP, iProtocol);
	FlashSetBank(iOrg);
}

void TaskLoadString(UCHAR iIDS, PCHAR pBuf)
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(MENU_BANK_OFFSET);
#ifdef CALL_NONE
	itoa(iIDS, pBuf, 10);
#else
	LoadString(iIDS, pBuf);
#endif
	FlashSetBank(iOrg);
}

void TaskUIHandler(UCHAR iType, USHORT sParam)
{
#ifdef CALL_NONE
	if (iType == UI_CHANGE_MODE)
	{
		Sys_iCurMode = (UCHAR)sParam;
	}
	else if (iType == UI_UPDATE_PROGRESS)
	{
		UI_UpdateProgress((UCHAR)sParam);
	}
#else
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(UI_BANK_OFFSET);
	UI_EventHandler(iType, sParam);
	FlashSetBank(iOrg);
#endif
}

BOOLEAN TaskSystemHandler(UCHAR iType, UCHAR iParam)
{
	UCHAR iOrg;
	BOOLEAN bRet;

	iOrg = FlashSetBankOffset(UI_BANK_OFFSET);
	bRet = SystemHandler(iType, iParam);
	FlashSetBank(iOrg);

	return bRet;
}

#ifndef CALL_NONE
#ifdef SYS_WEBCALL
void TaskWebcallStart()
{
	UCHAR iOrg;

	iOrg = FlashSetBankOffset(HTTP_BANK_OFFSET);
	WebcallStart();
	FlashSetBank(iOrg);
}
#endif
#endif

void delay1ms() __naked
{
	__asm
	ld	b,	#0x04
contiune_delay213:
	// 11
	push	bc  
	// 7
	ld	b,	#0xf7
contiune_delay31:
	// 10 cycles
	ld	hl, #0x4567
	// 13 cycles
	djnz	contiune_delay31
	// 10
	pop		bc
	// change mips delay, 13
	djnz	contiune_delay213
	ret 
	__endasm;
}

void Delay(USHORT sMilliSecond)
{
	USHORT s;

	if (IsHighSpeed())
	{
		sMilliSecond <<= 1;
	}

	for (s = 0; s < sMilliSecond; s ++)
	{
		delay1ms();
	}
}

#ifdef SYS_UART
extern volatile UCHAR Serial_iRecvHead, Serial_iRecvTail;
extern volatile UCHAR Serial_pRecv[SERIAL_BUF_SIZE_RX];

void UART_Isr(UCHAR iVal)
{
	if (Serial_iRecvTail != Serial_iRecvHead)
	{
		Serial_pRecv[Serial_iRecvHead] = iVal;
		Serial_iRecvHead ++;
		if (Serial_iRecvHead == SERIAL_BUF_SIZE_RX)
		{
			Serial_iRecvHead = 0;
		}
	}
}

#endif

#ifdef OEM_IP20

void sampleExec()
{
	UCHAR iOrg;
			
	iOrg = FlashSetBankOffset(UI_BANK_OFFSET);
	OnWake();
	//LcdDisplay("alarm2",2);
	FlashSetBank(iOrg);
}

UCHAR displayOnlineMissed(PCHAR ptr)
{
	UCHAR iOrg;	
	CALL_RECORD * p;
	UCHAR retval = 0;
	PCHAR ptr2;

	iOrg = FlashSetBankOffset(MENU_BANK_OFFSET);
	p = CallLogRead(0, 0);	
		
	if (p)
	{
		ptr2 = &p->iMem;
		strcpy(ptr, (PCHAR)(ptr2 + 1 + strlen(ptr2)));
		strcat(ptr, " <");
		strcat(ptr, ptr2);
		strcat_char(ptr, '>');
		retval = 1;		
	}
	FlashSetBank(iOrg);
	return retval;
}
#endif



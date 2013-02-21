/*-------------------------------------------------------------------------
   AR168R function copy right information

   Copyright (c) 2011-2013. Lin, Rongrong <woody@palmmicro.com>

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
#include "core.h"
#include "apps.h"
#include "bank1.h"
#include "dsp.h"
#include "sfr.h"

#ifdef VER_AR168R

extern BOOLEAN _bLogoned;	// function.c

#define VOICE_LEVEL		1

#ifdef OEM_ROIP
#define KEY_COR_HIGH	'9'
#define KEY_COR_LOW		'8'
#define MAX_TALK_INTERVAL	30
#endif

#define AUTO_DIAL_INTERVAL	30

extern volatile BOOLEAN _bCOR;
extern volatile UCHAR _iGPIO_COR;		// GPIO B2 COR input
UCHAR _iOldCOR;

USHORT _sRtpTotal;
UCHAR _iAutoDialCounter;

#ifdef OEM_ROIP
UCHAR _iMaxTalkCounter;

void _PTT_High()
{
	if (!(rGPIO_B & 0x01))
	{
		rGPIO_B |= 0x01;
		UdpDebugString("PTT HIGH");
	}
}

void _PTT_Low()
{
	if (rGPIO_B & 0x01)
	{
		rGPIO_B &= 0xfe;
		UdpDebugString("PTT LOW");
//		UdpDebugVal(_sRtpTotal, 10);
	}
}

void _COR_High()
{
	if (!Sys_bOptVad)
	{	// Send 'KEY_COR_HIGH' key to inform COR high
		TaskHandleEvent(UI_EVENT_DIGIT, (USHORT)KEY_COR_HIGH);
	}
	CodecMicMute(TRUE);
	CodecSetVol(Sys_iSpeakerVol);
	_iMaxTalkCounter = 0;
	UdpDebugString("COR HIGH");
}

void _COR_Low()
{
	if (!Sys_bOptVad)
	{	// Send 'KEY_COR_LOW' key to inform COR low
		TaskHandleEvent(UI_EVENT_DIGIT, (USHORT)KEY_COR_LOW);
	}
	CodecMicMute(FALSE);
	CodecSetVol(0);
	UdpDebugString("COR LOW");
}
#endif

void RoIP_Start()
{
	_bCOR = FALSE;
	_iOldCOR = 0;
	_sRtpTotal = 0;
	_iAutoDialCounter = 0;
#ifdef OEM_ROIP
	_iMaxTalkCounter = 0;
	_PTT_Low();
#endif

#ifndef LCD_HY1602
	// GPIO G0 as input when LCD not used
	rGPIO_G_Enable &= 0xfe;
	rGPIO_G_Enable |= 0x10;
#endif

	// GPIO B4 output
	rGPIO_B_Output_Enable |= 0x10;	
	rGPIO_B_Input_Enable &= ~0x10;
	rGPIO_B &= ~0x10;	// B4 output low

	// GPIO B5 input
	rGPIO_B_Output_Enable &= ~0x20;	
	rGPIO_B_Input_Enable |= 0x20;

#ifdef OEM_ROIP
	SpeakerRing();
	CodecSetVol(Sys_iSpeakerVol);
#elif defined OEM_UART
	SpeakerOff();
	CodecSetVol(Sys_iHandsetVol);
#endif
}

void RoIP_Run()
{
#ifdef OEM_ROIP
	if (!_bCOR)				return;
	if (_iOldCOR == (_iGPIO_COR & 0x04))	return;
	if (TaskGetCallInfo(CALL_INFO_STATE) != CALL_STATE_TALKING)	return;

	_iOldCOR = _iGPIO_COR & 0x04;
	if (_iOldCOR)
	{
		_COR_High();
	}
	else
	{	// I am talking now.
		_COR_Low();
	}
#endif

	_bCOR = FALSE;
}

void _CheckAutoDial()
{
	if (rGPIO_B & 0x20)
	{	// do self-defined auto dial out when GPIO_B5 HIGH
		if (Sys_iCurMode == SYS_MODE_STANDBY)
		{
			if (!Sys_bAutoAnswer)
			{
				if (_bLogoned)
				{	// automatically dial out every minute.
					OnHook();	
				}
			}
		}
	}
}

/*
void RoIP_MinuteTimer()
{
#ifdef OEM_ROIP
	_CheckAutoDial();
#endif
}
*/

void RoIP_Timer()
{
	BOOLEAN bTalking;

	bTalking = (TaskGetCallInfo(CALL_INFO_STATE) == CALL_STATE_TALKING) ? TRUE : FALSE;
#ifdef OEM_ROIP
	if (bTalking)
	{
		if (_iOldCOR)
		{
			if (Sys_bOptVad)
			{
				if (_sRtpTotal < G729_FRAME_LEN * VOICE_LEVEL)
				{
					_PTT_Low();
				}
				_sRtpTotal = 0;
			}
		}
		else
		{
			_iMaxTalkCounter ++;
			if (_iMaxTalkCounter >= MAX_TALK_INTERVAL)
			{	// talk too much, shut up
				_COR_High();
			}
		}
	}
#endif

	if (!bTalking)
	{
		_iAutoDialCounter ++;
		if (_iAutoDialCounter >= AUTO_DIAL_INTERVAL)
		{
			_iAutoDialCounter = 0;
			_CheckAutoDial();
		}
	}

#ifndef LCD_HY1602
	if (rGPIO_G & 0x01)
	{
		UdpDebugString("GPIO_G0 HIGH");
	}
	else
	{
		UdpDebugString("GPIO_G0 LOW");
	}
#endif
/*
	if (rGPIO_B & 0x20)
	{
		UdpDebugString("GPIO_B5 HIGH");
	}
	else
	{
		UdpDebugString("GPIO_B5 LOW");
	}
*/
}

void RoIP_Key(UCHAR iKey)
{
	UdpDebugChar(iKey);
#ifdef OEM_ROIP
	if (!Sys_bOptVad)
	{
		if (iKey == (UCHAR)KEY_COR_LOW)
		{
			_PTT_High();
			return;
		}
		else if (iKey == (UCHAR)KEY_COR_HIGH)
		{
			_PTT_Low();
			return;
		}
	}
#endif
}

void RoIP_LogonOk()
{
#ifdef OEM_ROIP
	_CheckAutoDial();
#endif
}

void RoIP_Ringing()
{
#ifdef OEM_ROIP
	_PTT_High();
	_iOldCOR = 0;
#endif
}

void RoIP_Dial()
{
	RoIP_Ringing();
}

void RoIP_Connected()
{
#ifdef OEM_ROIP
	_PTT_Low();
#endif
	_iAutoDialCounter = 0;
}

void RoIP_EndCall()
{
	RoIP_Connected();
}

void RoIP_IncomingData(USHORT sLen)
{
#ifdef OEM_ROIP
	if (Sys_bOptVad)
	{
		if (sLen >= G729_FRAME_LEN)
		{
			_PTT_High();
		}
		_sRtpTotal += sLen;
	}
//	UdpDebugVal(sLen, 10);
#else
	sLen;
#endif
}

#endif


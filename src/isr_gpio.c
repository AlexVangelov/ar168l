/*-------------------------------------------------------------------------
   AR1688 ISR GPIO function copy right information

   Copyright (c) 2006-2013. Lin, Rongrong <woody@palmmicro.com>
                            Lin, Guofeng  <alanda@palmmicro.com>

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
#include "core.h"
#include "ar168.h"
#include "apps.h"

#ifdef KEY_4X4
extern volatile UCHAR _p4x4_KeyVals[KEY_4X4_SIZE];
extern volatile BOOLEAN _b4x4_Key;
#endif

#ifdef KEY_5X5
extern volatile UCHAR _p5x5_KeyVals[KEY_5X5_SIZE];
extern volatile BOOLEAN _b5x5_Key;
#endif

#ifdef KEY_5X6
extern volatile UCHAR _p5x6_KeyVals[KEY_5X6_SIZE];
extern volatile BOOLEAN _b5x6_Key;
#endif

#ifdef KEY_7X8
extern volatile UCHAR _p7x8_KeyVals[KEY_7X8_SIZE];
extern volatile BOOLEAN _b7x8_Key;
#endif

#ifdef KEY_8X5
extern volatile UCHAR _p8x5_KeyVals[KEY_8X5_SIZE];
extern volatile BOOLEAN _b8x5_Key;
#endif

#ifdef KEY_ADC
extern volatile UCHAR _pADC_KeyVal[KEY_ADC_STAGE];
extern volatile UCHAR _pADC_KeyVal2[KEY_ADC_STAGE];
extern volatile UCHAR _iADC_KeyStage, _iADC_KeyStage2;
#endif

#ifdef KEY_C2
extern volatile UCHAR _iC2_KeyVal;
extern volatile BOOLEAN _bC2_Key;
#endif

#ifdef VER_AR168R
volatile BOOLEAN _bCOR;
volatile UCHAR _iGPIO_COR;		// GPIO B2 COR input
#endif

#ifdef SERIAL_WDCB
extern volatile BOOLEAN Serial_bSwitchChannel;
extern volatile BOOLEAN Serial_bFree;
volatile UCHAR _iSerialCTC;
#endif

#if defined HOOK_GPIO
volatile BOOLEAN _bHook;
volatile UCHAR _iGPIO_Hook;		// use GPIO C2 as hook when ADC key not used
#endif

volatile UCHAR _iCTC;
volatile BOOLEAN _bTimer;
volatile ULONG Sys_lTicker;
volatile ULONG Sys_lCurrentTime;
volatile UCHAR Sys_iDebugVal;		// temp debug use, report value as soon as none zero in UI_Run();
volatile USHORT Sys_sDebug;			// temp debug use, report non zero value in whole second timer UI_Timer();
volatile USHORT Sys_sMinuteDebug;	// temp debug use, report non zero value in whole minute timer UI_MinuteTimer();

#if defined KEY_5X6 || defined KEY_7X8
UCHAR _Check_B_By_B(UCHAR iAndMask)		// pull low
{
	UCHAR iVal, iOrMask;

	iOrMask = ~iAndMask;
	rGPIO_B_Input_Enable &= iAndMask;
	rGPIO_B_Output_Enable |= iOrMask;	
	rGPIO_B |= iOrMask;
	iVal = rGPIO_B;
	iVal &= iAndMask;
	rGPIO_B &= iAndMask;
	rGPIO_B_Output_Enable &= iAndMask;	
	rGPIO_B_Input_Enable |= iOrMask;

	return iVal;
}
#endif

#ifdef KEY_5X6

UCHAR _Check_D_By_D(UCHAR iAndMask)
{
	UCHAR iVal, iOrMask;

	iOrMask = ~iAndMask;
	rGPIO_D_Input_Enable &= iAndMask;
	rGPIO_D_Output_Enable |= iOrMask;	
	rGPIO_D |= iOrMask;
	iVal = rGPIO_D;
	iVal &= iAndMask;
	rGPIO_D &= iAndMask;
	rGPIO_D_Output_Enable &= iAndMask;	
	rGPIO_D_Input_Enable |= iOrMask;

	return iVal;
}

UCHAR _Check_D_By_B(UCHAR iAndMask)
{
	UCHAR iVal, iOrMask;

	iOrMask = ~iAndMask;
	rGPIO_B_Input_Enable &= iAndMask;
	rGPIO_B_Output_Enable |= iOrMask;	
	rGPIO_B |= iOrMask;
	iVal = rGPIO_D;
	rGPIO_B &= iAndMask;
	rGPIO_B_Output_Enable &= iAndMask;	
	rGPIO_B_Input_Enable |= iOrMask;

	return iVal;
}

UCHAR _Check_B_By_D(UCHAR iAndMask)
{
	UCHAR iVal, iOrMask;

	iOrMask = ~iAndMask;
	rGPIO_D_Input_Enable &= iAndMask;
	rGPIO_D_Output_Enable |= iOrMask;	
	rGPIO_D |= iOrMask;
	iVal = rGPIO_B;
	rGPIO_D &= iAndMask;
	rGPIO_D_Output_Enable &= iAndMask;	
	rGPIO_D_Input_Enable |= iOrMask;

	return iVal;
}

void _Check5x6_Keys()
{
	_b5x6_Key = TRUE;

	_p5x6_KeyVals[0] = _Check_D_By_D(0xdf);	// 11011111B, D5 output
	_p5x6_KeyVals[1] = _Check_D_By_D(0xef);	// 11101111B, D4 output

	_p5x6_KeyVals[2] = _Check_D_By_B(0xdf);	// 11011111B, B5 output
	_p5x6_KeyVals[3] = _Check_D_By_B(0xef);	// 11101111B, B4 output
	_p5x6_KeyVals[4] = _Check_D_By_B(0xfb);	// 11111011B, B2 output
	_p5x6_KeyVals[5] = _Check_D_By_B(0xfe);	// 11111110B, B0 output

	_p5x6_KeyVals[6] = _Check_B_By_D(0xdf);	// 11011111B, D5 output
	_p5x6_KeyVals[7] = _Check_B_By_D(0xef);	// 11101111B, D4 output

	_p5x6_KeyVals[8] = _Check_B_By_B(0xdf);	// 11011111B, B5 output
	_p5x6_KeyVals[9] = _Check_B_By_B(0xef);	// 11101111B, B4 output
	_p5x6_KeyVals[10] = _Check_B_By_B(0xfb);	// 11111011B, B2 output
	_p5x6_KeyVals[11] = _Check_B_By_B(0xfe);	// 11111110B, B0 output
}
#endif

#ifdef KEY_7X8
void _Check7x8_Keys()
{
	_b7x8_Key = TRUE;

	_p7x8_KeyVals[0] = _Check_B_By_B(0xfe);	// 11111110B, B0 output
	_p7x8_KeyVals[1] = _Check_B_By_B(0xfd);	// 11111101B, B1 output
	_p7x8_KeyVals[2] = _Check_B_By_B(0xfb);	// 11111011B, B2 output
	_p7x8_KeyVals[3] = _Check_B_By_B(0xf7);	// 11110111B, B3 output
	_p7x8_KeyVals[4] = _Check_B_By_B(0xef);	// 11101111B, B4 output
	_p7x8_KeyVals[5] = _Check_B_By_B(0xdf);	// 11011111B, B5 output
	_p7x8_KeyVals[6] = _Check_B_By_B(0xbf);	// 10111111B, B6 output
	_p7x8_KeyVals[7] = _Check_B_By_B(0x7f);	// 01111111B, B7 output
}
#endif

#ifdef KEY_5X5
UCHAR _Check_K_By_D(UCHAR iAndMask)
{
	UCHAR iVal;

	rGPIO_D_Output_Enable |= ~iAndMask;	
	rGPIO_D |= ~iAndMask;
	iVal = rGPIO_K;
	rGPIO_D &= iAndMask;
	rGPIO_D_Output_Enable &= iAndMask;	

	return iVal;
}

UCHAR _Check_K_By_K(UCHAR iAndMask)
{
	UCHAR iVal;

	rGPIO_K_Enable &= iAndMask;
	rGPIO_K |= ~iAndMask;
	NOP;
	NOP;
	NOP;
	iVal = rGPIO_K;
	rGPIO_K &= iAndMask;
	rGPIO_K_Enable |= ~iAndMask;

	return iVal;
}

void _Check5x5_Keys()
{
	_b5x5_Key = TRUE;

	_p5x5_KeyVals[0] = _Check_K_By_K(0xdf);	// 11011111B, K5 output
	_p5x5_KeyVals[1] = _Check_K_By_K(0xbf);	// 10111111B, K6 output
	_p5x5_KeyVals[2] = _Check_K_By_K(0x7f);	// 01111111B, K7 output
	_p5x5_KeyVals[3] = _Check_K_By_D(0xef);	// 11101111B, D4 output
	_p5x5_KeyVals[4] = _Check_K_By_D(0xdf);	// 11011111B, D5 output
}

#endif

#ifdef KEY_4X4
UCHAR _Check_B_By_B(UCHAR iAndMask)		// pull high
{
	UCHAR iVal, iOrMask;

	iOrMask = ~iAndMask;
//	rGPIO_B_Input_Enable &= iAndMask;
	rGPIO_B_Output_Enable |= iOrMask;	
	rGPIO_B &= iAndMask;
	iVal = rGPIO_B;
	iVal |= iOrMask;
	rGPIO_B |= iOrMask;
	rGPIO_B_Output_Enable &= iAndMask;	
//	rGPIO_B_Input_Enable |= iOrMask;

	return iVal;
}

void _Check4x4_Keys()
{
	_b4x4_Key = TRUE;

	_p4x4_KeyVals[0] = _Check_B_By_B(0xef);	// 11101111B, B4 output
	_p4x4_KeyVals[1] = _Check_B_By_B(0xdf);	// 11011111B, B5 output
	_p4x4_KeyVals[2] = _Check_B_By_B(0xbf);	// 10111111B, B6 output
	_p4x4_KeyVals[3] = _Check_B_By_B(0x7f);	// 01111111B, B7 output
}
#endif

#ifdef KEY_8X5
void _Check8x5_Keys()
{
	UCHAR i, iOrgHigh, iOrgLow;
	PCHAR p;

	_b8x5_Key = TRUE;

	p = (PCHAR)BANK_BASE;

	iOrgHigh = rEM_HighPage;
	iOrgLow = rEM_LowPage;

	// CE3 and A21 control 74LV164
	rEM_HighPage = 0x18;		// CE3, 00011000b
	rEM_LowPage = 0x40;			// A21, 01000000b
	p[0] = 0;
	NOP;
	NOP;
	_p8x5_KeyVals[0] = (rGPIO_D & 0x10) >> 1;
	_p8x5_KeyVals[0] += rGPIO_B;
	rEM_LowPage = 0x00;
	for (i = 1; i < KEY_8X5_SIZE; i ++)
	{
		p[0] = 0;
		_p8x5_KeyVals[i] = (rGPIO_D & 0x10) >> 1;
		_p8x5_KeyVals[i] += rGPIO_B;
	}

	rEM_HighPage = iOrgHigh;
	rEM_LowPage = iOrgLow;
}
#endif

#ifdef KEY_ADC
void _CheckADC_Keys()
{
	if (rGPIO_D_Output_Enable & 0x20)
	{
		if (_iADC_KeyStage < KEY_ADC_STAGE)
		{
			_pADC_KeyVal[_iADC_KeyStage] = rLRADC_Data;
			_iADC_KeyStage ++;
		}
		rGPIO_D_Output_Enable = (rGPIO_D_Output_Enable & 0xdf) | 0x10;
	}
	else
	{
		if (_iADC_KeyStage2 < KEY_ADC_STAGE)
		{
			_pADC_KeyVal2[_iADC_KeyStage2] = rLRADC_Data;
			_iADC_KeyStage2 ++;
		}
		rGPIO_D_Output_Enable = (rGPIO_D_Output_Enable & 0xef) | 0x20;
	}
}
#endif	// KEY_ADC

#ifdef SERIAL_WDCB
void _CheckSerialStatus()
{
	if (Serial_bFree)
	{
		Serial_bSwitchChannel = TRUE;
		_iSerialCTC = 0;
	}
	else
	{
		_iSerialCTC ++;
		if (_iSerialCTC == 50)
		{
			_iSerialCTC = 0;
			Serial_bFree = TRUE;
		}
	}	
}
#endif

void _10msCounter()
{
	_iCTC ++;
	if (_iCTC == 100)
	{
		Sys_lCurrentTime ++;
		_bTimer = TRUE;
		_iCTC = 0;
	}
	Sys_lTicker += 10;	// 10ms timer

#ifdef KEY_ADC
	_CheckADC_Keys();
#endif

#ifdef KEY_5X5
	_Check5x5_Keys();
#endif

#ifdef KEY_4X4
	_Check4x4_Keys();
#endif

#ifdef KEY_5X6
	_Check5x6_Keys();
#endif

#ifdef KEY_8X5
	_Check8x5_Keys();
#endif

#ifdef KEY_7X8
	_Check7x8_Keys();
#endif

#ifdef KEY_C2
	_bC2_Key = TRUE;
	_iC2_KeyVal = rGPIO_C;
#endif

#ifdef VER_AR168R
	_bCOR = TRUE;
	_iGPIO_COR = rGPIO_B;	// GPIO B2 COR input
#endif

#ifdef HOOK_GPIO
	_bHook = TRUE;
	_iGPIO_Hook = rGPIO_C;	// use GPIO C2 as hook when ADC key not used
#endif

#ifdef SERIAL_WDCB
	_CheckSerialStatus();
#endif
}


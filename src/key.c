/*-------------------------------------------------------------------------
   AR1688 GPIO key function copy right information

   Copyright (c) 2006-2013. Lin, Rongrong <woody@palmmicro.com>
                            Lin, Guofeng <alanda@palmmicro.com>

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

#include "bank1.h"

#ifdef CALL_NONE
void KeyInit()
{
#ifdef KEY_4X4
	// * key only at first
	rKeyScanControl = 0x88;
	rGPIO_B_Config = 0x18;
	rGPIO_B_Output_Enable = 0x10;
	rGPIO_B_Input_Enable = 0x08;
	rGPIO_B = 0;
#endif

#ifdef KEY_5X6	// B0, B2, B4, B5, D4, D5 input
	rKeyScanControl = 0;
	rGPIO_B_Config = 0;
	rGPIO_B_Output_Enable = 0;
	rGPIO_B_Input_Enable = 0x35;	// 00110101B
	rGPIO_B = 0;

	rGPIO_D_Output_Enable &= 0x0f;
	rGPIO_D_Input_Enable |= 0x30;	// 00110000B
	rGPIO_D &= 0xcf;
#endif

#ifdef KEY_7X8
	rKeyScanControl = 0;
	rGPIO_B_Config = 0;
	rGPIO_B_Output_Enable = 0;
	rGPIO_B_Input_Enable = 0xff;	// 11111111B
	rGPIO_B = 0;
#endif

#ifdef KEY_8x4 // B0. B2, B4, B5 input
	rKeyScanControl = 0;
	rGPIO_B_Config = 0;
	rGPIO_B_Output_Enable = 0;
	rGPIO_B_Input_Enable = 0x35;	// 00110101B
	rGPIO_B = 0;
#endif

#ifdef KEY_8X5	// B0, B2, B4, B5, D4 input
	rKeyScanControl = 0;
	rGPIO_B_Config = 0;
	rGPIO_B_Output_Enable = 0;
	rGPIO_B_Input_Enable = 0x35;	// 00110101B
	rGPIO_B = 0;

	rGPIO_D_Output_Enable &= 0x2f;
	rGPIO_D_Input_Enable |= 0x10;	// 00010000B
	rGPIO_D &= 0xef;
#endif

#ifdef VER_AR168R
	rGPIO_B_Config = 0;
	rGPIO_B_Output_Enable = 0x01;	// GPIO B0 PTT output, 00000001B
	rGPIO_B_Input_Enable = 0x04;	// GPIO B2 COR input, 00000100B
	rGPIO_B = 0;
#endif

#ifdef VER_AR168W
	rGPIO_B_Config = 0;
	// all B0/B2/B4/B5 output
	rGPIO_B_Output_Enable |= 0x35;	
	rGPIO_B_Input_Enable &= ~0x35;

	// GPIO G0 as input
	rGPIO_G_Enable &= 0xfe;
	rGPIO_G_Enable |= 0x10;
#endif

#ifdef SYS_CHIP_PLUS
	// pull GPIO C2 low as key to enter safemode
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xfb) | 0x40;	// 11111011, 01000000
#endif
}
#endif

void KeyStart()
{
#ifdef KEY_4X4
	rKeyScanControl = 0x80;	// enable key scan but do not actually use it

	rGPIO_B_Config = 0x0f;
	rGPIO_B_Output_Enable = 0x00;
	rGPIO_B = 0x0f;
//	rGPIO_B_Input_Enable = 0xff;
	rGPIO_B_Input_Enable = 0x0f;

	Key4x4_Start();
#endif

#ifdef KEY_5X5
	rDMA4_DstAddrHigh |= 0x40;		// enable GPIO K0-K1
	rDMA4_Control_Status |= 0x04;	// enable GPIO K2-K7

	// GPIO K0-K4 input low, K5-K7 input, D4, D5 3-state
	rGPIO_K_Enable = 0xff;
	rGPIO_K = 0x00;
	rGPIO_D_Output_Enable &= 0x0f;
//	rGPIO_D_Input_Enable |= 0x30;	// 00110000B
	rGPIO_D_Input_Enable &= 0xcf;	// 00110000B
	rGPIO_D &= 0xcf;

	Key5x5_Start();
#endif

#ifdef KEY_ADC
	// set GPIO D4, D5 (UART RX, TX) as low, D5 output first
	rGPIO_D_Output_Enable = (rGPIO_D_Output_Enable & 0xef) | 0x20;
	rGPIO_D &= 0xcf;

	rSARADC_Control = 0xc4;

	ADC_KeyStart();

#endif	// KEY_ADC

	KeyClear();
}


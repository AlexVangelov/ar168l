/*-------------------------------------------------------------------------
   AR1688 LED function copy right information

   Copyright (c) 2006-2013. Lin, Rongrong <woody@palmmicro.com>

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
#include "bank1.h"
#include "apps.h"

#ifdef SERIAL_UI

const UCHAR _pLed[] = "LED ";

void _SerialLED(UCHAR iIndex, UCHAR iType)
{
	UCHAR pBuf[7];

	strcpy(pBuf, _pLed);
	pBuf[4] = iIndex + '0';
	pBuf[5] = iType;
	pBuf[6] = 0;

	SerialSendString(pBuf);
}

#endif

void LED_Init()
{
#if defined VER_AR168F || defined VER_GF302 || defined VER_AR168J || defined VER_YWH201 || defined VER_GP1266 || defined VER_BT2008 || defined VER_FWV2800
	rGPIO_G_Enable |= 0x01;									// GPIO_G0 output enable
#elif defined VER_AR168G
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xbf) | 0x04;		// GPIO_C2 output enable, 10111111, 00000100
	rGPIO_G_Enable |= 0x01;									// GPIO_G0 output enable
#elif defined VER_AR168KM
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xef) | 0x01;		// GPIO_C0 output enable, 11101111, 00000001
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xdf) | 0x02;		// GPIO_C1 output enable, 11011111, 00000010
#elif defined VER_AR168P || defined VER_GP2266 || defined VER_BT2008N || defined VER_DXDT
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xef) | 0x01;		// GPIO_C0 output enable, 11101111, 00000001
#ifndef OEM_UART
	rGPIO_D_Output_Enable |= 0x20;							// GPIO_D5 output enable
#endif
	rGPIO_G_Enable |= 0x01;									// GPIO_G0 output enable
#elif defined VER_AR168M && defined OEM_BT2008
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xbf) | 0x04;		// GPIO_C2 output enable, 10111111, 00000100
#endif
}

void LED_On(UCHAR iIndex)
{
#ifdef SERIAL_UI
	_SerialLED(iIndex, '0');
#endif

	switch (iIndex)
	{
#if defined VER_AR168F || defined VER_GF302 || defined VER_AR168J
	case LED_LCD:
		rGPIO_G &= 0xfe;	// GPIO_G0 On
		break;

	case LED_MESSAGE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

	case LED_MUTE:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

#elif defined VER_YWH201
	case LED_LCD:
		rGPIO_G &= 0xfe;	// GPIO_G0 On
		break;

	case LED_ACTIVE:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_MUTE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

#elif defined VER_AR168G
	case LED_LCD:
		rGPIO_G &= 0xfe;	// GPIO_G0 On
		break;

	case LED_MESSAGE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

	case LED_MUTE:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_HOLD:
		rGPIO_C &= 0xfb;	// GPIO_C2 On, 11111011
		break;

#elif defined VER_GP1266 || defined VER_BT2008
	case LED_LCD:
		rGPIO_G &= 0xfe;	// GPIO_G0 On
		break;

	case LED_MESSAGE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

	case LED_MUTE:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_HOLD:
		Led_PRE_On();
		break;

	case LED_TRANSFER:
		Led_WP_On();
		break;

#elif defined VER_AR168K
	case LED_LCD:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_MESSAGE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

#elif defined VER_AR168R
	case LED_LCD:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_LOGON:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

	case LED_RINGING:
		rGPIO_A &= 0xfe;	// GPIO_A0 On
		break;

#elif defined VER_AR168W
	case LED_RINGING:
		rGPIO_A &= 0xfe;	// GPIO_A0 On
		break;

#elif defined VER_AR168KM
	case LED_LCD:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_MESSAGE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

	case LED_MUTE:
		rGPIO_C &= ~0x01;	// GPIO_C0 On
		break;

	case LED_HOLD:
		rGPIO_C &= ~0x02;	// GPIO_C1 On
		break;

#elif defined VER_FWV2800
	case LED_LCD:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_MESSAGE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

	case LED_TRANSFER:
		rGPIO_G &= 0xfe;	// GPIO_G0 On
		break;

#elif defined VER_AR168P || defined VER_GP2266 || defined VER_BT2008N || defined VER_DXDT
	case LED_LCD:
		rGPIO_A &= 0xfd;	// GPIO_A1 On
		break;

	case LED_MESSAGE:
		rGPIO_A &= 0xfb;	// GPIO_A2 On
		break;

#ifndef OEM_UART
	case LED_MUTE:
		rGPIO_D &= 0xdf;	// GPIO_D5 On
		break;
#endif

	case LED_HOLD:
		rGPIO_C &= 0xfe;	// GPIO_C0 On, 11111110
		break;

	case LED_TRANSFER:
		rGPIO_G &= 0xfe;	// GPIO_G0 On
		break;

#elif defined VER_AR168M && defined OEM_BT2008
							// AR1688 pin name		pin number	AR168M name
	case LED_HOLD:
		rGPIO_B &= ~0x10;	// GPIO_B4 low			73			GPIO3
		break;

	case LED_MUTE:
		rGPIO_C &= ~0x04;	// GPIO_C2 low			45			GPIO7
		break;

	case LED_MESSAGE:
		rGPIO_B &= ~0x20;	// GPIO_B5 low			74			SMOSI
		break;

	case LED_ACTIVE:
		rGPIO_A &= ~0x04;	// GPIO_A2 low			65			module led
		break;

	case LED_RINGING:		
		rGPIO_B &= ~0x04;	// GPIO_B2 low			48			SCLK
		break;

	case LED_KEYPAD:		
		rGPIO_B |= 0x01;	// GPIO_B0 high			43			GPIO6
		break;
#endif
	}
}

void LED_Off(UCHAR iIndex)
{
#ifdef SERIAL_UI
	_SerialLED(iIndex, '1');
#endif

#if defined OEM_2KLABS || defined OEM_WEBNEED
	if (iIndex == LED_LCD)
	{
		return;
	}
#endif
	switch (iIndex)
	{
#if defined VER_AR168F || defined VER_GF302 || defined VER_AR168J
	case LED_LCD:
		rGPIO_G |= 0x01;	// GPIO_G0 Off
		break;

	case LED_MESSAGE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

	case LED_MUTE:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

#elif defined VER_YWH201
	case LED_LCD:
		rGPIO_G |= 0x01;	// GPIO_G0 Off
		break;

	case LED_ACTIVE:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_MUTE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

#elif defined VER_AR168G
	case LED_LCD:
		rGPIO_G |= 0x01;	// GPIO_G0 Off
		break;

	case LED_MESSAGE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

	case LED_MUTE:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_HOLD:
		rGPIO_C |= 0x04;	// GPIO_C2 Off, 00000100
		break;

#elif defined VER_GP1266 || defined VER_BT2008
	case LED_LCD:
		rGPIO_G |= 0x01;	// GPIO_G0 Off
		break;

	case LED_MESSAGE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

	case LED_MUTE:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_HOLD:
		Led_PRE_Off();
		break;

	case LED_TRANSFER:
		Led_WP_Off();
		break;

#elif defined VER_AR168K
	case LED_LCD:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_MESSAGE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

#elif defined VER_AR168R
	case LED_LCD:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_LOGON:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

	case LED_RINGING:
		rGPIO_A |= 0x01;	// GPIO_A0 Off
		break;

#elif defined VER_AR168W
	case LED_RINGING:
		rGPIO_A |= 0x01;	// GPIO_A0 Off
		break;

#elif defined VER_AR168KM
	case LED_LCD:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_MESSAGE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

	case LED_MUTE:
		rGPIO_C |= 0x01;	// GPIO_C0 Off
		break;

	case LED_HOLD:
		rGPIO_C |= 0x02;	// GPIO_C1 Off
		break;

#elif defined VER_FWV2800
	case LED_LCD:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_MESSAGE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

	case LED_TRANSFER:
		rGPIO_G |= 0x01;	// GPIO_G0 Off
		break;

#elif defined VER_AR168P || defined VER_GP2266 || defined VER_BT2008N || defined VER_DXDT
	case LED_LCD:
		rGPIO_A |= 0x02;	// GPIO_A1 Off
		break;

	case LED_MESSAGE:
		rGPIO_A |= 0x04;	// GPIO_A2 Off
		break;

#ifndef OEM_UART
	case LED_MUTE:
		rGPIO_D |= 0x20;	// GPIO_D5 Off
		break;
#endif

	case LED_HOLD:
		rGPIO_C |= 0x01;	// GPIO_C0 Off, 00000001
		break;

	case LED_TRANSFER:
		rGPIO_G |= 0x01;	// GPIO_G0 Off
		break;

#elif defined VER_AR168M && defined OEM_BT2008
							// AR1688 pin name		pin number	AR168M name
	case LED_HOLD:
		rGPIO_B |= 0x10;	// GPIO_B4 high			73			GPIO3
		break;

	case LED_MUTE:
		rGPIO_C |= 0x04;	// GPIO_C2 high			45			GPIO7
		break;

	case LED_MESSAGE:
		rGPIO_B |= 0x20;	// GPIO_B5 high			74			SMOSI
		break;

	case LED_ACTIVE:
		rGPIO_A |= 0x04;	// GPIO_A2 high			65			module led
		break;

	case LED_RINGING:		
		rGPIO_B |= 0x04;	// GPIO_B2 high			48			SCLK
		break;

	case LED_KEYPAD:		
		rGPIO_B &= ~0x01;	// GPIO_B0 low			43			GPIO6
		break;
#endif
	}
}

void LED_Blink(UCHAR iIndex)
{
#ifdef SERIAL_UI
	_SerialLED(iIndex, '2');
#endif

	switch (iIndex)
	{
	case LED_MESSAGE:
#if defined VER_AR168M && defined OEM_BT2008
		if (rGPIO_B & 0x20)		// GPIO_B5 Off?
		{
			LED_On(LED_MESSAGE);
		}
		else
		{
			LED_Off(LED_MESSAGE);
		}
#elif !defined VER_AR168R && !defined SYS_PROGRAMMER
		if (rGPIO_A & 0x04)		// GPIO_A2 Off?
		{
			LED_On(LED_MESSAGE);
		}
		else
		{
			LED_Off(LED_MESSAGE);
		}
#endif
		break;

	case LED_RINGING:
#if defined VER_AR168M && defined OEM_BT2008
		if (rGPIO_B & 0x04)		// GPIO_B2 Off?
		{
			LED_On(LED_RINGING);
		}
		else
		{
			LED_Off(LED_RINGING);
		}
#elif defined VER_AR168R || defined VER_AR168W
		if (rGPIO_A & 0x01)		// GPIO_A0 Off?
		{
			LED_On(LED_RINGING);
		}
		else
		{
			LED_Off(LED_RINGING);
		}
#endif
		break;
	}
}

void LED_SetState(UCHAR iIndex, BOOLEAN bOn)
{
	if (bOn)
	{
		LED_On(iIndex);
	}
	else
	{
		LED_Off(iIndex);
	}
}

#ifdef OEM_IP20

#include "ar168.h"

#define IP20_LED_NUM	15

const UCHAR ledMap[IP20_LED_NUM][2] = {{~0x01, ~0x00}, {~0x02, ~0x00}, {~0x04, ~0x00}, {~0x08, ~0x00}, {~0x10, ~0x00}, {~0x20, ~0x00}, {~0x40, ~0x00}, {~0x80, ~0x00}, 
										{~0x00, ~0x01}, {~0x00, ~0x02}, {~0x00, ~0x04}, {~0x00, ~0x08}, {~0x00, ~0x10}, {~0x00, ~0x20}, {~0x00, ~0x40}};
void IP20_LED_On(PCHAR pMatchNumber)
{
	UCHAR i;
	UCHAR pBuf[MAX_USER_NUMBER_LEN];

	SkipField(pMatchNumber,'<');

	for (i = 0; i < IP20_LED_NUM; i ++)
	{
		GetPhoneBookNumber(i + 1, pBuf);
		if (!strcmp(pMatchNumber, pBuf))
		{
			IP20_LedControl(ledMap[i][0], ledMap[i][1]);		
			break;
		}
	}	
}

void IP20_LED_Off()
{
	IP20_LedControl(~0x00, ~0x00); 
}
#endif

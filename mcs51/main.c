/*-------------------------------------------------------------------------
   AR168M demo external controller MCS51
   Main function copy right information

   Copyright (c) 2008-2010. Lin, Rongrong <woody@palmmicro.com>

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

#include "type.h"
#include "8051.h"
#include "sys.h"

#ifdef SYS_DEBUG
volatile BOOLEAN _bTimer;
volatile DCHAR _iWholeSecond;
#endif

// timer 0 used for systemclock
//#define TIMER0_RELOAD_VALUE -OSCILLATOR/12/1000 // 0.999348ms for 11.059Mhz
#define TIMER0_RELOAD_VALUE -OSCILLATOR/12/100 // 9.99348ms for 11.059Mhz

void Timer0Interrupt() __interrupt 1 __using 1	// 10ms interrupt
{
	TL0 = TIMER0_RELOAD_VALUE & 0xff;
	TH0 = TIMER0_RELOAD_VALUE >> 8;

#ifdef SYS_DEBUG
	_iWholeSecond ++;
	if (_iWholeSecond >= 100)
	{
		_iWholeSecond = 0;
		_bTimer = TRUE;
	}
#endif

	HookISR();
	KeyISR();
}

void ControlInitialize()
{
	// initialize timer0 for system clock
	TR0 = 0;						// stop timer 0
	TMOD = (TMOD & 0xf0) | 0x01;	// T0 = 16bit timer
	// timeout is xtal/12
	TL0 = TIMER0_RELOAD_VALUE & 0xff;
	TH0 = TIMER0_RELOAD_VALUE >> 8;
	TR0 = 1;						// start timer 0
	ET0 = 1;						// enable timer 0 interrupt

	// initialize timer1 for baudrate
	TR1 = 0;						// stop timer 1
	TMOD = (TMOD & 0x0f) | 0x20;	// T1 = 8bit autoreload timer
	// baud = ((2^SMOD)*xtal)/(32*12*(256-TH1))
	PCON |= 0x80;					// SMOD = 1: double baudrate
	TH1 = TL1 = TIMER1_VALUE;
	TR1 = 1;						// start timer 1

	// initialize serial port
	SCON = 0x52;					// mode 1, ren, txrdy, rxempty
//	SCON = 0x50;					// set serial port in mode 1, and enable receive
	ES = 1;

	IP = 0;							// no interrupt has higher piority
//	PS = 1;							// Serial interrupt higher piority

	EA = 1;							// enable global interrupt
}

#ifdef SYS_DEBUG
const CCHAR _cHello[] = "Hello, world!";
const CCHAR _cCount[] = "0";
#endif

void main()
{
#ifdef SYS_DEBUG
	DCHAR pBuf[2];

	_bTimer = FALSE;
	_iWholeSecond = 0;
#endif

	HookInit();
	KeyInit();
	LcdInit();
	UART_Init();
	UI_Init();
	ControlInitialize();

#ifdef SYS_DEBUG
	LcdDisplayC(_cHello, 0);
	LcdDisplayC(_cCount, 1);
	strcpy_c2d(pBuf, _cCount);
	LED_On(0);
#endif

	do
	{
		HookRun();
		KeyRun();
		UART_Run();
#ifdef SYS_DEBUG
		if (_bTimer)
		{
			pBuf[0] ++;
			LcdDisplay(pBuf, 1);
			if (pBuf[0] == '9')
			{
				pBuf[0] = '0' - 1;
			}
			_bTimer = FALSE;
			LED_Blink(0);
		}
#endif
	} while (1);
}

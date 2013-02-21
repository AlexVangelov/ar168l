/*-------------------------------------------------------------------------
   AR168M demo external controller MCS51
   LED function copy right information

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

#ifdef SYS_WINBOND
#include "winbond.h"
#endif

#define LED_0		P2_0
#define LED_1		P2_1
#define LED_2		P2_2		
#define LED_3		P2_3

#ifdef SYS_WINBOND
#define LED_4		P4_0
#define LED_5		P4_1
#define LED_6		P4_2		
#define LED_7		P4_3
#endif

void LED_On(UCHAR iIndex)
{
	switch (iIndex)
	{
	case 0:
		LED_0 = 0;
		break;

	case 1:
		LED_1 = 0;
		break;

	case 2:
		LED_2 = 0;
		break;

	case 3:
		LED_3 = 0;
		break;

#ifdef SYS_WINBOND
	case 4:
		LED_4 = 0;
		break;

	case 5:
		LED_5 = 0;
		break;

	case 6:
		LED_6 = 0;
		break;

	case 7:
		LED_7 = 0;
		break;
#endif
	}
}

void LED_Off(UCHAR iIndex)
{
	switch (iIndex)
	{
	case 0:
		LED_0 = 1;
		break;

	case 1:
		LED_1 = 1;
		break;

	case 2:
		LED_2 = 1;
		break;

	case 3:
		LED_3 = 1;
		break;

#ifdef SYS_WINBOND
	case 4:
		LED_4 = 1;
		break;

	case 5:
		LED_5 = 1;
		break;

	case 6:
		LED_6 = 1;
		break;

	case 7:
		LED_7 = 1;
		break;
#endif
	}
}

void LED_Blink(UCHAR iIndex)
{
	BOOLEAN bVal;

	bVal = FALSE;
	switch (iIndex)
	{
	case 0:
		bVal = LED_0;
		break;

	case 1:
		bVal = LED_1;
		break;

	case 2:
		bVal = LED_2;
		break;

	case 3:
		bVal = LED_3;
		break;

#ifdef SYS_WINBOND
	case 4:
		bVal = LED_4;
		break;

	case 5:
		bVal = LED_5;
		break;

	case 6:
		bVal = LED_6;
		break;

	case 7:
		bVal = LED_7;
		break;
#endif
	}

	if (bVal)	
	{
		LED_On(iIndex);
	}
	else
	{
		LED_Off(iIndex);
	}
}


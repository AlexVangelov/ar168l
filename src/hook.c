/*-------------------------------------------------------------------------
   AR1688 Hook function copy right information

   Copyright (c) 2006-2010. Lin, Rongrong <woody@palmmicro.com>

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

#if defined HOOK_GPIO || defined KEY_ADC

UCHAR _iHandsetDownCounter;


#ifdef CALL_NONE
void OnHandsetDown()
{
	UdpDebugString("Handset down, reboot");
	FlashReboot();
}
#endif	// CALL_NONE

void HookHandleHandsetDown(UCHAR iCount)
{
	_iHandsetDownCounter ++;
	if (_iHandsetDownCounter >= iCount)
	{
		OnHandsetDown();
		_iHandsetDownCounter = 0;
	}
}
#endif

#if defined HOOK_GPIO

extern volatile UCHAR _iGPIO_Hook;
extern volatile BOOLEAN _bHook;

void HookStart()
{
	_iHandsetDownCounter = 0;
	_iGPIO_Hook = 0x04;
	_bHook = FALSE;
}

void HookRun()
{
	if (!_bHook)	return;

	if (_iGPIO_Hook & 0x04)
	{
		HookHandleHandsetDown(12);
	}
	else
	{
		_iHandsetDownCounter = 0;
#ifndef CALL_NONE
		OnHook();
#endif
	}

	_bHook = FALSE;
}
	
#endif	// HOOK_GPIO


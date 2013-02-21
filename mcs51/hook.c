/*-------------------------------------------------------------------------
   AR168M demo external controller MCS51
   Hook function copy right information

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

#define HOOK_IO		P2_4

#define HOOK_DOWN_DELAY		15	// wait 150ms for hook down to be steady

DCHAR _iHookDownCounter;
BOOLEAN _bOldHookDown;
volatile BOOLEAN _bHook;
volatile BOOLEAN _bHookDown;

void HookISR() __using 1
{
	if (_bHook)	return;

	_bHookDown = HOOK_IO;
	_bHook = TRUE;
}

void HookInit()
{
	_bHook = FALSE;
	_bHookDown = _bOldHookDown = TRUE;
}

void HookRun()
{
	if (!_bHook)	return;

	if (_bHookDown)
	{
		_iHookDownCounter ++;
		if (_iHookDownCounter >= HOOK_DOWN_DELAY)
		{
			if (!_bOldHookDown)
			{
				UI_HandleKey('Z');
				_bOldHookDown = TRUE;
			}
			_iHookDownCounter = 0;
		}
	}
	else
	{
		_iHookDownCounter = 0;
		if (_bOldHookDown)
		{
			UI_HandleKey('z');
			_bOldHookDown = FALSE;
		}
	}

	_bHook = FALSE;
}

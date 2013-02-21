/*-------------------------------------------------------------------------
   AR168M demo external controller MCS51
   Keypad function copy right information

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

#define ZERO_KEY_NUM	5

#define	KEY_COL_NUM		8
#define	KEY_ROW_NUM		6

#define KEY_ROW0	P3_2
#define KEY_ROW1	P3_3
#define KEY_ROW2	P3_4
#define KEY_ROW3	P3_5
#define KEY_ROW4	P3_6
#define KEY_ROW5	P3_7

#define	KEY_COL_P1	0xff

volatile DCHAR _pKeyVals[KEY_ROW_NUM];
volatile BOOLEAN _bKey;

DCHAR _iOldKey, _iZeroKey;

void KeyISR() __using 1
{
	if (_bKey)	return;

	KEY_ROW0 = 0;
	_pKeyVals[0] = P1;
	KEY_ROW0 = 1;

	KEY_ROW1 = 0;
	_pKeyVals[1] = P1;
	KEY_ROW1 = 1;

	KEY_ROW2 = 0;
	_pKeyVals[2] = P1;
	KEY_ROW2 = 1;

	KEY_ROW3 = 0;
	_pKeyVals[3] = P1;
	KEY_ROW3 = 1;

	KEY_ROW4 = 0;
	_pKeyVals[4] = P1;
	KEY_ROW4 = 1;

	KEY_ROW5 = 0;
	_pKeyVals[5] = P1;
	KEY_ROW5 = 1;

	_bKey = TRUE;
}

void KeyInit()
{
	KEY_ROW0 = 1;
	KEY_ROW1 = 1;
	KEY_ROW2 = 1;
	KEY_ROW3 = 1;
	KEY_ROW4 = 1;
	KEY_ROW5 = 1;

	_iOldKey = _iZeroKey = 0;
	_bKey = FALSE;
	memset_d(_pKeyVals, KEY_COL_P1, KEY_ROW_NUM);
}

const CCHAR _ppKeyMap[KEY_ROW_NUM][KEY_COL_NUM] = {
	{'1', '4', '7', '*', 'm', 'j', 'f', 'u'}, 
	{'2', '5', '8', '0', 'o', 'k', 'e', 'v'}, 
	{'3', '6', '9', '#', 'n', 'h', 'B', 'w'}, 
	{'a', 'b', 'c', 'd', 'l', 'i', 'D', 'x'}, 
	{'p', 'q', 'r', 's', 't', 'g', 'C', 'y'}, 
	{'A', 'E', 'F', 'G', 'H', 'I', 'J', 'K'}, 
/*
	{'1', '2', '3', 'a', 'e', 'i', 'm', 'q'}, 
	{'4', '5', '6', 'b', 'f', 'j', 'n', 'r'}, 
	{'7', '8', '9', 'c', 'g', 'k', 'o', 's'}, 
	{'*', '0', '#', 'd', 'h', 'l', 'p', 't'}, 
	{'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B'}, 
	{'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'}, 
*/
};

void KeyRun()
{
	UCHAR i, j, iVal, iKey;

	if (!_bKey)	return;

	iKey = 0;
	for (i = 0; i < KEY_ROW_NUM; i ++)
	{
		iVal = _pKeyVals[i];
		if (iVal != KEY_COL_P1)
		{
			iVal = ~iVal;
			for (j = 0; j < KEY_COL_NUM; j ++)
			{
				if (iVal == (0x01 << j))
				{
					iKey = _ppKeyMap[i][j];
					goto Next;
				}
			}
		}
	}

Next:
	_bKey = FALSE;
	if (iKey == _iOldKey)	return;
	if (!iKey)
	{
		_iZeroKey ++;
		if (_iZeroKey < ZERO_KEY_NUM)	return;
	}
	_iZeroKey = 0;
	_iOldKey = iKey;
	UI_HandleKey(iKey);
}


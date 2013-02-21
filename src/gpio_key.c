/*-------------------------------------------------------------------------
   AR1688 GPIO key function copy right information

   Copyright (c) 2006-2012. Lin, Rongrong <woody@palmmicro.com>
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
#include "type.h"
#include "core.h"
#include "apps.h"

#include "bank1.h"

#define ZERO_KEY_NUM	5

#ifdef KEY_4X4

/*
		1			2			3			a		--------------KEYI0(b0)
		4			5			6			b		--------------KEYI1(b1)
		7			8			9			c		--------------KEYI2(b2)
		*			0			#			d		--------------KEYI3(b3)
		|			|			|			|
		KEYO0		KEYO1		KEYO2		KEYO3
		(b4)		(b5)		(b6)		(b7)
*/

volatile UCHAR _p4x4_KeyVals[KEY_4X4_SIZE];
volatile BOOLEAN _b4x4_Key;

UCHAR _iOld4x4_Key;
UCHAR _iZero4x4_Key;

const UCHAR _iKey4x4[KEY_4X4_SIZE][KEY_4X4_SIZE] = {
	{'1', '2', '3', 'a'}, 
	{'4', '5', '6', 'b'}, 
	{'7', '8', '9', 'c'}, 
	{'*', '0', '#', 'd'}, 
};

void Key4x4_Start()
{
	memset(_p4x4_KeyVals, 0, KEY_4X4_SIZE);
	_b4x4_Key = FALSE;
	_iOld4x4_Key = 0;
	_iZero4x4_Key = 0;
}

void Key4x4_Run()
{
	UCHAR i, j, iVal, iKey;
	UCHAR pVals[KEY_4X4_SIZE];

	if (!_b4x4_Key)	return;
	_b4x4_Key = FALSE;

	memcpy4(pVals, _p4x4_KeyVals);
	memset(_p4x4_KeyVals, 0, KEY_4X4_SIZE);

	iKey = 0;
	for (i = 0; i < KEY_4X4_SIZE; i ++)
	{
		iVal = pVals[i] & 0x0f;
		if (iVal != 0x0f)
		{
//			UdpDebugString("4x4 key");
//			UdpDebugVal(i, 10);
//			UdpDebugVal(iVal, 16);
			iVal = (~iVal) & 0x0f;
			for (j = 0; j < KEY_4X4_SIZE; j ++)
			{
				if (iVal == (0x01 << j))
				{
					iKey = _iKey4x4[j][i];
					break;
				}
			}

#ifdef VER_YWH201
			if (!iKey)
			{
				if (i == 0 && iVal == 0x0c)		// to fix ywh201 bug keypad problem
				{
					iKey = '7';
				}
			}
#endif

		}
	}

	if (iKey == _iOld4x4_Key)	return;
	if (!iKey)
	{
		_iZero4x4_Key ++;
		if (_iZero4x4_Key < ZERO_KEY_NUM)	return;
	}
	_iZero4x4_Key = 0;
	_iOld4x4_Key = iKey;
	UI_HandleKeys(iKey);
}

#endif

#if defined KEY_5X5 || defined KEY_5X6 || defined KEY_8X5 || defined KEY_7X8 || defined KEY_C2

UCHAR _iOldKey, _iZeroKey;

void OldKeyStart()
{
	_iOldKey = _iZeroKey = 0;
}

void OldKeyRun(UCHAR iKey)
{
	if (iKey == _iOldKey)	return;
	if (!iKey)
	{
		_iZeroKey ++;
		if (_iZeroKey < ZERO_KEY_NUM)	return;
	}
	_iZeroKey = 0;
	_iOldKey = iKey;
//	UdpDebugVal(iKey, 16);
	UI_HandleKeys(iKey);
}

#endif

#ifdef KEY_5X5
const UCHAR _iKey5x5[KEY_5X5_SIZE][KEY_5X5_SIZE] = {
	{'j', 'k', 'h', 'i', 'g'}, 
	{'m', 'o', 'n', 'l', 'D'}, 
	{'f', 'e', 'A', 'B', 'C'}, 
	{'p', 'q', 'r', 's', 't'}, 
	{'u', 'v', 'w', 'x', 'y'}, 
};

volatile UCHAR _p5x5_KeyVals[KEY_5X5_SIZE];
volatile BOOLEAN _b5x5_Key;

void Key5x5_Start()
{
	memset(_p5x5_KeyVals, 0, KEY_5X5_SIZE);
	_b5x5_Key = FALSE;
	OldKeyStart();
}

void Key5x5_Run()
{
	UCHAR i, j, iVal, iKey;
	UCHAR pVals[KEY_5X5_SIZE];

	if (!_b5x5_Key)	return;
	_b5x5_Key = FALSE;

	memcpy(pVals, _p5x5_KeyVals, KEY_5X5_SIZE);
	memset(_p5x5_KeyVals, 0, KEY_5X5_SIZE);

	iKey = 0;
	for (i = 0; i < KEY_5X5_SIZE; i ++)
	{
		iVal = pVals[i] & 0x1f;
		if (iVal)
		{
//			UdpDebugString("5x5 key");
//			UdpDebugVal(i, 10);
//			UdpDebugVal(iVal, 16);
			for (j = 0; j < KEY_5X5_SIZE; j ++)
			{
				if (iVal == (0x01 << j))
				{
					iKey = _iKey5x5[j][i];
					break;
				}
			}
		}
	}

	OldKeyRun(iKey);
}

#endif


#ifdef KEY_5X6
volatile UCHAR _p5x6_KeyVals[KEY_5X6_SIZE];
volatile BOOLEAN _b5x6_Key;

void Key5x6_Start()
{
	memset(_p5x6_KeyVals, 0, KEY_5X6_SIZE);
	_b5x6_Key = FALSE;
	OldKeyStart();
}

void Key5x6_Run()
{
	UCHAR i, iKey;
	UCHAR pVals[KEY_5X6_SIZE];

	if (!_b5x6_Key)	return;
	_b5x6_Key = FALSE;

	memcpy(pVals, _p5x6_KeyVals, KEY_5X6_SIZE);
	memset(_p5x6_KeyVals, 0, KEY_5X6_SIZE);

	iKey = 0;
	for (i = 0; i < KEY_5X6_SIZE; i ++)
	{
		if (pVals[i])
		{
//			UdpDebugString("5x6 key");
//			UdpDebugVal(i, 10);
//			UdpDebugVal(pVals[i], 16);
			switch (pVals[i])
			{
#ifdef YNTX_KEYPAD
			case 0x01:
				if (i == 6)			iKey = 'd';	// 5
				else if (i == 7)	iKey = 'e';	// 4
				else if (i == 8)	iKey = '3';
				else if (i == 9)	iKey = '2';
				else if (i == 10)	iKey = '1';
				break;

			case 0x04:
				if (i == 6)			iKey = 'l';	// 0
				else if (i == 7)	iKey = 'g';	// 9
				else if (i == 8)	iKey = '6';	// 8
				else if (i == 9)	iKey = '5';	// 7
				else if (i == 11)	iKey = 'p';	// 6
				break;

			case 0x10:
				if (i == 0)			iKey = 'o';	// l
				else if (i == 2)	iKey = 'a';	// o
				else if (i == 3)	iKey = 'i';	// k
				else if (i == 4)	iKey = 'h';	// e
				else if (i == 5)	iKey = 'n';		// K21
				else if (i == 6)	iKey = 'n';	// c
				else if (i == 7)	iKey = 'b';	
				else if (i == 8)	iKey = '9';	// a
				else if (i == 10)	iKey = '4';	// #
				else if (i == 11)	iKey = 'q';	// *
				break;

			case 0x20:
				if (i == 1)			iKey = 't';		// K30
				else if (i == 2)	iKey = '#';	// s
				else if (i == 3)	iKey = '0';	// r
				else if (i == 4)	iKey = '*';	// q
				else if (i == 5)	iKey = 'p';		// K26
				else if (i == 6)	iKey = 'm';	
				else if (i == 7)	iKey = 'c';	// i
				else if (i == 9)	iKey = '8';	// h
				else if (i == 10)	iKey = '7';	// g
				else if (i == 11)	iKey = 'r';	// d
				break;
#else
			case 0x01:
				if (i == 6)			iKey = '5';
				else if (i == 7)	iKey = '4';
				else if (i == 8)	iKey = '3';
				else if (i == 9)	iKey = '2';
				else if (i == 10)	iKey = '1';
				break;

			case 0x04:
				if (i == 6)			iKey = '0';		// K10
				else if (i == 7)	iKey = '9';
				else if (i == 8)	iKey = '8';
				else if (i == 9)	iKey = '7';
				else if (i == 11)	iKey = '6';
				break;

			case 0x10:
				if (i == 0)			iKey = 'l';		// K25
				else if (i == 2)	iKey = 'o';		// K24
				else if (i == 3)	iKey = 'k';		// K23
				else if (i == 4)	iKey = 'e';		// K22
				else if (i == 5)	iKey = 'n';		// K21
				else if (i == 6)	iKey = 'c';		// K15
				else if (i == 7)	iKey = 'b';		// K14
				else if (i == 8)	iKey = 'a';		// K13
				else if (i == 10)	iKey = '#';		// K12
				else if (i == 11)	iKey = '*';		// K11
				break;

			case 0x20:
				if (i == 1)			iKey = 't';		// K30
				else if (i == 2)	iKey = 's';		// K29
				else if (i == 3)	iKey = 'r';		// K28
				else if (i == 4)	iKey = 'q';		// K27
				else if (i == 5)	iKey = 'p';		// K26
				else if (i == 6)	iKey = 'm';		// K20
				else if (i == 7)	iKey = 'i';		// K19
				else if (i == 9)	iKey = 'h';		// K18
				else if (i == 10)	iKey = 'g';		// K17
				else if (i == 11)	iKey = 'd';		// K16
				break;
#endif	// YNTX_KEYPAD
			}
		}
	}

	OldKeyRun(iKey);
}

#endif

#ifdef KEY_8X5
volatile UCHAR _p8x5_KeyVals[KEY_8X5_SIZE];
volatile BOOLEAN _b8x5_Key;

void Key8x5_Start()
{
	memset(_p8x5_KeyVals, 0, KEY_8X5_SIZE);
	_b8x5_Key = FALSE;
	OldKeyStart();
}

const UCHAR _iKey8x5[6][KEY_8X5_SIZE] = {
#ifdef VER_FWV2800
	{'m', 'g', 'e', 'o', 'c', '0', '0', '0'}, 
	{'0', '0', '0', '0', '0', '0', '0', '0'}, 
	{'0', '1', '4', '7', '*', '0', '0', '0'}, 
	{'b', '3', '6', '9', '#', '0', '0', '0'}, 
	{'i', '2', '5', '8', '0', '0', '0', '0'}, 
	{'a', '0', 'n', 'l', 'h', '0', '0', '0'}, 
#else
	{'1', '4', '7', '*', 'm', 'j', 'f', 'u'}, 
	{'0', '0', '0', '0', '0', '0', '0', '0'}, 
	{'2', '5', '8', '0', 'o', 'k', 'e', 'v'}, 
	{'p', 'q', 'r', 's', 't', 'g', '0', 'y'}, 
	{'3', '6', '9', '#', 'n', 'h', 'B', 'w'}, 
	{'a', 'b', 'c', 'd', 'l', 'i', 'D', 'x'}, 
#endif
};

void Key8x5_Run()
{
	UCHAR i, j, iKey, iVal;
	UCHAR pVals[KEY_8X5_SIZE];

	if (!_b8x5_Key)	return;
	_b8x5_Key = FALSE;

	memcpy(pVals, _p8x5_KeyVals, KEY_8X5_SIZE);
	memset(_p8x5_KeyVals, 0, KEY_8X5_SIZE);

	iKey = 0;
	for (i = 0; i < KEY_8X5_SIZE; i ++)
	{
		iVal = pVals[i];
		if (iVal)
		{
//			UdpDebugString("8x5 key");
//			UdpDebugVal(i, 10);
//			UdpDebugVal(iVal, 16);
			for (j = 0; j < KEY_8X5_SIZE; j ++)
			{
				if (iVal == (0x01 << j))
				{
//					UdpDebugVal(j, 10);
					iKey = _iKey8x5[j][i];
					break;
				}
			}
		}
	}

	OldKeyRun(iKey);
}

#endif

#ifdef KEY_7X8
volatile UCHAR _p7x8_KeyVals[KEY_7X8_SIZE];
volatile BOOLEAN _b7x8_Key;

void Key7x8_Start()
{
	memset(_p7x8_KeyVals, 0, KEY_7X8_SIZE);
	_b7x8_Key = FALSE;
	OldKeyStart();
}

#ifdef OEM_IP20
const UCHAR _iKey7x8[KEY_7X8_SIZE][KEY_7X8_SIZE] = {
//x,hold,trans,mute,x,x,mem8,mem7		
//mes,x,cancel,conf,x,x,mem9,mem6		
//red,fla,x,menu,x,x,mem10,mem5		
//spk,call,OK,x,x,x,mem11,mem4		
//*,0,#,vol-,x,x,mem12,mem3		
//7,8,9,vol+,x,x,x,mem2		
//4,5,6,<,DND,x,x,mem1		
//1,2,3,>,WUP,x,x,x		
	{  0, 'm', 'n', 'l',   0,   0, 'p'+7, 'p'+6}, 
	{'o',   0, 'e', 'A',   0,   0, 'p'+8, 'p'+5}, 
	{'b', 'd',   0, 'g',   0,   0, 'p'+9, 'p'+4}, 
	{'c', 'a', 'f',   0,   0,   0, 'P'+0, 'p'+3}, 
	{'*', '0', '#', 'i',   0,   0, 'P'+1, 'p'+2}, 
	{'7', '8', '9', 'h',   0,   0,     0, 'p'+1}, 
	{'4', '5', '6', 'j', 'D',   0,     0, 'p'+0}, 
	{'1', '2', '3', 'k', 'B',   0,     0,     0}, 
};
#else
const UCHAR _iKey7x8[KEY_7X8_SIZE][KEY_7X8_SIZE] = {
	{'0', '0', '0', '0', '0', '0', '0', '0'}, 
	{'0', '0', '8', 'q', 'r', 'c', 'G', 'H'}, 
	{'A', 'g', '0', '#', 'a', 'K', 'I', 'F'}, 
	{'d', 'j', '7', '0', 'b', 'L', 'E', 'y'}, 
	{'*', 'k', '9', 'e', '0', 'J', 'x', 'w'}, 
	{'0', 'f', '0', 'o', 'p', '0', 'v', 'u'}, 
	{'m', 'i', '4', '5', '6', '0', '0', 's'}, 
	{'n', 'h', '1', '2', '3', '0', 't', '0'}, 
};
#endif

void Key7x8_Run()
{
	UCHAR i, j, iVal, iKey;
	UCHAR pVals[KEY_7X8_SIZE];

	if (!_b7x8_Key)	return;
	_b7x8_Key = FALSE;

	memcpy(pVals, _p7x8_KeyVals, KEY_7X8_SIZE);
	memset(_p7x8_KeyVals, 0, KEY_7X8_SIZE);

	iKey = 0;
	for (i = 0; i < KEY_7X8_SIZE; i ++)
	{
		iVal = pVals[i];
		if (iVal)
		{
//			UdpDebugString("7x8 key");
//			UdpDebugVal(i, 10);
//			UdpDebugVal(iVal, 16);
			for (j = 0; j < KEY_7X8_SIZE; j ++)
			{
				if (iVal == (0x01 << j))
				{
					iKey = _iKey7x8[j][i];
					break;
				}
			}
		}
	}
	
	OldKeyRun(iKey);
}
#endif

#ifdef KEY_C2

#define C2_DEFAULT_VAL	0x04

volatile UCHAR _iC2_KeyVal;
volatile BOOLEAN _bC2_Key;

void KeyC2_Start()
{
	_iC2_KeyVal = C2_DEFAULT_VAL;
	_bC2_Key = FALSE;
	OldKeyStart();
}

void KeyC2_Run()
{
	UCHAR iKey, iVal;

	if (!_bC2_Key)	return;
	_bC2_Key = FALSE;

	iKey = 0;
	iVal = _iC2_KeyVal;
	_iC2_KeyVal = C2_DEFAULT_VAL;

	if ((iVal & C2_DEFAULT_VAL) != C2_DEFAULT_VAL)
	{
		iKey = 'Y';
	}

	OldKeyRun(iKey);
}

#endif

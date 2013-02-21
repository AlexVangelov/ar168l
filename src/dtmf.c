/*-------------------------------------------------------------------------
   AR1688 DTMF function copy right information

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

// const to generate DTMF singals
#define DTMF_SIGNAL_0		0
#define DTMF_SIGNAL_1		1
#define DTMF_SIGNAL_2		2
#define DTMF_SIGNAL_3		3
#define DTMF_SIGNAL_4		4
#define DTMF_SIGNAL_5		5
#define DTMF_SIGNAL_6		6
#define DTMF_SIGNAL_7		7
#define DTMF_SIGNAL_8		8
#define DTMF_SIGNAL_9		9
#define DTMF_SIGNAL_STAR	10
#define DTMF_SIGNAL_WELL	11

#define DTMF_TONE_NUM		5

#define DTMF_SIGNAL_ALL		(DTMF_SIGNAL_WELL+DTMF_TONE_NUM+1)

const USHORT _sToneIntervals[DTMF_TONE_NUM][DTMF_TONE_DSP_DURATION] = {	// [dial,busy,waiting,congestion,ring][ON1, OFF1, ON2, OFF2]
#if defined RES_AR
	{30000, 0,     0,     0},     {300,   200,   300,   200},   {300,   10000, 300,   10000}, {300,   400,   300,   400},   {1000,  4000,  1000,  4000},
#elif defined RES_AU
	{30000, 0,     0,     0},     {375,   375,   375,   375},   {200,   200,   200,   4400},  {375,   375,   375,   375},   {400,   200,   400,   200},
#elif defined RES_BE
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {175,   175,   175,   3500},  {167,   167,   167,   167},   {1000,  3000,  1000,  3000},
#elif defined RES_BG
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {150,   150,   150,   4000},  {250,   250,   250,   250},   {1000,  4000,  1000,  4000},
#elif defined RES_BR
	{30000, 0,     0,     0},     {250,   250,   250,   250},   {50,    1000,  50,    1000},  {250,   250,   250,   250},   {1000,  4000,  1000,  4000},
#elif defined RES_CA
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {300,   10000, 300,   10000}, {250,   250,   250,   250},   {2000,  4000,  2000,  4000},
#elif defined RES_CN
	{30000, 0,     0,     0},     {350,   350,   350,   350},   {400,   4000,  400,   4000},  {700,   700,   700,   700},   {1000,  4000,  1000,  4000},
#elif defined RES_CL
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {500,   500,   500,   500},   {200,   200,   200,   200},   {1000,  3000,  1000,  3000},
#elif defined RES_CZ
	{30000, 0,     0,     0},     {330,   330,   330,   330},   {330,   9000,  330,   9000},  {165,   165,   165,   165},   {1000,  4000,  1000,  4000},
#elif defined RES_DE
	{30000, 0,     0,     0},     {480,   480,   480,   480},   {200,   200,   200,   5000},  {240,   240,   240,   240},   {1000,  4000,  1000,  4000},
#elif defined RES_EG
	{30000, 0,     0,     0},     {1000,  4000,  1000,  4000},  {200,   200,   200,   200},   {500,   500,   500,   500},   {2000,  1000,  2000,  1000},
#elif defined RES_ES
	{30000, 0,     0,     0},     {200,   200,   200,   200},   {175,   175,   175,   3500},  {200,   200,   200,   600},   {1500,  3000,  1500,  3000},
#elif defined RES_FR
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {300,   8000,  400,   8000},  {500,   500,   500,   500},   {1500,  3500,  1500,  3500},
#elif defined RES_GB
	{30000, 0,     0,     0},     {375,   375,   375,   375},   {250,   250,   250,   3250},  {400,   350,   225,   525},   {400,   200,   400,   2000},
#elif defined RES_GR
	{30000, 0,     0,     0},     {300,   300,   300,   300},   {300,   10000, 300,   10000}, {150,   150,   150,   150},   {1000,  4000,  1000,  4000},
#elif defined RES_HK
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {500,   500,   500,   500},   {250,   250,   250,   250},   {400,   200,   400,   3000},
#elif defined RES_HU
	{30000, 0,     0,     0},     {300,   300,   300,   300},   {40,    1960,  40,    1960},  {300,   300,   300,   300},   {1250,  3750,  1250,  3750},
#elif defined RES_IN
	{30000, 0,     0,     0},     {750,   750,   750,   750},   {200,   100,   200,   7500},  {250,   250,   250,   250},   {400,   200,   400,   2000},
#elif defined RES_IT
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {400,   100,   250,   100},   {200,   200,   200,   200},   {1000,  4000,  1000,  4000},
#elif defined RES_JP
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {100,   100,   100,   3000},  {500,   500,   500,   500},   {1000,  2000,  1000,  2000},
#elif defined RES_KR
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {250,   250,   250,   3250},  {300,   200,   300,   200},   {1000,  2000,  1000,  2000},
#elif defined RES_LK
	{30000, 0,     0,     0},     {750,   750,   750,   750},   {500,   2500,  500,   2500},  {250,   250,   250,   250},   {400,   200,   400,   2000},
#elif defined RES_MX											// This value is inaccurate
	{30000, 0,     0,     0},     {250,   250,   250,   250},   {200,   200,   200,   200},   {250,   250,   250,   250},   {1000,  4000,  1000,  4000},
#elif defined RES_NL
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {500,   9500,  500,   9500},  {250,   250,   250,   250},   {1000,  4000,  1000,  4000},
#elif defined RES_NO
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {200,   600,   200,   1000},  {200,   200,   200,   200},   {1000,  4000,  1000,  4000},
#elif defined RES_NZ
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {250,   250,   250,   3250},  {500,   500,   500,   500},   {400,   200,   400,   2000},
#elif defined RES_PL
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {150,   150,   150,   4000},  {500,   500,   500,   500},   {1000,  4000,  1000,  4000},
#elif defined RES_PT
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {200,   200,   200,   5000},  {200,   200,   200,   200},   {1000,  5000,  1000,  5000},
#elif defined RES_RO
	{30000, 0,     0,     0},     {350,   350,   350,   350},   {200,   5000,  200,   5000},  {175,   175,   175,   175},   {1000,  4000,  1000,  4000},
#elif defined RES_RU
	{30000, 0,     0,     0},     {350,   350,   350,   350},   {200,   5000,  200,   5000},  {175,   175,   175,   175},   {1000,  4000,  1000,  4000},
#elif defined RES_SG
	{30000, 0,     0,     0},     {750,   750,   750,   750},   {300,   200,   300,   3200},  {250,   250,   250,   250},   {400,   200,   400,   2000},
#elif defined RES_SK
	{30000, 0,     0,     0},     {330,   330,   330,   330},   {330,   9000,  330,   9000},  {165,   165,   165,   165},   {1000,  4000,  1000,  4000},
#elif defined RES_TR
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {300,   10000, 300,   10000}, {250,   250,   250,   250},   {2000,  4000,  2000,  4000},
#elif defined RES_TW
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {250,   250,   250,   5250},  {500,   500,   500,   500},   {1000,  2000,  1000,  2000},
#elif defined RES_ZA
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {400,   4000,  400,   4000},  {250,   250,   250,   250},   {400,   200,   400,   2000},
#else // RES_US and others
	{30000, 0,     0,     0},     {500,   500,   500,   500},   {300,   10000, 300,   10000}, {250,   250,   250,   250},   {2000,  4000,  2000,  4000},
#endif
};

const USHORT _sDtmfFreq1[DTMF_SIGNAL_ALL] = {1336, 1209, 1336, 1477, 1209, 1336, 1477, 1209, 1336, 1477, 1209, 1477,
//                                            dial,busy,waiting,congestion,ring
#if defined RES_AR
											  425,	425,  425,	425, 425,
#elif defined RES_AU
											  425,	425,  425,	425, 400,
#elif defined RES_BE
											  425,	425,  1400,	425, 425,
#elif defined RES_BG
											  425,	425,  425,	425, 425,
#elif defined RES_BR
											  425,	425,  425,	425, 425,
#elif defined RES_CA
											  350,  480,  440,  480, 440,
#elif defined RES_CN
											  450,  450,  450,  450, 450,
#elif defined RES_CL
											  400,  400,  900,  400, 400,
#elif defined RES_CZ
											  425,  425,  425,  425, 425,
#elif defined RES_DE
											  425,	425,  425,	425, 425,
#elif defined RES_EG	// Dial tone and waiting tone are inaccurate
											  425,	425,  425,	450, 425,	
#elif defined RES_ES
											  425,	425,  425,	425, 425,
#elif defined RES_FR
											  330,  440,  440,  440, 440,
#elif defined RES_GB
											  350,  400,  440,  400, 400,
#elif defined RES_GR
											  425,  425,  425,  425, 425,
#elif defined RES_HK	
											  350,	480,  440,	480, 440,
#elif defined RES_HU	
											  425,	425,  425,	425, 425,
#elif defined RES_IN	
											  400,	400,  400,	400, 400,
#elif defined RES_IT
											  425,	425,  425,	425, 425,
#elif defined RES_JP
											  400,  400,  400,  400, 400,
#elif defined RES_KR
											  350,  480,  350,  480, 440,
#elif defined RES_LK
											  425,	425,  425,	425, 425,
#elif defined RES_MX	// Waiting tone is inaccurate
											  425,	425,  425,	425, 425,
#elif defined RES_NL
											  425,	425,  425,	425, 425,
#elif defined RES_NO
											  425,	425,  425,	425, 425,
#elif defined RES_NZ
											  400,  400,  400,  900, 400,
#elif defined RES_PL
											  425,	425,  425,	425, 425,
#elif defined RES_PT
											  425,	425,  425,	425, 425,
#elif defined RES_RO
											  425,	425,  425,	425, 425,
#elif defined RES_RU
											  425,	425,  425,	425, 425,
#elif defined RES_SG
											  425,	425,  425,	425, 425,
#elif defined RES_SK
											  425,	425,  425,	425, 425,
#elif defined RES_TR
											  450,  450,  450,  450, 450,
#elif defined RES_TW
											  350,	480,  350,	480, 440,
#elif defined RES_ZA
											  400,  400,  400,  400, 400,
#else // RES_US and others
											  350,  480,  440,  480, 440,
#endif
};

const USHORT _sDtmfFreq2[DTMF_SIGNAL_ALL] = {941,  697,  697,  697,  770,  770,  770,  852,  852,  852, 941,  941,
//                                            dial,busy,waiting,congestion,ring
#if defined RES_AR
											  425,	425,  425,	425, 425,
#elif defined RES_AU
											  425,	425,  425,	425, 400,
#elif defined RES_BE
											  425,	425,  1400,	425, 425,
#elif defined RES_BG
											  425,	425,  425,	425, 425,
#elif defined RES_BR
											  425,	425,  425,	425, 425,
#elif defined RES_CA
											  440,  620,  440,  620, 480,
#elif defined RES_CN
											  450,  450,  450,  450, 450,
#elif defined RES_CL
											  400,  400,  1300,  400, 400,
#elif defined RES_CZ
											  425,  425,  425,  425, 425,
#elif defined RES_DE
											  425,	425,  425,	425, 425,
#elif defined RES_EG	// Dial tone and waiting tone are inaccurate
											  425,	425,  425,	450, 425,
#elif defined RES_ES
											  425,	425,  425,	425, 425,
#elif defined RES_FR
											  440,  440,  440,  440, 440,
#elif defined RES_GB
											  440,  400,  440,  400, 450,
#elif defined RES_GR
											  425,  425,  425,  425, 425,
#elif defined RES_HK	
											  440,	620,  440,	620, 480,
#elif defined RES_HU	
											  425,	425,  425,	425, 425,
#elif defined RES_IN	
											  400,	400,  400,	400, 400,
#elif defined RES_IT
											  425,	425,  425,	425, 425,
#elif defined RES_JP
											  400,  400,  400,  400, 400,
#elif defined RES_KR
											  440,  620,  440,  620, 480,
#elif defined RES_LK
											  425,	425,  425,	425, 425,
#elif defined RES_MX	// Waiting tone is inaccurate
											  425,	425,  425,	425, 425,
#elif defined RES_NL
											  425,	425,  425,	425, 425,
#elif defined RES_NO
											  425,	425,  425,	425, 425,
#elif defined RES_NZ
											  400,  400,  400,  900, 450,
#elif defined RES_PL
											  425,	425,  425,	425, 425,
#elif defined RES_PT
											  425,	425,  425,	425, 425,
#elif defined RES_RO
											  425,	425,  425,	425, 425,
#elif defined RES_RU
											  425,	425,  425,	425, 425,
#elif defined RES_SG
											  425,	425,  425,	425, 425,
#elif defined RES_SK
											  425,	425,  425,	425, 425,
#elif defined RES_TR
											  450,  450,  450,  450, 450,
#elif defined RES_TW
											  440,	620,  440,	620, 480,
#elif defined RES_ZA
											  400,  400,  400,  400, 400,
#else // RES_US and others
											  440,  620,  440,  620, 480,
#endif
};

// stutter tone 3x 0.1s on 0.1s off then on, put 400 in first position is because the speaker will cut the first tone
const USHORT _cStutterToneIntervals[STUTTER_TONE_DSP_DURATION] = {400, 100, 100, 100, 100, 100, 30000, 0};

void _DtmfGenTone(USHORT sFreq1, USHORT sFreq2, PCHAR pIntervals, UCHAR iMaxDuration)
{
	Dsp_sDtmf[0] = sFreq1;
	Dsp_sDtmf[1] = sFreq2;
	memcpy((PCHAR)Dsp_sDurations, pIntervals, iMaxDuration*2);
	Dsp_iMaxDuration = iMaxDuration;
	Dsp_iDurationIndex = 0;
	Dsp_sOpDuration = Dsp_sDurations[0];
}


void DtmfPlayTone(UCHAR iType)
{
	_DtmfGenTone(_sDtmfFreq1[iType], _sDtmfFreq2[iType], (PCHAR)(&_sToneIntervals[iType-DTMF_SIGNAL_DIAL]), DTMF_TONE_DSP_DURATION);
}

void DtmfPlayStutter()
{
	_DtmfGenTone(_sDtmfFreq1[DTMF_SIGNAL_DIAL], _sDtmfFreq2[DTMF_SIGNAL_DIAL], (PCHAR)(_cStutterToneIntervals), STUTTER_TONE_DSP_DURATION);
}

void DtmfPlayKey(UCHAR iAsciiKey)
{
	UCHAR iDtmfCode;

	if (iAsciiKey == (UCHAR)'.')
	{
		iAsciiKey = '*';
	}
	iDtmfCode = ascii2keyid(iAsciiKey);
	if (iDtmfCode == MAP_KEY_UNKNOWN)	
	{
		memset((PCHAR)Dsp_sDtmf, 0, 4);
	}
	else
	{
		Dsp_sDtmf[0] = _sDtmfFreq1[iDtmfCode];
		Dsp_sDtmf[1] = _sDtmfFreq2[iDtmfCode];
	}
	memset((PCHAR)Dsp_sDurations, 0, MAX_DSP_DURATION*2);
	Dsp_iDurationIndex = 0;
	Dsp_sOpDuration = DTMF_KEY_TIME;
}

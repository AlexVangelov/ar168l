/*-------------------------------------------------------------------------
   AR1688 UI string copy right information

   Copyright (C) 2006-2010. Lin, Rongrong <woody@palmmicro.com>
                            Tang, Li      <tangli@palmmicro.com>
                            Li, Yajing    <liyajing@palmmicro.com>

   Sebastien RAILLARD <sebastien.raillard at gmail.com> added French support
   Alfredo Sorbello <sorbello at antek.it> added Italian support
   Diego Frick <dfrick at bit2net.com> added Spanish support
   Ferhat OLGUN <ferhatolgun2 at yahoo.com> added Turkish support
   Geisom <geisom at canall.com.br> added Brasil Portuguese support

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
#include "ar168.h"
#include "core.h"
#include "apps.h"
#include "bank4.h"

#ifdef OEM_BT2008
#if defined RES_CN && defined DISPLAY_DOT
#include "res\\bt2008\\cn\\menu.h"
#else // Default = REG_US
#include "res\\bt2008\\us\\menu.h"
#endif
#elif defined OEM_INNOMEDIA
#include "res\\innomedia\\us\\menu.h"
#else
#ifdef RES_BR // Brasil
#include "res\\br\\menu.h"
#elif defined RES_CN && defined DISPLAY_DOT
#include "res\\cn\\menu.h"
#elif defined RES_ES // Español
#include "res\\es\\menu.h"
#elif defined RES_FR // French
#include "res\\fr\\menu.h"
#elif defined RES_IT // Italy
#include "res\\it\\menu.h"
#elif defined RES_KR && defined DISPLAY_DOT // Korean
#include "res\\kr\\menu.h"
#elif defined RES_RO // Romanian
#include "res\\ro\\menu.h"
#elif defined RES_RU && defined DISPLAY_DOT // Russian
#include "res\\ru\\menu.h"
#elif defined RES_TR // Turkish
#ifdef OEM_IP20
#include "res\\ip20\\tr\\menu.h"
#else
#include "res\\tr\\menu.h"
#endif
#elif defined RES_VN // Vietnam
#include "res\\vn\\menu.h"
#else // Default = REG_US
#include "res\\us\\menu.h"
#endif
#endif

const UCHAR _cIDS[][DISPLAY_MAX_CHAR+1] = {
{'V', 'e', 'r', ':', ' ', SOFT_VER_HIGH+'0', '.', SOFT_VER_LOW+'0', SOFT_VER_BUILD+'0', '.', ENG_BUILD_HIGH+'0', ENG_BUILD_MID+'0', ENG_BUILD_LOW+'0', 0},
#ifdef OEM_BT2008
#if defined RES_CN && defined DISPLAY_DOT
#include "res\\bt2008\\cn\\str.h"
#else // Default = REG_US
#include "res\\bt2008\\us\\str.h"
#endif
#elif defined OEM_INNOMEDIA
#include "res\\innomedia\\us\\str.h"
#else
#ifdef RES_BR // Brasil
#include "res\\br\\str.h"
#elif defined RES_CN && defined DISPLAY_DOT
#include "res\\cn\\str.h"
#elif defined RES_ES // Español
#include "res\\es\\str.h"
#elif defined RES_FR // French
#include "res\\fr\\str.h"
#elif defined RES_IT // Italy
#include "res\\it\\str.h"
#elif defined RES_KR && defined DISPLAY_DOT // Korean
#include "res\\kr\\str.h"
#elif defined RES_RO // Romanian
#include "res\\ro\\str.h"
#elif defined RES_RU && defined DISPLAY_DOT // Russian
#include "res\\ru\\str.h"
#elif defined RES_TR // Turkish
#ifdef OEM_IP20
#include "res\\ip20\\tr\\str.h"
#else
#include "res\\tr\\str.h"
#endif
#elif defined RES_VN // Vietnam
#include "res\\vn\\str.h"
#else // Default = REG_US
#include "res\\us\\str.h"
#endif
#endif
};

void LoadString(UCHAR iIDS, PCHAR pBuf)
{
	if (iIDS == IDS_DISPLAY_MODEL)
	{
#ifdef OEM_BIT2NET
		strcpy(pBuf, "b2N1000");
#elif defined VER_AR168G
		strcpy(pBuf, (PCHAR)"GP1260");
#elif defined OEM_IP20
		strcpy(pBuf, "KAREL IP20");
#elif defined OEM_WEBNEED
		strcpy(pBuf, (PCHAR)"WebNeed");
#elif defined OEM_AEM
		strcpy(pBuf, (PCHAR)"AEM2266");
#elif defined OEM_INNOMEDIA
		strcpy(pBuf, (PCHAR)"MTA7308-SL");
#else
		OptionsGetString(pBuf, OPT_FLAG_PRODUCT, MAX_FLAG_PRODUCT_LEN);
		// use strcpy(pBuf, (PCHAR)"XXXXXXXX"); to replace GP1266 display and SIP "User-Agent" head.
#endif	
	}
	else
	{
		strcpy(pBuf, _cIDS[iIDS]);
	}
}

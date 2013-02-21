/*-------------------------------------------------------------------------
   AR1688 HTTP Digest Authentication function copy right information

   Copyright (c) 2006-2010. Tang, Li <tangli@palmmicro.com>

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
#include "core.h"
#include "apps.h"
#include "bank6.h"

#define ALG_MD5		0
#define ALG_MD5SESS	1
#define ALG_NONE	2

#define QOP_AUTH		0
#define QOP_AUTHINT		1
#define QOP_NONE		2

const UCHAR _cCnonce[] = "0a4f113b";
const UCHAR _cNc[] = "00000001";
const UCHAR _cHEntity[] = "d41d8cd98f00b204e9800998ecf8427e";
const UCHAR _cQopType[][9] = {"auth", "auth-int"};
const UCHAR _cAlgType[][9] = {"MD5", "MD5-sess"};

PCHAR _unquote_str(PCHAR pStr)
{
	PCHAR pTemp;
	PCHAR pCur;

	pCur = pStr;
	pTemp = strchr(pCur, '"');
	if (pTemp)
	{
		pCur = pTemp;
		pCur ++;
		SkipField(pCur, '"');
	}
	else
	{
		SkipField(pCur, ' ');
	}
	return pCur;
}

void http_digest(PCHAR pDstBuf, PCHAR pAuthData, PCHAR pUri, PCHAR pAccount, PCHAR pPassword, PCHAR pMethod)
{
	PCHAR pCur;
	PCHAR pTemp;
	PCHAR pValue;
	PCHAR pRealm;
	PCHAR pNonce;
	PCHAR pOpaque;
	BOOLEAN bRealm, bNonce, bOpaque;
	UCHAR iAlg, iQop;
	UCHAR pHA1[33];
	UCHAR pHA2[33];
	UCHAR pMD5Value[MD5_SIGNATURE_SIZE];

	strcpy(pDstBuf, pAuthData);
	pCur = pDstBuf;
	if (memcmp_lowercase(pCur, "digest "))
	{
		pDstBuf[0] = 0;
		return;
	}
	pCur += 7;

	iQop = QOP_NONE;
	iAlg = ALG_NONE;
	pRealm = NULL;
	pNonce = NULL;
	pOpaque = NULL;
	bRealm = FALSE;
	bNonce = FALSE;
	bOpaque = FALSE;
	do
	{
		pTemp = SkipField(pCur, ',');
		pValue = SkipField(pCur, '=');
		pValue = _unquote_str(pValue);
		if (!strcmp_lowercase(pCur, "realm"))
		{
			if (!bRealm)
			{
				pRealm = heap_save_str(pValue);
				bRealm = TRUE;
			}
		}
		else if (!strcmp_lowercase(pCur, "nonce"))
		{
			if (!bNonce)
			{
				pNonce = heap_save_str(pValue);
				bNonce = TRUE;
			}
		}
		else if (!strcmp_lowercase(pCur, "opaque"))
		{
			if (!bOpaque)
			{
				pOpaque = heap_save_str(pValue);
				bOpaque = TRUE;
			}
		}
		else if (!strcmp_lowercase(pCur, "qop"))
		{
			iQop = QOP_AUTH;
		}
		else if (!strcmp_lowercase(pCur, "algorithm"))
		{
			if (!strcmp_lowercase(pValue, "md5"))
			{
				iAlg = ALG_MD5;
			}
			if (!strcmp_lowercase(pValue, "md5-sess"))
			{
				iAlg = ALG_MD5SESS;
			}
		}
		if (pTemp)
		{
			pCur = pTemp;
//			pCur ++;
			pCur += count_space(pCur);
		}
	} while (pTemp);	

	line_backup();
	// generate HA1
	line_start(pDstBuf);
	line_add_str_with_char(pAccount, ':');
	line_add_str_with_char(pRealm, ':');
	line_add_str(pPassword);
	MD5GenValue(pHA2, pDstBuf, line_get_len());
	TaskMiniRun();
	if (iAlg == ALG_MD5SESS)
	{
		line_start(pDstBuf);
		line_add_data(pHA2, MD5_SIGNATURE_SIZE);
		line_add_char(':');
		line_add_str_with_char(pNonce, ':');
		line_add_str(_cCnonce);
		MD5GenValue(pHA2, pDstBuf, line_get_len());
		TaskMiniRun();
	}
	char2asc_str(pHA1, pHA2, MD5_SIGNATURE_SIZE, FALSE);

	// Generate HA2
	line_start(pDstBuf);
	line_add_str_with_char(pMethod, ':');
	line_add_str(pUri);
	if (iQop == QOP_AUTHINT)
	{
		line_add_char(':');
		line_add_str(_cHEntity);
	}
	MD5GenValue(pMD5Value, pDstBuf, line_get_len());
	TaskMiniRun();
	char2asc_str(pHA2, pMD5Value, MD5_SIGNATURE_SIZE, FALSE);

	// generate final response
	line_start(pDstBuf);
	line_add_str_with_char(pHA1, ':');
	line_add_str_with_char(pNonce, ':');
	if (iQop != QOP_NONE)
	{
		line_add_str_with_char(_cNc, ':');
		line_add_str_with_char(_cCnonce, ':');
		line_add_str_with_char(_cQopType[iQop], ':');
	}
	line_add_str(pHA2);
	MD5GenValue(pMD5Value, pDstBuf, line_get_len());
	TaskMiniRun();
	char2asc_str(pHA2, pMD5Value, MD5_SIGNATURE_SIZE, FALSE);

	// Write response
	line_start(pDstBuf);
	line_add_str("Digest username=\"");
	line_add_str_with_char(pAccount, '"');
	if (bRealm)
	{
		line_add_str(",realm=\"");
		line_add_str_with_char(pRealm, '"');
	}
	if (bNonce)
	{
		line_add_str(",nonce=\"");
		line_add_str_with_char(pNonce, '"');
	}
	line_add_str(",uri=\"");
	line_add_str_with_char(pUri, '"');
	// qop=auth, some system don't allow quote sign qop="auth" here
	if (iQop != QOP_NONE)
	{
		line_add_str(",qop=");
		line_add_str(_cQopType[iQop]);
		line_add_str(",nc=");
		line_add_str(_cNc);
		line_add_str(",cnonce=\"");
		line_add_str_with_char(_cCnonce, '"');
	}
	if (bOpaque)
	{
		line_add_str(",opaque=\"");
		line_add_str_with_char(pOpaque, '"');
	}
	if (iAlg != ALG_NONE)
	{
		line_add_str(",algorithm=");
		line_add_str(_cAlgType[iAlg]);
	}
	line_add_str(",response=\"");
	line_add_str_with_char(pHA2, '"');

	if (pRealm)		free(pRealm);
	if (pNonce)		free(pNonce);
	if (pOpaque)	free(pOpaque);

	line_restore();
}


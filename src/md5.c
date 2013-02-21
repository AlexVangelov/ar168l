
/*
 **********************************************************************
 ** md5.c                                                            **
 ** RSA Data Security, Inc. MD5 Message Digest Algorithm             **
 ** Created: 2/17/90 RLR                                             **
 ** Revised: 1/91 SRD,AJ,BSK,JT Reference C Version                  **
 **********************************************************************
 */

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

#include "type.h"
#include "core.h"

/* -- include the following line if the md5.h header file is separate -- */
#include "md5.h"

/* forward declaration */
void Transform (PLONG buf, PLONG in);

const UCHAR PADDING[64] = {
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G and H are basic MD5 functions: selection, majority, parity */
//#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define F(x, y, z) ((((y) ^ (z)) & (x)) ^ (z))

//#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define G(x, y, z) ((((x) ^ (y)) & (z)) ^ (y))

#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z))) 

/* ROTATE_LEFT rotates x left n bits */
//#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define ROTATE_LEFT(x, n) rlc_32x8((x), (n))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */
#define FF(a, b, c, d, x, s, ac) \
  {(a) += F ((b), (c), (d)) + (x) + (ULONG)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) \
  {(a) += G ((b), (c), (d)) + (x) + (ULONG)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) \
  {(a) += H ((b), (c), (d)) + (x) + (ULONG)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) \
  {(a) += I ((b), (c), (d)) + (x) + (ULONG)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }

void MD5Update (P_MD5_CTX mdContext, PCHAR inBuf, UCHAR iInLen)
{
	ULONG in[16];
//	UCHAR i;
	UCHAR mdi;

	/* compute number of bytes mod 64 */
	mdi = (UCHAR)(rr_32x8(mdContext->i[0], 3) & 0x3F);

	/* update number of bits */
//	if ((mdContext->i[0] + rl_32x8((ULONG)iInLen, 3)) < mdContext->i[0])
	if ((mdContext->i[0] + ((USHORT)iInLen << 3)) < mdContext->i[0])
		mdContext->i[1]++;
	mdContext->i[0] += ((USHORT)iInLen << 3);
//	mdContext->i[0] += rl_32x8((ULONG)iInLen, 3);
//	mdContext->i[1] += rr_32x8((ULONG)iInLen, 29);

	do
	{
		if (iInLen + mdi >= 64)
		{
			memcpy((PCHAR)(&mdContext->in[mdi]), inBuf, 64 - mdi);
			memcpy((PCHAR)(&in[0]), mdContext->in, 64);
			Transform (mdContext->buf, in);
			inBuf += 64 - mdi;
			iInLen -= 64 - mdi;
			mdi = 0;
		}
		else
		{
			memcpy((PCHAR)(&mdContext->in[mdi]), inBuf, iInLen);
			break;
		}
	} while (iInLen);
/*
	while (iInLen--) 
	{
		// add new character to buffer, increment mdi
		mdContext->in[mdi++] = *inBuf++;

		// transform if necessary
		if (mdi == 0x40) 
		{
//			for (i = 0; i < 16; i++)
//			{
//				in[i] = (((ULONG)mdContext->in[ii+3]) << 24) |
//					(((ULONG)mdContext->in[ii+2]) << 16) |
//					(((ULONG)mdContext->in[ii+1]) << 8) |
//					((ULONG)mdContext->in[ii]);
//			}
			memcpy((PCHAR)(&in[0]), mdContext->in, 64);
			Transform (mdContext->buf, in);
			mdi = 0;
		}
	}
*/
}

//void MD5Final (P_MD5_CTX mdContext, PCHAR outBuf)
void MD5Final (P_MD5_CTX mdContext)
{
	ULONG in[16];
//	UCHAR i;
	UCHAR mdi, padLen;
//	PCHAR pCur;
//	UCHAR pPadding[64];

	/* save number of bits */
	in[14] = mdContext->i[0];
	in[15] = mdContext->i[1];

	/* compute number of bytes mod 64 */
	mdi = (UCHAR)(rr_32x8(mdContext->i[0], 3) & 0x3F);

	/* pad out to 56 mod 64 */
	padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);

//	memset((PCHAR)pPadding, 0, 64);
//	pPadding[0] = 0x80;
//	MD5Update(mdContext, pPadding, padLen);
	MD5Update(mdContext, PADDING, padLen);

	/* append length in bits and transform */
//	pCur = mdContext->in;
//	for (i = 0; i < 14; i++)
//	{
/*		in[i] = (((ULONG)mdContext->in[ii+3]) << 24) |
			(((ULONG)mdContext->in[ii+2]) << 16) |
			(((ULONG)mdContext->in[ii+1]) << 8) |
			((ULONG)mdContext->in[ii]);
*/
//		in[i] = PXCHAR2ULONG_L(pCur);
//		pCur += 4;
//	}
	memcpy((PCHAR)(&in[0]), mdContext->in, 56);
	Transform (mdContext->buf, in);

	/* store buffer in digest */
//	pCur = outBuf;
//	for (i = 0; i < 4; i++) 
//	{
/*		mdContext->digest[ii] = (UCHAR)(mdContext->buf[i] & 0xFF);
		mdContext->digest[ii+1] = (UCHAR)((mdContext->buf[i] >> 8) & 0xFF);
		mdContext->digest[ii+2] = (UCHAR)((mdContext->buf[i] >> 16) & 0xFF);
		mdContext->digest[ii+3] = (UCHAR)((mdContext->buf[i] >> 24) & 0xFF);
*/
//		ULONG2PCHAR_L(mdContext->buf[i], (PCHAR)pCur);
//		pCur += 4;
//	}
//	memcpy(outBuf, (PCHAR)(&mdContext->buf[0]), 16);
}

/* Basic MD5 step. Transform buf based on in.
 */

  /* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
/*
	const UCHAR S1x[4] = {7, 12, 17, 22};
	const ULONG FF_L[16] = {
3614090360,
3905402710,
 606105819,
3250441966,
4118548399,
1200080426,
2821735955,
4249261313,
1770035416,
2336552879,
4294925233,
2304563134,
1804603682,
4254626195,
2792965006,
1236535329};
*/

  /* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
/*
	const UCHAR S2x[4] = {5, 9, 14, 20};
	const ULONG GG_L[16] = {
4129170786,
3225465664,
 643717713,
3921069994,
3593408605,
  38016083,
3634488961,
3889429448,
 568446438,
3275163606,
4107603335,
1163531501,
2850285829,
4243563512,
1735328473,
2368359562};
*/
  /* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
/*
	const UCHAR S3x[4] = {4, 11, 16, 23};
	const ULONG HH_L[16] = {
4294588738,
2272392833,
1839030562,
4259657740,
2763975236,
1272893353,
4139469664,
3200236656,
 681279174,
3936430074,
3572445317,
  76029189,
3654602809,
3873151461,
 530742520,
3299628645};
*/
  /* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
/*
	const UCHAR S4x[4] = {6, 10, 15, 21};
	const ULONG II_L[16] = {
4096336452,
1126891415,
2878612391,
4237533241,
1700485571,
2399980690,
4293915773,
2240044497,
1873313359,
4264355552,
2734768916,
1309151649,
4149444226,
3174756917,
 718787259,
3951481745};
*/

void Transform (PLONG buf, PLONG in)
{
  ULONG a = buf[0], b = buf[1], c = buf[2], d = buf[3];

  FF ( a, b, c, d, in[ 0], S11, 3614090360ul); /* 1 */
  FF ( d, a, b, c, in[ 1], S12, 3905402710ul); /* 2 */
  FF ( c, d, a, b, in[ 2], S13,  606105819ul); /* 3 */
  FF ( b, c, d, a, in[ 3], S14, 3250441966ul); /* 4 */
  FF ( a, b, c, d, in[ 4], S11, 4118548399ul); /* 5 */
  FF ( d, a, b, c, in[ 5], S12, 1200080426ul); /* 6 */
  FF ( c, d, a, b, in[ 6], S13, 2821735955ul); /* 7 */
  FF ( b, c, d, a, in[ 7], S14, 4249261313ul); /* 8 */
  FF ( a, b, c, d, in[ 8], S11, 1770035416ul); /* 9 */
  FF ( d, a, b, c, in[ 9], S12, 2336552879ul); /* 10 */
  FF ( c, d, a, b, in[10], S13, 4294925233ul); /* 11 */
  FF ( b, c, d, a, in[11], S14, 2304563134ul); /* 12 */
  FF ( a, b, c, d, in[12], S11, 1804603682ul); /* 13 */
  FF ( d, a, b, c, in[13], S12, 4254626195ul); /* 14 */
  FF ( c, d, a, b, in[14], S13, 2792965006ul); /* 15 */
  FF ( b, c, d, a, in[15], S14, 1236535329ul); /* 16 */

  GG ( a, b, c, d, in[ 1], S21, 4129170786ul); /* 17 */
  GG ( d, a, b, c, in[ 6], S22, 3225465664ul); /* 18 */
  GG ( c, d, a, b, in[11], S23,  643717713ul); /* 19 */
  GG ( b, c, d, a, in[ 0], S24, 3921069994ul); /* 20 */
  GG ( a, b, c, d, in[ 5], S21, 3593408605ul); /* 21 */
  GG ( d, a, b, c, in[10], S22,   38016083ul); /* 22 */
  GG ( c, d, a, b, in[15], S23, 3634488961ul); /* 23 */
  GG ( b, c, d, a, in[ 4], S24, 3889429448ul); /* 24 */
  GG ( a, b, c, d, in[ 9], S21,  568446438ul); /* 25 */
  GG ( d, a, b, c, in[14], S22, 3275163606ul); /* 26 */
  GG ( c, d, a, b, in[ 3], S23, 4107603335ul); /* 27 */
  GG ( b, c, d, a, in[ 8], S24, 1163531501ul); /* 28 */
  GG ( a, b, c, d, in[13], S21, 2850285829ul); /* 29 */
  GG ( d, a, b, c, in[ 2], S22, 4243563512ul); /* 30 */
  GG ( c, d, a, b, in[ 7], S23, 1735328473ul); /* 31 */
  GG ( b, c, d, a, in[12], S24, 2368359562ul); /* 32 */

  HH ( a, b, c, d, in[ 5], S31, 4294588738ul); /* 33 */
  HH ( d, a, b, c, in[ 8], S32, 2272392833ul); /* 34 */
  HH ( c, d, a, b, in[11], S33, 1839030562ul); /* 35 */
  HH ( b, c, d, a, in[14], S34, 4259657740ul); /* 36 */
  HH ( a, b, c, d, in[ 1], S31, 2763975236ul); /* 37 */
  HH ( d, a, b, c, in[ 4], S32, 1272893353ul); /* 38 */
  HH ( c, d, a, b, in[ 7], S33, 4139469664ul); /* 39 */
  HH ( b, c, d, a, in[10], S34, 3200236656ul); /* 40 */
  HH ( a, b, c, d, in[13], S31,  681279174ul); /* 41 */
  HH ( d, a, b, c, in[ 0], S32, 3936430074ul); /* 42 */
  HH ( c, d, a, b, in[ 3], S33, 3572445317ul); /* 43 */
  HH ( b, c, d, a, in[ 6], S34,   76029189ul); /* 44 */
  HH ( a, b, c, d, in[ 9], S31, 3654602809ul); /* 45 */
  HH ( d, a, b, c, in[12], S32, 3873151461ul); /* 46 */
  HH ( c, d, a, b, in[15], S33,  530742520ul); /* 47 */
  HH ( b, c, d, a, in[ 2], S34, 3299628645ul); /* 48 */

  II ( a, b, c, d, in[ 0], S41, 4096336452ul); /* 49 */
  II ( d, a, b, c, in[ 7], S42, 1126891415ul); /* 50 */
  II ( c, d, a, b, in[14], S43, 2878612391ul); /* 51 */
  II ( b, c, d, a, in[ 5], S44, 4237533241ul); /* 52 */
  II ( a, b, c, d, in[12], S41, 1700485571ul); /* 53 */
  II ( d, a, b, c, in[ 3], S42, 2399980690ul); /* 54 */
  II ( c, d, a, b, in[10], S43, 4293915773ul); /* 55 */
  II ( b, c, d, a, in[ 1], S44, 2240044497ul); /* 56 */
  II ( a, b, c, d, in[ 8], S41, 1873313359ul); /* 57 */
  II ( d, a, b, c, in[15], S42, 4264355552ul); /* 58 */
  II ( c, d, a, b, in[ 6], S43, 2734768916ul); /* 59 */
  II ( b, c, d, a, in[13], S44, 1309151649ul); /* 60 */
  II ( a, b, c, d, in[ 4], S41, 4149444226ul); /* 61 */
  II ( d, a, b, c, in[11], S42, 3174756917ul); /* 62 */
  II ( c, d, a, b, in[ 2], S43,  718787259ul); /* 63 */
  II ( b, c, d, a, in[ 9], S44, 3951481745ul); /* 64 */

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}

/*
	const UCHAR i1[4] = {0, 3, 2, 1};
	const UCHAR i2[4] = {1, 0, 3, 2};
	const UCHAR i3[4] = {2, 1, 0, 3};
	const UCHAR i4[4] = {3, 2, 1, 0};

static void Transform (PLONG buf, PLONG in)
{
	ULONG abcd[4];
	UCHAR i, j, k;

	memcpy((PCHAR)abcd, (PCHAR)buf, 16);

	for (i = 0; i < 16; i ++)
	{
		j = i&3;
		FF ( abcd[i1[j]], abcd[i2[j]], abcd[i3[j]], abcd[i4[j]], in[i], S1x[j], FF_L[i]);
	}

	k = 1;
	for (i = 0; i < 16; i ++)
	{
		j = i&3;
		GG ( abcd[i1[j]], abcd[i2[j]], abcd[i3[j]], abcd[i4[j]], in[k], S2x[j], GG_L[i]);
		k += 5;
		k &= 15;
	}

	k = 5;
	for (i = 0; i < 16; i ++)
	{
		j = i&3;
		HH ( abcd[i1[j]], abcd[i2[j]], abcd[i3[j]], abcd[i4[j]], in[k], S3x[j], HH_L[i]);
		k += 3;
		k &= 15;
	}

	k = 0;
	for (i = 0; i < 16; i ++)
	{
		j = i&3;
		II ( abcd[i1[j]], abcd[i2[j]], abcd[i3[j]], abcd[i4[j]], in[k], S4x[j], II_L[i]);
		k += 7;
		k &= 15;
	}

  buf[0] += abcd[0];
  buf[1] += abcd[1];
  buf[2] += abcd[2];
  buf[3] += abcd[3];
}
*/

/*
const ULONG cMD5Init[6] = {0, 0, 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
	memcpy((PCHAR)(mdContext->i), (PCHAR)cMD5Init, 24);

void MD5Init (P_MD5_CTX mdContext)
{
  mdContext->i[0] = mdContext->i[1] = (ULONG)0;

  // Load magic initialization constants.
   mdContext->buf[0] = (ULONG)0x67452301;
  mdContext->buf[1] = (ULONG)0xefcdab89;
  mdContext->buf[2] = (ULONG)0x98badcfe;
  mdContext->buf[3] = (ULONG)0x10325476;
}
*/

const ULONG cMD5Init[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};

void MD5GenValue(PCHAR pDst, PCHAR pSrc, UCHAR iLen)
{
	MD5_CTX mdContext;

//	MD5Init (&mdContext);
	memset((PCHAR)mdContext.i, 0, 8);
	memcpy(pDst, (PCHAR)cMD5Init, 16);
	mdContext.buf = (PLONG)pDst;

	MD5Update (&mdContext, pSrc, iLen);
//	MD5Final (&mdContext, pDst);
	MD5Final (&mdContext);

//  p_memcpy_x2x(pDst, mdContext.digest, MD5_SIGNATURE_SIZE);
}


/* hmac-sha1.c -- hashed message authentication codes
   Copyright (C) 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Simon Josefsson.  */

#include "version.h"
#include "type.h"
#ifdef OEM_INNOMEDIA

#include "core.h"
#include "sha1.h"

#define IPAD 0x36
#define OPAD 0x5c

void memxor(PCHAR dst, PCHAR src, UCHAR len)
{
	UCHAR i;
	for (i = 0; i < len; i ++)
	{
		*dst ++ ^= *src ++;
	}
}

void hmac_sha1(PCHAR key, UCHAR keylen, PCHAR in, UCHAR inlen, PCHAR resbuf)
{
	SHA1Context keyhash;
	UCHAR optkeybuf[20];
	UCHAR block[64];
	UCHAR innerhash[20];

	/* Reduce the key's size, so that it becomes <= 64 bytes large.  */

	if (keylen > 64)
	{
		SHA1Reset(&keyhash);
		SHA1Input(&keyhash, key, keylen);
		SHA1Result(&keyhash, optkeybuf);
		key = optkeybuf;
		keylen = 20;
    }

	/* Compute INNERHASH from KEY and IN.  */

	memset(block, IPAD, sizeof(block));
	memxor(block, key, keylen);

	SHA1Reset(&keyhash);
	SHA1Input(&keyhash, block, 64);
	SHA1Input(&keyhash, in, inlen);
	SHA1Result(&keyhash, innerhash);

	/* Compute result from KEY and INNERHASH.  */

	memset(block, OPAD, sizeof(block));
	memxor(block, key, keylen);

	SHA1Reset(&keyhash);
	SHA1Input(&keyhash, block, 64);
	SHA1Input(&keyhash, innerhash, 20);
	SHA1Result(&keyhash, resbuf);
}

#endif

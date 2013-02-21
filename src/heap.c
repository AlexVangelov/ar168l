/*-------------------------------------------------------------------------
   malloc and free copy right information

   Copyright (C) 2004 - Maarten Brock, sourceforge.brock@dse.nl

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
-------------------------------------------------------------------------*/

#include "version.h"
#include "type.h"
#include "core.h"
#include "apps.h"
#include "ar168.h"

#ifdef SYS_CHIP_PLUS
#define HEAP_SIZE	0x2100
#else
#define HEAP_SIZE	0x1000
#endif

typedef struct _MEM_LIST MEM_LIST;
struct _MEM_LIST
{
	MEM_LIST *	next;
	MEM_LIST *	prev;
	USHORT		sLen;
	UCHAR		iMem;
};
#define MEM_LIST_SIZE (sizeof(MEM_LIST)-sizeof(char))

/* Base of heap_start is the first byte of the heap. */
MEM_LIST * p_heap_start;

void heap_init()
{
	p_heap_start = (MEM_LIST *)SRAM_BASE;
	p_heap_start->next = (MEM_LIST *)(SRAM_BASE + HEAP_SIZE - MEM_LIST_SIZE);
	p_heap_start->next->next = NULL; // mark it as last
	p_heap_start->prev       = NULL; // mark first as first
//	p_heap_start->sLen        = 0;    //Empty and ready.
	p_heap_start->sLen        = MEM_LIST_SIZE;    // do not actually use first node
}

void heap_debug()
{
	MEM_LIST * current_header;

	current_header = p_heap_start;
	UdpDebugString("heap debug:");
	while (1)
	{
		UdpDebugInfo("", (USHORT)current_header, current_header->sLen);
//		UdpDebugString(&current_header->iMem);
		current_header = current_header->next;    //else try next
		if (!current_header->next)
		{ //if end_of_list reached
			break;
		}
	}
}

/*
void * malloc(USHORT sSize)
{
	MEM_LIST * current_header;
	MEM_LIST * new_header;
	void * ret;
	
	sSize += MEM_LIST_SIZE; //We need a memory for header too
	current_header = p_heap_start;

	while (1)
	{
		//    current
		//    |   len       next
		//    v   v         v
		//....*****.........******....
		//         ^^^^^^^^^
		//           spare
		if ((((USHORT)current_header->next) - ((USHORT)current_header) - current_header->sLen) >= sSize)
		{ //if spare is more than needed, create new header at the begin of spare
			new_header = (MEM_LIST * )((USHORT)current_header + current_header->sLen);
			new_header->next = current_header->next; //and plug it into the chain
			new_header->prev = current_header;
			current_header->next = new_header;
			if (new_header->next)
			{
				new_header->next->prev = new_header;
			}
			new_header->sLen  = sSize; //mark as used
			ret = &new_header->iMem;
//			ret = &current_header->iMem;
//			UdpDebugInfo("malloc", (USHORT)ret - MEM_LIST_SIZE, sSize);
			break;
		}
		current_header = current_header->next;    //else try next
		if (!current_header->next)
		{ //if end_of_list reached
			ret = NULL;
			break;
		}
	}

	return ret;
}
*/

// Alex Gradinar 'Best Fit' algorithm implementation
void * malloc(USHORT sSize)
{
	MEM_LIST * current_header;
	MEM_LIST * new_header;
	void * ret;
	USHORT bestSize;
	USHORT currSize;
	MEM_LIST * best_header;

	bestSize=HEAP_SIZE;
	best_header=NULL;
	ret=NULL;

/*
	if (sSize > (HEAP_SIZE-MEM_LIST_SIZE))
	{
		return NULL; //To prevent overflow in next line
	}
*/
	sSize += MEM_LIST_SIZE; //We need a memory for header too
	current_header = p_heap_start;

	while (1)
	{
		//    current
		//    |   len       next
		//    v   v         v
		//....*****.........******....
		//         ^^^^^^^^^
		//           spare
		currSize=(((USHORT)current_header->next) - ((USHORT)current_header) - current_header->sLen);
		if (currSize >= sSize)
		{
			if (currSize < bestSize) {
				bestSize=currSize;
				best_header=current_header;
			}
		}
		current_header = current_header->next;
		if (!current_header->next)
		{ //if end_of_list reached
			break;
		}
	}

	if (best_header) {
		new_header = (MEM_LIST * )((USHORT)best_header + best_header->sLen);
		new_header->next = best_header->next; //and plug it into the chain
		new_header->prev = best_header;
		best_header->next = new_header;
		if (new_header->next)
		{
			new_header->next->prev = new_header;
		}
		new_header->sLen  = sSize; //mark as used
		ret = &new_header->iMem;
//		UdpDebugInfo("malloc", (USHORT)ret - MEM_LIST_SIZE, sSize);
	}

/*
	if (ret)
	{
		if (!current_header->sLen)
		{	//This code works only for first_header in the list and only
			current_header->sLen = sSize; //for first allocation
		}
		else
		{	//else create new header at the begin of spare
			new_header = (MEM_LIST * )((USHORT)current_header + current_header->sLen);
			new_header->next = current_header->next; //and plug it into the chain
			new_header->prev = current_header;
			current_header->next  = new_header;
			if (new_header->next)
			{
				new_header->next->prev = new_header;
			}
			new_header->sLen  = sSize; //mark as used
			ret = &new_header->iMem;
		}
		UdpDebugInfo("malloc", (USHORT)ret - MEM_LIST_SIZE, sSize);
	}
*/

//	heap_debug();

	return ret;
}

void free(void *p)
{
	MEM_LIST *prev_header, *pthis;

	if (p) //For allocated pointers only!
	{
		pthis = (MEM_LIST * )((USHORT)p - MEM_LIST_SIZE); //to start of header
//		UdpDebugInfo(" free ", (USHORT)pthis, pthis->sLen);
		if (pthis->prev) // For the regular header
		{
			prev_header = pthis->prev;
			prev_header->next = pthis->next;
			if (pthis->next)
			{
				pthis->next->prev = prev_header;
			}
		}
/*		else
		{
			pthis->sLen = 0; //For the first header
		}
*/
	}
//	else
//	{
//		UdpDebugString("free mismatch");
//	}
}

/*
USHORT _DebugStack() __naked
{
	__asm
	ld		hl,#0
	add		hl,sp
	ret
	__endasm;
}
*/

PCHAR heap_save_data(PCHAR pData, USHORT sLen)
{
	PCHAR pTemp;

	pTemp = NULL;
	if (sLen)
	{
		pTemp = (PCHAR)malloc(sLen);
		if (pTemp)
		{
			memcpy(pTemp, pData, sLen);
		}
		else
		{
			UdpDebugInfo("malloc failed", 0, sLen);
			heap_debug();
		}
	}
	return pTemp;
}

PCHAR heap_save_str(PCHAR pStr)
{
	USHORT sLen;

	sLen = strlen(pStr);
	if (sLen)
	{
		return heap_save_data(pStr, sLen + 1);
	}
	return NULL;
}


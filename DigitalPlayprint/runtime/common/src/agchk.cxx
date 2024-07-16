/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file agchk.cxx
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */

// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2018 Wizzer Works
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//  For information concerning this source file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

// Include system header files.
#include <stdio.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"
#include "mle/mlAssert.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/agopcode.h"
#include "mle/agchk.h"


// Code to resolve media references in an actor group chunk.

MleDppActorGroupChunk::MleDppActorGroupChunk(const char *filename, MlBoolean isLittle)
 : m_mediaNames(NULL), m_littleEndian(isLittle)
{
	m_fp = fopen(filename, "rb");
	MLE_ASSERT(m_fp != NULL);

	int err = fread(&m_nameOffset, sizeof(int), 1, m_fp);
	MLE_ASSERT(err == 1);

	err = fseek(m_fp, 0, SEEK_END);
	MLE_ASSERT(err == 0);

	int fileEnd = ftell(m_fp);
	MLE_ASSERT(fileEnd > 0);

	err = fseek(m_fp, m_nameOffset, SEEK_SET);
	MLE_ASSERT(err == 0);
	if ((m_nameLength = fileEnd - m_nameOffset) > 0)
	{
		m_mediaNames = new char[m_nameLength];
		err = fread(m_mediaNames, m_nameLength, 1, m_fp);
		MLE_ASSERT(err == 1);
	}
}


int MleDppActorGroupChunk::countNames(void)
{
	if (m_mediaNames == NULL)
		return 0;

	char *mediaEnd = &m_mediaNames[m_nameLength];
	char *namePtr = m_mediaNames;
	int counter = 0;
	while (namePtr < mediaEnd)
	{
		counter += 1;
		while (*namePtr++ && namePtr < mediaEnd)
			/* Do nothing. */ ;
	}

	return counter;
}


const char* MleDppActorGroupChunk::nextName(const char* name)
{
	MLE_ASSERT(m_mediaNames != NULL);

	char *mediaEnd = &m_mediaNames[m_nameLength];
	if (name == NULL || name >= mediaEnd)
		return NULL;

	const char* namePtr = name;
	while (*namePtr++ && namePtr < mediaEnd)
		/* Do nothing. */ ;

	return namePtr < mediaEnd ? namePtr : NULL;	
}


int MleDppActorGroupChunk::writeIndex(MlInt index)
{
	int counter = (32 + valueBits - 1) / valueBits;
	char writeOut = 0;
	
	// Make sure the buffer is large enough to hold the new index.
	//   maximum size is 5 bytes
	int buflen = m_resolvedChunkPtr - m_resolvedChunkBuf;
	while ( buflen + 5 > m_resolvedChunkSize )
	{
		m_resolvedChunkBuf = (MlByte *)
			mlRealloc(m_resolvedChunkBuf,2*m_resolvedChunkSize);
		m_resolvedChunkPtr = m_resolvedChunkBuf + buflen;
		m_resolvedChunkSize *= 2;
	}

	MLE_ASSERT(index >= 0);
	while (--counter >= 0)
	{
		if (writeOut || index >= 1 << valueBits * counter)
		{
			writeOut = 1;
			*m_resolvedChunkPtr++ = index >> valueBits * counter & valueMask;
		}
	}

	return index;
}

int MleDppActorGroupChunk::writeInt(MlInt index)
{
	int value = 0;

	// make sure the buffer is large enough to hold the new index
	//   int size is 4 bytes
	int buflen = m_resolvedChunkPtr - m_resolvedChunkBuf;
	while ( buflen + 4 > m_resolvedChunkSize )
	{
		m_resolvedChunkBuf = (MlByte *)
			mlRealloc(m_resolvedChunkBuf,2*m_resolvedChunkSize);
		m_resolvedChunkPtr = m_resolvedChunkBuf + buflen;
		m_resolvedChunkSize *= 2;
	}

    if (m_littleEndian)
	{
		*m_resolvedChunkPtr = (index) & 0xff;
		value = *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
		*m_resolvedChunkPtr = (index>>8) & 0xff;
		value |= *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
		*m_resolvedChunkPtr = (index>>16) & 0xff;
		value |= *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
		*m_resolvedChunkPtr = (index>>24) & 0xff;
		value |= *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
    } else {
		*m_resolvedChunkPtr = (index>>24) & 0xff;
		value = *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
		*m_resolvedChunkPtr = (index>>16) & 0xff;
		value = (value<<8) | *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
		*m_resolvedChunkPtr = (index>>8) & 0xff;
		value = (value<<8) | *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
		*m_resolvedChunkPtr = (index) & 0xff;
		value = (value<<8) | *m_resolvedChunkPtr;
		m_resolvedChunkPtr++;
    }

    return value;
}


unsigned char MleDppActorGroupChunk::nextChunkByte()
{
	MLE_ASSERT(m_chunkRef >= m_chunkStart && m_chunkRef < m_chunkEnd);

	return *m_chunkRef++;
}

unsigned char
MleDppActorGroupChunk::copyStreamByte()
{
	int buflen = m_resolvedChunkPtr - m_resolvedChunkBuf;
	if ( buflen == m_resolvedChunkSize )
	{
		m_resolvedChunkBuf = (unsigned char *)
			mlRealloc(m_resolvedChunkBuf,2*m_resolvedChunkSize);
		m_resolvedChunkPtr = m_resolvedChunkBuf + buflen;
		m_resolvedChunkSize *= 2;
	}

	return *m_resolvedChunkPtr++ = nextChunkByte();
}

int MleDppActorGroupChunk::readIndex(void)
{
	//long value = 0;
	int value = 0;
	MlChar byte;
	
	while ((byte = nextChunkByte()) >= 0)
	{
		MLE_ASSERT((value & (MlInt) 0x80000000 >> (valueBits - 1)) == 0);
		value = (value << valueBits) | byte;
	}
	MLE_ASSERT(value >= 0);

	m_chunkRef--; /* Back up one, since while loop always over-advances by one. */

	return value;
}

int MleDppActorGroupChunk::readInt(void)
{
    int value = nextChunkByte();

    if (m_littleEndian)
	{
		value |= (nextChunkByte()<<8);
		value |= (nextChunkByte()<<16);
		value |= (nextChunkByte()<<24);
    } else {
		value = (value<<8) | nextChunkByte();
		value = (value<<8) | nextChunkByte();
		value = (value<<8) | nextChunkByte();
    }


    return value;
}


const MlByte *
MleDppActorGroupChunk::resolveNames(int *length, const int* indices)
{
    int nameCount = countNames();

    // Worst case: there are five bytes per index in the stream.
#if 0
    MlInt resolvedByteStreamSize = 1 + m_nameOffset + m_nameCount * 5;
    MlByte *resolvedByteStream = new MlByte[ resolvedByteStreamSize ];
    m_resolvedChunkPtr = resolvedByteStream;
#else
    m_resolvedChunkSize = sizeof(MlInt);
    m_resolvedChunkBuf = (MlByte *)mlMalloc(sizeof(MlInt));
    m_resolvedChunkPtr = m_resolvedChunkBuf;
#endif
    int propertyLength;
    int chunkLength = m_nameOffset - sizeof(MlInt);
    //m_chunkStart = new unsigned char[chunkLength];
	m_chunkStart = (MlByte *)mlMalloc(chunkLength);
    m_chunkRef = m_chunkStart;
    m_chunkEnd = &m_chunkStart[chunkLength];

    int err = fseek(m_fp, sizeof(MlInt), SEEK_SET);
    MLE_ASSERT(err == 0);

    err = fread((char *) m_chunkStart, chunkLength, 1, m_fp);
    MLE_ASSERT(err == 1);

    // The first int is the index of the group in the table of group
    // classes, so we can invoke the right constructor.    
    copyInt();

    // The index is the count of actors in this group.  It can be an index
    // because we know an opcode follows it.
    copyIndex();

	int bytesToCopy, numElements;
    do {
		switch (copyStreamByte())
		{
		  case setForumOpcode:
		  case createActorOpcode:
		  case bindDelegateOpcode:
		  case parentDelegateOpcode:
		  case childDelegateOpcode:
		  case setPropertyOffsetOpcode:
			copyIndex();
			break;

		  case setPropertyLengthOpcode:
			propertyLength = copyIndex();
			break;

		  case copyPropertyOpcode:
			MLE_ASSERT(propertyLength > 0);
			bytesToCopy = propertyLength;
			while (--bytesToCopy >= 0)
			  copyStreamByte();
			break;

		  case endActorGroupOpcode:
			m_chunkRef = m_chunkEnd;
			break;

		  case copyMediaRefOpcode:
			writeIndex(indices[readIndex()]);
			break;

		  case copyDynamicPropertyOpcode:
			bytesToCopy = copyInt();
			while (--bytesToCopy >= 0)
			  copyStreamByte();
			break;

		  case copyArrayPropertyOpcode:
			numElements = copyInt();
			bytesToCopy = propertyLength * numElements;
			while (--bytesToCopy >= 0)
			  copyStreamByte();
			break;

		  default:
			MLE_ASSERT(0);
		}
    } while (m_chunkRef < m_chunkEnd);

	// Clean up.
    //delete (unsigned char*) m_chunkStart;
	mlFree((void *)m_chunkStart);
	m_chunkStart = NULL;

	// Calculate the length of the resolved table.
    *length = m_resolvedChunkPtr - m_resolvedChunkBuf;

	// Returne a pointer to the resolved chunk table.
    return m_resolvedChunkBuf;
}

void *
MleDppActorGroupChunk::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppActorGroupChunk::operator delete(void *p)
{
	mlFree(p);
}

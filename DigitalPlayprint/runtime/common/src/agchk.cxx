/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file agchk.cxx
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 */

// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2025 Wizzer Works
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
 : m_fp(NULL), m_mediaNames(NULL), m_nameOffset(0), m_nameLength(0),
   m_littleEndian(isLittle), m_fileLittle(FALSE), m_chunkRef(NULL), m_chunkStart(NULL),
   m_chunkEnd(NULL), m_resolvedChunkSize(0), m_resolvedChunkBuf(NULL),
   m_resolvedChunkPtr(NULL), m_valid(FALSE), m_bad(FALSE)
{
	// The offset and the names come from the file, so they are checked. The
	// file starts with a marker that gives its byte order (see agopcode.h),
	// then the offset of the names, in that byte order.
	m_fp = fopen(filename, "rb");
	if (m_fp == NULL)
		return;

	unsigned char header[MLE_DPP_CHUNKFILE_HEADER_SIZE];
	if (fread(header, sizeof(header), 1, m_fp) != 1)
		return;
	int order = mleDppChunkFileOrder(header);
	if (order == 0)
		return;
	m_fileLittle = (order == 2);
	m_nameOffset = mleDppChunkFileInt(header + MLE_DPP_CHUNKFILE_MARKER_SIZE, m_fileLittle);

	if (fseek(m_fp, 0, SEEK_END) != 0)
		return;

	long fileEnd = ftell(m_fp);
	if (fileEnd < MLE_DPP_CHUNKFILE_HEADER_SIZE ||
	    m_nameOffset < MLE_DPP_CHUNKFILE_HEADER_SIZE || m_nameOffset > fileEnd)
		return;

	if (fseek(m_fp, m_nameOffset, SEEK_SET) != 0)
		return;
	if ((m_nameLength = fileEnd - m_nameOffset) > 0)
	{
		m_mediaNames = new char[m_nameLength];
		// The names are NUL terminated strings; the last one has to be too.
		if (fread(m_mediaNames, m_nameLength, 1, m_fp) != 1 ||
		    m_mediaNames[m_nameLength - 1] != '\0')
		{
			delete [] m_mediaNames;
			m_mediaNames = NULL;
			m_nameLength = 0;
			return;
		}
	}

	m_valid = TRUE;
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
	if (m_mediaNames == NULL)
		return NULL;

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

	if (index < 0)
	{
		m_bad = TRUE;
		return 0;
	}
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
	// Reading past the end of the stream damages it. The byte that is returned
	// has the high bit, so that a loop that reads an index ends.
	if (!(m_chunkRef >= m_chunkStart && m_chunkRef < m_chunkEnd))
	{
		m_bad = TRUE;
		return 0x80;
	}

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
		if ((value & (MlInt) 0x80000000 >> (valueBits - 1)) != 0)
		{
			// The index does not fit.
			m_bad = TRUE;
			return 0;
		}
		value = (value << valueBits) | byte;
	}
	if (m_bad)
		return 0;

	m_chunkRef--; /* Back up one, since while loop always over-advances by one. */

	return value;
}

int MleDppActorGroupChunk::readInt(void)
{
    int value = nextChunkByte();

    if (m_fileLittle)
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
    *length = 0;
    if (! m_valid)
        return NULL;

    // The bytes of a property cannot be swapped, as their types are not
    // known, so the chunk file has to be in the byte order of the result.
    if (m_fileLittle != m_littleEndian)
        return NULL;

    int nameCount = countNames();

    // The stream is from the file, so it is read with checks: a read that
    // is past its end, an unknown opcode or a bad index sets m_bad, and
    // there is no result.
    m_bad = FALSE;
    m_resolvedChunkSize = sizeof(MlInt);
    m_resolvedChunkBuf = (MlByte *)mlMalloc(sizeof(MlInt));
    m_resolvedChunkPtr = m_resolvedChunkBuf;

    int propertyLength = 0;
    int chunkLength = m_nameOffset - MLE_DPP_CHUNKFILE_HEADER_SIZE;
    if (chunkLength <= 0 || fseek(m_fp, MLE_DPP_CHUNKFILE_HEADER_SIZE, SEEK_SET) != 0)
    {
        mlFree(m_resolvedChunkBuf);
        m_resolvedChunkBuf = NULL;
        return NULL;
    }
	m_chunkStart = (MlByte *)mlMalloc(chunkLength);
    m_chunkRef = m_chunkStart;
    m_chunkEnd = &m_chunkStart[chunkLength];

    if (fread((char *) m_chunkStart, chunkLength, 1, m_fp) != 1)
        m_bad = TRUE;

    // The first int is the index of the group in the table of group
    // classes, so we can invoke the right constructor.    
    if (! m_bad)
        copyInt();

    // The index is the count of actors in this group.  It can be an index
    // because we know an opcode follows it.
    if (! m_bad)
        copyIndex();

	int bytesToCopy, numElements, mediaIndex;
	MlBoolean ended = FALSE;
    while (! m_bad && m_chunkRef < m_chunkEnd)
	{
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
			bytesToCopy = propertyLength;
			while (--bytesToCopy >= 0 && ! m_bad)
			  copyStreamByte();
			break;

		  case endActorGroupOpcode:
			ended = TRUE;
			m_chunkRef = m_chunkEnd;
			break;

		  case copyMediaRefOpcode:
			mediaIndex = readIndex();
			if (m_bad)
			  break;
			if (mediaIndex < 0 || mediaIndex >= nameCount || indices == NULL)
			  m_bad = TRUE;
			else
			  writeIndex(indices[mediaIndex]);
			break;

		  case copyDynamicPropertyOpcode:
			bytesToCopy = copyInt();
			if (bytesToCopy < 0)
			  m_bad = TRUE;
			while (--bytesToCopy >= 0 && ! m_bad)
			  copyStreamByte();
			break;

		  case copyArrayPropertyOpcode:
			numElements = copyInt();
			// The count is from the file: it has to fit in what is left.
			if (numElements < 0 ||
			    (long long) propertyLength * numElements > (long long) (m_chunkEnd - m_chunkRef))
			  m_bad = TRUE;
			else
			  bytesToCopy = propertyLength * numElements;
			while (! m_bad && --bytesToCopy >= 0)
			  copyStreamByte();
			break;

		  default:
			m_bad = TRUE;
		}
    }

	// Clean up.
	mlFree((void *)m_chunkStart);
	m_chunkStart = NULL;

	// The stream of a group ends with its end opcode.
	if (! ended)
		m_bad = TRUE;

	if (m_bad)
	{
		mlFree(m_resolvedChunkBuf);
		m_resolvedChunkBuf = NULL;
		m_resolvedChunkPtr = NULL;
		return NULL;
	}

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

void*
MleDppActorGroupChunk::operator new[](size_t tSize)
{
	void* p = mlMalloc(tSize);
	return p;
}

void
MleDppActorGroupChunk::operator delete[](void* p)
{
	mlFree(p);
}

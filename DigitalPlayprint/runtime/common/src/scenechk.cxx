/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file scenechk.cxx
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 */

// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2024 Wizzer Works
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
#include "mle/scenechk.h"


// Code to resolve group references in an actor group chunk.

MleDppSceneChunk::MleDppSceneChunk(const char *filename, MlBoolean isLittle)
 : m_fp(NULL), m_groupNames(NULL), m_nameOffset(0), m_nameLength(0),
   m_littleEndian(isLittle), m_fileLittle(FALSE), m_chunkRef(NULL), m_chunkStart(NULL),
   m_chunkEnd(NULL), m_resolvedChunkPtr(NULL), m_valid(FALSE), m_bad(FALSE)
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
		m_groupNames = new char[m_nameLength];
		// The names are NUL terminated strings; the last one has to be too.
		if (fread(m_groupNames, m_nameLength, 1, m_fp) != 1 ||
		    m_groupNames[m_nameLength - 1] != '\0')
		{
			delete [] m_groupNames;
			m_groupNames = NULL;
			m_nameLength = 0;
			return;
		}
	}

	m_valid = TRUE;
}


int MleDppSceneChunk::countNames(void)
{
	if (m_groupNames == NULL)
		return 0;

	char* groupEnd = &m_groupNames[m_nameLength];
	char* namePtr = m_groupNames;
	int counter = 0;
	while (namePtr < groupEnd)
	{
		counter += 1;
		while (*namePtr++ && namePtr < groupEnd)
			/* Do nothing. */;
	}

	return counter;
}


const char * MleDppSceneChunk::nextName(const char* name)
{
	if (m_groupNames == NULL)
		return NULL;

	char* groupEnd = &m_groupNames[m_nameLength];
	if (name == NULL || name >= groupEnd)
		return NULL;

	const char* namePtr = name;

	while (*namePtr++ && namePtr < groupEnd)
		/* Do noting. */;

	return namePtr < groupEnd ? namePtr : NULL;	
}


int MleDppSceneChunk::writeIndex(MlInt index)
{
	int counter = (32 + valueBits - 1) / valueBits;
	char writeOut = 0;
	
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

MlInt MleDppSceneChunk::writeInt(MlInt index)
{
    if (m_littleEndian)
	{
		*m_resolvedChunkPtr++ = (index) & 0xff;
		*m_resolvedChunkPtr++ = (index>>8) & 0xff;
		*m_resolvedChunkPtr++ = (index>>16) & 0xff;
		*m_resolvedChunkPtr++ = (index>>24) & 0xff;
    } else {
		*m_resolvedChunkPtr++ = (index>>24) & 0xff;
		*m_resolvedChunkPtr++ = (index>>16) & 0xff;
		*m_resolvedChunkPtr++ = (index>>8) & 0xff;
		*m_resolvedChunkPtr++ = (index) & 0xff;
    }
    return index;
}


MlByte MleDppSceneChunk::nextChunkByte()
{
	// Reading past the end damages the chunk. The byte that is returned has
	// the high bit, so that a loop that reads an index ends.
	if (!(m_chunkRef >= m_chunkStart && m_chunkRef < m_chunkEnd))
	{
		m_bad = TRUE;
		return 0x80;
	}

	return *m_chunkRef++;
}


MlInt MleDppSceneChunk::readIndex(void)
{
	long value = 0;
	MlByte byte;
	
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

MlInt MleDppSceneChunk::readInt(void)
{
    MlInt value = nextChunkByte();
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
MleDppSceneChunk::loadChunk(MlInt *length)
{
	*length = 0;
	if (! m_valid)
		return NULL;

	// The chunk is a number of ints, from the file: it has to have at least
	// one, and no part of an int.
	MlInt chunkLength = m_nameOffset - MLE_DPP_CHUNKFILE_HEADER_SIZE;
	if (chunkLength <= 0 || chunkLength % sizeof(MlInt) != 0 ||
	    fseek(m_fp, MLE_DPP_CHUNKFILE_HEADER_SIZE, SEEK_SET) != 0)
		return NULL;

	m_bad = FALSE;
    MlByte *resolvedByteStream = new MlByte[m_nameOffset];
    m_resolvedChunkPtr = resolvedByteStream;

    m_chunkStart = new MlByte[chunkLength];
    m_chunkRef = m_chunkStart;
    m_chunkEnd = &m_chunkStart[chunkLength];

    if (fread((char *) m_chunkStart, chunkLength, 1, m_fp) != 1)
		m_bad = TRUE;

    while (! m_bad && m_chunkRef < m_chunkEnd)
	    writeInt(readInt());

	// Clean up.
    delete [] (MlByte *) m_chunkStart;
	m_chunkStart = NULL;

	if (m_bad)
	{
		delete [] resolvedByteStream;
		return NULL;
	}

	// Calculate the size of the resolved table.
    *length = m_resolvedChunkPtr - resolvedByteStream;

	// Return a pointer to the resolved table.
    return resolvedByteStream;
}

void *
MleDppSceneChunk::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppSceneChunk::operator delete(void *p)
{
	mlFree(p);
}

void*
MleDppSceneChunk::operator new[](size_t tSize)
{
	void* p = mlMalloc(tSize);
	return p;
}

void
MleDppSceneChunk::operator delete[](void* p)
{
	mlFree(p);
}

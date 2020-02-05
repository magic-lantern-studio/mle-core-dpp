/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file scenechk.cxx
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @created September 15, 2004
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
#include "mle/scenechk.h"


// Code to resolve group references in an actor group chunk.


MleDppSceneChunk::MleDppSceneChunk(const char *filename, MlBoolean isLittle)
 : m_groupNames(NULL), m_littleEndian(isLittle)
{
	m_fp = fopen(filename, "rb");
	MLE_ASSERT(m_fp != NULL);

	int err = fread(&m_nameOffset, sizeof(MlInt), 1, m_fp);
	MLE_ASSERT(err == 1);

	err = fseek(m_fp, 0, SEEK_END);
	MLE_ASSERT(err == 0);

	int fileEnd = ftell(m_fp);
	MLE_ASSERT(fileEnd > 0);

	err = fseek(m_fp, m_nameOffset, SEEK_SET);
	MLE_ASSERT(err == 0);

	if ((m_nameLength = fileEnd - m_nameOffset) > 0)
	{
		m_groupNames = new char[m_nameLength];
		err = fread(m_groupNames, m_nameLength, 1, m_fp);
		MLE_ASSERT(err == 1);
	}
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
	MLE_ASSERT(m_groupNames != NULL);

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
	MLE_ASSERT(m_chunkRef >= m_chunkStart && m_chunkRef < m_chunkEnd);

	return *m_chunkRef++;
}


MlInt MleDppSceneChunk::readIndex(void)
{
	long value = 0;
	MlByte byte;
	
	while ((byte = nextChunkByte()) >= 0)
	{
		MLE_ASSERT((value & (MlInt) 0x80000000 >> valueBits - 1) == 0);
		value = (value << valueBits) | byte;
	}
	MLE_ASSERT(value >= 0);

	m_chunkRef--; /* Back up one, since while loop always over-advances by one. */

	return value;
}

MlInt MleDppSceneChunk::readInt(void)
{
    MlInt value = nextChunkByte();
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
MleDppSceneChunk::loadChunk(MlInt *length)
{
	
    MlByte *resolvedByteStream = new MlByte[m_nameOffset];
    m_resolvedChunkPtr = resolvedByteStream;

    MlInt chunkLength = m_nameOffset - sizeof(MlInt);
    m_chunkStart = new MlByte[chunkLength];
    m_chunkRef = m_chunkStart;
    m_chunkEnd = &m_chunkStart[chunkLength];

    MlInt err = fseek(m_fp, sizeof(MlInt), SEEK_SET);
    MLE_ASSERT(err == 0);

    err = fread((char *) m_chunkStart, chunkLength, 1, m_fp);
    MLE_ASSERT(err == 1);

    do {
	    writeInt(readInt());
    } while (m_chunkRef < m_chunkEnd);

	// Clean up.
    delete (MlByte *) m_chunkStart;

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

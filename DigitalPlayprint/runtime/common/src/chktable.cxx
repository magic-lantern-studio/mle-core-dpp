/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file DppChunkTable.cpp
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

/*
 * ChunkTable manager
 *
 * This object maintains an array of mappings that are used in a transitory
 * way in the mastering process.
 */

// Include system header files.
#include <stdio.h>
#if defined(_WINDOWS) || defined(__linux__)
#include <string.h>
#else
#include <unistd.h>
#include <strings.h>
#endif

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/chktable.h"


MleDppChunkTable::MleDppChunkTable()
{
    m_used = m_allocated = 0;
    m_tbl = NULL;
}

MlUInt MleDppChunkTable::getOffset(MlInt index)
{
    return index >= m_used? 0 : m_tbl[index].m_offset;
}

char *MleDppChunkTable::getName(MlInt index)
{
    return index >= m_used? NULL : m_tbl[index].m_name;
}

MlChunkEntryType MleDppChunkTable::getType(MlInt index)
{
    return m_tbl[index].m_type;
}

char *MleDppChunkTable::getFilename(MlInt index)
{
    return index >= m_used? NULL : m_tbl[index].m_filename;
}

void MleDppChunkTable::addEntry(
	MlChunkEntryType type,
	char *filename,
	char *name,
	MlUInt offset)
{
    extendChunkTable();

    m_tbl[m_used].m_type = type;
#if defined(WIN32)
	m_tbl[m_used].m_filename = _strdup(filename);
	m_tbl[m_used].m_name = _strdup(name);
#else
    m_tbl[m_used].m_filename = strdup(filename);
	m_tbl[m_used].m_name = strdup(name);
#endif
    m_tbl[m_used].m_offset = offset;

    m_used++;
}

void MleDppChunkTable::setOffset(int index, MlUInt offset)
{
    if ( index < m_used )
	{
		m_tbl[index].m_offset = offset;
    }
}

  
/*
 * Extend the dynamic array of chunks.
 */

#define ALLOC_INC 128

void MleDppChunkTable::extendChunkTable()
{
    if ( m_used >= m_allocated )
	{
		if ( m_allocated == 0 )
		{
			 m_tbl = (MleDppChunkEntry *)mlMalloc(sizeof(MleDppChunkEntry) * ALLOC_INC);
		} else { 
			 m_tbl = (MleDppChunkEntry *)mlRealloc(m_tbl,
					sizeof(MleDppChunkEntry) * (ALLOC_INC + m_allocated));
		}
		m_allocated += ALLOC_INC;
    }
}

void *
MleDppChunkTable::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppChunkTable::operator delete(void *p)
{
	mlFree(p);
}

void*
MleDppChunkTable::operator new[](size_t tSize)
{
	void* p = mlMalloc(tSize);
	return p;
}

void
MleDppChunkTable::operator delete[](void* p)
{
	mlFree(p);
}

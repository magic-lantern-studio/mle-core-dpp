/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file chktable.h
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_DPP_CHUNKTABLE_H_
#define __MLE_DPP_CHUNKTABLE_H_

// Include Digital Playprint header files.
#include "mle/mlTypes.h"
#include "mle/Dpp.h"

class MleDppChunkTable;

typedef enum 
{
	CHUNK_MEDIA,
	CHUNK_GROUP,
	CHUNK_SET,
	CHUNK_SCENE
} MlChunkEntryType;

/**
 * This class is used to facilitate entries in the chunk table manager.
 */
class MleDppChunkEntry
{
    friend class MleDppChunkTable;

	/** The type of chunk for this entry. */
    MlChunkEntryType m_type;

	/** The name of the chunk file. */
    char *m_filename;

	/** The name of the chunk table. */
    char *m_name;

	/** The offset into the chunk table for this entry. */
    MlUInt m_offset;
};

/**
 */
class MleDppChunkTable
{
  public:

    MleDppChunkTable();

    int getUsed()
	{ return m_used; };

    MlChunkEntryType getType(int);

    char *getFilename(int index);

    char *getName(int index);

    MlUInt getOffset(MlInt index);

    void addEntry(MlChunkEntryType type,
		  char *filename,
		  char *name,
		  MlUInt offset = 0);

    void setOffset(int index, MlUInt offset);

	/**
	 * Override operator new.
	 *
	 * @param tSize The size, in bytes, to allocate.
	 */
	void* operator new(size_t tSize);

	/**
	 * Override operator delete.
	 *
	 * @param p A pointer to the memory to delete.
	 */
    void  operator delete(void *p);

  private:

	// Extend the chunk table.
    void extendChunkTable();

	// The chunk table is a collection of entries.
    MleDppChunkEntry *m_tbl;
	// The number of allocated entries in the table.
    int m_allocated;
	// The number of used entries in the table.
    int m_used;
};

#endif /* __MLE_DPP_CHUNKTABLE_H_ */

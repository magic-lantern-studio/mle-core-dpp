/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Runtime Library API - Master */

/**
 *  @file mrefchk.h
 *  @ingroup MleDPPMaster
 *
 *  This file defines a utility class for reading and writing Digital
 *  Playprint MediaRef chunk files.
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

#ifndef __MLE_MREFCHUNK_H_
#define __MLE_MREFCHUNK_H_

// Include Magic Lantern header files.
#include "mle/mlTypes.h"
#include "mle/chunk.h"
#include "mle/chkfile.h"


/**
 * @brief MrefChunkFile
 *
 * This class is used to read and write MediaRef chunk files.
 * The MediaRef chunk file uses the Playprint IFF format to
 * organize the data contained within.
 *
 * @see MleChunkFile
 */
class MleMrefChunkFile : public MleChunkFile
{
    // Declare member variables.

  public:

    // Define the Playprint version for this reader/writer.
    enum {
        PLAYPRINT_VERSION = 1
    };

    // Define the valid types of references.
    enum {
        EXTERNAL = 0,
        INTERNAL = 1
    } Type;

  private:

    MleDppChunkInfo m_mrefListInfo;

    // declare member functions.

  public:

    // constructor.
    MleMrefChunkFile(const char *filename,Mode flags);

    // destructor.
    virtual ~MleMrefChunkFile(void);

    // begin IFF 'MREF' LIST chunk.
    MlBoolean beginList(void);

    // end IFF 'MREF' LIST chunk
    MlBoolean endList(void);

    // read IFF 'info' chunk
    MlBoolean readInfo(MleMediaRefInfoChunk *chunkData);

    // write IFF 'info' chunk
    MlBoolean writeInfo(MleMediaRefInfoChunk *chunkData);

    // read IFF 'mref' chunk
    MlBoolean readData(MleMediaRefChunk *chunkData);

    // write IFF 'mref' chunk
    MlBoolean writeData(MleMediaRefChunk *chunkData);

    /**
     * This routine is used to process the media into a format
     * which can be consumed as an internal or external reference.
     * An external reference means that the data in the chunk file
     * is the name of the file containing the media. An internal
     * reference means that the data in the chunk file is the
     * actual media.
	 */
    MleMediaRefChunk *processMediaRef(MleMediaRefChunk *chunkData);

	/**
	 * Override operator new.
	 *
	 * @param tSize The size, in bytes, to allocate.
	 */
	void* operator new(size_t tSize);

    /**
     * Override operator new array.
     *
     * @param tSize The size, in bytes, to allocate.
     */
    void* operator new[](size_t tSize);

	/**
	 * Override operator delete.
	 *
	 * @param p A pointer to the memory to delete.
	 */
    void  operator delete(void *p);

    /**
     * Override operator delete array.
     *
     * @param p A pointer to the memory to delete.
     */
    void  operator delete[](void* p);

  private:

	// Hide default constructor.
    MleMrefChunkFile(void) :MleChunkFile(NULL,OFF) {};
};


#endif /* __MLE_MREFCHUNK_H_ */

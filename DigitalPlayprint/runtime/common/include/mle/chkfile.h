/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Runtime Library API - Master */

/**
 *  @file chkfile.h
 *  @ingroup MleDPPMaster
 *
 *  This file defines a utility class for reading and writing Digital
 *  Playprint chunk files.
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

#ifndef __MLE_CHKFILE_H_
#define __MLE_CHKFILE_H_


// Include Magic Lantern header files
#include "mle/mlTypes.h"
#include "mle/chunk.h"
#include "mle/pp.h"


/**
 * @brief A Digital Playprint Chunk File.
 *
 * This class is used to read and write chunk files.
 * The chunk file uses the Playprint IFF format to
 * organize the data contained within.
 *
 * @see MleDpp, MleMrefChunkFile
 */
class MleChunkFile
{
    // Declare member variables.

  public:

    typedef enum
    {
        OFF = 0,
        READING = 1,
        WRITING = 2
    } Mode;

  protected:

    // mode: if READING, class is being used to read a chunk file.
    //       if WRITING, class is being used to write a chunk file.
    Mode m_mode;

    // fp: file handle
    FILE *m_fp;

  private:

    // formInfo: IFF bookkeeping structure
    MleDppChunkInfo m_formInfo;

    // swapWrite: if TRUE, will swap short/long words before writing.
    MlBoolean m_swapWrite;
    
    // swapRead: if TRUE, will swap short/long words after reading.
    MlBoolean m_swapRead;

    // Declare member functions.

  public:

    /**
	 * A constructor.
	 */
    MleChunkFile(const char *filename,Mode flags);

    /**
	 * The destructor.
	 */
    virtual ~MleChunkFile(void);

    /**
	 * Accessor to get chunk file handle.
	 */
    FILE *getFp(void) { return m_fp; };
 
    /**
	 * Begin IFF 'MLCF' form chunk.
	 */
    MlBoolean begin(MlUInt type);

    /**
	 * End IFF 'MLCF' form chunk.
	  */
    MlBoolean end(void);

    /**
	 * Read IFF 'hdr' chunk.
	  */
    MlBoolean readHeader(MleChunkFileHeader *chunkData);

    /**
	 * Write IFF 'hdr' chunk.
	 */
    MlBoolean writeHeader(MleChunkFileHeader *chunkData);

    void setSwap(MlBoolean swap);

    void setSwapWrite(MlBoolean swap)
    { m_swapWrite = swap; mlDppSetSwapWrite(swap); };

    MlBoolean getSwapWrite(void)
    { return m_swapWrite; };

    void setSwapRead(MlBoolean swap)
    { m_swapRead = swap; mlDppSetSwapRead(swap); };

    MlBoolean getSwapRead(void)
    { return m_swapRead; };

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

	  // Hide the default constructor.
      MleChunkFile(void) {};
};


#endif /* __MLE_CHKFILE_H_ */

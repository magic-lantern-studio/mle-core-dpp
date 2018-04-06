/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Runtime Library API - Master */

/**
 *  @file ppoutput.h
 *  @ingroup MleDPPMaster
 *
 *  This file defines a utility class for writing Digital
 *  Playprint chunk files.
 *
 *  @author Mark S. Millard
 *  @date July 10, 2003
 *  @version 0.1
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

#ifndef __MLE_PPOUTPUT_H_
#define __MLE_PPOUTPUT_H_

// Include system header files

// Include Magic Lantern header files
#include "mle/mlFileio.h"
#include "mle/playprnt.h"
#include "mle/pp.h"
#include "mle/chunk.h"
#include "mle/chktable.h"


/**
 * @brief This class is used to write Digital Playprint files.
 *
 * @see MleDpp,MleDppInput
 */
class MleDppOutput: public MleDpp
{
    // Declare member variables.

  private:

    // mrefListInfo: IFF book keeping structure.
    MleDppChunkInfo m_mrefListInfo;


    // Declare member functions.

  public:

    // constructor.
    MleDppOutput(const char *filename);

    // destructor.
    virtual ~MleDppOutput(void);
    
    // write IFF 'hdr' chunk.
    MlBoolean writeHeader(void);

    // write IFF 'grp' chunk.
    MlBoolean writeGroup(MleDppChunkTable *table, MlInt groupIndex);

    // write IFF 'MRFL' LIST chunk (includes 'mref' chunks).
    MlBoolean writeMedia(char *filename);

    // write IFF 'set' chunk.
    MlBoolean writeSet(char *filename);
    
    // write IFF 'scn' chunk.
    MlBoolean writeScene(MleDppChunkTable *table, MlInt sceneIndex);
    
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

    // Hide default constructor.
    MleDppOutput(void) {};

    // MediaRef utilities.
    MlBoolean beginMrefList(void);
    MlBoolean endMrefList(void);
    MlBoolean writeMrefInfo(MleMediaRefInfoChunk *chunkData);
    MlBoolean writeMref(MleMediaRefChunk *chunkData);
};


#endif /* __MLE_PPOUTPUT_H_ */

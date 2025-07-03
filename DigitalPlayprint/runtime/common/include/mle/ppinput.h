/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Runtime Library API - Master */

/**
 *  @file ppinput.h
 *  @ingroup MleDPPMaster
 *
 *  This file defines a utility class for reading Digital
 *  Playprint files.
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

#ifndef __MLE_PPINPUT_H_
#define __MLE_PPINPUT_H_

// Include system header files.

// Include Magic Lantern header files.
#include "mle/mlFileio.h"
#include "mle/playprnt.h"
#include "mle/pp.h"
#include "mle/chunk.h"


/**
 * This class is used to read Digital Playprint files.
 *
 * @see MleDpp,MleDppOutput
 */
class MleDppInput: public MleDpp
{
    // Declare member variables.

  private:

    // mrefListInfo: IFF book keeping structure.

    MleDppChunkInfo m_mrefListInfo;
    
    // declare member functions.

  public:

    // constructor.
    MleDppInput(const char *filename);

    // destructor.
    virtual ~MleDppInput(void);
    
    // read IFF 'hdr' chunk.
    int readHeader(MleDppCallback func,void *clientData);
    
    // read IFF 'grp' chunk.
    int readGroup(MleDppCallback func,void *clientData);

    // begin IFF 'MRFL' LIST chunk.
    int beginMref(MleDppCallback func,void *clientData);

    // end IFF 'MRFL' LIST chunk.
    int endMref(MleDppCallback func,void *clientData);

    // read IFF 'mref' chunk.
    int readMedia(MleDppCallback func,void *clientData);

    // read IFF 'set' chunk.
    int readSet(MleDppCallback func,void *clientData);

    // read IFF 'scn' chunk.
    int readScene(MleDppCallback func,void *clientData);
    
    // read IFF 'toc' chunk.
    int readTOC(MleDppCallback func,void *clientData);

    // get chunk offset from table-of-content.
    MlInt getTOCOffset(MlUInt tocIndex);

    // go to location within Playprint.
    void goTo(MlInt tocOffset);

#if 0
    MlDppQuark getTOCLabel(MlUInt tocIndex);
#endif /* 0 */
    
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
    MleDppInput(void) {};

    // MediaRef utilities
    MlBoolean beginMrefList(void);
    MlBoolean endMrefList(void);
    MlBoolean readMrefInfo(MleMediaRefInfoChunk *chunkData);
    MlBoolean readMref(MleMediaRefChunk *chunkData);
};


#endif /* __MLE_PPINPUT_H_ */

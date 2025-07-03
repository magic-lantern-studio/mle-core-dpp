/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Runtime Library API - Master */

/**
 *  @file pp.h
 *  @ingroup MleDPPMaster
 *
 *  This file defines a utility class for reading and writing Digital
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

#ifndef __MLE_PP_H_
#define __MLE_PP_H_

// Include system header files.
#include <stdio.h>

// Include Magic Lantern header files.
#include "mle/playprnt.h"


// Declare supporting structures for Table-Of-Contents.
typedef MlInt MlDppQuark;

typedef struct
{
    MlDppQuark m_label;       /**< Label for table-of-contents entry. */
    MlUInt     m_offset;      /**< Offset to entry from beginning of file. */
} MleDppTocEntry;

typedef struct _MlDppTOC
{
    MleDppTocEntry m_entry;
    struct _MlDppTOC *m_next;
} MleDppTOC;


/**
 * @brief This class is used to read and write Digital Playprint files.
 *
 * @see MleDppOutput,MleDppInput
 */
class MleDpp
{

  public:

    // Define the Playprint version for this reader/writer.
    enum {
	    PLAYPRINT_VERSION = 1
    };
    
    // Define the Playprint access modes.
    typedef enum
    {
        OFF = 0,
        READING = 1,
        WRITING = 2
    } Mode;

    // Declare member variables.

  protected:

    // mode: if READING, class is being used to read a Playprint file
    //       if WRITING, class is being used to write a Playprint file
    Mode m_mode;

    // file handle.
    FILE *m_fp;
    
    // useTOC: if TRUE, will generate a table-of-contents
    MlBoolean m_useTOC;
    
    // list of entries in table-of-contents.
    MlUInt     m_numTOCReserved;
    MlUInt     m_numTOCUsed;
    MleDppTOC *m_TOC;

  private:

    // formInfo: IFF bookkeeping structure.
    MleDppChunkInfo m_formInfo;

    // swapWrite: if TRUE, will swap short/long words before writing.
    MlBoolean m_swapWrite;
    
    // swapRead: if TRUE, will swap short/long words after reading.
    MlBoolean m_swapRead;


    // Declare member functions.

  public:

    // constructor
    MleDpp(const char *filename, Mode flags);

    // destructor
    virtual ~MleDpp(void);

    // accessor to get Playprint file handle    
    FILE *getFp(void)
	{ return m_fp; };
    
    // Begin IFF 'MLRT' form chunk.
    MlBoolean begin(MlUInt type);

    // end IFF 'FWRT' form chunk.
    MlBoolean end(void);

    // read IFF 'hdr' chunk.
    MlBoolean readHeader(MleDppHeader *chunkData);

    // write IFF 'hdr' chunk
    MlBoolean writeHeader(MleDppHeader *chunkData);

    // Accessor utiliites.

    void setSwap(MlBoolean swap);

    void setSwapWrite(MlBoolean swap)
    { m_swapWrite = swap; mlDppSetSwapWrite(swap); };

    MlBoolean getSwapWrite(void)
    { return m_swapWrite; };

    void setSwapRead(MlBoolean swap)
    { m_swapRead = swap; mlDppSetSwapRead(swap); };

    MlBoolean getSwapRead(void)
    { return m_swapRead; };

    // Support for table-of-contents

    void setTOC(MlBoolean use)
	{ m_useTOC = use; };

    MlBoolean beginTOC(int num);

    MlBoolean addTOCEntry(MlDppQuark, MlUInt);

    MlBoolean endTOC(void);

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

  protected:

	// Protect the default constructor.
    MleDpp(void) {};
};


/**
 * @brief Callback defintion for processing Digital Playprint.
 *
 * @param clientData A pointer to user-supplied data.
 * @param callData A pointer to DPP processing data.
 */
typedef int (*MleDppCallback)(void *clientData,void *callData);


#endif /* __MLE_PP_H_ */

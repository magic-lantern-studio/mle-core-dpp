/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file pp.cxx
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
#include <string.h>

// Include Magic Lantern header files.
#include "mle/pp.h"
#include "mle/mlMalloc.h"
#include "mle/mlFileio.h"
#include "mle/mlErrno.h"
#include "mle/mlAssert.h"


MleDpp::MleDpp(const char *filename,Mode flags)
    :m_mode(flags),
     m_swapRead(FALSE),
     m_swapWrite(FALSE),
     m_useTOC(FALSE),
     m_TOC(NULL),
     m_numTOCReserved(0),
     m_numTOCUsed(0)
{
    // Initialize Playprint book keeping structures.
    memset(&m_formInfo,0,sizeof(MleDppChunkInfo));

    // Open file handle.
    if (flags == READING)
        m_fp = mlFOpen(filename,"rb");
    else if (flags == WRITING)
        m_fp = mlFOpen(filename,"wb+");
    MLE_VALIDATE_PTR(m_fp);
}


MleDpp::~MleDpp(void)
{
    // Reset low-level Playprint swap flags.
    //setSwap(FALSE);
	if (getSwapRead()) setSwapRead(FALSE);
	if (getSwapWrite()) setSwapWrite(FALSE);

    // Close file handle.
    mlFClose(m_fp);
}


void MleDpp::setSwap(MlBoolean swap)
{
    // Set byte ordering flag.
    setSwapWrite(swap);
    setSwapRead(swap);
}


MlBoolean MleDpp::begin(MlUInt type)
{
    if (m_mode == READING)
	{
        // Initialize the Playprint file.
        m_formInfo.m_form = mlDppMakeTag('M','L','R','T');
        MLE_ERROR(mlDppDescend(getFp(),&m_formInfo,NULL,type));
    } else if (m_mode == WRITING)
	{
        // Initialize the Playprint file.
        m_formInfo.m_form = mlDppMakeTag('M','L','R','T');
        MLE_ERROR(mlDppCreateChunk(getFp(),&m_formInfo,type));
    }

    return(TRUE);
}


MlBoolean MleDpp::end(void)
{
    if (m_mode == READING)
	{
        // Wrap-up Playprint book keeping.
        MLE_ERROR(mlDppAscend(getFp(),&m_formInfo,0));
    } else if (m_mode == WRITING)
	{
        if (! m_useTOC)
            // Wrap-up Playprint book keeping.
            mlDppAscend(m_fp,&m_formInfo,0);
    }

    return(TRUE);
}


MlBoolean MleDpp::readHeader(MleDppHeader *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo hdrInfo;
    int status = TRUE;

    if (m_mode == READING)
	{
        memset(&hdrInfo,0,sizeof(MleDppChunkInfo));

        MLE_ERROR(mlDppDescend(getFp(),&hdrInfo,NULL,0));
        if (hdrInfo.m_chunk.m_tag == mlDppMakeTag('h','d','r',' '))
		{
            // Read the chunk file header.
        	size_t nBytes;
            nBytes = mlFRead(chunkData,hdrInfo.m_chunk.m_size,1,getFp());

            if (m_swapRead)
			{
                chunkData->m_version = mlDppSwapInt(chunkData->m_version);
                chunkData->m_date = mlDppSwapInt(chunkData->m_date);
            }
        }

        MLE_ERROR(mlDppAscend(getFp(),&hdrInfo,0));
    }

    return(status);
}


MlBoolean MleDpp::writeHeader(MleDppHeader *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo hdrInfo;
    MleDppHeader hdrData;

    if (m_mode == WRITING)
	{
        memset(&hdrInfo,0,sizeof(MleDppChunkInfo));

        // Write header chunk structure.
        hdrInfo.m_chunk.m_tag = mlDppMakeTag('h','d','r',' ');
        hdrInfo.m_chunk.m_size = sizeof(MleDppHeader);
        MLE_WARN(mlDppCreateChunk(getFp(),&hdrInfo,0));

        // Process header data.
        hdrData.m_version = chunkData->m_version;
        hdrData.m_date = chunkData->m_date;

        if (m_swapWrite)
		{
            hdrData.m_version = mlDppSwapInt(hdrData.m_version);
            hdrData.m_date = mlDppSwapInt(hdrData.m_date);
        }

        if (mlFWrite(&hdrData,sizeof(hdrData),1,getFp()) != 1)
		{
            mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
            return(FALSE);
        }

        return(mlDppAscend(getFp(),&hdrInfo,0));
    } else
        return(TRUE);
}


MlBoolean MleDpp::beginTOC(int num)
{
    // Declare local variables.
    MleDppChunkInfo tocInfo;
    int chunkSize;
    MlByte byte = 0;

    MLE_ASSERT(num >= 0);

    if (m_mode == READING)
	{

        m_numTOCReserved = num;
        m_useTOC = TRUE;
        return(TRUE);
    } else if (m_mode == WRITING)
	{

        memset(&tocInfo,0,sizeof(MleDppChunkInfo));
        chunkSize = sizeof(MlInt) + (sizeof(MleDppTocEntry) * num);

        // Write TOC chunk structure.
        tocInfo.m_chunk.m_tag = mlDppMakeTag('t','o','c',' ');
        tocInfo.m_chunk.m_size = chunkSize;
        MLE_WARN(mlDppCreateChunk(m_fp,&tocInfo,0));

        // Write dummy TOC chunk data.
        for (int i = 0; i < chunkSize; i++)
		{
            if (mlFWrite(&byte,sizeof(MlByte),1,m_fp) != 1)
			{
                mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
                return(FALSE);
            }
        }

        m_numTOCReserved = num;
        m_useTOC = TRUE;

        return(mlDppAscend(m_fp,&tocInfo,0));
    } else
        return(TRUE);
}


MlBoolean MleDpp::addTOCEntry(MlDppQuark label,MlUInt offset)
{
    /* Declare local variables */
    MleDppTOC *newEntry,*nextEntry;
    MlBoolean retValue = TRUE;

    if ((m_mode == READING) || (m_mode == WRITING))
	{

        if (! m_useTOC)
		{
            // XXX -- should set mlErrno here.
            return(FALSE);
        }

        if (m_numTOCUsed == m_numTOCReserved)
		{
            // XXX -- should set mlErrno here.
            return(FALSE);
        }

        newEntry = (MleDppTOC *)mlMalloc(sizeof(MleDppTOC));
        if (newEntry != NULL)
		{
            // Add new entry to table of contents.
            newEntry->m_entry.m_label = label;
            newEntry->m_entry.m_offset = offset;
            newEntry->m_next = NULL;

            nextEntry = m_TOC;
            if (! nextEntry)
			{
                // First entry on the list.
                m_TOC = newEntry;
            } else {
                // Add entry to end of list.
                while (nextEntry->m_next != NULL)
                    nextEntry = nextEntry->m_next;
                nextEntry->m_next = newEntry;
            }

            m_numTOCUsed++;
        } else
            // XXX -- should set mlErrno here.
            retValue = FALSE;
    } else
        retValue = TRUE;

    return(retValue);
}


MlBoolean MleDpp::endTOC()
{
    /* Declare local variables. */
    MleDppChunkInfo tocInfo;
    MlInt longWrite;
    MleDppTOC *nextEntry;
    MlBoolean retValue = TRUE;
    int num;

    if (m_mode == WRITING)
	{

        if (! m_useTOC)
		{
            // XXX -- should set mlErrno here.
            return(FALSE);
        } else
		{
            // Wrap-up Playprint book keeping.
            mlDppAscend(m_fp,&m_formInfo,0);
        }

        // Initialize table-of-contents chunk info.
        memset(&tocInfo,0,sizeof(MleDppChunkInfo));

        tocInfo.m_chunk.m_tag = mlDppMakeTag('t','o','c',' ');
        MLE_WARN(mlDppDescend(m_fp,&tocInfo,&m_formInfo,MLE_DPP_FINDCHUNK));

        // Write TOC data.
        longWrite = m_numTOCUsed;
        if (m_swapWrite)
            longWrite = mlDppSwapInt(longWrite);
        num = mlFWrite(&longWrite,sizeof(MlInt),1,m_fp);

        nextEntry = m_TOC;
        while (nextEntry)
		{
            // Write label for TOC entry.
            longWrite = nextEntry->m_entry.m_label;
            if (m_swapWrite)
                longWrite = mlDppSwapInt(longWrite);
            num = mlFWrite(&longWrite,sizeof(MlInt),1,m_fp);
            // Write offset for TOC entry.
            longWrite = nextEntry->m_entry.m_offset;
            if (m_swapWrite)
                longWrite = mlDppSwapInt(longWrite);
            num = mlFWrite(&longWrite,sizeof(MlInt),1,m_fp);
            nextEntry = nextEntry->m_next;
        }

        mlFFlush(m_fp);

        MLE_WARN(mlDppAscend(m_fp,&tocInfo,0));
    }

    return(retValue);
}

void *
MleDpp::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDpp::operator delete(void *p)
{
	mlFree(p);
}

#ifdef UNIT_TEST

// Include system header files.
#include <sys/types.h>
#include <time.h>


#define TESTBE_FILE "testbe.dpp"
#define TESTLE_FILE "testle.dpp"


void main(int argc,char *argv[])
{
    // Declare local variables.
    MleDpp *in,*out;
    MleDppHeader hdrData;
    MlUInt timeStamp;

    timeStamp = time(NULL);

    hdrData.m_version = MleDpp::VERSION;
    hdrData.m_date = timeStamp;

    // Create Big Endian Playprint.
    out = new MleDpp(TESTBE_FILE,MleDpp::WRITING);
#if BYTE_ORDER != BIG_ENDIAN
	out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPBE);
    out->writeHeader(&hdrData);
    out->end();
    delete out;

    // Create Little Endian Playprint.
    out = new MleDpp(TESTLE_FILE,MleDpp::WRITING);
#if BYTE_ORDER != LITTLE_ENDIAN
    out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPLE);
    out->writeHeader(&hdrData);
    out->end();
    delete out;

    memset(&hdrData,0,sizeof(MleDppHeader));

    // Read Big Endian Playprint.
    in = new MleDpp(TESTBE_FILE,MleDpp::READING);
#ifdef BYTE_ORDER != BIG_ENDIAN
	in->setSwap(TRUE);
#endif
    in->begin(MLE_DPP_FINDPPBE);
    in->readHeader(&hdrData);
    in->end();
    delete in;

    if ((hdrData.m_version != MleDpp::VERSION) ||
        (hdrData.m_date != timeStamp))
    {
        fprintf(stderr,"Big Endian MleDpp Test Failed!\n");
    } else
    {
        fprintf(stderr,"Big Endian MleDpp Test Succeeded.\n");
    }

    memset(&hdrData,0,sizeof(MleDppHeader));

    // Read Little Endian chunk file.
    in = new MleDpp(TESTLE_FILE,MleDpp::READING);
#if BYTE_ORDER != LITTLE_ENDIAN
    in->setSwap(TRUE);
#endif
    in->begin(MLE_DPP_FINDPPLE);
    in->readHeader(&hdrData);
    in->end();
    delete in;

    if ((hdrData.m_version != MleDpp::VERSION) ||
        (hdrData.m_date != timeStamp))
    {
        fprintf(stderr,"Little Endian MleDpp Test Failed!\n");
    } else
    {
        fprintf(stderr,"Little Endian MleDpp Test Succeeded.\n");
    }
}


#endif /* UNIT_TEST */

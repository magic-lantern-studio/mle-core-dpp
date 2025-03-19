/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file chkfile.cxx
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
#include "mle/chkfile.h"
#include "mle/mlFileio.h"
#include "mle/mlErrno.h"
#include "mle/mlMalloc.h"
#include "mle/mlAssert.h"


MleChunkFile::MleChunkFile(const char *filename,Mode flags)
    :m_mode(flags),
     m_swapRead(FALSE),
     m_swapWrite(FALSE)
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


MleChunkFile::~MleChunkFile(void)
{
    // Reset low-level playprint swap flags.
    //setSwap(FALSE);
	if (getSwapRead()) setSwapRead(FALSE);
	if (getSwapWrite()) setSwapWrite(FALSE);

    // close file handle.
    mlFClose(m_fp);
}


void MleChunkFile::setSwap(MlBoolean swap)
{
    // Set byte ordering flag.
    setSwapWrite(swap);
    setSwapRead(swap);
}


MlBoolean MleChunkFile::begin(MlUInt type)
{
    if (m_mode == READING)
	{
        // Initialize the Playprint file.
        m_formInfo.m_form = mlDppMakeTag('M','L','C','F');
        MLE_ERROR(mlDppDescend(getFp(),&m_formInfo,NULL,type));
    } else if (m_mode == WRITING)
	{
        // Initialize the Playprint file.
        m_formInfo.m_form = mlDppMakeTag('M','L','C','F');
        MLE_ERROR(mlDppCreateChunk(getFp(),&m_formInfo,type));
    }

    return(TRUE);
}


MlBoolean MleChunkFile::end(void)
{
    if ((m_mode == READING) || (m_mode == WRITING))
	{
        // Wrap-up Playprint book keeping.
       MLE_ERROR(mlDppAscend(getFp(),&m_formInfo,0));
    }

    return(TRUE);
}


MlBoolean MleChunkFile::readHeader(MleChunkFileHeader *chunkData)
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
                chunkData->m_id = mlDppSwapInt(chunkData->m_id);
                chunkData->m_version = mlDppSwapInt(chunkData->m_version);
                chunkData->m_time = mlDppSwapInt(chunkData->m_time);
                chunkData->m_crc = mlDppSwapInt(chunkData->m_crc);
            }
        }

        MLE_ERROR(mlDppAscend(getFp(),&hdrInfo,0));
    }

    return(status);
}


MlBoolean MleChunkFile::writeHeader(MleChunkFileHeader *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo hdrInfo;
    MleChunkFileHeader hdrData;

    if (m_mode == WRITING)
	{
        memset(&hdrInfo,0,sizeof(MleDppChunkInfo));

        // Write header chunk structure.
        hdrInfo.m_chunk.m_tag = mlDppMakeTag('h','d','r',' ');
        hdrInfo.m_chunk.m_size = sizeof(MleChunkFileHeader);
        MLE_WARN(mlDppCreateChunk(getFp(),&hdrInfo,0));

        // Process header data.
        hdrData.m_id = chunkData->m_id;
        hdrData.m_version = chunkData->m_version;
        hdrData.m_time = chunkData->m_time;
        hdrData.m_crc = chunkData->m_crc;

        if (m_swapWrite)
		{
            hdrData.m_id = mlDppSwapInt(hdrData.m_id);
            hdrData.m_version = mlDppSwapInt(hdrData.m_version);
            hdrData.m_time = mlDppSwapInt(hdrData.m_time);
            hdrData.m_crc = mlDppSwapInt(hdrData.m_crc);
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

void *
MleChunkFile::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleChunkFile::operator delete(void *p)
{
	mlFree(p);
}

#ifdef UNIT_TEST

// Include system header files.
#include <sys/types.h>
#include <time.h>


#define TESTBE_FILE "testbe.chk"
#define TESTLE_FILE "testle.chk"

#define TEST_HEADER_ID      0
#define TEST_HEADER_VERSION 1
#define TEST_HEADER_CRC     0

void main(int argc,char *argv[])
{
    // Declare local variables.
    MleChunkFile *in,*out;
    MleChunkFileHeader hdrData;
    MlUInt timeStamp;

    timeStamp = time(NULL);

    hdrData.m_id = TEST_HEADER_ID;
    hdrData.m_version = TEST_HEADER_VERSION;
    hdrData.m_time = timeStamp;
    hdrData.m_crc = TEST_HEADER_CRC;

    // Create Big Endian chunk file.
    out = new MleChunkFile(TESTBE_FILE,MleChunkFile::WRITING);
#if BYTE_ORDER != BIG_ENDIAN
	out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPBE);
    out->writeHeader(&hdrData);
    out->end();
    delete out;

    // Create Little Endian chunk file.
    out = new MleChunkFile(TESTLE_FILE,MleChunkFile::WRITING);
#if BYTE_ORDER != LITTLE_ENDIAN
    out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPLE);
    out->writeHeader(&hdrData);
    out->end();
    delete out;

    memset(&hdrData,0,sizeof(MleChunkFileHeader));

    // Read Big Endian chunk file.
    in = new MleChunkFile(TESTBE_FILE,MleChunkFile::READING);
#if BYTE_ORDER != BIG_ENDIAN
	in->setSwap(TRUE);
#endif
    in->begin(MLE_DPP_FINDPPBE);
    in->readHeader(&hdrData);
    in->end();
    delete in;

    if ((hdrData.m_id != TEST_HEADER_ID) ||
        (hdrData.m_version != TEST_HEADER_VERSION) ||
        (hdrData.m_time != timeStamp) ||
        (hdrData.m_crc != TEST_HEADER_CRC))
    {
        fprintf(stderr,"Big Endian ChunkFile Test Failed!\n");
    } else
    {
        fprintf(stderr,"Big Endian ChunkFile Test Succeeded.\n");
    }

    memset(&hdrData,0,sizeof(MleChunkFileHeader));

    // Read Little Endian chunk file
    in = new MleChunkFile(TESTLE_FILE,MleChunkFile::READING);
#if BYTE_ORDER != LITTLE_ENDIAN
    in->setSwap(TRUE);
#endif
    in->begin(MLE_DPP_FINDPPLE);
    in->readHeader(&hdrData);
    in->end();
    delete in;

    if ((hdrData.m_id != TEST_HEADER_ID) ||
        (hdrData.m_version != TEST_HEADER_VERSION) ||
        (hdrData.m_time != timeStamp) ||
        (hdrData.m_crc != TEST_HEADER_CRC))
    {
        fprintf(stderr,"Little Endian ChunkFile Test Failed!\n");
    } else
    {
        fprintf(stderr,"Little Endian ChunkFile Test Succeeded.\n");
    }
}


#endif /* UNIT_TEST */

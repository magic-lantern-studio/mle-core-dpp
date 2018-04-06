/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file mrefchk.cxx
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
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
#include <string.h>
#include <sys/stat.h>

// Include Magic Lantern header files.
#include "mle/mrefchk.h"
#include "mle/mlMalloc.h"
#include "mle/mlErrno.h"
#include "mle/mlAssert.h"


MleMrefChunkFile::MleMrefChunkFile(const char *filename,Mode flags)
    :MleChunkFile(filename,flags)
{
    // Initialize Playprint book keeping structures.
    memset(&m_mrefListInfo,0,sizeof(MleDppChunkInfo));
}


MleMrefChunkFile::~MleMrefChunkFile(void)
{
    // Do nothing.
}


MlBoolean
MleMrefChunkFile::beginList(void)
{
    // Declare local variables.
    MlBoolean retValue;

    if (m_mode == READING)
	{
        m_mrefListInfo.m_form = mlDppMakeTag('M','R','F','L');
        retValue = mlDppDescend(getFp(),&m_mrefListInfo,NULL,0);
    } else if (m_mode == WRITING)
	{
        m_mrefListInfo.m_form = mlDppMakeTag('M','R','F','L');
        retValue = mlDppCreateChunk(getFp(),&m_mrefListInfo,MLE_DPP_CREATELIST);
    } else
        retValue = TRUE;

    return(retValue);
}


MlBoolean
MleMrefChunkFile::endList(void)
{
    // Declare local variables.
    MlBoolean retValue;

    if ((m_mode == READING) || (m_mode == WRITING))
	{
        retValue = mlDppAscend(getFp(),&m_mrefListInfo,0);
    } else
        retValue = TRUE;

    return(retValue);
}


MlBoolean
MleMrefChunkFile::readInfo(MleMediaRefInfoChunk *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo mediaInfo;
    int status = TRUE;

    if (m_mode == READING)
	{
        memset(&mediaInfo,0,sizeof(MleDppChunkInfo));

        MLE_ERROR(mlDppDescend(getFp(),&mediaInfo,NULL,0));
        if (mediaInfo.m_chunk.m_tag == mlDppMakeTag('i','n','f','o'))
		{
            // Read the 'info' data.
            mlFRead(chunkData,mediaInfo.m_chunk.m_size,1,getFp());

            if (getSwapRead())
			{
                chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
                chunkData->m_type = mlDppSwapInt(chunkData->m_type);
                chunkData->m_numMrefs = mlDppSwapInt(chunkData->m_numMrefs);
            }
        }

        MLE_ERROR(mlDppAscend(getFp(),&mediaInfo,0));
    }

    return(status);
}


MlBoolean
MleMrefChunkFile::writeInfo(MleMediaRefInfoChunk *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo mediaInfo;

    if (m_mode == WRITING)
	{
        memset(&mediaInfo,0,sizeof(MleDppChunkInfo));

        // Create 'info' chunk structure.
        mediaInfo.m_chunk.m_tag = mlDppMakeTag('i','n','f','o');
        mediaInfo.m_chunk.m_size = sizeof(MleMediaRefInfoChunk);
        MLE_WARN(mlDppCreateChunk(getFp(),&mediaInfo,0));

        if (getSwapWrite())
		{
            chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
            chunkData->m_type = mlDppSwapInt(chunkData->m_type);
            chunkData->m_numMrefs = mlDppSwapInt(chunkData->m_numMrefs);
        }

        // Write out 'mref' data.
        if (mlFWrite(chunkData,sizeof(MleMediaRefInfoChunk),1,getFp()) != 1)
        {
            mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
            return(FALSE);
        }

        return(mlDppAscend(getFp(),&mediaInfo,0));
    } else
        return(TRUE);
}


#define MEDIAREF_CHUNK_SIZE (3 * sizeof(MlUInt))

MlBoolean
MleMrefChunkFile::readData(MleMediaRefChunk *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo mediaInfo;
    int status = TRUE;

    if (m_mode == READING)
	{
        memset(&mediaInfo,0,sizeof(MleDppChunkInfo));

        MLE_ERROR(mlDppDescend(getFp(),&mediaInfo,NULL,0));
        if (mediaInfo.m_chunk.m_tag == mlDppMakeTag('m','r','e','f'))
		{
            // Read the 'mref' data.
            mlFRead(chunkData,MEDIAREF_CHUNK_SIZE,1,getFp());

            if (getSwapRead())
			{
                chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
                chunkData->m_type = mlDppSwapInt(chunkData->m_type);
                chunkData->m_size = mlDppSwapInt(chunkData->m_size);
            }

			// The extra byte is for the string terminator. This will only
			// be required by an EXTERNAL media reference. XXX - fix this
			// for INTERNAL media references; may be a non-issue.
            chunkData->m_data = (char *)mlMalloc(chunkData->m_size + 1);
            mlFRead(chunkData->m_data,chunkData->m_size,1,getFp());
			chunkData->m_data[chunkData->m_size] = '\0';
        }

        MLE_ERROR(mlDppAscend(getFp(),&mediaInfo,0));
    }

    return(status);
}


MlBoolean
MleMrefChunkFile::writeData(MleMediaRefChunk *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo mediaInfo;
    int size;

    if (m_mode == WRITING)
	{
        memset(&mediaInfo,0,sizeof(MleDppChunkInfo));

        // Create 'mref' chunk structure.
        mediaInfo.m_chunk.m_tag = mlDppMakeTag('m','r','e','f');
        mediaInfo.m_chunk.m_size = chunkData->m_size + MEDIAREF_CHUNK_SIZE;
        MLE_WARN(mlDppCreateChunk(getFp(),&mediaInfo,0));

        //Rremember actual size to write.
        size = chunkData->m_size;

        if (getSwapWrite())
		{
            chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
            chunkData->m_type = mlDppSwapInt(chunkData->m_type);
            chunkData->m_size = mlDppSwapInt(chunkData->m_size);
        }

        // Write out 'mref' data
        if (mlFWrite(chunkData,MEDIAREF_CHUNK_SIZE,1,getFp()) != 1)
        {
            mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
            return(FALSE);
        }

        if (mlFWrite(chunkData->m_data,size,1,getFp()) != 1)
        {
            mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
            return(FALSE);
        }

        return(mlDppAscend(getFp(),&mediaInfo,0));
    } else
        return(TRUE);
}


MleMediaRefChunk *
MleMrefChunkFile::processMediaRef(MleMediaRefChunk *chunkData)
{
    // Declare local variables.
    char *filename;
    MleMediaRefChunk *newData;

    newData = new MleMediaRefChunk();
    if (! newData)
        return(NULL);

    // The "chunkData" argument should contain a filename and path
    // in the "data" field. The "size" field should contain the length
    // of this filename path.

    // determine the filename.
    filename = (char *)mlMalloc(chunkData->m_size + 1);
    strncpy(filename,chunkData->m_data,chunkData->m_size);
    filename[chunkData->m_size] = '\0';

	FILE *mediaFp;
    switch (chunkData->m_flags)
    {
        case 0:
#ifdef MLE_DEBUG
            printf("MleMrefChunkFile::processMedia: external media reference\n");
#endif
            newData->m_flags = chunkData->m_flags;
            newData->m_type = chunkData->m_type;
            newData->m_size = chunkData->m_size;
            newData->m_data = filename;
            break;

        case 1:
#ifdef MLE_DEBUG
            printf("MleMrefChunkFile::processMedia: internal media reference\n");
#endif

            newData->m_flags = chunkData->m_flags;
            newData->m_type = chunkData->m_type;

            // Retrieve the size of the file denoted by filename.
            struct stat file_info;
            if (stat(filename, &file_info) != 0)
            {
#ifdef MLE_DEBUG
                printf("%s: %s %s\n",
                       "MleMrefChunkFile::processMedia:"
                       "couldn't find media file",
                       filename);
#endif
                mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
                mlFree(filename);
                return(NULL);
            } else
                newData->m_size = file_info.st_size;

            // Open media chunk file
            mediaFp = mlFOpen(filename, "r");
            if (mediaFp == NULL)
            {
#ifdef MLE_DEBUG
                printf("%s: %s %s\n",
                       "MleMediaRefChunk::processMedia",
                       "couldn't open media file",
                       filename);
#endif
                mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
                mlFree(filename);
                return(NULL);
            } else {
#ifdef MLE_DEBUG
                printf("%s: %s %s\n",
                       "MleMediaRefChunk::processMedia",
                       "opened media file",
                       filename);
#endif
            }
            
            // Read media data.
            chunkData->m_data = (char *)mlMalloc(file_info.st_size);
            mlFRead(chunkData->m_data,file_info.st_size,1,mediaFp);

            // Close media file and clean up.
            mlFClose(mediaFp);
            mlFree(filename);
            break;

        default:
#ifdef MLE_DEBUG
            printf("%s: %s %d\n",
                   "MleMrefChunkFile::processMedia",
                   "unknown media reference type",
                   chunkData->m_flags);
#endif
            mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
            mlFree(filename);
            return(NULL);
    }

    return(newData);
}

void *
MleMrefChunkFile::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleMrefChunkFile::operator delete(void *p)
{
	mlFree(p);
}

#ifdef UNIT_TEST


// Include system header files.
#include <sys/types.h>
#include <time.h>


#define TESTBE_FILE "testbe.dpp"
#define TESTLE_FILE "testle.dpp"

#define TEST_HEADER_ID      0
#define TEST_HEADER_CRC     0
#define TEST_EXTERNAL_MEDIAREF "/usr/people/msm/testfile"

void main(int argc,char *argv[])
{
    // Declare local variables.
    MleMrefChunkFile *in,*out;
    MleChunkFileHeader hdrData;
    MleMediaRefChunk mrefData;
    MleMediaRefInfoChunk mrefInfoData;
    MlUInt timeStamp;
    MlBoolean success = FALSE;

    timeStamp = time(NULL);

    hdrData.m_id = TEST_HEADER_ID;
    hdrData.m_version = MleMrefChunkFile::VERSION;
    hdrData.m_time = timeStamp;
    hdrData.m_crc = TEST_HEADER_CRC;

    mrefInfoData.m_flags = 0;
    mrefInfoData.m_type = 0;
    mrefInfoData.m_numMrefs = 1;

    mrefData.m_flags = 0;
    mrefData.m_size = strlen(TEST_EXTERNAL_MEDIAREF);
    mrefData.m_data = TEST_EXTERNAL_MEDIAREF;

    // Create Big Endian chunk file.
    out = new MleMrefChunkFile(TESTBE_FILE,MleChunkFile::WRITING);
#if BYTE_ORDER != BIG_ENDIAN
	out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPBE);
    out->writeHeader(&hdrData);
    out->beginList();
    out->writeInfo(&mrefInfoData);
    out->writeData(&mrefData);
    out->endList();
    out->end();
    delete out;

	// Note: reset mrefInfoData and mrefData because values in those
	// strucutres changed due to setting the swap on for the Big
	// Endian format.
#if BYTE_ORDER != BIG_ENDIAN
	mrefInfoData.m_flags = 0;
    mrefInfoData.m_type = 0;
    mrefInfoData.m_numMrefs = 1;

    mrefData.m_flags = 0;
    mrefData.m_size = strlen(TEST_EXTERNAL_MEDIAREF);
    mrefData.m_data = TEST_EXTERNAL_MEDIAREF;
#endif

    // Create Little Endian chunk file.
    out = new MleMrefChunkFile(TESTLE_FILE,MleChunkFile::WRITING);
#if BYTE_ORDER != LITTLE_ENDIAN
    out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPLE);
    out->writeHeader(&hdrData);
    out->beginList();
    out->writeInfo(&mrefInfoData);
    out->writeData(&mrefData);
    out->endList();
    out->end();
    delete out;

    memset(&hdrData,0,sizeof(MleChunkFileHeader));
    memset(&mrefData,0,sizeof(MleMediaRefChunk));
    memset(&mrefInfoData,0,sizeof(MleMediaRefInfoChunk));

    // Read Big Endian chunk file.
    in = new MleMrefChunkFile(TESTBE_FILE,MleChunkFile::READING);
#if BYTE_ORDER != BIG_ENDIAN
	in->setSwap(TRUE);
#endif
    in->begin(MLE_DPP_FINDPPBE);
    in->readHeader(&hdrData);
    in->beginList();
    in->readInfo(&mrefInfoData);
    in->readData(&mrefData);
    in->endList();
    in->end();
    delete in;

    if ((hdrData.m_id != TEST_HEADER_ID) ||
        (hdrData.m_version != MleMrefChunkFile::VERSION) ||
        (hdrData.m_time != timeStamp) ||
        (hdrData.m_crc != TEST_HEADER_CRC))
    {
        success = FALSE;
    } else
    {
        if ((mrefInfoData.m_flags != 0) ||
            (mrefInfoData.m_type != 0) ||
            (mrefInfoData.m_numMrefs != 1) ||
            (mrefData.m_flags != 0) ||
            (mrefData.m_size != strlen(TEST_EXTERNAL_MEDIAREF)) ||
            (strncmp(mrefData.m_data,TEST_EXTERNAL_MEDIAREF,mrefData.m_size)))
		{
            success = FALSE;
        } else {
            success = TRUE;
        }
    }

    if (success)
        fprintf(stderr,"Big Endian MrefChunk Test Succeeded.\n");
    else
        fprintf(stderr,"Big Endian MrefChunk Test Failed!\n");

    memset(&hdrData,0,sizeof(MleChunkFileHeader));
    memset(&mrefData,0,sizeof(MleMediaRefChunk));
    memset(&mrefInfoData,0,sizeof(MleMediaRefInfoChunk));

    // Read Little Endian chunk file.
    in = new MleMrefChunkFile(TESTLE_FILE,MleChunkFile::READING);
#if BYTE_ORDER != LITTLE_ENDIAN
    in->setSwap(TRUE);
#endif
    in->begin(MLE_DPP_FINDPPLE);
    in->readHeader(&hdrData);
    in->beginList();
    in->readInfo(&mrefInfoData);
    in->readData(&mrefData);
    in->endList();
    in->end();
    delete in;

    if ((hdrData.m_id != TEST_HEADER_ID) ||
        (hdrData.m_version != MleMrefChunkFile::VERSION) ||
        (hdrData.m_time != timeStamp) ||
        (hdrData.m_crc != TEST_HEADER_CRC))
    {
        success = FALSE;
    } else
    {
        if ((mrefInfoData.m_flags != 0) ||
            (mrefInfoData.m_type != 0) ||
            (mrefInfoData.m_numMrefs != 1) ||
            (mrefData.m_flags != 0) ||
            (mrefData.m_size != strlen(TEST_EXTERNAL_MEDIAREF)) ||
            (strncmp(mrefData.m_data,TEST_EXTERNAL_MEDIAREF,mrefData.m_size)))
		{
            success = FALSE;
        } else {
            success = TRUE;
        }
    }

    if (success)
        fprintf(stderr,"Little Endian MrefChunk Test Succeeded.\n");
    else
        fprintf(stderr,"Little Endian MrefChunk Test Failed!\n");
}


#endif /* UNIT_TEST */

/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file ppinput.cxx
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

// Declare system header files.
#include <limits.h>
#include <string.h>

// Include Magic Lantern header files.
#include "mle/ppinput.h"
#include "mle/mlMalloc.h"
#include "mle/mlErrno.h"
#include "mle/mlAssert.h"


MleDppInput::MleDppInput(const char *filename)
    :MleDpp(filename,MleDpp::READING)
{
    // Initialize Playprint book keeping structures.
    memset(&m_mrefListInfo,0,sizeof(MleDppChunkInfo));
}


MleDppInput::~MleDppInput()
{
}


int MleDppInput::readHeader(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleDppHeader *hdrData;
    int status = FALSE;
    void *callData;

    hdrData = (MleDppHeader *)mlCalloc(1,sizeof(MleDppHeader));

    // Read Playprint header. A damaged header is an error.
    if (! MleDpp::readHeader(hdrData))
    {
        mlFree(hdrData);
        return(FALSE);
    }

    if (func)
	{
        callData = (void *)hdrData;
        status = func(clientData,callData);
    } else
        status = TRUE;

    mlFree(hdrData);
    return(status);
}


int MleDppInput::readTOC(MleDppCallback func,void *clientData)
{
    // declare local variables.
    MleDppChunkInfo tocInfo;
    MleDppTocEntry entry;
    MlUInt numEntries;
    int status = FALSE;

    memset(&tocInfo,0,sizeof(MleDppChunkInfo));

    mlDppDescend(getFp(),&tocInfo,NULL,0);
    if (tocInfo.m_chunk.m_tag == mlDppMakeTag('t','o','c',' '))
	{
        // Read number of TOC entries.
    	size_t nBytes;
        nBytes = mlFRead(&numEntries,sizeof(MlUInt),1,getFp());
        if (nBytes != 1)
            return(FALSE);
        if (getSwapRead())
            numEntries = mlDppSwapInt(numEntries);

        // The number of entries is from the file, so check that they are in
        // the chunk and in the file before they are read.
        if (numEntries > (MlUInt)INT_MAX ||
            (tocInfo.m_chunk.m_size < sizeof(MlUInt) ||
             numEntries > (tocInfo.m_chunk.m_size - sizeof(MlUInt)) / sizeof(MleDppTocEntry)) ||
            ! mlDppBytesRemain(getFp(),numEntries * (MlUInt)sizeof(MleDppTocEntry)))
            return(FALSE);

        beginTOC(numEntries);

        // Read each TOC entry.
        for (MlUInt i = 0; i < numEntries; i++)
		{
            nBytes = mlFRead(&entry,sizeof(MleDppTocEntry),1,getFp());
            if (nBytes != 1)
                break;
            if (getSwapRead())
            {
                entry.m_label = mlDppSwapInt(entry.m_label);
                entry.m_offset = mlDppSwapInt(entry.m_offset);
            }
            addTOCEntry(entry.m_label,entry.m_offset);
        }

        // Finish TOC reading.
        endTOC();

        if (func)
		{
        	void *calldata = (void *)m_TOC;
            status = func(clientData,calldata);
        } else
            status = TRUE;
    }

    mlDppAscend(getFp(),&tocInfo,0);
    return(status);
}


MlInt MleDppInput::getTOCOffset(MlUInt index)
{
    // Declare local variables.
    MleDppTOC *nextEntry;

    MLE_ASSERT(index < m_numTOCUsed);

    if (m_TOC)
        nextEntry = m_TOC;
    else
        return(-1);

    for (MlUInt i = 0; i < index; i++)
        nextEntry = nextEntry->m_next;

    return(nextEntry->m_entry.m_offset);
}


void MleDppInput::goTo(MlInt tocOffset)
{
    mlFSeek(getFp(),tocOffset,0);
}


#if 0
MleDppQuark MleDppInput::getTOCLabel(MlUInt index)
{
    // declare local variables
    MleDppTOC *nextEntry;

    MLE_ASSERT(index < m_numTOCUsed);

    if (m_TOC)
        nextEntry = m_TOC;
    else
        return(-1);

    for (int i = 0; i < index; i++)
        nextEntry = nextEntry->m_next;

    return(nextEntry->m_entry.m_label);
}
#endif /* 0 */


int MleDppInput::readGroup(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleDppChunkInfo groupInfo;
    void *callData;
    int status = FALSE;

    memset(&groupInfo,0,sizeof(MleDppChunkInfo));

    mlDppDescend(getFp(),&groupInfo,NULL,0);
    if (groupInfo.m_chunk.m_tag == mlDppMakeTag('g','r','p',' '))
	{
        if (func)
		{
        	size_t nBytes;

            // The size is from the file. The callback may look at the first
            // 12 bytes (the Actor Group header) whatever the size, so the
            // buffer is at least that big and is zeroed.
            if (! mlDppBytesRemain(getFp(),groupInfo.m_chunk.m_size))
                return(FALSE);
            MlUInt bufSize = groupInfo.m_chunk.m_size < 12 ? 12 : groupInfo.m_chunk.m_size;
            callData = mlCalloc(1,bufSize);
            nBytes = mlFRead(callData,groupInfo.m_chunk.m_size,1,getFp());

            if (groupInfo.m_chunk.m_size == 0 || nBytes == 1)
                status = func(clientData,callData);

            mlFree(callData);
        } else
            status = TRUE;
    }

    mlDppAscend(getFp(),&groupInfo,0);
    return(status);
}


int MleDppInput::readScene(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleDppChunkInfo sceneInfo;
    void *callData;
    int status = FALSE;

    memset(&sceneInfo,0,sizeof(MleDppChunkInfo));

    mlDppDescend(getFp(),&sceneInfo,NULL,0);
    if (sceneInfo.m_chunk.m_tag == mlDppMakeTag('s','c','n',' '))
	{
        if (func)
		{
        	size_t nBytes;

            // The size is from the file, so check that it is in the file. The
            // buffer is zeroed and has at least one byte.
            if (! mlDppBytesRemain(getFp(),sceneInfo.m_chunk.m_size))
                return(FALSE);
            callData = mlCalloc(1,sceneInfo.m_chunk.m_size ? sceneInfo.m_chunk.m_size : 1);
            nBytes = mlFRead(callData,sceneInfo.m_chunk.m_size,1,getFp());

            if (sceneInfo.m_chunk.m_size == 0 || nBytes == 1)
                status = func(clientData,callData);

            mlFree(callData);
        } else
            status = TRUE;
    }

    mlDppAscend(getFp(),&sceneInfo,0);
    return(status);
}


int MleDppInput::readSet(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleDppChunkInfo setInfo;
    void *callData;
    int status = FALSE;

    memset(&setInfo,0,sizeof(MleDppChunkInfo));

    mlDppDescend(getFp(),&setInfo,NULL,0);
    if (setInfo.m_chunk.m_tag == mlDppMakeTag('s','e','t',' '))
	{
        if (func)
		{
        	size_t nBytes;

            // The size is from the file, so check that it is in the file. The
            // buffer is zeroed and has at least one byte.
            if (! mlDppBytesRemain(getFp(),setInfo.m_chunk.m_size))
                return(FALSE);
            callData = mlCalloc(1,setInfo.m_chunk.m_size ? setInfo.m_chunk.m_size : 1);
            nBytes = mlFRead(callData,setInfo.m_chunk.m_size,1,getFp());

            if (setInfo.m_chunk.m_size == 0 || nBytes == 1)
                status = func(clientData,callData);

            mlFree(callData);
        } else
            status = TRUE;
    }

    mlDppAscend(getFp(),&setInfo,0);
    return(status);
}


int MleDppInput::beginMref(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleMediaRefInfoChunk mrefInfoData;
    void *callData;
    int status = TRUE;;

    memset(&mrefInfoData,0,sizeof(mrefInfoData));

    // Begin IFF 'MRFL' LIST chunk.
    if (! beginMrefList())
        return(FALSE);

    // Read IFF 'info' chunk.
    if (! readMrefInfo(&mrefInfoData))
        return(FALSE);

    if (func)
	{
        callData = (void *)&mrefInfoData;
        status = func(clientData,callData);
    }

    return(status);
}


int MleDppInput::endMref(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    int status = TRUE;

    // End IFF 'MRFL' LIST chunk.
    endMrefList();

    if (func)
	{
        status = func(clientData,NULL);
    }

    return(status);
}


int MleDppInput::readMedia(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleMediaRefChunk *mrefData;
    void *callData;
    int status = TRUE;

    mrefData = (MleMediaRefChunk *)mlCalloc(1,sizeof(MleMediaRefChunk));

    // Read IFF 'mref' chunk. A damaged chunk is an error, and has no data.
    if (! readMref(mrefData))
        status = FALSE;
    else if (func)
	{
        callData = (void *)mrefData;
        status = func(clientData,callData);
    }

    // Free chunk buffer.
    if (mrefData->m_data)
        mlFree(mrefData->m_data);

    // Free chunk.
    mlFree(mrefData);

    return(status);
}


MlBoolean MleDppInput::beginMrefList(void)
{
    m_mrefListInfo.m_form = mlDppMakeTag('M','R','F','L');
    return(mlDppDescend(getFp(),&m_mrefListInfo,NULL,0));
}


MlBoolean MleDppInput::endMrefList(void)
{
    return(mlDppAscend(getFp(),&m_mrefListInfo,0));
}


MlBoolean MleDppInput::readMrefInfo(MleMediaRefInfoChunk *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo mediaInfo;
    int status = TRUE;

    memset(&mediaInfo,0,sizeof(MleDppChunkInfo));
    memset(chunkData,0,sizeof(MleMediaRefInfoChunk));

    mlDppDescend(getFp(),&mediaInfo,NULL,0);
    if (mediaInfo.m_chunk.m_tag != mlDppMakeTag('i','n','f','o') ||
        mediaInfo.m_chunk.m_size < sizeof(MleMediaRefInfoChunk))
        status = FALSE;
    else
	{
        // Read the 'info' data. The size of the chunk is from the file and is
        // not used for the read, as the data is a fixed size.
    	size_t nBytes;
        nBytes = mlFRead(chunkData,sizeof(MleMediaRefInfoChunk),1,getFp());
        if (nBytes != 1)
            status = FALSE;

        if (getSwapRead())
		{
            chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
            chunkData->m_type = mlDppSwapInt(chunkData->m_type);
            chunkData->m_numMrefs = mlDppSwapInt(chunkData->m_numMrefs);
        }
    }

    mlDppAscend(getFp(),&mediaInfo,0);

    return(status);
}


#define MEDIAREF_CHUNK_SIZE (3 * sizeof(MlUInt))

MlBoolean MleDppInput::readMref(MleMediaRefChunk *chunkData)
{
    // declare local variables.
    MleDppChunkInfo mediaInfo;
    int status = TRUE;

    memset(&mediaInfo,0,sizeof(MleDppChunkInfo));
    memset(chunkData,0,sizeof(MleMediaRefChunk));

    mlDppDescend(getFp(),&mediaInfo,NULL,0);
    if (mediaInfo.m_chunk.m_tag != mlDppMakeTag('m','r','e','f') ||
        mediaInfo.m_chunk.m_size < MEDIAREF_CHUNK_SIZE)
        status = FALSE;
    else
	{
        // Read the 'mref' data.
    	size_t nBytes;
        nBytes = mlFRead(chunkData,MEDIAREF_CHUNK_SIZE,1,getFp());
        if (nBytes != 1)
            status = FALSE;

        if (getSwapRead())
		{
            chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
            chunkData->m_type = mlDppSwapInt(chunkData->m_type);
            chunkData->m_size = mlDppSwapInt(chunkData->m_size);
        }

        // The size of the data is from the file, so check that it is in the
        // chunk and in the file before it is read.
        if (status &&
            (chunkData->m_size > mediaInfo.m_chunk.m_size - MEDIAREF_CHUNK_SIZE ||
             ! mlDppBytesRemain(getFp(),chunkData->m_size)))
            status = FALSE;

        if (status)
        {
            if (chunkData->m_flags == 0)
                // External MediaRef; data is a filename.
                chunkData->m_data = (char *)mlMalloc(chunkData->m_size + 1);
            else
                chunkData->m_data = (char *)mlMalloc(chunkData->m_size ? chunkData->m_size : 1);
            if (chunkData->m_size > 0 &&
                mlFRead(chunkData->m_data,chunkData->m_size,1,getFp()) != 1)
            {
                mlFree(chunkData->m_data);
                chunkData->m_data = NULL;
                status = FALSE;
            }
        }

        if (status && chunkData->m_flags == 0)
		{
            // terminate filename
            chunkData->m_data[chunkData->m_size] = '\0';
            chunkData->m_size += 1;
        }
    }

    mlDppAscend(getFp(),&mediaInfo,0);

    return(status);
}

void *
MleDppInput::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppInput::operator delete(void *p)
{
	mlFree(p);
}

void*
MleDppInput::operator new[](size_t tSize)
{
    void* p = mlMalloc(tSize);
    return p;
}

void
MleDppInput::operator delete[](void* p)
{
    mlFree(p);
}

#ifdef UNIT_TEST

#define TESTBE_FILE "testbe.dpp"
#define TESTLE_FILE "testle.dpp"

int headerCB(void *clientData, void *callData)
{
    // Declare local variables.
    MleDppHeader *hdrData = (MleDppHeader *)callData;
	if (hdrData->m_version != MleDpp::PLAYPRINT_VERSION)
	{
        // XXX -- need to set an errno here.
        return(FALSE);
    }

	fprintf(stdout,"Read Header\n");
	return(TRUE);
}

int tocCB(void *clientData, void *callData)
{
    // Declare local variables.

	fprintf(stdout,"Read Table Of Contents\n");
	return(TRUE);
}

int readGroupCB(void *clientData, void *callData)
{
    // Declare local variables.
    void *group = (void *)callData;
	if (group == NULL)
	{
        mlSetErrno(MLE_DPP_ERR_CANNOTREAD);
        return(FALSE);
    }

	fprintf(stdout,"Read Actor Group Chunk\n");
	return(TRUE);
}

void main(int argc,char *argv[])
{
    // Declare local variables.
    MleDppInput *in;
    char *filename;
    char group1[128];

    // Parse arguments.
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <file>\n",argv[0]);
        exit(-1);
    } else
        filename = argv[1];

    // Test Playprint retrieval.
	in = new MleDppInput(filename);
	in->begin(MLE_DPP_CREATEPPLE);
	in->readTOC(tocCB,NULL);
    in->readHeader(headerCB,NULL);
    in->readGroup(readGroupCB,group1);
	in->end();
    delete in;
}


#endif /* UNIT_TEST */

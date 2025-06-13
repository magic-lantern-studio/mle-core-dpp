/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file ppinput.cxx
 * @ingroup MleDPPMaster
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */

// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2024 Wizzer Works
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

    hdrData = (MleDppHeader *)mlMalloc(sizeof(MleDppHeader));

    // Read Playprint header.
    MleDpp::readHeader(hdrData);

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

    MLE_ERROR(mlDppDescend(getFp(),&tocInfo,NULL,0));
    if (tocInfo.m_chunk.m_tag == mlDppMakeTag('t','o','c',' '))
	{
        // Read number of TOC entries.
    	size_t nBytes;
        nBytes = mlFRead(&numEntries,sizeof(MlUInt),1,getFp());
        beginTOC(numEntries);

        // Read each TOC entry.
        for (MlUInt i = 0; i < numEntries; i++)
		{
            nBytes = mlFRead(&entry,sizeof(MleDppTocEntry),1,getFp());
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

    MLE_ERROR(mlDppAscend(getFp(),&tocInfo,0));
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

    MLE_ERROR(mlDppDescend(getFp(),&groupInfo,NULL,0));
    if (groupInfo.m_chunk.m_tag == mlDppMakeTag('g','r','p',' '))
	{
        if (func)
		{
        	size_t nBytes;
            callData = mlMalloc(groupInfo.m_chunk.m_size);
            nBytes = mlFRead(callData,groupInfo.m_chunk.m_size,1,getFp());

            status = func(clientData,callData);

            mlFree(callData);
        } else
            status = TRUE;
    }

    MLE_ERROR(mlDppAscend(getFp(),&groupInfo,0));
    return(status);
}


int MleDppInput::readScene(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleDppChunkInfo sceneInfo;
    void *callData;
    int status = FALSE;

    memset(&sceneInfo,0,sizeof(MleDppChunkInfo));

    MLE_ERROR(mlDppDescend(getFp(),&sceneInfo,NULL,0));
    if (sceneInfo.m_chunk.m_tag == mlDppMakeTag('s','c','n',' '))
	{
        if (func)
		{
        	size_t nBytes;
            callData = mlMalloc(sceneInfo.m_chunk.m_size);
            nBytes = mlFRead(callData,sceneInfo.m_chunk.m_size,1,getFp());

            status = func(clientData,callData);

            mlFree(callData);
        } else
            status = TRUE;
    }

    MLE_ERROR(mlDppAscend(getFp(),&sceneInfo,0));
    return(status);
}


int MleDppInput::readSet(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleDppChunkInfo setInfo;
    void *callData;
    int status = FALSE;

    memset(&setInfo,0,sizeof(MleDppChunkInfo));

    MLE_ERROR(mlDppDescend(getFp(),&setInfo,NULL,0));
    if (setInfo.m_chunk.m_tag == mlDppMakeTag('s','e','t',' '))
	{
        if (func)
		{
        	size_t nBytes;
            callData = mlMalloc(setInfo.m_chunk.m_size);
            nBytes = mlFRead(callData,setInfo.m_chunk.m_size,1,getFp());

            status = func(clientData,callData);

            mlFree(callData);
        } else
            status = TRUE;
    }

    MLE_ERROR(mlDppAscend(getFp(),&setInfo,0));
    return(status);
}


int MleDppInput::beginMref(MleDppCallback func,void *clientData)
{
    // Declare local variables.
    MleMediaRefInfoChunk mrefInfoData;
    void *callData;
    int status = TRUE;;

    // Begin IFF 'MRFL' LIST chunk.
    beginMrefList();

    // Read IFF 'info' chunk.
    readMrefInfo(&mrefInfoData);

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

    mrefData = (MleMediaRefChunk *)mlMalloc(sizeof(MleMediaRefChunk));

    // Read IFF 'mref' chunk.
    readMref(mrefData);

    if (func)
	{
        callData = (void *)mrefData;
        status = func(clientData,callData);
    }

    // Free chunk buffer.
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

    MLE_ERROR(mlDppDescend(getFp(),&mediaInfo,NULL,0));
    if (mediaInfo.m_chunk.m_tag == mlDppMakeTag('i','n','f','o'))
	{
        // Read the 'info' data.
    	size_t nBytes;
        nBytes = mlFRead(chunkData,mediaInfo.m_chunk.m_size,1,getFp());

        if (getSwapRead())
		{
            chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
            chunkData->m_type = mlDppSwapInt(chunkData->m_type);
            chunkData->m_numMrefs = mlDppSwapInt(chunkData->m_numMrefs);
        }
    }

    MLE_ERROR(mlDppAscend(getFp(),&mediaInfo,0));

    return(status);
}


#define MEDIAREF_CHUNK_SIZE (3 * sizeof(MlUInt))

MlBoolean MleDppInput::readMref(MleMediaRefChunk *chunkData)
{
    // declare local variables.
    MleDppChunkInfo mediaInfo;
    int status = TRUE;

    memset(&mediaInfo,0,sizeof(MleDppChunkInfo));

    MLE_ERROR(mlDppDescend(getFp(),&mediaInfo,NULL,0));
    if (mediaInfo.m_chunk.m_tag == mlDppMakeTag('m','r','e','f'))
	{
        // Read the 'mref' data.
    	size_t nBytes;
        nBytes = mlFRead(chunkData,MEDIAREF_CHUNK_SIZE,1,getFp());

        if (getSwapRead())
		{
            chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
            chunkData->m_type = mlDppSwapInt(chunkData->m_type);
            chunkData->m_size = mlDppSwapInt(chunkData->m_size);
        }

        if (chunkData->m_flags == 0)
            // External MediaRef; data is a filename.
            chunkData->m_data = (char *)mlMalloc(chunkData->m_size + 1);
        else
            chunkData->m_data = (char *)mlMalloc(chunkData->m_size);
        nBytes = mlFRead(chunkData->m_data,chunkData->m_size,1,getFp());

        if (chunkData->m_flags == 0)
		{
            // terminate filename
            chunkData->m_data[chunkData->m_size] = '\0';
            chunkData->m_size += 1;
        }
    }

    MLE_ERROR(mlDppAscend(getFp(),&mediaInfo,0));

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

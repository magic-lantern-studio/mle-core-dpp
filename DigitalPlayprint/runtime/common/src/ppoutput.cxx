/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file ppoutput.cxx
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
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

// Include Magic Lantern header files.
#include "mle/mlErrno.h"
#include "mle/mlMalloc.h"
#include "mle/mlAssert.h"

// Include Digital Playprint header files.
#include "mle/ppoutput.h"
//#include "mle/mediaref.h"
#include "mle/mrefchk.h"
#include "mle/agchk.h"
#include "mle/scenechk.h"

// Include Digital Workprint header files.
#include "mle/DwpMediaRef.h"


MleDppOutput::MleDppOutput(const char *filename)
    :MleDpp(filename,MleDpp::WRITING)
{
    // Initialize Playprint book keeping structures.
    memset(&m_mrefListInfo,0,sizeof(MleDppChunkInfo));
}


MleDppOutput::~MleDppOutput()
{
    // Do nothing extra.
}


MlBoolean MleDppOutput::writeHeader()
{
    // Declare local variables.
    MleDppHeader    hdrData;

    // Write header chunk data.
    hdrData.m_version = MleDpp::PLAYPRINT_VERSION;
    hdrData.m_date = time(NULL);

    return(MleDpp::writeHeader(&hdrData));
}


MlBoolean MleDppOutput::writeGroup(MleDppChunkTable *table, MlInt groupIndex)
{
    int i;

    // Create group chunk info structure.
    MleDppChunkInfo groupInfo;
    memset(&groupInfo,0,sizeof(MleDppChunkInfo));

    // Open group chunk file and resolve media references.
    MlBoolean isLittle = getSwapWrite();
#if BYTE_ORDER == LITTLE_ENDIAN
    isLittle = ML_TRUE;
#endif
    MleDppActorGroupChunk *groupChunk =
         new MleDppActorGroupChunk(table->getFilename(groupIndex),isLittle);

    const char *namesOfMediaRefsInThisChunkFile = groupChunk->getNames();
    MlInt numberOfMediaRefsInCurrentChunkFile = groupChunk->countNames();
    MlInt numberOfChunkFiles = table->getUsed();
#ifdef VERBOSE
    printf("Found %d media refs in group chunk file %s\n", numberOfMediaRefsInCurrentChunkFile, table->getFilename(groupIndex));
#endif
    MlInt *indices = NULL;
    if (numberOfMediaRefsInCurrentChunkFile > 0)
    {
		indices = (int *) mlMalloc(numberOfMediaRefsInCurrentChunkFile * sizeof(MlInt));
		memset(indices, 0, numberOfMediaRefsInCurrentChunkFile * sizeof(MlInt));
		const char *nextRefName = namesOfMediaRefsInThisChunkFile;

		// XXX should just skip rest if there's no name.
		do
		  printf(">>>>>>>>MEDIA NAME: %s\n", nextRefName);
		while (nextRefName = groupChunk->nextName(nextRefName));
		nextRefName = namesOfMediaRefsInThisChunkFile;

		int mrefcount = 0;
		do
		    for (i=0; i < numberOfChunkFiles; i++)
			{
			  if (strcmp(table->getName(i), nextRefName) == 0)
			  {
			      indices[mrefcount] = i;
			      mrefcount++;
			      break;
			  }
		  }
		while (nextRefName = groupChunk->nextName(nextRefName));
    }

    const MlByte *resolvedChunk;
    MlInt size;
    resolvedChunk = groupChunk->resolveNames(&size, indices);

    // Create 'grp ' chunk structure.
    groupInfo.m_chunk.m_tag = mlDppMakeTag('g','r','p',' ');
    groupInfo.m_chunk.m_size = size;
    MLE_WARN(mlDppCreateChunk(m_fp,&groupInfo,0));

    // Write out 'grp ' data.
    if (mlFWrite(resolvedChunk,size,1,m_fp) != 1)
	{
        mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
        return(FALSE);
    }

    //delete (unsigned char *) resolvedChunk;
	mlFree((void *)resolvedChunk);

    /*
     * Update the Playprint table of content if needed. This places
     * the TOC offset at the beginning of the 'grp' chunk.
     */
    if (m_useTOC)
        // Add TOC entry.
        addTOCEntry(0, // XXX ignored for now (was label)
                    groupInfo.m_offset - sizeof(MleDppChunk));

    return(mlDppAscend(m_fp,&groupInfo,0));
}


MlBoolean MleDppOutput::writeScene(MleDppChunkTable *table, MlInt sceneIndex)
{
    // Create scene chunk info structure.
    MleDppChunkInfo sceneInfo;
    memset(&sceneInfo,0,sizeof(MleDppChunkInfo));

    // Open scene chunk file .
    MleDppSceneChunk *sceneChunk =
        new MleDppSceneChunk(table->getFilename(sceneIndex),getSwapWrite());

    MlInt numberOfChunkFiles = table->getUsed();
    const unsigned char *resolvedChunk;
    MlInt size;
    resolvedChunk = sceneChunk->loadChunk(&size);

    // Create 'scn ' chunk structure.
    sceneInfo.m_chunk.m_tag = mlDppMakeTag('s','c','n',' ');
    sceneInfo.m_chunk.m_size = size;
    MLE_WARN(mlDppCreateChunk(m_fp,&sceneInfo,0));

    // Write out 'scn ' data
    if (mlFWrite(resolvedChunk,size,1,m_fp) != 1)
	{
        mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
        return(FALSE);
    }

    /*
     * Update the Playprint table of content if needed. This places
     * the TOC offset at the beginning of the 'scn' chunk.
     */
    if (m_useTOC)
        // Add TOC entry.
        addTOCEntry(0, // XXX ignored for now (was label)
                    sceneInfo.m_offset - sizeof(MleDppChunk));

    // The ascend will go back to the top, figure out the chunk\'s size,
    // and write it into the header.
    return(mlDppAscend(m_fp,&sceneInfo,0));
}


MlBoolean MleDppOutput::writeMedia(char *chunkFilename)
{
    // Declare local variables.
    MleMrefChunkFile *in;
    MleChunkFileHeader hdrData;
    MleMediaRefInfoChunk inMrefInfoData,outMrefInfoData;
    MleMediaRefChunk mrefData;

    // Prepare the MediaRef Chunk File for reading.
    in = new MleMrefChunkFile(chunkFilename,MleMrefChunkFile::READING);
    if (getSwapRead())
	{
        in->setSwap(TRUE);
    }
#if BYTE_ORDER == BIG_ENDIAN
	if (getSwapWrite())
		in->begin(MLE_DPP_FINDPPLE);
	else
	    in->begin(MLE_DPP_FINDPPBE);
#else
	if (getSwapWrite())
		in->begin(MLE_DPP_FINDPPBE);
	else
		in->begin(MLE_DPP_FINDPPLE);
#endif
    in->readHeader(&hdrData);
    in->beginList();

    // Begin IFF 'MRFL' LIST chunk.
    beginMrefList();

    // Read IFF 'info' chunk.
    in->readInfo(&inMrefInfoData);

    memcpy(&outMrefInfoData,&inMrefInfoData,sizeof(MleMediaRefInfoChunk));

    // Write IFF 'info' chunk.
    writeMrefInfo(&outMrefInfoData);

    for (MlUInt i = 0; i < inMrefInfoData.m_numMrefs; i++)
	{
        // Read IFF 'mref' chunk.
        in->readData(&mrefData);

        // Write IFF 'mref' chunk.
        writeMref(&mrefData);
    }

    // End IFF 'MRFL' LIST chunk.
    endMrefList();

    // Close MediaRef Chunk File.
    in->endList();
    in->end();
    delete in;

    return(TRUE);
}


MlBoolean
MleDppOutput::writeSet(char *chunkFilename)
{
    MlByte *ppSetChunk;
    MlInt size, ppSetChunkLength;
    struct stat fileStat;
    FILE *setChunkFilePtr;
    MleDppChunkInfo setChunkInfo;

    /*
     * Stat the set chunk file.
     */
    if (stat(chunkFilename, &fileStat) != 0)
	{
#ifdef VERBOSE
		printf("MleDppOutput::writeSet: couldn't find set chunk file %s\n",
			   chunkFilename);
#endif
		mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
		return(FALSE);
    }

    /*
     * Open the set chunk file.
     */
    setChunkFilePtr = mlFOpen(chunkFilename, "r");
    if (! setChunkFilePtr)
	{
#ifdef VERBOSE
		printf("MleDppOutput::writeSet: couldn't open set chunk file %s\n",
			   chunkFilename);
#endif
		mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
		return(FALSE);
	}
#ifdef VERBOSE
	else
		printf("MleDppOutput::writeSet: opened chunk file %s (%d bytes)\n",
			   chunkFilename, fileStat.st_size);
#endif

    /*
     * Allocate a buffer to store the set chunk that is going
     * into the playprint.  Note we don't need the first four bytes
     * which contain the size of this set chunk file.
     */
    ppSetChunkLength = fileStat.st_size - 4;
    ppSetChunk = new MlByte[ppSetChunkLength];
    memset(ppSetChunk, 0, ppSetChunkLength);

    /*
     * Read the first four bytes in the set chunk file (contains the
     * file the size).  Read in the rest of set chunk file and close it
     * after finish reading.
     */
    mlFRead(&size, 4, 1, setChunkFilePtr);
    mlFRead(ppSetChunk, ppSetChunkLength, 1, setChunkFilePtr);
    mlFClose(setChunkFilePtr);

    /*
     * The size from stat should agree with the size read in from the
     * set chunk file.
     */
    MLE_ASSERT(size == fileStat.st_size);

    /*
     * Initialize 'frm' (set) chunk structure for the playprint and
     * write out the set chunk into the playprint.
     */
    memset(&setChunkInfo, 0, sizeof(MleDppChunkInfo));
    setChunkInfo.m_chunk.m_tag = mlDppMakeTag('s', 'e', 't', ' ');
    setChunkInfo.m_chunk.m_size = ppSetChunkLength;

    MLE_WARN(mlDppCreateChunk(m_fp, &setChunkInfo, 0));

    if (mlFWrite(ppSetChunk, ppSetChunkLength, 1, m_fp) != 1)
	{
		mlSetErrno(MLE_DPP_ERR_CANNOTWRITE);
		return(FALSE);
    }

    /*
     * Update the Playprint table of content if needed. This places
     * the TOC offset at the beginning of the 'frm' chunk.
     */
    if (m_useTOC)
        addTOCEntry(0, // XXX ignored for now (wsa label).
	            setChunkInfo.m_offset - sizeof(MleDppChunk));

    delete [] ppSetChunk;

    return(mlDppAscend(m_fp, &setChunkInfo, 0));
}


MlBoolean MleDppOutput::beginMrefList(void)
{
    // Declare local variables.
    MlBoolean status;

    m_mrefListInfo.m_form = mlDppMakeTag('M','R','F','L');
    status = mlDppCreateChunk(getFp(),&m_mrefListInfo,MLE_DPP_CREATELIST);

    /*
     * Update the Playprint table of content if needed. This places
     * the TOC offset at the beginning of the 'MRFL' LIST chunk.
     */
    if (status && m_useTOC)
        addTOCEntry(0, // XXX ignored for now (wsa label).
	            m_mrefListInfo.m_offset - sizeof(MleDppChunk) - 4);

    return(status);
}


MlBoolean MleDppOutput::endMrefList(void)
{
    return(mlDppAscend(getFp(),&m_mrefListInfo,0));
}


MlBoolean MleDppOutput::writeMrefInfo(MleMediaRefInfoChunk *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo mediaInfo;

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
}


#define MEDIAREF_CHUNK_SIZE (3 * sizeof(MlUInt))

MlBoolean MleDppOutput::writeMref(MleMediaRefChunk *chunkData)
{
    // Declare local variables.
    MleDppChunkInfo mediaInfo;
    int size;

    memset(&mediaInfo,0,sizeof(MleDppChunkInfo));

    // Create 'mref' chunk structure.
    mediaInfo.m_chunk.m_tag = mlDppMakeTag('m','r','e','f');
    mediaInfo.m_chunk.m_size = chunkData->m_size + MEDIAREF_CHUNK_SIZE;
    MLE_WARN(mlDppCreateChunk(getFp(),&mediaInfo,0));

    // Remember actual size to write.
    size = chunkData->m_size;

    if (getSwapWrite())
	{
        chunkData->m_flags = mlDppSwapInt(chunkData->m_flags);
        chunkData->m_type = mlDppSwapInt(chunkData->m_type);
        chunkData->m_size = mlDppSwapInt(chunkData->m_size);
    }

    // Write out 'mref' data.
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
}

void *
MleDppOutput::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppOutput::operator delete(void *p)
{
	mlFree(p);
}

#ifdef UNIT_TEST

// Include Digital Playprint header files.
#include "mle/Dpp.h"
#include "mle/DwpInput.h"
#include "mle/DppActorGroupOutput.h"
#include "mle/DwpFinder.h"
#include "mle/DwpGroup.h"
#include "mle/DppGroup.h"


#define TEST_DWP_FILE	"test.dwp"
#define TEST_BYTEORDER	TRUE
#define TEST_TAGS		"win32"
#define TEST_ACTORID	"actorid.h"
#define TEST_GROUPID	"groupid.h"
#define TEST_LANGUAGE	FALSE

#define TEST_BE_FILE	"testbe.dpp"
#define TEST_LE_FILE	"testle.dpp"

/*
 * This data structure is used simply to test the Playprint
 * table-of-contents.
 *
 */
typedef struct _TOCEntry {
    MlDppQuark quark;
    char      *str;
} TOCEntry;

TOCEntry TOC[] = {
  { 0, "GROUP_List" },
  { 1, "Group_1" },
  { 2, "Group_2" },
  { 3, "Group_3" }
};

#if 0
unsigned char g0[38] = {
  0x00, 0x00, 0x00, 0x26, 0x03, 0x80, 0x81, 0x82,
  0x84, 0x85, 0x04, 0x86, 0x0a, 0x00, 0x00, 0x00,
  0x83, 0x81, 0x01, 0x84, 0x03, 0x86, 0x04, 0x00,
  0x00, 0x00, 0x82, 0x81, 0x84, 0x86, 0x04, 0x00,
  0x00, 0x00, 0x82, 0x83, 0x01, 0x88
};

unsigned char g1[38] = {
  0x00, 0x00, 0x00, 0x26, 0x03, 0x80, 0x81, 0x82,
  0x84, 0x85, 0x04, 0x86, 0x0a, 0x00, 0x00, 0x00,
  0x83, 0x81, 0x01, 0x84, 0x03, 0x86, 0x04, 0x00,
  0x00, 0x00, 0x82, 0x81, 0x84, 0x86, 0x04, 0x00,
  0x00, 0x00, 0x82, 0x83, 0x01, 0x88
};

unsigned char g2[38] = {
  0x00, 0x00, 0x00, 0x26, 0x03, 0x80, 0x81, 0x82,
  0x84, 0x85, 0x04, 0x86, 0x0a, 0x00, 0x00, 0x00,
  0x83, 0x81, 0x01, 0x84, 0x03, 0x86, 0x04, 0x00,
  0x00, 0x00, 0x82, 0x81, 0x84, 0x86, 0x04, 0x00,
  0x00, 0x00, 0x82, 0x83, 0x01, 0x88
};
#endif /* 0 */

static void _printTOC(FILE *fp,MleDppChunkInfo * /* info */)
{
    /* Declare local variables. */
    MlInt i,numRead;
    MlUInt num;
    MleDppTocEntry entry;

    numRead = mlFRead(&num,sizeof(MlUInt),1,fp);
    MLE_ASSERT(numRead == 1);
    for (i = 0; i < num; i++) {
        numRead = mlFRead(&entry,sizeof(MleDppTocEntry),1,fp);
        MLE_ASSERT(numRead == 1);
        /* fprintf(stdout,"\t\tLabel: %s\n",MleQuarkToString(entry.label)); */
        fprintf(stdout,"\t\tLabel: %s\n",TOC[entry.m_label].str);
        fprintf(stdout,"\t\tOffset: %d\n",entry.m_offset);
    }
    fprintf(stdout,"\n");
}


// Create the chunks in the test workprint.
static void _createChunks()
{
    // Create a new DWP input object.
    MleDwpInput *in = new MleDwpInput;
    
    // Open the DWP.
    in->openFile(TEST_DWP_FILE);
    
    // Build the entire DWP, instantiating each MleDwpItem.
    MleDwpItem *root = MleDwpItem::readAll(in);
    
    // Create an Actor Group File output object.
    //MleDppActorGroupOutput *out = new MleDppActorGroupOutput(
	//	root, FALSE, TEST_BYTEORDER, TEST_TAGS, TEST_ACTORID, TEST_GROUPID);
	MleDppActorGroupOutput *out = new MleDppActorGroupOutput(
		root, FALSE, TEST_BYTEORDER, TEST_LANGUAGE, NULL);
	out->init(TEST_TAGS, TEST_ACTORID, TEST_GROUPID);
    
	// Set the Scalar format to Floating-point.
    out->setScalarFormat(ML_SCALAR_FLOAT);

	// Find all the MleDwpGroup items.
    MleDwpFinder groupFinder(MleDwpGroup::typeId,NULL,1);
    groupFinder.find(root);
    
    // Get the results of the find.
    int nItems = groupFinder.getNumItems();
    MleDwpItem **items = groupFinder.getItems();

	// Loop over all the groups.
    for ( int i = 0; i < nItems; i++ )
    {
        ((MleDppGroup *)items[i])->write(out);
        if (out->closeFile() == 0)
		{
			char groupChunkFile[FILENAME_MAX*2];
			sprintf(groupChunkFile, "%s.chk", items[i]->getName());

            MleDppActorGroupChunk chunk(groupChunkFile,TEST_BYTEORDER);
            int *indices = new int[chunk.countNames()];
            int *indicesPtr = indices;
            const char* names = chunk.getNames();
            if (names)
			{
                // Just for something to do, this test uses the length of the
                // media file name as the replacement index.
                do 
                    *indicesPtr++ = strlen(names);
                while ((names = chunk.nextName(names)) != NULL);
            }

            int resolvedLength;
            const MlByte* resolved = chunk.resolveNames(&resolvedLength, indices);
            for (int counter = 0; counter < resolvedLength; counter++)
			{
				printf("%02x ", resolved[counter]);
				if ((counter % 8) == 7 || counter == resolvedLength - 1)
				  printf("\n");
			}

            delete [] indices;

            //delete (unsigned char*) resolved;
			mlFree((void *)resolved);

        }
    }

	delete in;
	delete out;
}


void main(int argc,char *argv[])
{
    // Declare local variables.
    MleDppOutput *out;
    MleDppChunkTable chunkTable;
    unsigned long group0 = 0;
    unsigned long group1 = 1;
    unsigned long group2 = 2;
    //FILE *g0Fp,*g1Fp,*g2Fp;

    // Initialize Digital Playprint/Workprint toolkit.
    mleDppInit();

    // Initialize the chunk table with test data.
	_createChunks();
#if 0
    g0Fp = mlFOpen("g0","w+");
    g1Fp = mlFOpen("g1","w+");
    g2Fp = mlFOpen("g2","w+");
    mlFWrite(g0,sizeof(MlByte),38,g0Fp);
    mlFWrite(g1,sizeof(MlByte),38,g1Fp);
    mlFWrite(g2,sizeof(MlByte),38,g2Fp);
    mlFClose(g0Fp);
    mlFClose(g1Fp);
    mlFClose(g2Fp);
    chunkTable.addEntry(CHUNK_GROUP,"g0","group0",group0);
    chunkTable.addEntry(CHUNK_GROUP,"g1","group1",group1);
    chunkTable.addEntry(CHUNK_GROUP,"g2","group2",group2);
#endif /* 0 */

    // Create a new DWP input object.
    MleDwpInput *in = new MleDwpInput;
    
    // Open the DWP.
    in->openFile(TEST_DWP_FILE);
    
    // Build the entire DWP, instantiating each MleDwpItem.
    MleDwpItem *root = MleDwpItem::readAll(in);

	// Find all the MleDwpGroup items.
    MleDwpFinder groupFinder(MleDwpGroup::typeId,NULL,1);
    groupFinder.find(root);
    
    // Get the results of the find.
    int nItems = groupFinder.getNumItems();
    MleDwpItem **items = groupFinder.getItems();

	// Loop over all the groups.
	int i;
	for (i = 0; i < nItems; i++)
	{
		char groupChunkFile[FILENAME_MAX*2];
		sprintf(groupChunkFile, "%s.chk", items[i]->getName());

		if (i == 0)
			chunkTable.addEntry(CHUNK_GROUP,groupChunkFile,"teapot",i);
		else if (i == 1)
			chunkTable.addEntry(CHUNK_GROUP,groupChunkFile,"mandrill",i);
	}

#if 0
    // Test Playprint creation in Big Endian format.
    out = new MleDppOutput(TEST_BE_FILE);
#ifdef WIN32
	out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPBE);
    out->writeHeader();
    out->beginTOC(4);
    out->writeGroup(&chunkTable,group0);
    out->writeGroup(&chunkTable,group1);
    out->writeGroup(&chunkTable,group2);
    out->endTOC();
    out->end();
    delete out;
#endif /* 0 */

    // Test Playprint creation in Little Endian format.
    out = new MleDppOutput(TEST_LE_FILE);
#ifndef WIN32
    out->setSwap(TRUE);
#endif
    out->begin(MLE_DPP_CREATEPPLE);
    out->beginTOC(4);
    out->writeHeader();
#if 0
    out->writeGroup(&chunkTable,group0);
    out->writeGroup(&chunkTable,group1);
    out->writeGroup(&chunkTable,group2);
#endif /* 0 */
	for (i = 0; i < nItems; i++)
	{
		out->writeGroup(&chunkTable,i);
	}
    out->endTOC();
    out->end();
    delete out;

#if 0
    unlink("g0");
    unlink("g1");
    unlink("g2");
#endif /* 0 */
}


#endif /* UNIT_TEST */

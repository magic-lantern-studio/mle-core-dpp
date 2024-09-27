/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file genmedia.cpp
 * @ingroup MleDPPMaster
 *
 * Program to generate MediaRef chunk files.
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

// Include system header files.
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

// Include Magic Lantern header files.
#include <mle/mlMalloc.h>
#include <mle/mlFileio.h>
#include <mle/MlePath.h>
#ifdef WIN32
#include <mle/mlGetOpt.h>
#include <mle/MleWin32Path.h>
#else
#include <unistd.h>
#include <mle/MleLinuxPath.h>
#endif

// Include Digital Workprint header files.
#include <mle/Dwp.h>
#include <mle/DwpItem.h>
#include <mle/DwpInput.h>
#include <mle/DwpOutput.h>
#include <mle/DwpFinder.h>
#include <mle/DwpMediaRef.h>

// Include Digital Playprint Runtime header files.
#include "mle/mrefchk.h"

// Include Digital Playprint header files.
#include "mle/DppTblMgr.h"

#define MLE_DWP_TYPE_MEDIAREF  "MleDwpMediaRef"
#define MLE_DWP_TYPE_TARGET    "MleDwpMediaRefTarget"


// Declare external routines.
//extern MlBoolean mlVerifyTargetWorkprint(MleDwpItem *root,char *tags);
MlBoolean mlVerifyTargetWorkprint(MleDwpItem * /*root*/,char * /*tags*/)
{
    return TRUE;
}


#ifdef WIN32
static char *getCanonicalPath(char *path)
{
    char *cpath = NULL;
    MleWin32Path *wpath = new MleWin32Path((MlChar *)path, true);
    //cpath = strdup((char *)wpath->getPath());
    cpath = _strdup((char *)wpath->getPath());
    delete wpath;
    return cpath;
}
#else
static char *getCanonicalPath(char *path)
{
    return strdup(path);
}
#endif /* WIN32 */


// argument structures for parser.
typedef struct _pattern
{
    char            *str;
    struct _pattern *next;
} Pattern;

typedef struct _ArgStruct
{
    char       *commandName;  /* Name of command. */
    MlBoolean  verbose;       /* Flag indicating that command should be verbose. */
    MlBoolean  byteOrder;     /* TRUE = Little Endian, FALSE = Big Endian. */
    char       *workprint;    /* Name of workprint file to build. */
    char       *inventory;    /* Name of media inventory file to build. */
    char       *tags;         /* Digital Workprint tags. */
    char       *outputDir;    /* Directory where chunk files get built. */
} ArgStruct;


const char *usage_str = "\
Syntax:   genmedia  [-b|-l] [-d <directory>] <tags> <workprint> <media_inventory>\n\
          -b                   Use Big Endian byte ordering\n\
          -l                   Use Little Endian byte ordering\n\
          -d <directory>       Directory where chunk files are built\n\
          <tags>               Digital Workprint tags\n\
          <workprint>          Digital Workprint\n\
          <media_inventory>    Generated media inventory file\n\
\n\
Function: Generate Magic Lantern Media Reference Chunk Files.\n";


int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    /* Declare local variables. */
    int c;
    int errflg;
    extern char *optarg;
    extern int optind;

#if 0
    /* Dump arguments. */
    fprintf(stdout, "genmedia arguments:\n");
    for (int i = 0; i < argc; i++)
    {
        fprintf(stdout, "\targ[%d]: %s\n", i, argv[i]);
    }
#endif /* 0 */

    errflg = 0;
    //while ((c = getopt(argc, argv, "bld::")) != -1)
    while ((c = getopt(argc, argv, "bld:")) != -1)
    {
        switch (c)
        {
          case 'b':
            /* Use Big Endian byte ordering. */
            args->byteOrder = FALSE;
            break;
          case 'l':
            /* Use Little Endian byte ordering. */
            args->byteOrder = TRUE;
            break;
          case 'd':
            /* Place files in this directory. */
            args->outputDir = optarg;
            break;
          case '?':
            errflg++;
        }
    }

    if (errflg)
    {
        (void)fprintf(stderr, "%s\n", usage_str);
        return FALSE;
    }

    for ( ; optind < argc; optind++)
    {
        if (! args->tags) {
#ifdef WIN32
            args->tags = _strdup(argv[optind]);
#else
            args->tags = strdup(argv[optind]);
#endif /* WIN32 */
        } else if (! args->workprint)
        {
            args->workprint = getCanonicalPath(argv[optind]);
        } else if (! args->inventory)
        {
#ifdef WIN32
            args->inventory = _strdup(argv[optind]);
#else
            args->inventory = strdup(argv[optind]);
#endif /* WIN32 */
        } else
        {
            fprintf(stderr,"%s\n",usage_str);
            return FALSE;
        }
    }
    
    /* If there is no specified workprint, complain. */
    if (args->tags == NULL ||
        args->workprint == NULL ||
        args->inventory == NULL)
    {
        fprintf(stderr,"%s\n",usage_str);
        return FALSE;
    }
    
    /* Having made it to here implies that we have good arguments. */
    
    return TRUE;
}

// The global directory for generated output.
static MlePath *g_outputDir = NULL;


// Utility to append the global directory path to the specified path.
static char *_expandPath(char * path)
{
    MLE_ASSERT(path);

    MlePath *dir = g_outputDir;

    // Expand directory path, if necessary.
    char *tmpPath;
    if (dir != NULL)
    {
        tmpPath = (char *)mlMalloc(strlen((char *)dir->getPlatformPath()) + strlen(path) + 2);
        strcpy(tmpPath,(char *)dir->getPlatformPath());
#ifdef WIN32
        strcat(tmpPath,"\\");
#else /* ! WIN32 */
        strcat(tmpPath,"/");
#endif /* WIN32 */
        strcat(tmpPath,path);
    } else
        tmpPath = path;

    return tmpPath;
}


int
main(int argc,char **argv)
{
    // Declare local variables.
    ArgStruct args;
    int littleEndian = 0;
    FILE *inventoryOut = NULL;
    MleDwpInput *in;
    MleMrefChunkFile *out;

    // parse arguments
    args.commandName = argv[0];
    args.verbose = FALSE;
    args.byteOrder = FALSE;
    args.workprint = NULL;
    args.inventory = NULL;
    args.tags = NULL;
    args.outputDir = NULL;
    if (! parseArgs(argc, argv, &args))
    {
      exit(1);
    }
    
    // Redirect generated output.
#ifdef WIN32
    if (args.outputDir != NULL)
    {
        g_outputDir = new MleWin32Path((MlChar *)args.outputDir,true);
    }
#else
    if (args.outputDir != NULL)
    {
        g_outputDir = new MleLinuxPath((MlChar *)args.outputDir,true);
    }
#endif /* WIN32 */

    // Check for Little Endian  byte ordering.
    if (args.byteOrder)
        littleEndian = 1;

    // Open output file (media inventory file).
    if (args.inventory != NULL)
    {
        char * inventoryPath;
        if (g_outputDir != NULL)
            inventoryPath = _expandPath(args.inventory);
        else
            inventoryPath = args.inventory;
        inventoryOut = mlFOpen(inventoryPath, "w");

        if (g_outputDir != NULL)
            mlFree(inventoryPath);

        fprintf(inventoryOut,"/*\n * Digital Workprint: %s\n * Media Inventory\n */\n",
            args.workprint);
    } else
    {
        fprintf(stderr,"%s : Error : Could not create media inventory file %s.[c,h]\n",
               args.commandName,args.inventory);
        return 1;
    }

    // Initialize Workprint readers and writers.
    mleDwpInit();

    // Create a new DWP input item.
    in = new MleDwpInput;

    // Open the DWP.
    if (in->openFile(args.workprint) > 0)
    {
        fprintf(stderr,"%s : %s\n",args.commandName,
                "Unable to open Digital Workprint");
        exit(1);
    }

    // Build the entire DWP, instantiating each MleDwpItem.
    MleDwpItem *root;

    root = MleDwpItem::readAll(in);

    // Verify digital workprint.
    if (! mlVerifyTargetWorkprint(root,args.tags))
    {
        fprintf(stderr,"%s : Error : %s\n",args.commandName,
                "Unable to verify Digital Workprint.");
        exit(1);
    }

    // Set discriminators for finding all MediaRefs.
    char *processTags;
    char *tag,*nextTag;
    MleDwpTagAllDiscriminator *discriminator = new MleDwpTagAllDiscriminator;
#if defined(WIN32)
    processTags = _strdup(args.tags);
#else
    processTags = strdup(args.tags);
#endif
    tag = processTags;
    while (tag)
    {
        nextTag = strchr(tag,':');
        if (nextTag) *nextTag++ = '\0';
        discriminator->addTag(tag);
        tag = nextTag;
    }
    root->setDiscriminator(discriminator);
    mlFree(processTags);

    // Find all MediaRef items in the DWP.
    MleDwpFinder mediaRefFinder;
    MleDwpMediaRef **mediaRefItems;
    MleDwpMediaRefTargetMedia *entries;
    int numItems,numEntries;

    mediaRefFinder.setType(MleDwpMediaRef::typeId);
    mediaRefFinder.setFindAll(1);
    mediaRefFinder.find(root);
    mediaRefItems = (MleDwpMediaRef **)mediaRefFinder.getItems();
    numItems = mediaRefFinder.getNumItems();

    // Create Digital Playprint media chunk files
    // and media inventory file for the given target
    // MleDppMediaOutput *pp_out = new MleDppMediaOutput(args.tags,0,littleEndian);

    // Process the media references.
    MleDppTblMgr tblMgr;
    tblMgr.setDiscriminators(root, args.tags);
    if (! tblMgr.buildIndexTables(root)) {
        fprintf(stderr,"%s: Error: %s.\n", args.commandName,
                "Unable to build table of MediaRef classes.");
        return(-1);
    }
    TblMgrItems *mrefTable = tblMgr.getMediaRefTable();

    MleDwpMediaRef *nextMediaRef;
    unsigned long mrefType;

    for (int i = 0; i < numItems; i++)
    {
        MleChunkFileHeader hdrData;
        MleMediaRefInfoChunk mrefInfo;

        // Initialize chunk file header info.
        hdrData.m_id = i;
        hdrData.m_version = MleMrefChunkFile::PLAYPRINT_VERSION;
        hdrData.m_time = time(NULL);
        hdrData.m_crc = 0;

        nextMediaRef = mediaRefItems[i];

        // Build filename string.
        char *fileName = (char *) mlMalloc(strlen(nextMediaRef->getName())+5);
        strcpy(fileName,nextMediaRef->getName());
        strcat(fileName,".chk");

        // Redirect the generated output.
        char *filePath;
        if (g_outputDir != NULL)
        {
            filePath = _expandPath(fileName);
        } else
            filePath = fileName;

        // Create chunk file.
        out = new MleMrefChunkFile(filePath,MleChunkFile::WRITING);
        if (littleEndian)
        {
#if BYTE_ORDER == BIG_ENDIAN
            out->setSwap(TRUE);
#endif
            out->begin(MLE_DPP_CREATEPPLE);
        } else {
#if BYTE_ORDER == LITTLE_ENDIAN
            out->setSwap(TRUE);
#endif
            out->begin(MLE_DPP_CREATEPPBE);
        }
        out->writeHeader(&hdrData);
        out->beginList();

        // Calculate class table index.
        mrefType = 0;
        for (int k = 0; k < tblMgr.getMediaRefTableSize(); k++)
        {
            if (! strcmp(nextMediaRef->getMediaRefClass(),
                         ((MediaRefClassTableItem *)mrefTable->items[k])->name))
                break;
            else
                mrefType++;
        }

        // Retrieve target entries from MediaRef.
        numEntries = nextMediaRef->queryTargetEntries(&entries);

        // Write MediaRef 'info' chunk.
        mrefInfo.m_flags = 0;
        mrefInfo.m_type = mrefType;
        mrefInfo.m_numMrefs = numEntries;
        out->writeInfo(&mrefInfo);

        // Process target entries.
        for (int j = 0; j < numEntries; j++)
        {
            MleMediaRefChunk mrefData;

            if (entries[j].m_flags == MleMrefChunkFile::EXTERNAL)
            {
                // Process media data.
                //char *targetFilename = (char *)mlMalloc(
                //    strlen(entries[j].m_filename)+1);
                //strcpy(targetFilename,entries[j].m_filename);
#ifdef WIN32
                MleWin32Path *targetPath = new MleWin32Path((MlChar *)entries[j].m_filename,false);
#else
                MleLinuxPath *targetPath = new MleLinuxPath((MlChar *)entries[j].m_filename,false);
#endif

                mrefData.m_flags = MleMrefChunkFile::EXTERNAL;
                //mrefData.m_size = strlen(targetFilename);
                mrefData.m_size = strlen((char *)targetPath->getPlatformPath());
                mrefData.m_type = mlDppStringToTag(entries[j].m_label,0);
                //mrefData.m_data = targetFilename;
                mrefData.m_data = (char *)targetPath->getPlatformPath();

                // Write chunk data.
                out->writeData(&mrefData);

                // Clean up.
                //mlFree(targetFilename);
                delete targetPath;
            } else
            {
                printf("%s: Warning: %s, %s\n",
                       args.commandName,
                       nextMediaRef->getName(),
                       "internal media referencing is not supported in this release.");
                return 1;
            }
        }

        // Close chunk file.
        out->endList();
        out->end();
        delete out;

        // Process inventory file.
        if (inventoryOut != NULL)
            fprintf(inventoryOut,
                "\nMEDIA %s (INVENTORY NOT YET SUPPORTED)\n",
                nextMediaRef->getName());

        // Clean up.
        mlFree(fileName);
        mlFree(entries);
        if (g_outputDir != NULL)
        {
            mlFree(filePath);
        }
    }

    // Close the DWP.
    in->closeFile();

    // Close the inventory file.
    if (inventoryOut != NULL)
        mlFClose(inventoryOut);

    // Cleanup and exit.
    delete g_outputDir;

    return 0;
}

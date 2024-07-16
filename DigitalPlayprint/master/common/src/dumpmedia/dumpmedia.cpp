/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file dumpmedia.cpp
 * @ingroup MleDPPMaster
 *
 * Program to dump MediaRef chunk files.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2015-2020 Wizzer Works
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
//  For information concerning this source code, contact Mark S. Millard,
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
#include <time.h>

// Include Magic Lantern header files.
#ifdef WIN32
#include "mle/mlGetOpt.h"
#endif
#include "mle/mlMalloc.h"

// Include Digital Playprint header files.
//#include "mle/mediaref.h"
#include "mle/mrefchk.h"


// Argument structures for parser.
typedef struct _pattern
{
    char            *str;
    struct _pattern *next;
} Pattern;

typedef struct _ArgStruct
{
    char       *commandName;  /* Name of command. */
    Pattern    *chunkFiles;   /* Chunk files. */
} ArgStruct;


const char *usage_str = "\
Syntax:   dumpmedia <chunk file> ...\n\
          <chunk file>          MediaRef Chunk File\n\
\n\
Function: Dump Magic Lantern MediaRef Chunk Files.\n";


int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    /* Declare local variables. */
    int c;
    int errflg;
    Pattern *nextPattern,**nextFile;
    extern char *optarg;
    extern int optind;
    
    errflg = 0;
    while ((c = getopt(argc, argv, "?")) != -1)
	{
        switch (c)
		{
          case '?':
            errflg++;
        }
    }

    if (errflg)
	{
        (void)fprintf(stderr, "%s\n", usage_str);
        return FALSE;
    }

    nextFile = &args->chunkFiles;
    for ( ; optind < argc; optind++)
	{
        nextPattern = (Pattern *)mlMalloc(sizeof(Pattern));
#if defined(WIN32)
		nextPattern->str = _strdup(argv[optind]);
#else
        nextPattern->str = strdup(argv[optind]);
#endif
        nextPattern->next = NULL;

        *nextFile = nextPattern;
        nextFile = &nextPattern->next;
    }
    
    /* If there is no specified chunk file, complain. */
    if (args->chunkFiles == NULL)
	{
        fprintf(stderr,"%s\n",usage_str);
        return FALSE;
    }
    
    /* Having made it to here implies that we have good arguments. */
    
    return TRUE;
}


int main(int argc,char *argv[])
{
    // Declare local variables.
    ArgStruct args;
    Pattern *nextFile;
    MleMrefChunkFile *in;
    MleMediaRefChunk mrefData;
    MleMediaRefInfoChunk mrefInfoData;
    MleChunkFileHeader hdrData;
   
    // Parse arguments.
    args.commandName = argv[0];
    args.chunkFiles = NULL;
    parseArgs(argc,argv,&args);

    nextFile = args.chunkFiles;
    while (nextFile)
	{
        // Open the chunk file.
        in = new MleMrefChunkFile(nextFile->str,MleChunkFile::READING);
        if (! in)
		{
            fprintf(stderr,"Unable To Open MediaRef Chunk File: %s\n",
                    nextFile->str);
        } else
		{
            // Verify that the file is a valid MediaRef chunk file.

            fprintf(stdout,"Dumping MediaRef Chunk File: %s\n\n",
                    nextFile->str);

            memset(&hdrData,0,sizeof(MleChunkFileHeader));
            memset(&mrefInfoData,0,sizeof(MleMediaRefInfoChunk));

            if (mlDppCheckFormat(in->getFp()) == 2)
			{
                fprintf(stdout,"%s: %s\n\n",
                        "File Format","Little Endian");
#if BYTE_ORDER == BIG_ENDIAN
                in->setSwap(TRUE);
#endif
                in->begin(MLE_DPP_FINDPPLE);
            } else {
                fprintf(stdout,"%s: %s\n\n",
                        "File Format","Big Endian");
#if BYTE_ORDER == LITTLE_ENDIAN
                in->setSwap(TRUE);
#endif
                in->begin(MLE_DPP_FINDPPBE);
            }
            in->readHeader(&hdrData);
            in->beginList();
            in->readInfo(&mrefInfoData);

            fprintf(stdout,"%s: %d\n",
                    "Chunk File ID",hdrData.m_id);
            fprintf(stdout,"%s: %d\n",
                    "Chunk File Version",hdrData.m_version);
            fprintf(stdout,"%s: %s",
                    "Chunk File Creation Date",ctime((time_t *)&hdrData.m_time));
            fprintf(stdout,"%s: %d\n",
                    "Chunk File CRC",hdrData.m_crc);
            fprintf(stdout,"\n");
            fprintf(stdout,"%s: %d\n",
                    "Flag Data",mrefInfoData.m_flags);
            fprintf(stdout,"%s: %d\n",
                    "Type Data",mrefInfoData.m_type);
            fprintf(stdout,"%s: %d\n",
                    "Number of MediaRefs",mrefInfoData.m_numMrefs);

            // Read chunk file data.
            for (unsigned int i = 0; i < mrefInfoData.m_numMrefs; i++)
			{
                memset(&mrefData,0,sizeof(MleMediaRefChunk));

                in->readData(&mrefData);

                fprintf(stdout,"\t%s: %s\n",
                        "Media Type",mlDppTagToString(mrefData.m_type));

                if (mrefData.m_flags == MleMrefChunkFile::EXTERNAL)
				{
                    fprintf(stdout,"\t%s: %s\n",
                            "External Media Reference",(char *)mrefData.m_data);
                } else if (mrefData.m_flags == MleMrefChunkFile::INTERNAL)
				{
                    fprintf(stdout,"\t%s: %s\n",
                            "Internal Media Reference","Data Unknown");
                } else {
                    fprintf(stdout,"\t%s: type %d\n",
                            "Unknown Media Reference",mrefData.m_flags);
                }
            }

            // Clean up.
            in->endList();
            in->end();
            delete in;
        }

        // Process next chunk file.
        nextFile = nextFile->next;

        if (nextFile)
            fprintf(stdout,
                    "\n==================================================\n\n");
    }
}

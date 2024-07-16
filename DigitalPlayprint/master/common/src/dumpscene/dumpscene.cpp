/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file dumpscene.cpp
 * @ingroup MleDPPMaster
 *
 * Program to dump Scene chunk files.
 *
 * @author Mark S. Millard
 * @date November 30, 2007
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
#include "mle/mlFileio.h"
#include "mle/mlTypes.h"

// Include Digital Playprint header files.

// Forward declarations.
static unsigned char *readFileToMemory(char *filename, int *size);
static void parseStream(unsigned char*& contents, int size);
static int readInt(unsigned char*& contents);
static unsigned char nextStreamByte(unsigned char*& streamPtrRef);


// Argument structures for parser.
typedef struct _pattern
{
    char            *str;
    struct _pattern *next;
} Pattern;

typedef struct _ArgStruct
{
    char       *commandName;  /* Name of command. */
	MlBoolean  byteOrder;     /* TRUE = Little Endian, FALSE = Big Endian. */
    Pattern    *chunkFiles;   /* Chunk files. */
} ArgStruct;


// Usage string declaration.
const char *usage_str = "\
Syntax:   dumpscene [-b|-l] <chunk file> ...\n\
          -b            Use Big Endian byte ordering (default)\n\
          -l            Use Little Endian byte ordering\n\
          <chunk file>  Scene Chunk File\n\
\n\
Function: Dump Magic Lantern Scene Chunk Files.\n";


// Flag indicating whether to process chunks using Big/Little Endian formatting.
static MlBoolean g_littleEndian = FALSE;


// parsArgs - Parse the command-line arguments.
int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    // Declare local variables.
    int c;
    int errflg;
    Pattern *nextPattern,**nextFile;
    extern char *optarg;
    extern int optind;
    
    errflg = 0;
    while ((c = getopt(argc, argv, "bl?")) != -1)
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
    
    // If there is no specified chunk file, complain.
    if (args->chunkFiles == NULL)
	{
        fprintf(stderr,"%s\n",usage_str);
        return FALSE;
    }
    
    // Having made it to here implies that we have good arguments.
    
    return TRUE;
}

// readFileToMemory - read the entire chunk file into memory.
//     filename == NULL read from stdin
//     filename == "-" read from stdin

#define READ_CHUNK_SIZE 4096

static unsigned char *readFileToMemory(char *filename, int *size)
{
    FILE *ifd;
    int closeMe;
    unsigned char *data;
    int length, used, allocked;

    if ( filename == NULL || strcmp(filename,"-") == 0 )
	{
		ifd = stdin;
		closeMe = 0;
    } else
	{
		if ( (ifd = fopen(filename, "rb")) == NULL )
		{
			return NULL;
		}
		closeMe = 1;
    }

    data = (unsigned char *)mlMalloc(READ_CHUNK_SIZE);
    allocked = READ_CHUNK_SIZE;
    used = 0;
    while((length = mlFRead(data+used,sizeof(char),READ_CHUNK_SIZE,ifd)) > 0)
	{
		used += length;
		allocked += READ_CHUNK_SIZE;
		data = (unsigned char *)mlRealloc(data, allocked);
    }

    if ( closeMe )
	{
		fclose(ifd);
    }

    if ( used != allocked )
	{
		data = (unsigned char *)mlRealloc(data, used);
    }

    if ( size != NULL )
	{
		*size = used;
    }

    return data;
}


// nextStreamByte - Retrieve the next byte from the specified array.
static unsigned char nextStreamByte(unsigned char*& streamPtrRef)
{
	return *streamPtrRef++;
}


// readInt - Read an integer from the specified array.
static int readInt(unsigned char*& streamPtrRef)
{
    long value = 0;
    if (g_littleEndian)
	{
		// Little Endian format.
		value = nextStreamByte(streamPtrRef);
		value = value | (nextStreamByte(streamPtrRef) << 8);
		value = value | (nextStreamByte(streamPtrRef) << 16);
		value = value | (nextStreamByte(streamPtrRef) << 24);
    } else
	{
		// Big Endian format.
		//value = nextStreamByte(streamPtrRef);
		//value = (value<<8) | nextStreamByte(streamPtrRef);
		//value = (value<<8) | nextStreamByte(streamPtrRef);
		//value = (value<<8) | nextStreamByte(streamPtrRef);
		value = nextStreamByte(streamPtrRef);
		value = (value>>8) | nextStreamByte(streamPtrRef);
		value = (value>>8) | nextStreamByte(streamPtrRef);
		value = (value>>8) | nextStreamByte(streamPtrRef);
    }

    return value;
}

// parseStream - Parse the specified buffer, dumping the contents
// to stdout.
static void parseStream(unsigned char*& contents, int size)
{
    int index;
    unsigned char *contentsEnd = contents + size;

	int offset = readInt(contents);
	printf("Group Name Offset (offset=%d)\n", offset);

	int sceneId = readInt(contents);
	printf("Scene Id: %d\n", sceneId);

	long numGroups = readInt(contents);
	printf("Number of Groups: %ld\n\n", numGroups);
    
    while ( contents < contentsEnd )
	{
		index = readInt(contents);
		printf("\tGroup %d\n", index);
	}
}

// The main entry point for dumping the Scene chunk.
int main(int argc,char *argv[])
{
    // Declare local variables.
    ArgStruct args;
    Pattern *nextFile;
    unsigned char *contents;
    int size;
   
    // Parse arguments.
    args.commandName = argv[0];
	args.byteOrder = FALSE;
    args.chunkFiles = NULL;
    parseArgs(argc,argv,&args);

	if (args.byteOrder == FALSE)
		g_littleEndian = FALSE;
	else
	    g_littleEndian = TRUE;

    nextFile = args.chunkFiles;
    while (nextFile)
	{
		fprintf(stdout, "Contents of %s\n\n", nextFile->str);

        // Open the chunk file.
		contents = readFileToMemory(nextFile->str, &size);

		// Dump the contents.
		parseStream(contents, size);

        // Process next chunk file.
        nextFile = nextFile->next;

        if (nextFile)
            fprintf(stdout,
                    "\n==================================================\n\n");
    }
}

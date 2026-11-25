/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file dumpgroup.cpp
 * @ingroup MleDPPMaster
 *
 * Program to dump Group chunk files.
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
//  For information concerning this source code, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

// Include system header files.
#ifdef _WINDOWS
#include <windows.h>
#else
#ifdef __linux__
#include <string.h>
#else
#include <strings.h>
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Inclclude Magic Lantern header files.
#ifdef _WINDOWS
#include "mle/mlGetOpt.h"
#else
#include <unistd.h>
#endif
#include "mle/mlMalloc.h"
#include "mle/mlFileio.h"
#include "mle/mlAssert.h"
#include "mle/mlTypes.h"

// Include Digital Playprint header files.
#include "mle/agopcode.h"

static unsigned char *readFileToMemory(char *filename, int *size);
static void parseStream(unsigned char*& contents, int size);
static long readIndex(unsigned char*& contents);
static int readInt(unsigned char*& contents);
static int writeInt(long value);
static int copyInt(unsigned char*& contents);
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
    int        requested;     /* 0 = from the marker, 1 = Big Endian, 2 = Little Endian. */
	MlBoolean  language;      /* TRUE = Java, FALSE = C++. */
    int        verbose;       /* Be verbose. */
	char       *chunkFile;    /* Group chunk file to dump. */
} ArgStruct;

static MlBoolean g_bigEndian;

// The number of errors found in the chunk file.
static int g_errors = 0;

// The end of the chunk in memory, and whether a read went past it. Every byte
// is read with nextStreamByte(), which does not read past the end.
static unsigned char *g_end = NULL;
static int g_truncated = 0;

const char *usage_str = "\
Syntax:   dumpgroup  [-b|-l] [-v] <group_chunk_file>\n\
\n\
          -b                   Require Big Endian byte ordering\n\
          -l                   Require Little Endian byte ordering\n\
          -v                   Be verbose\n\
          <group_chunk_file>   Generated Group chunk file (.chk)\n\
\n\
Function: Dump Magic Lantern Actor Group Chunk Files. The byte order is\n\
          read from the marker at the start of the file; -b or -l makes\n\
          it an error if the file is in the other byte order.\n\
\n";

int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    /* declare local variables */
    int c;
    int errflg;
    extern char *optarg;
    extern int optind;
    
    errflg = 0;
    while ((c = getopt(argc, argv, (char *)"blv")) != -1)
	{
		switch (c)
		{
		  case 'b':
			/* Require Big Endian byte ordering. */
			args->requested = 1;
			break;
		  case 'l':
			/* Require Little Endian byte ordering. */
			args->requested = 2;
			break;
		  case 'v':
			args->verbose = TRUE;
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
		if (! args->chunkFile)
		{
			args->chunkFile = strdup(argv[optind]);
		} else {
			fprintf(stderr,"%s\n",usage_str);
			return FALSE;
		}
	}
    
    /* If there is no specified chunk file, complain. */
    if (args->chunkFile == NULL)
	{
		fprintf(stderr,"%s\n",usage_str);
		return FALSE;
    }
    
    /* Having made it to here implies that we have good arguments. */
    
    return TRUE;
}

int main(int argc, char **argv)
{
    unsigned char *contents;
    int size;
	ArgStruct args;

    // Parse arguments.
    args.commandName = argv[0];
    args.requested = 0;
    args.verbose = FALSE;
    args.chunkFile = NULL;
    if (! parseArgs(argc, argv, &args))
	{
      exit(1);
    }

    contents = readFileToMemory(args.chunkFile, &size);
    if (contents == NULL || size == 0)
    {
        fprintf(stderr, "%s : Unable to read chunk file %s\n",
                args.commandName, args.chunkFile ? args.chunkFile : "(stdin)");
        exit(1);
    }

    // The file starts with a marker that gives its byte order, then the
    // offset of the media names, in that byte order.
    int order = size >= MLE_DPP_CHUNKFILE_HEADER_SIZE ? mleDppChunkFileOrder(contents) : 0;
    if (order == 0)
    {
        fprintf(stderr, "%s : Not an Actor Group chunk file (no byte order marker): %s\n",
                args.commandName, args.chunkFile ? args.chunkFile : "(stdin)");
        exit(1);
    }
    if (args.requested != 0 && args.requested != order)
    {
        fprintf(stderr, "%s : Actor Group chunk file is %s Endian, not %s Endian: %s\n",
                args.commandName, order == 1 ? "Big" : "Little",
                order == 1 ? "Little" : "Big", args.chunkFile ? args.chunkFile : "(stdin)");
        exit(1);
    }
    g_bigEndian = (order == 1);

    unsigned char *stream = contents + MLE_DPP_CHUNKFILE_MARKER_SIZE;
    parseStream(stream, size - MLE_DPP_CHUNKFILE_MARKER_SIZE);
    return g_errors > 0 ? 1 : 0;
}

/*
 * readFileToMemory - read the entire script file into memory
 *    filename == NULL read from stdin
 *    filename == "-" read from stdin
 */

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


static void parseStream(unsigned char*& contents, int size)
{
    long propertyLength = 0;
    int currentActor, currentRole, currentSet, opcode, index;
    unsigned char *contentsEnd = contents+size;
    
    g_end = contentsEnd;
    g_truncated = 0;

    currentActor = -1;
    currentRole = -1;
    currentSet = -1;

	int offset = readInt(contents);
	printf("Media Reference Offset (offset=%d)\n",offset);

	int groupId = readInt(contents);
	printf("Group Id: %d\n",groupId);

	long numActors = readIndex(contents);
	printf("Number of Actors: %ld\n\n",numActors);
    
    while ( contents < contentsEnd && ! g_truncated )
	{
		opcode = (int)*contents++;
		switch (opcode)
		{
		  case setSetOpcode:
			if (currentActor)
			{
				currentActor = -1;
				currentRole = -1;
			}
			currentSet = readIndex(contents);
			printf("%04x %03d SetSet set=%d\n",
			   opcode, currentSet,currentSet);
			break;

		  case createActorOpcode:
			currentActor = readIndex(contents);
			printf("%04x %03d CreateActor actor=%d\n",
			   opcode, currentActor, currentActor);
			break;

		  case bindRoleOpcode:
			index = readIndex(contents);
			printf("%04x %03d BindRole role=%d, # actor=%d\n",
			   opcode, index, index, currentActor);
			if ( currentActor == -1 )
			{
				printf("         # ERROR: currentActor undefined.\n");
				g_errors++;
			}
			if ( currentRole != -1 )
			{
				printf("         # ERROR: currentRole already defined (%d)\n",
				   currentRole);
				g_errors++;
			}
			currentRole = index;
			break;

		  case parentRoleOpcode:
			index = readIndex(contents);
			printf("%04x %03d ParentRole role=%d\n",
			   opcode, index, index);

			// XXX At the moment, we always generate a role attachment,
			//   even when no role is present.  This is tested and ignored
			//   in actorgc.cxx.  This is slightly wasteful, but we'll fix
			//   it in the playprint generation code rewrite.  In the meantime,
			//   don't print this error.
	//	    if (currentRole == -1) {
	//		printf("         # ERROR: currentRole undefined.\n");
	//	    }
			if (currentSet == -1)
			{
				printf("         # ERROR: currentSet undefined.\n");
				g_errors++;
			}
			currentRole = -1;
			break;

		  case childRoleOpcode:
			index = readIndex(contents);
			printf("%04x %03d ChildRole role=%d\n",
			   opcode, index, index);
			break;

		  case setPropertyOffsetOpcode:
			index = readIndex(contents);
			printf("%04x %03d SetProperty index=%d\n",
			   opcode, index, index);
			break;

		  case setPropertyLengthOpcode:
			propertyLength = readIndex(contents);
			printf("%04x %03ld SetPropertyLength length=%ld\n",
			   opcode, propertyLength, propertyLength );
			break;

		  case copyPropertyOpcode:
			printf("%04x     CopyProperty", opcode);
			long bytesToCopy;
			if ( propertyLength < 0 || propertyLength > contentsEnd - contents )
				g_truncated = 1;
			for ( bytesToCopy=0 ; bytesToCopy<propertyLength && ! g_truncated ; bytesToCopy++)
			{
				if ( bytesToCopy%8 == 0 )
				{
					printf("\n         Data");
				}
				printf(" %02x",nextStreamByte(contents));
			}
			printf("\n");
			break;

		  case copyArrayPropertyOpcode:
			{
				printf("%04x     CopyArrayProperty", opcode);
				int numElements = copyInt(contents);
				long arrayLength = propertyLength * (long)numElements;
				if ( numElements < 0 || arrayLength < 0 || arrayLength > contentsEnd - contents )
					g_truncated = 1;
				for ( bytesToCopy=0 ; bytesToCopy<arrayLength && ! g_truncated ; bytesToCopy++)
				{
					if ( bytesToCopy%8 == 0 )
					{
						printf("\n         Data");
					}
					printf(" %02x",nextStreamByte(contents));
				}
				printf("\n");
			}
			break;

		  case endActorGroupOpcode:
			printf("%04x     EndActorGroup\n", opcode);
			contents = contentsEnd;
			break;

		  case copyDynamicPropertyOpcode:
			{
				char charbuf[9];
				char nb;
				int i;
				int length = copyInt(contents);
				printf("%04x %03d CopyDynamicProperty\n", opcode, length);
				if ( length < 0 || length > contentsEnd - contents )
					g_truncated = 1;
				for ( i=0 ; i<length && ! g_truncated ; i++ )
				{
					if ( i%8 == 0 )
					{
						printf("         Data");
					}
					nb = nextStreamByte(contents);
					printf(" %02x", nb);
					if ( isprint((unsigned char)nb) )
					{
						charbuf[i%8] = nb;
					} else {
						charbuf[i%8] = '.';
					}
					if ( i%8 == 7 )
					{
						charbuf[8] = 0;
						printf("    # \"%s\"\n", charbuf);
						sprintf(charbuf, "%*.*s", (int)(sizeof charbuf-1), (int)(sizeof charbuf-1), " ");
					}
				}
				if ( i%8 != 0 )
				{
					charbuf[i%8] = 0;
					for ( ; i%8!=0 ; i++ )
					{
						printf("   ");
					}
					printf("    # \"%s\"\n", charbuf);
				}
			}
			break;

		  case copyMediaRefOpcode:
			index = readIndex(contents);
			printf("%04x %03d CopyMediaRef mediaref=%d\n",
			   opcode, index, index);
			break;

		  case createSetOpcode:
			{
				long set = readIndex(contents);
				printf("%04x %03ld CreateSet set=%ld\n", opcode, set, set);
			}
			break;

		  case endSetOpcode:
			printf("%04x     EndSet\n", opcode);
	//	    contents = contentsEnd;
			break;

		  default:
			printf("         # ERROR: invalid opcode(%d)\n",opcode);
			g_errors++;
		}
    }

    if ( g_truncated )
	{
		printf("         # ERROR: the chunk ends in the middle of an item\n");
		g_errors++;
	}
}

static long readIndex(unsigned char*& contents)
{
	long value = 0;
	signed char byte;
	
	while (! g_truncated && (byte = nextStreamByte(contents)) >= 0)
	{
		value = (value << valueBits) | byte;
	}
	if (! g_truncated)
		contents--; /* Back up one, since while loop always over-advances by one. */
	return value;
}

static unsigned char nextStreamByte(unsigned char*& streamPtrRef)
{
	if (streamPtrRef >= g_end)
	{
		g_truncated = 1;
		return 0;
	}
	return *streamPtrRef++;
}

static int readInt(unsigned char*& streamPtrRef)
{
    long value = 0;

	if (! g_bigEndian)
	{
		// Little Endian format.
		value = nextStreamByte(streamPtrRef);
		value = value | (nextStreamByte(streamPtrRef) << 8);
		value = value | (nextStreamByte(streamPtrRef) << 16);
		value = value | (nextStreamByte(streamPtrRef) << 24);
	} else
	{
		// Big Endian format.
		value = nextStreamByte(streamPtrRef);
		value = (value<<8) | nextStreamByte(streamPtrRef);
		value = (value<<8) | nextStreamByte(streamPtrRef);
		value = (value<<8) | nextStreamByte(streamPtrRef);
	}

    return value;
}

static int writeInt(long index)
{
	int value = 0;
	unsigned char tmpValue =0;

    if (! g_bigEndian)
	{
		// Little Endian format.
		tmpValue = (index) & 0xff;
		value |= tmpValue;
		tmpValue = (index>>8) & 0xff;
		value |= tmpValue;
		tmpValue = (index>>16) & 0xff;
		value |= tmpValue;
		tmpValue = (index>>24) & 0xff;
		value |= tmpValue;
	} else
	{
		// Big Endian format.
		tmpValue = (index>>24) & 0xff;
		value = tmpValue;
		tmpValue = (index>>16) & 0xff;
		value = (value<<8) | tmpValue;
		tmpValue = (index>>8) & 0xff;
		value = (value<<8) | tmpValue;
		tmpValue = (index) & 0xff;
		value = (value<<8) | tmpValue;
	}

    return value;
}

static int copyInt(unsigned char*& streamPtrRef)
{
	long strmValue = readInt(streamPtrRef);
	return writeInt(strmValue);
};

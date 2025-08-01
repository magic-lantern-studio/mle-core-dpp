/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file dumpdpp.cpp
 * @ingroup MleDPPMaster
 *
 * Program to dump Digital Playprint files.
 */


// COPYRIGHT_BEGIN
//
// Copyright (c) 2017-2025 Wizzer Works
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
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

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
#include "mle/ppinput.h"

static int dumpBytes(const char *buffer, size_t size);
static int dumpHeader(void *clientdata, void *calldata);
static int dumpTOC(void *clientdata, void *calldata);
static int dumpGroup(void *clientdata, void *calldata);
static int dumpMref(void *clientdata, void *calldata);
static int readDpp(char *filename);

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
    MlBoolean  byteOrder;     /* TRUE = Little Endian, FALSE = Big Endian. */
    MlBoolean  language;      /* TRUE = Java, FALSE = C++. */
    int        verbose;       /* Be verbose. */
    char       *playprint;    /* Group chunk file to dump. */
} ArgStruct;

static MlBoolean g_bigEndian;

const char *usage_str = "\
Syntax:   dumpdpp  [-b|-l] [-v] <playprint_file>\n\
\n\
          -b                   Use Big Endian byte ordering (default)\n\
          -l                   Use Little Endian byte ordering\n\
          -v                   Be verbose\n\
          <playprint_file>     Generated Digital Playprint file (.dpp)\n\
\n\
Function: Dump Magic Lantern Digital Playprint Files.\n\
\n";

int parseArgs(int argc, char *argv[], ArgStruct *args)
{
    /* declare local variables */
    int c;
    int errflg;
    extern char *optarg;
    extern int optind;
    
    errflg = 0;
    while ((c = getopt(argc, argv, "blv")) != -1)
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
        if (! args->playprint)
        {
            args->playprint = strdup(argv[optind]);
        } else {
            fprintf(stderr,"%s\n",usage_str);
            return FALSE;
        }
    }
    
    /* If there is no specified playprint, complain. */
    if (args->playprint == NULL)
    {
        fprintf(stderr,"%s\n",usage_str);
        return FALSE;
    }
    
    /* Having made it to here implies that we have good arguments. */
    
    return TRUE;
}

int main(int argc, char **argv)
{
    int retValue = -1;
    unsigned char *contents;
    int size;
    ArgStruct args;

    // Parse arguments.
    args.commandName = argv[0];
    args.byteOrder = TRUE;
    args.verbose = FALSE;
    args.playprint = NULL;
    if (! parseArgs(argc, argv, &args))
    {
        exit(1);
    }

    if (! args.byteOrder)
        g_bigEndian = TRUE;
    else
        g_bigEndian = FALSE;

    //contents = readFileToMemory(args.playprint, &size);
    //parseStream(contents, size);

    // Todo: Validate that the args.playprint exists and is readable.
    // Also, expand the filename into an absolute path.
    retValue = readDpp(args.playprint);

    return retValue;
}

static char const _hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B','C','D','E','F'};

static std::string byte_2_str(const char* bytes, size_t size)
{
    std::string str;
    for (int i = 0; i < size; ++i)
    {
        const char ch = bytes[i];
        str.append(&_hex[(ch  & 0xF0) >> 4], 1);
        str.append(&_hex[ch & 0xF], 1);
        str.append(" ", 1);
    }
    return str;
}

// Print the byte array, 'buffer', as a string in hexadecimal format.
// 'size' is the number of bytes to display.
static int dumpBytes(const char *buffer, size_t size)
{
    std::string str = byte_2_str(buffer, size);
	const char *cstr = str.c_str();
	fprintf(stdout, "%s\n", cstr);
	return TRUE;
}

static int dumpHeader(void *clientdata, void *calldata)
{
	fprintf(stdout, "Digital Playprint Header\n");

	MleDppHeader *header = (MleDppHeader *) calldata;

	fprintf(stdout,"\t%s: %d\n","Version", header->m_version);

	char *datestamp = ctime((time_t *)&header->m_date);
	fprintf(stdout,"\t%s: %s\n","Creation Date", datestamp);

	return TRUE;
}

static int dumpTOC(void *clientdata, void *calldata)
{
	fprintf(stdout, "Digital Playprint Table of Contents\n");

	MleDppTOC *toc = (MleDppTOC *) calldata;
	MleDppTOC *nextEntry = toc;
	while (nextEntry != NULL)
	{
		// Process TOC entry.
		MlDppQuark label = nextEntry->m_entry.m_label;
		unsigned long offset = nextEntry->m_entry.m_offset;
		fprintf(stdout, "\tProcessing next entry: quark = %u: offset = %lu.\n", label, offset);
	    nextEntry = nextEntry->m_next;
	}
	fprintf(stdout, "\n");

	return TRUE;
}

static int dumpGroup(void *clientdata, void *calldata)
{
    char *group = (char *)calldata;
    if (group == NULL)
    {
        //mlSetErrno(MLE_DPP_ERR_CANNOTREAD);
        return(FALSE);
    }

    fprintf(stdout, "Actor Group Chunk\n");
    char *p = group;
    // The 'grp' chunk payload (not including tag and size) is always 12 bytes long.
    dumpBytes(p, 12);

    // Todo: interpret what these 12 bytes hold.

    return TRUE;
}

static int dumpList(void *clientdata, void *calldata)
{
	return TRUE;
}

static int dumpMref(void *clientdata, void *calldata)
{
	fprintf(stdout, "Media Reference Chunk\n");
	return TRUE;
}

int readDpp(char *playprint)
{
    MleDppInput *dpp = new MleDppInput(playprint);
    dpp->begin(MLE_DPP_FINDPPLE);
    dpp->readHeader(dumpHeader, NULL);
    dpp->readTOC(dumpTOC, NULL);
    dpp->readGroup(dumpGroup, NULL);
    // How do we know how many groups to read from playprint?
    //dpp->readList(dumpList, NULL);
    //dpp->readMref(dumpMref, NULL);
    dpp->end();
    delete dpp;
    return 0;
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
    
    currentActor = -1;
    currentRole = -1;
    currentSet = -1;

    int offset = readInt(contents);
    printf("Media Reference Offset (offset=%d)\n",offset);

    int groupId = readInt(contents);
    printf("Group Id: %d\n",groupId);

    long numActors = readIndex(contents);
    printf("Number of Actors: %ld\n\n",numActors);
    
    while ( contents < contentsEnd )
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
            }
            if ( currentRole != -1 )
            {
                printf("         # ERROR: currentRole already defined (%d)\n",
                   currentRole);
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
    //        if (currentRole == -1) {
    //        printf("         # ERROR: currentRole undefined.\n");
    //        }
            if (currentSet == -1)
            {
                printf("         # ERROR: currentSet undefined.\n");
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
            for ( bytesToCopy=0 ; bytesToCopy<propertyLength ; bytesToCopy++)
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
                int arrayLength = propertyLength * numElements;
                for ( bytesToCopy=0 ; bytesToCopy<arrayLength ; bytesToCopy++)
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
                for ( i=0 ; i<length ; i++ )
                {
                    if ( i%8 == 0 )
                    {
                        printf("         Data");
                    }
                    nb = nextStreamByte(contents);
                    printf(" %02x", nb);
                    if ( isprint(nb) )
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
    //        contents = contentsEnd;
            break;

          default:
            printf("         # ERROR: invalid opcode(%d)\n",opcode);
        }
    }
}

static long readIndex(unsigned char*& contents)
{
    long value = 0;
    signed char byte;
    
    while ((byte = nextStreamByte(contents)) >= 0)
    {
        value = (value << valueBits) | byte;
    }
    contents--; /* Back up one, since while loop always over-advances by one. */
    return value;
}

static unsigned char nextStreamByte(unsigned char*& streamPtrRef)
{
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

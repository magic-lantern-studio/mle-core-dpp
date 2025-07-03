/** @defgroup MleDPPBase Magic Lantern Digital Workprint Library API - Base */

/**
 * @file DppActorGroupOutput.cpp
 * @ingroup MleDPPBase
 *
 * This file implements the Actor Group Context Output used by the
 * Magic Lantern Digital Playprint Library API.
 */

// COPYRIGHT_BEGIN
//
// The MIT License (MIT)
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
#include <stdio.h>
#include <string.h>
#include <time.h>

// Include Magic Lantern header files.
#include <mle/mlAssert.h>
#include <mle/mlFileio.h>
#include <mle/mlMalloc.h>
#include <mle/mlErrno.h>
#include <mle/mlDebug.h>
#ifdef _WINDOWS
#include <mle/MleWin32Path.h>
#else
#include <mle/MleLinuxPath.h>
#endif

// Include Magic Lantern Digital Workprint header files.
#include "mle/DwpItem.h"
#include "mle/DwpDataUnion.h"
#include "mle/DwpMediaRef.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/DppActorGroupOutput.h"
#include "mle/DppTblMgr.h"


// The global directory for generated output.
MlePath *MleDppActorGroupOutput::g_outputDir = NULL;
// Be verbose.
MlBoolean MleDppActorGroupOutput::g_verbose = FALSE;


#ifdef _WINDOWS
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
#endif /* _WINDOWS */


// Utility to append the global directory path to the specified path.
static char *_expandPath(char * path)
{
    MLE_ASSERT(path);

    MlePath *dir = MleDppActorGroupOutput::getOutputDirectory();

    // Expand directory path, if necessary.
    char *tmpPath;
    if (dir != NULL)
    {
        tmpPath = (char *)mlMalloc(strlen((char *)dir->getPlatformPath()) + strlen(path) + 2);
        strcpy(tmpPath,(char *)dir->getPlatformPath());
#ifdef _WINDOWS
        strcat(tmpPath,"\\");
#else /* ! _WINDOWS */
        strcat(tmpPath,"/");
#endif /* _WINDOWS */
        strcat(tmpPath,path);
    } else
        tmpPath = path;

    return tmpPath;
}


MleDppActorGroupOutput::MleDppActorGroupOutput(MleDwpItem *root, char isFixed,
    char isLittle,
    char language,
    char *package)
  : m_fp(NULL), m_curGroupChunkFile(NULL), m_language(language),
    m_curSetChunkFile(NULL), m_convertFloatToFixed(isFixed),
    m_littleEndian(isLittle), m_lastOffset(-1), m_lastLength(0),
    m_currentActorClass(NULL), m_currentSetClass(NULL),
    m_currentProperty(NULL), m_references(NULL),
    m_numReferences(0), m_actorCount(0), m_actorCountCumulative(0),
    m_actorid(NULL), m_groupid(NULL), m_package(NULL), m_root(root)
{
    if (m_language)
    {
        // Set the Java package namespace.
#ifdef _WINDOWS
        m_package = _strdup(package);
#else
        m_package = strdup(package);
#endif
    }
}

MlBoolean
MleDppActorGroupOutput::init(
    char *tags,
    char *actoridFileName,
    char *groupidFileName)
{
    // For now set the format of scalar to be default (16.16),
    // but in the future set it based on user input to the DPP generator.

    if (g_verbose)
    {
        fprintf(stdout,"Generating %s\n",actoridFileName);
        fprintf(stdout,"Generating %s\n",groupidFileName);
    }

    setScalarFormat();

    // Build the tables to process.
    m_table = new MleDppTblMgr;
    m_table->setDiscriminators(m_root, tags);
    m_table->buildIndexTables(m_root);

    // Open the ActorId file.
    char *actoridPath = _expandPath(actoridFileName);
    //char *actoridPath = getCanonicalPath(tmpActoridPath);
    if ((m_actorid = mlFOpen(actoridPath, "w+")) == NULL)
    {
        fprintf(stderr,"Unable to open %s: %d\n",actoridPath,errno);
        if (g_outputDir != NULL)
            mlFree(actoridPath);
        delete m_table;
        m_table = NULL;
        return FALSE;
    }
    
    // Open the GroupId file.
    char *groupidPath = _expandPath(groupidFileName);
    //char *groupidPath = getCanonicalPath(tmpGroupidPath);
    if ((m_groupid = mlFOpen(groupidPath, "w+")) == NULL)
    {
        fprintf(stderr,"Unable to open %s: %d\n",groupidPath,errno);
        if (g_outputDir != NULL)
        {
            mlFree(actoridPath);
            mlFree(groupidPath);
        }
        mlFClose(m_actorid);
        m_actorid = NULL;
        delete m_table;
        m_table = NULL;
        return FALSE;
    }

    // Calculate the date time stamp.
    time_t now;
    char s[80];
    time(&now);
    struct tm *date = localtime(&now);
    strftime(s, sizeof s, "%c", date);

    if (m_language)
    {
        // Output ActorID Java file preamble.
        fprintf(m_actorid, "/*\n");
        fprintf(m_actorid, " * This file contains defines for mapping Actor Names\n");
        fprintf(m_actorid, " * to their position in the array returned by loading a Group.\n");
        fprintf(m_actorid, " *\n");
        fprintf(m_actorid, " * Automatically generated code.\n");
        fprintf(m_actorid, " *       File: ActorID.java\n");
        fprintf(m_actorid, " *       Date: %s\n", s);
        fprintf(m_actorid, " */\n\n");
        fprintf(m_actorid, "// Declare package.\n");
        fprintf(m_actorid, "package %s;\n\n",m_package);
        fprintf(m_actorid, "public class ActorID\n");
        fprintf(m_actorid, "{\n");

        // Output the GroupId Java file preamble.
        fprintf(m_groupid, "/*\n");
        fprintf(m_groupid, " * This file contains definitions for mapping Group Names\n");
        fprintf(m_groupid, " * to their position in the global array of all groups.\n");
        fprintf(m_groupid, " *\n");
        fprintf(m_groupid, " * Automatically generated code.\n");
        fprintf(m_groupid, " *       File: GroupID.java\n");
        fprintf(m_groupid, " *       Date: %s\n", s);
        fprintf(m_groupid, " */\n\n");
        fprintf(m_groupid, "// Declare package.\n");
        fprintf(m_groupid, "package %s;\n\n",m_package);
        fprintf(m_groupid, "public class GroupID\n");
        fprintf(m_groupid, "{\n");
    } else
    {
        // Output ActorID header file preamble.
        fprintf(m_actorid, "#ifndef __ACTORID_H_\n");
        fprintf(m_actorid, "#define __ACTORID_H_\n\n");
        fprintf(m_actorid, "/*\n");
        fprintf(m_actorid, " * This file contains defines for mapping Actor Names\n");
        fprintf(m_actorid, " * to their position in the array returned by mlLoadGroup.\n");
        fprintf(m_actorid, " *\n");
        fprintf(m_actorid, " * Automatically generated code.\n");
        fprintf(m_actorid, " *       File: actorid.h\n");
        fprintf(m_actorid, " *       Date: %s\n", s);
        fprintf(m_actorid, " */\n\n");

        // Output the GroupId header file preamble.
        fprintf(m_groupid, "#ifndef __GROUPID_H_\n");
        fprintf(m_groupid, "#define __GROUPID_H_\n\n");
        fprintf(m_groupid, "/*\n");
        fprintf(m_groupid, " * This file contains definitions for mapping Group Names\n");
        fprintf(m_groupid, " * to their position in the global array of all groups.\n");
        fprintf(m_groupid, " *\n");
        fprintf(m_groupid, " * Automatically generated code.\n");
        fprintf(m_groupid, " *       File: groupid.h\n");
        fprintf(m_groupid, " *       Date: %s\n", s);
        fprintf(m_groupid, " */\n\n");
    }

    // Clean up.
    if (g_outputDir != NULL)
    {
        mlFree(actoridPath);
        mlFree(groupidPath);
    }

    return TRUE;
}


MleDppActorGroupOutput::~MleDppActorGroupOutput()
{
    // Delete the table.
    delete m_table;

    // Close the Digital Playprint.
    closeFile();

    // Close the Actor ID File.
    if (m_actorid)
    {
        if (m_language)
        {
            // Wrap-up Java output.
            fprintf(m_actorid, "\n} /* ActorID Class */\n");
        } else
        {
            // Wrap-up C/C++ output.
            fprintf(m_actorid, "\n#endif /* __ACTORID_H_ */\n");
        }
        mlFClose(m_actorid); 
        m_actorid = NULL;
    }

    // Close the Group ID File.
    if (m_groupid)
    {
        if (m_language)
        {
            // Wrap-up Java output.
            fprintf(m_groupid, "\n} /* GroupID Class */\n");
        } else
        {
            // Wrap-up C/C++ output.
            fprintf(m_groupid, "\n#endif /* __GROUPID_H_ */\n");
        }
        mlFClose(m_groupid); 
        m_groupid = NULL;
    }

    if (m_package)
        mlFree(m_package);
}


int MleDppActorGroupOutput::openFile(const char *filename)
{
    int status = 1;

    if (m_fp)
        closeFile();

    // Expand directory path, if necessary.
    char *tmpPath;
    if (g_outputDir != NULL)
    {
        tmpPath = (char *)mlMalloc(strlen((char *)g_outputDir->getPlatformPath()) + strlen(filename) + 2);
        strcpy(tmpPath,(char *)g_outputDir->getPlatformPath());
#ifdef _WINDOWS
        strcat(tmpPath,"\\");
#else /* ! _WINDOWS */
        strcat(tmpPath,"/");
#endif /* _WINDOWS */
        strcat(tmpPath,filename);
    } else
        tmpPath = (char *)filename;

    // Open the Group Chunk.
    if ( (m_fp = mlFOpen(tmpPath,"wb")) != NULL )
    {
        m_curGroupChunkFile = m_fp;

        int placeHolder = 0;
        mlFWrite(&placeHolder, sizeof(int), 1, m_fp);

        status = 0;
    }

    // Clean up.
    if (g_outputDir != NULL)
        mlFree(tmpPath);

    return status;
}

int MleDppActorGroupOutput::closeFile(void)
{
    m_lastOffset = -1;
    m_lastLength = 0;
    m_actorCount = 0;

    if (m_fp == NULL)
        return 1;

    writeOpcode(endActorGroupOpcode);
    int stringStart = mlFTell(m_fp);
    if (m_references)
    {
        // Write out reference strings.
        for (int counter = 0; counter < m_numReferences; counter++)
            mlFWrite(m_references[counter]->getName(),
                strlen(m_references[counter]->getName()) + 1, 1, m_fp);
        delete m_references;
        m_references = NULL;
        m_numReferences = 0;
    }

    // Update length.
    mlFSeek(m_fp, 0, SEEK_SET);
    mlFWrite(&stringStart, sizeof(int), 1, m_fp);
    mlFSeek(m_fp, 0, SEEK_END);
    mlFClose(m_fp);
    m_fp = NULL;
    m_curGroupChunkFile = NULL;

    return 0;
}


int
MleDppActorGroupOutput::openForumChunkFile(const char *filename)
{
    int status = 1;
    int placeHolder = 0;

    /*
     * There should be no Set chunk file or Group chunk file already opened.
     */
    MLE_ASSERT(m_fp == NULL);
    MLE_ASSERT(m_curSetChunkFile == NULL);
    MLE_ASSERT(m_curGroupChunkFile == NULL);

    // Expand directory path, if necessary.
    char *tmpPath;
    if (g_outputDir != NULL)
    {
        tmpPath = (char *)mlMalloc(strlen((char *)g_outputDir->getPlatformPath()) + strlen(filename) + 2);
        strcpy(tmpPath,(char *)g_outputDir->getPlatformPath());
#ifdef _WINDOWS
        strcat(tmpPath,"\\");
#else /* ! _WINDOWS */
        strcat(tmpPath,"/");
#endif /* _WINDOWS */
        strcat(tmpPath,filename);
    } else
        tmpPath = (char *)filename;

    // Open the Set Chunk file.
    if ((m_fp = mlFOpen(tmpPath,"wb")) != NULL )
    {
        m_curSetChunkFile = m_fp;
        mlFWrite(&placeHolder, sizeof(int), 1, m_fp);

        status = 0;
    }

    // Clean up.
    if (g_outputDir != NULL)
        mlFree(tmpPath);

    return status;
}

int
MleDppActorGroupOutput::openSetChunkFile(const char *filename)
{
    return openForumChunkFile(filename);
}

int MleDppActorGroupOutput::closeForumChunkFile(void)
{
    int fileSize;

    /*
     * There should be a Set chunk file already opened, but no group
     * chunk file opened.
     */
    MLE_ASSERT(m_fp);
    MLE_ASSERT(m_fp == m_curSetChunkFile);
    MLE_ASSERT(m_curGroupChunkFile == NULL);

    /*
     * Write out the last opcode for this Set chunk file.
     */
    writeOpcode(endSetOpcode);

    /*
     * Write the size of the file into the first word (4 bytes) of the file.
     */
    fileSize = mlFTell(m_fp);
    mlFSeek(m_fp, 0, SEEK_SET);
    mlFWrite(&fileSize, sizeof(int), 1, m_fp);
    mlFSeek(m_fp, 0, SEEK_END);
    mlFClose(m_fp);

    /*
     * Re-initialize member variable.
     */
    m_fp = NULL;
    m_curSetChunkFile = NULL;
    m_lastOffset = -1;
    m_lastLength = 0;

    return 0;
}

int MleDppActorGroupOutput::closeSetChunkFile(void)
{
    return closeForumChunkFile();
}

void MleDppActorGroupOutput::setFilePointer(FILE *ufp)
{
    if (m_fp == m_curGroupChunkFile)
        m_curGroupChunkFile = ufp;
    else if (m_fp == m_curSetChunkFile)
        m_curSetChunkFile = ufp;

    m_fp = ufp;
}


/*
    This function writes a string to the output.
*/
int MleDppActorGroupOutput::writeString(const char *s)
{
    unsigned int length = strlen(s);
    
    MLE_ASSERT(length > 0);

    if ( mlFWrite(s, 1, length, m_fp) == length )
        return 0;
    else
        return 1;
}

/*
    This function writes an int in big or little endian order.
*/
int MleDppActorGroupOutput::writeInt(int d)
{
    unsigned char data[4];
    unsigned long intBits = *(unsigned long *)&d;
    
    if (m_littleEndian)
    {
        data[3] = (unsigned char)(intBits >> 24);
        data[2] = (unsigned char)(intBits >> 16);
        data[1] = (unsigned char)(intBits >> 8);
        data[0] = (unsigned char)(intBits);
    } else {
        data[0] = (unsigned char)(intBits >> 24);
        data[1] = (unsigned char)(intBits >> 16);
        data[2] = (unsigned char)(intBits >> 8);
        data[3] = (unsigned char)(intBits);
    }
    if ( mlFWrite(data, 1, 4, m_fp) == 4 )
        return 0;
    else
        return 1;
}

/*
    This function writes a scalar as a float
    or 16.16 fixed, in big or little endian order.
*/
int MleDppActorGroupOutput::writeScalar(float f)
{
    unsigned char data[4];
    unsigned long floatBits;
    
    MLE_ASSERT(sizeof(long) == sizeof(float));
    floatBits = m_convertFloatToFixed ? f * 65536.0 : *(unsigned long *) &f;
    if (m_littleEndian) {
        data[3] = (unsigned char)(floatBits >> 24);
        data[2] = (unsigned char)(floatBits >> 16);
        data[1] = (unsigned char)(floatBits >> 8);
        data[0] = (unsigned char)(floatBits);
    } else {
        data[0] = (unsigned char)(floatBits >> 24);
        data[1] = (unsigned char)(floatBits >> 16);
        data[2] = (unsigned char)(floatBits >> 8);
        data[3] = (unsigned char)(floatBits);
    }
    if ( mlFWrite(data, 1, 4, m_fp) == 4 )
        return 0;
    else
        return 1;
}

int MleDppActorGroupOutput::writeFloat(float f)
{
    unsigned char data[4];
    unsigned long floatBits;
    
    MLE_ASSERT(sizeof(long) == sizeof(float));
    floatBits = *(unsigned long *) &f;
    if (m_littleEndian)
    {
        data[3] = (unsigned char)(floatBits >> 24);
        data[2] = (unsigned char)(floatBits >> 16);
        data[1] = (unsigned char)(floatBits >> 8);
        data[0] = (unsigned char)(floatBits);
    } else {
        data[0] = (unsigned char)(floatBits >> 24);
        data[1] = (unsigned char)(floatBits >> 16);
        data[2] = (unsigned char)(floatBits >> 8);
        data[3] = (unsigned char)(floatBits);
    }
    if ( mlFWrite(data, 1, 4, m_fp) == 4 )
        return 0;
    else
        return 1;
}

/*
    This function writes a single char to the output.
*/
int MleDppActorGroupOutput::writeChar(char c)
{
    if ( mlFWrite(&c, 1, 1, m_fp) == 1 )
        return 0;
    else
        return 1;
}

int MleDppActorGroupOutput::writeOpcode(unsigned char opcode)
{
    if ( mlFWrite(&opcode, 1, 1, m_fp) == 1 )
        return 0;
    else
        return 1;
}

int MleDppActorGroupOutput::writeIndex(long index)
{
    unsigned char bits;
    int counter = (32 + valueBits - 1) / valueBits;
    char writeOut = 0;
    
    MLE_ASSERT(index >= 0);
    while (--counter >= 0)
    {
        if (writeOut || index >= 1 << valueBits * counter)
        {
            writeOut = 1;
            bits = (unsigned char)(index >> valueBits * counter & valueMask);
            if (mlFWrite(&bits, 1, 1, m_fp) != 1)
                return 1;
        }
    }
    return 0;
}

int MleDppActorGroupOutput::writeOffset()
{
    long offset;

    MLE_ASSERT(m_currentProperty != NULL);

    if (m_curSetChunkFile)
    {
        MLE_ASSERT(m_currentSetClass != NULL);
        offset = m_table->setPropRuntimeName(m_currentSetClass,
            m_currentProperty);
    }
    else
    {
        MLE_ASSERT(m_currentActorClass != NULL);
        offset = m_table->actorPropRuntimeName(m_currentActorClass,
            m_currentProperty);
    }

    if (offset != m_lastOffset)
    {
        writeOpcode(setPropertyOffsetOpcode);
        writeIndex(offset);
        m_lastOffset = offset;
    }

    return 0;
}

int MleDppActorGroupOutput::writeOffsetLength(int byteLength)
{
    long offset;

    MLE_ASSERT(m_currentProperty != NULL);

    if (m_curSetChunkFile)
    {
        MLE_ASSERT(m_currentSetClass != NULL);
        offset = m_table->setPropRuntimeName(m_currentSetClass,
            m_currentProperty);
    }
    else
    {
        MLE_ASSERT(m_currentActorClass != NULL);
        offset = m_table->actorPropRuntimeName(m_currentActorClass,
            m_currentProperty);
    }

    if (offset != m_lastOffset)
    {
        writeOpcode(setPropertyOffsetOpcode);
        writeIndex(offset);
        m_lastOffset = offset;
    }

    if (byteLength != 0 && byteLength != m_lastLength)
    {
        writeOpcode(setPropertyLengthOpcode);
        writeIndex(byteLength);
        m_lastLength = byteLength;
    }
    writeOpcode(byteLength ? copyPropertyOpcode : copyMediaRefOpcode);
    return 0;
}

int MleDppActorGroupOutput::writeArrayOffsetLength(int byteLength,int numElements)
{
    long offset;

    MLE_ASSERT(m_currentProperty != NULL);
    if (m_curSetChunkFile)
    {
        MLE_ASSERT(m_currentSetClass != NULL);
        offset = m_table->setPropRuntimeName(m_currentSetClass,
            m_currentProperty);
    }
    else
    {
        MLE_ASSERT(m_currentActorClass != NULL);
        offset = m_table->actorPropRuntimeName(m_currentActorClass,
            m_currentProperty);
    }
    
    if (offset != m_lastOffset)
    {
        writeOpcode(setPropertyOffsetOpcode);
        writeIndex(offset);
        m_lastOffset = offset;
    }
    if (byteLength != 0 && byteLength != m_lastLength)
    {
        writeOpcode(setPropertyLengthOpcode);
        writeIndex(byteLength);
        m_lastLength = byteLength;
    }
    writeOpcode(copyArrayPropertyOpcode);
    writeInt(numElements);
    return 0;
}


void MleDppActorGroupOutput::setOutputDirectory(const char *path)
{
    // Clear the output directory path, if path is NULL.
    if (path == NULL)
    {
        delete g_outputDir;
        g_outputDir = NULL;
        return;
    }

    if (g_outputDir == NULL)
#if defined(_WINDOWS)
        g_outputDir = new MleWin32Path((MlChar *)path, TRUE);
#else
        g_outputDir = new MleLinuxPath((MlChar *)path, TRUE);
#endif /* _WINDOWS */
    else
        g_outputDir->setPlatformPath((MlChar *)path);
}


MlePath *MleDppActorGroupOutput::getOutputDirectory()
{
    return g_outputDir;
}


void MleDppActorGroupOutput::setVerbosity(MlBoolean verbose)
{
    g_verbose = verbose;
}

void *
MleDppActorGroupOutput::operator new(size_t tSize)
{
    void *p = mlMalloc(tSize);
    return p;
}

void
MleDppActorGroupOutput::operator delete(void *p)
{
    mlFree(p);
}

void*
MleDppActorGroupOutput::operator new[](size_t tSize)
{
    void* p = mlMalloc(tSize);
    return p;
}

void
MleDppActorGroupOutput::operator delete[](void* p)
{
    mlFree(p);
}

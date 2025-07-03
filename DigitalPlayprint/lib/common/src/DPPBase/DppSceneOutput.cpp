/** @defgroup MleDPPBase Magic Lantern Digital Workprint Library API - Base */

/**
 * @file DppSceneOutput.cpp
 * @ingroup MleDPPBase
 *
 * This file implements the Scene Output used by the Magic Lantern Digital
 * Playprint Library API.
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


// Include system header files.#include <stdio.h>
#include <string.h>
#include <time.h>

// Include Magic Lantern header files.
#include <mle/mlAssert.h>
#include <mle/mlFileio.h>
#include <mle/mlMalloc.h>
#ifdef _WINDOWS
#include <mle/MleWin32Path.h>
#else
#include <mle/MleLinuxPath.h>
#endif

// Include Magic Lantern Digital Workprint header files.
#include "mle/DwpItem.h"
#include "mle/DwpDataUnion.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/DppSceneOutput.h"
#include "mle/DppTblMgr.h"


// The global directory for generated output.
MlePath *MleDppSceneOutput::g_outputDir = NULL;
// Be verbose.
MlBoolean MleDppSceneOutput::g_verbose = FALSE;


// Utility to append the global directory path to the specified path.
static char *_expandPath(char * path)
{
    MLE_ASSERT(path);

    MlePath *dir = MleDppSceneOutput::getOutputDirectory();

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


MleDppSceneOutput::MleDppSceneOutput(
    MleDwpItem *root, char isFixed,
    char isLittle,
    char language,
    char *package)
  : m_sceneCount(0), m_fp(NULL), m_curSceneChunkFile(NULL),
    m_convertFloatToFixed(isFixed), m_language(language),
    m_littleEndian(isLittle), m_lastOffset(-1), m_lastLength(0),
    m_currentGroupClass(NULL),
    m_sceneid(NULL), m_package(package), m_root(root)
{
    if (m_language)
    {
#if defined(_WINDOWS)
        m_package = _strdup(package);
#else
        m_package = strdup(package);
#endif
    }
}


MleDppSceneOutput::~MleDppSceneOutput()
{ 
    delete m_table;

    closeFile();

    if (m_sceneid)
    {
        if (m_language)
        {
            // Wrap-up Java output.
            fprintf(m_sceneid, "\n} /* SceneID Class */\n");
        } else
        {
            // Wrap-up C/C++ output.
            fprintf(m_sceneid, "\n#endif /* __SCENEID_H_ */\n");
        }
        mlFClose(m_sceneid); 
        m_sceneid = NULL;
    }

    if (m_package)
        mlFree(m_package);
}


MlBoolean
MleDppSceneOutput::init(
    char *tags,
    char *sceneidFileName)
{
    // For now set the format of scalar to be default (16.16),
    // but in the future set it based on user input to the PP generator.
    
    if (g_verbose)
    {
        fprintf(stdout,"Generating %s\n",sceneidFileName);
    }

    setScalarFormat();

    // Build the tables to process.
    m_table = new MleDppTblMgr; 
    m_table->setDiscriminators(m_root, tags);
    m_table->buildIndexTables(m_root);

    // Calculate the date time stamp.
    time_t now;
    char s[80];
    time(&now);
    struct tm *date = localtime(&now);
    strftime(s, sizeof s, "%c", date);

    // Open the GroupId file.
    char *sceneidPath = _expandPath(sceneidFileName);
    if ((m_sceneid = mlFOpen(sceneidPath, "w+")) == NULL)
    {
        if (g_outputDir != NULL)
            mlFree(sceneidPath);
        delete m_table;
        m_table = NULL;
        return FALSE;
    }

    if (m_language)
    {
        // Output SceneId Java file preamble.
        fprintf(m_sceneid, "/*\n");
        fprintf(m_sceneid, " * This file contains definitions for mapping Scene Names\n");
        fprintf(m_sceneid, " * to a global integer constant.\n");
        fprintf(m_sceneid, " * It also contains definitions for mapping Group and Scene names\n");
        fprintf(m_sceneid, " * to their position in the array returned by mlLoadScene.\n");
        fprintf(m_sceneid, " *\n");
        fprintf(m_sceneid, " * Automatically generated code.\n");
        fprintf(m_sceneid, " *       File: SceneID.java\n");
        fprintf(m_sceneid, " *       Date: %s\n", s);
        fprintf(m_sceneid, " */\n\n");
        fprintf(m_sceneid, "// Declare package.\n");
        fprintf(m_sceneid, "package %s;\n\n",m_package);
        fprintf(m_sceneid, "public class SceneID\n");
        fprintf(m_sceneid, "{\n");
    } else
    {
        // Output SceneId header file preamble.
        fprintf(m_sceneid, "#ifndef __SCENEID_H_\n");
        fprintf(m_sceneid, "#define __SCENEID_H_\n\n");
        fprintf(m_sceneid, "/*\n");
        fprintf(m_sceneid, " * This file contains definitions for mapping Scene Names\n");
        fprintf(m_sceneid, " * to a global integer constant.\n");
        fprintf(m_sceneid, " * It also contains definitions for mapping Group and Scene names\n");
        fprintf(m_sceneid, " * to their position in the array returned by mlLoadScene.\n");
        fprintf(m_sceneid, " *\n");
        fprintf(m_sceneid, " * Automatically generated code.\n");
        fprintf(m_sceneid, " *       File: sceneid.h\n");
        fprintf(m_sceneid, " *       Date: %s\n", s);
        fprintf(m_sceneid, " */\n\n");
    }

    // Clean up.
    if (g_outputDir != NULL)
    {
        mlFree(sceneidPath);
    }

    return TRUE;
}

int MleDppSceneOutput::openFile(const char *filename)
{
    int status = 1;

    if (m_fp)
        closeFile();

    // Open the Scene Chunk file.
    char *tmpPath = _expandPath((char *)filename);
    if ( (m_fp = mlFOpen(tmpPath,"wb")) != NULL )
    {
        m_curSceneChunkFile = m_fp;

        int placeHolder = 0;
        mlFWrite(&placeHolder, sizeof(int), 1, m_fp);

        status = 0;
    }
    
    // Clean up.
    if (g_outputDir != NULL)
        mlFree(tmpPath);

    return status;
}

int MleDppSceneOutput::closeFile(void)
{
    m_lastOffset = -1;
    m_lastLength = 0;

    if (m_fp == NULL)
        return 1;

    // Update length.
    int stringStart = mlFTell(m_fp);
    mlFSeek(m_fp, 0, SEEK_SET);
    mlFWrite(&stringStart, sizeof(int), 1, m_fp);
    mlFSeek(m_fp, 0, SEEK_END);
    mlFClose(m_fp);
    m_fp = NULL;
    m_curSceneChunkFile = NULL;

    return 0;
}


void
MleDppSceneOutput::setFilePointer(FILE *ufp)
{
    if (m_fp == m_curSceneChunkFile)
        m_curSceneChunkFile = ufp;

    m_fp = ufp;
}


/*
    This function writes a string to the output.
*/
int MleDppSceneOutput::writeString(const char *s)
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
int MleDppSceneOutput::writeInt(int d)
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
int MleDppSceneOutput::writeScalar(float f)
{
    unsigned char data[4];
    unsigned long floatBits;
    
    MLE_ASSERT(sizeof(long) == sizeof(float));
    floatBits = m_convertFloatToFixed ? f * 65536.0 : *(unsigned long *) &f;
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

int MleDppSceneOutput::writeFloat(float f)
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
int MleDppSceneOutput::writeChar(char c)
{
    if ( mlFWrite(&c, 1, 1, m_fp) == 1 )
        return 0;
    else
        return 1;
}

int MleDppSceneOutput::writeIndex(long index)
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


void MleDppSceneOutput::setOutputDirectory(const char *path)
{
    // Clear the output directory path, if path is NULL.
    if (path == NULL)
    {
        delete g_outputDir;
        g_outputDir = NULL;
        return;
    }

    if (g_outputDir == NULL)
#if _WINDOWS
        g_outputDir = new MleWin32Path((MlChar *)path, TRUE);
#else
        g_outputDir = new MleLinuxPath((MlChar *)path, TRUE);
#endif /* _WINDOWS */
    else
        g_outputDir->setPlatformPath((MlChar *)path);
}

inline MlePath *
MleDppSceneOutput::getOutputDirectory()
{
    return g_outputDir;
}

void *
MleDppSceneOutput::operator new(size_t tSize)
{
    void *p = mlMalloc(tSize);
    return p;
}

void
MleDppSceneOutput::operator delete(void *p)
{
    mlFree(p);
}

void*
MleDppSceneOutput::operator new[](size_t tSize)
{
    void* p = mlMalloc(tSize);
    return p;
}

void
MleDppSceneOutput::operator delete[](void* p)
{
    mlFree(p);
}

/** @defgroup MleDPPBase Magic Lantern Digital Playprint Library API - Base */

/**
 * @file DppSceneOutput.h
 * @ingroup MleDPPBase
 *
 * This file defines the Scene Output used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @date July 17, 2004
 */

// COPYRIGHT_BEGIN
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Wizzer Works
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_DPP_SCENEPPOUTPUT_H_
#define __MLE_DPP_SCENEPPOUTPUT_H_

// #include <stdio.h>

// Include Magic Lantern header files.
#include <mle/mlFileio.h>
#include <mle/mlMalloc.h>
#include <mle/MlePath.h>
#ifdef WIN32
#include <mle/MleWin32Path.h>
#endif /* WIN32 */

// Include Magic Lantern Math header files.
#include "math/scalar.h"

// Include Magic Lantern Digital Playprint header files.
#include "mle/agopcode.h"
#include "mle/DppTblMgr.h"


class MleDppSceneOutput
{
  public:

	MleDppSceneOutput(
		MleDwpItem *root,
		char isFixed,
		char isLittle,
		char language,
		char *package);

	virtual ~MleDppSceneOutput();
	
	/**
	 * Initialize the class.
	 */
	MlBoolean init(
		char* tags,
		char* sceneIDfilename);

	// Set output sink.
	int openFile(const char *filename);

	// Returns zero if some content was written to a file.
	int closeFile(void);

	void setFilePointer(FILE *ufp);

    // Set the type of output we desire.
    void setScalarFormat( MlScalarType fmt = ML_SCALAR_FIXED_16_16 )
	{ m_scalarFormat = fmt; };

    MlScalarType getScalarFormat(void) { return m_scalarFormat; };

	/**
	 * @brief Set the path for the generated output.
	 *
	 * @param path A character array identifying the output directory
	 * for the generated assets. This path is platform specific.
	 */
	static void setOutputDirectory(const char *path);

	/**
	 * @brief Get the path for the generated output.
	 *
	 * @return A path object is returned from which the platform
	 * specific representation of the path can be obtained.
	 */
	static MlePath *getOutputDirectory();

	// output functions
	//   write{String,Int,Float,Char} write an item of the indicated
	//   type to the output.  All the functions return FALSE if an
	//   error is encountered.
	int writeString(const char *s);

	int writeInt(int d);

	int writeScalar(float f);

	int writeFloat(float f);

	int writeChar(char c);

	int writeOpcode(unsigned char opcode);

	int writeIndex(long index);

	/**
	 * Override operator new.
	 *
	 * @param tSize The size, in bytes, to allocate.
	 */
	void* operator new(size_t tSize);

	/**
	 * Override operator delete.
	 *
	 * @param p A pointer to the memory to delete.
	 */
    void  operator delete(void *p);

    /** The current Group class. */
	const char *m_currentGroupClass;

	/** The current Scene class. */
	const char *m_currentSceneClass;

	MleDppTblMgr *m_table;

	int m_sceneCount;

	/** File pointer to a Scene Chunk File. */
	FILE *m_sceneid;
	/** The generation language for the scene id files. */
	char m_language;
	/** The package information for the generated code. */
	char *m_package;


  private:

	// File pointer to the Digital Playprint.
	FILE *m_fp;
	// The current Scene Chunk file.
	FILE *m_curSceneChunkFile;
	// Flag indicating whether to convert from floating-point to fixed-point format.
	char m_convertFloatToFixed;
	// Flag indicating whether to use Big or Little Endian.
	char m_littleEndian;
	// Offset of last Digital Playprint chunk.
	long m_lastOffset;
	// Length of last Digital Playprint chunk.
	long m_lastLength;
	// The floating-point arithmetic format to use.
    MlScalarType m_scalarFormat;

	// The root of the Digital Workprint.
    MleDwpItem *m_root;

	// The directory path for the generated output.
	static MlePath *g_outputDir;
    // Be verbose.
	static MlBoolean g_verbose;
};

#endif /* __MLE_DPP_SCENEPPOUTPUT_H_ */

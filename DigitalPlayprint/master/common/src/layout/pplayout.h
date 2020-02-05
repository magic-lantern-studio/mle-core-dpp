/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Library API - Master */

/**
 * @file pplayout.h
 * @ingroup MleDPPMaster
 *
 * Digital Playprint layout definitions.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
 */


// COPYRIGHT_BEGIN
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

#ifndef __MLE_DPP_LAYOUT_H_
#define __MLE_DPP_LAYOUT_H_

// Include Magic Lantern header files.
#include <mle/mlFileio.h>
#include <mle/mlAssert.h>
#include <mle/MleTemplate.h>

// Include Digital Workprint header files.
#include <mle/DwpItem.h>

// Include Digital Playprint header files.
#include "mle/ppoutput.h"
#include "mle/agchk.h"


/**
 * @brief This structure is used by TCL to manage state between
 * each Digital Playprint layout command.
 */
typedef struct {
    char             *m_commandName;  /* Name of command. */
    MlBoolean         m_byteOrder;    /* TRUE = Little Endian, FALSE = Big Endian. */
    char             *m_scriptfile;   /* Name of input script file. */
    char             *m_playprint;    /* The name of playprint file to build. */
    char             *m_codefile;     /* The root name of the files to generate. */
    FILE             *m_codefd;       /* A file pointer to the generated source file. */
    FILE             *m_headerfd;     /* A file pointer to the generated header file. */
    MleTemplate      *m_tmplate;      /* The template to use for code generation. */
	char             *m_workprint;    /* The name of the workprint file to read. */
	char             *m_outputDir;    /* The directory for generated output. */
	char             *m_inputDir;     /* The directory to find input resources. */
	MlBoolean         m_language;     /* TRUE = Java, FALSE = C/C++. */
	char             *m_package;      /* The Java package. */
    MleDwpItem       *m_root;         /* The root item of the input workprint. */
    char             *m_tags;         /* DWP discriminators. */
	MleDppOutput     *m_dpp;          /* Internal management class for DPP. */
    MleDppChunkTable *m_chunks;       /* Internal management class for chunks. */
} LayoutState;

#endif /* __MLE_DPP_LAYOUT_H_ */

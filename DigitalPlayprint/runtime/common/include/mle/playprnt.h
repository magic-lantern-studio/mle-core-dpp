/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Runtime Library API - Master */

/**
 * @file playprnt.h
 * @ingroup MleDPPMaster
 *
 * This file contains header information for the Magic Lantern
 * Digital Playprint Runtime library. It is designed to be shared
 * by both C and C++ programs.
 *
 * @author Mark S. Millard
 * @date July 10, 2003
 * @version 0.1
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_PLAYPRNT_H_
#define __MLE_PLAYPRNT_H_

/* Include system header files. */
#include <stdio.h>

/* Include Magic Lantern header files. */
#include "mle/mlTypes.h"
#include "mle/mlFileio.h"


/* Define generic Digital Playprint structures. */

typedef MlUInt MlDppTag; /**< Four-character code. */

typedef struct
{
    MlDppTag m_tag;        /**< Chunk ID. */
    MlUInt   m_size;       /**< Size of data. */
/*
 *  Note: the actual data of the chunk follows the "size" field.
 *        This information is here for reference only.
 *  MlU1  m_data[m_size];
 */
} MleDppChunk;

typedef struct
{
    MleDppChunk m_chunk;       /**< Chunk. */
    MlDppTag    m_form;        /**< Type of Playprint form. */
    MlUInt      m_offset;      /**< Chunk offset. */
    MlUInt      m_flags;       /**< Additional flags information. */
} MleDppChunkInfo;

typedef struct
{
    MlUInt m_version;     /**< Version number of file format. */
    MlInt  m_date;        /**< Date file was created. */
} MleDppHeader;


/* Define flags. */
#define MLE_DPP_CREATEPPBE    0x0020
#define MLE_DPP_CREATELIST    0x0040
#define MLE_DPP_CREATEPPLE    0x0080

#define MLE_DPP_FINDCHUNK     0x0010
#define MLE_DPP_FINDPPBE      0x0020
#define MLE_DPP_FINDLIST      0x0040
#define MLE_DPP_FINDPPLE      0x0080

#define MLE_DPP_DIRTY         0x10000000

#define MLE_DPP_TOUPPER       0x0010


/* Define errno numbers. */
enum MleDppErr
{
    MLE_DPP_ERR_BADTAG           = 512,
    MLE_DPP_ERR_CANNOTWRITE      = 513,
    MLE_DPP_ERR_CANNOTREAD       = 514,
    MLE_DPP_ERR_CANNOTSEEK       = 515,
    MLE_DPP_ERR_CHUNKNOTFOUND    = 516
};


/* Define parameterized macros. */

#define mlDppMakeTag(ch0,ch1,ch2,ch3) \
    ( (MlInt)(ch0)<<24L | (MlInt)(ch1)<<16L | (ch2)<<8 | (ch3) )


/* Define commonly used tags. */
#define MLE_PPBE_TAG mlDppMakeTag('P','P','B','E')
#define MLE_PPLE_TAG mlDppMakeTag('P','P','L','E')
#define MLE_LIST_TAG mlDppMakeTag('L','I','S','T')


/* Declare function prototypes. */

EXTERN MlBoolean mlDppAscend(FILE *,MleDppChunkInfo *,MlUInt);
EXTERN MlBoolean mlDppDescend(FILE *,MleDppChunkInfo *,MleDppChunkInfo *,MlUInt);
EXTERN int mlDppCheckFormat(FILE *);
#ifndef MLE_RUNTIME
EXTERN MlBoolean mlDppCreateChunk(FILE *,MleDppChunkInfo *,MlUInt);
EXTERN MlDppTag mlDppStringToTag(char *,MlUInt);
EXTERN char *mlDppTagToString(MlDppTag);
EXTERN MlULong mlDppSwapLong(MlULong);
EXTERN MlUInt mlDppSwapInt(MlUInt);
EXTERN MlUShort mlDppSwapShort(MlUShort);
EXTERN void mlDppSetSwapRead(MlBoolean);
EXTERN void mlDppSetSwapWrite(MlBoolean);
EXTERN MlBoolean mlDppGetSwapRead(void);
EXTERN MlBoolean mlDppGetSwapWrite(void);
#endif /* MLE_RUNTIME */


#endif /* __MLE_PLAYPRNT_H_ */

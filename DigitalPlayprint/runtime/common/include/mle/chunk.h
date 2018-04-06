/** @defgroup MleDPPMaster Magic Lantern Digital Playprint Runtime Library API - Master */

/**
 *  @file chunk.h
 *  @ingroup MleDPPMaster
 *
 *  This file defines Digital Playprint chunks.
 *
 *  @author Mark S. Millard
 *  @date July 10, 2003
 *  @version 0.1
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

#ifndef __MLE_CHUNK_H_
#define __MLE_CHUNK_H_

// Include Magic Lantern header files.
#include "playprnt.h"


/**
 * This structure is used for reading/writing a chunk file header.
 */
typedef struct
{
    MlUInt m_id;       /**< Chunk file identifier. */
    MlUInt m_version;  /**< Version of chunk file. */
    MlUInt m_time;     /**< Creation time stamp. */
    MlUInt m_crc;      /**< Total CRC. */
} MleChunkFileHeader;


/**
 * This structure is used for reading/writing a Group chunk.
 */
typedef struct
{
    MlUInt m_flags;
    MlUInt m_size;
    char *m_data;
} MleGroupChunk;


/**
 * This structure is used for reading/writing a Scene chunk.
 */
typedef struct
{
    MlUInt m_flags;
    MlUInt m_size;
    char *m_data;
} MleSceneChunk;


/**
 * This structure is used for reading/writing a Set chunk.
 */
typedef struct
{
    MlUInt m_flags;
    MlUInt m_size;
    char *data;
} MleSetChunk;


/**
 * This structure is used for reading/writing a MediaRef 'info' chunk.
 */
typedef struct
{
	MlUInt m_flags;
	MlUInt m_type;     /**< Index into runtime class constructor table. */
	MlUInt m_numMrefs; /**< Number of MediaRef chunks in MRFL LIST. */
} MleMediaRefInfoChunk;


/**
 * This structure is used for reading/writing a MediaRef 'mref' chunk.
 */
typedef struct
{
	MlUInt m_flags;
    MlDppTag m_type;
    MlUInt m_size;
    char *m_data;
} MleMediaRefChunk;


#endif /* __MLE_CHUNK_H_ */

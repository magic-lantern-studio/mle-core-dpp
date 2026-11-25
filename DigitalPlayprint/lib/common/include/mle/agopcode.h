/** @defgroup MleDPPBase Magic Lantern Digital Playprint Library API - Base */

/**
 * @file agopcode.h
 * @ingroup MleDPPBase
 *
 * This file defines the Actor Group Operation Codes used by the Magic Lantern Digital
 * Playprint Library API.
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
//  For information concerning this header file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

#ifndef __MLE_DPP_ACTORGROUPOPCODE_H_
#define __MLE_DPP_ACTORGROUPOPCODE_H_

enum {
    beginActorSetOpcode			= 0x80,
    createActorOpcode			= 0x81,
    setForumOpcode				= 0x82,
	setSetOpcode				= 0x82,
    bindDelegateOpcode			= 0x83,
	bindRoleOpcode				= 0x83,
    parentDelegateOpcode		= 0x84,
	parentRoleOpcode			= 0x84,
    childDelegateOpcode			= 0x85,
	childRoleOpcode				= 0x85,
    setPropertyOffsetOpcode		= 0x86,
    setPropertyLengthOpcode		= 0x87,
    copyPropertyOpcode			= 0x88,
    copyMediaRefOpcode			= 0x89,
    endActorGroupOpcode			= 0x8a,
    copyDynamicPropertyOpcode	= 0x8b,
    copyArrayPropertyOpcode		= 0x8c,
    createForumOpcode			= 0x8d,
	createSetOpcode				= 0x8d,
    endForumOpcode				= 0x8e,
	endSetOpcode				= 0x8e
};

#define valueBits 7
#define valueMask ((1 << (valueBits)) - 1) 

/*
 * A Group, Scene or Set chunk file starts with a four byte marker that says
 * the byte order of the integers in the file. The marker is bytes, so it is
 * the same in either order. The integer that follows it (the offset of the
 * names, or the size of the file) is in that byte order, and the offsets
 * count from the start of the file, marker included.
 *
 * Group and Scene: marker, offset of the names, data, names.
 * Set:             marker, size of the file, data.
 */
#define MLE_DPP_CHUNKFILE_MARKER_SIZE   4
#define MLE_DPP_CHUNKFILE_MARKER_BE     "MLBE"
#define MLE_DPP_CHUNKFILE_MARKER_LE     "MLLE"
/* The size of the marker and the integer that follows it. */
#define MLE_DPP_CHUNKFILE_HEADER_SIZE   8

/*
 * The byte order that a marker gives: 1 for Big Endian, 2 for Little Endian
 * and 0 if the bytes are not a marker.
 */
static inline int mleDppChunkFileOrder(const unsigned char *bytes)
{
    if (bytes[0] == 'M' && bytes[1] == 'L' && bytes[3] == 'E')
    {
        if (bytes[2] == 'B')
            return 1;
        if (bytes[2] == 'L')
            return 2;
    }
    return 0;
}

/*
 * The integer in four bytes of a chunk file, given the byte order of the file.
 */
static inline int mleDppChunkFileInt(const unsigned char *b, int littleEndian)
{
    unsigned int v;
    if (littleEndian)
        v = b[0] | (b[1] << 8) | (b[2] << 16) | ((unsigned int) b[3] << 24);
    else
        v = ((unsigned int) b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
    return (int) v;
}

#endif /* __MLE_DPP_ACTORGROUPOPCODE_H_ */

/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppVector3Array.h
 * @ingroup MleDPPType
 *
 * This file defines the Vector3 array data type used by the Magic Lantern Digital
 * Playpriint Library API.
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

#ifndef __MLE_DPP_VECTOR3ARRAY_H_
#define __MLE_DPP_VECTOR3ARRAY_H_

// Import Magic Lantern header files.
#include "mle/DppArray.h"

/**
 * This is the datatype object for an array of 3-vectors.
 *
 * BUGS
 * Commas, which are required between array values, must be delimited by
 * white space.  That is,
 *
 *     1 4 5,2 9 3 will be read incorrectly
 *     1 4 5 , 2 9 3 will be read correctly
 *
 * Values written by the datatype are output correctly so they may be read
 * by the datatype.
 *
 * @see MleDppArray, MleDwpDatatype
 */
class MleDppVector3Array : public MleDppArray
{
	MLE_DWP_DATATYPE_HEADER(MleDppVector3Array);

  public:

    /**
     * Reads a single Vector3 element from the input object and writes
     * it into the memory location passed in.
	 */
	virtual int readElement(MleDwpInput *in,void *data) const;

    /**
     * Interprets the memory location passed in as a Vector3 element,
     * and writes it to the output object.
	 */
	virtual int writeElement(MleDwpOutput *out,void *data) const;

	/**
	 */
	virtual int writeElement(MleDppActorGroupOutput *out,void *data) const;

    /**
	 * Returns the binary size of a Vecto3 element in bytes.
	 */
	virtual int getElementSize(void) const;

	/**
	 */
	virtual void setElement(MleDwpDataUnion *data,void *src) const;

	/**
	 */
	virtual void getElement(MleDwpDataUnion *data,void *dst) const;

	/**
	 */
    virtual void set(MleDwpDataUnion *data,void *src) const;

	/**
	 */
    virtual void get(MleDwpDataUnion *data,void *dst) const;

	/**
	 * Override operator new.
	 *
	 * @param tSize The size, in bytes, to allocate.
	 */
	void* operator new(size_t tSize);

	/**
     * Override operator new array.
     *
     * @param tSize The size, in bytes, to allocate.
     */
	void* operator new[](size_t tSize);

	/**
	 * Override operator delete.
	 *
	 * @param p A pointer to the memory to delete.
	 */
    void  operator delete(void *p);

	/**
     * Override operator delete array.
     *
     * @param p A pointer to the memory to delete.
     */
	void  operator delete[](void* p);
};

#endif /* __MLE_DPP_VECTOR3ARRAY_H_ */

/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppIntArray.h
 * @ingroup MleDPPType
 *
 * This file defines the int array data type used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 12, 2004
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

#ifndef __MLE_DPP_INTARRAY_H_
#define __MLE_DPP_INTARRAY_H_

// Include Digital Playprint header files.
#include "mle/DppArray.h"
#include "mle/DppActorGroupOutput.h"

/**
 * @brief This is the datatype object for an array of ints.
 *
 * BUGS
 * Values and commas separating values in the workprint must be delimited
 * by white space to be read property.  That is,
 *
 *     3,4,9            will be read incorrectly
 *     3 , 4 , 9        will be read correctly
 *
 * Arrays written by this datatype are output so they can be read properly.
 *
 * @see MleDwpDatatype, MleDppArray
 */
class MleDppIntArray : public MleDppArray
{
    MLE_DWP_DATATYPE_HEADER(MleDppIntArray);

  public:

	/**
	 */
	virtual int readElement(MleDwpInput *in, void *data) const;

	/**
	 */
	virtual int writeElement(MleDwpOutput *out, void *data) const;

	/**
	 */
    virtual int writeElement(MleDppActorGroupOutput *out,void *data) const;

	/**
	 */
	virtual int getElementSize(void) const;

	/**
	 */
    virtual void setElement(MleDwpDataUnion *data, void *src) const;

	/**
	 */
    virtual void getElement(MleDwpDataUnion *data, void *dst) const;

	/**
	 */
	virtual void set(MleDwpDataUnion *data, void *src) const;

    /**
	 */
	virtual void get(MleDwpDataUnion *data, void *dst) const;

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

};

#endif /* __MLE_DPP_INTARRAY_H_ */

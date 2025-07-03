/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppVector3.h
 * @ingroup MleDPPType
 *
 * This file defines the 3-element vector data type used by the Magic Lantern Digital
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

#ifndef __MLE_DPP_VECTOR3_H_
#define __MLE_DPP_VECTOR3_H_


// Inlclude Digital Workprint header files.
#include "mle/DwpVector3.h"
#include "mle/DwpDatatype.h"
#include "mle/DwpDataUnion.h"
#include "mle/DwpInput.h"
#include "mle/DwpOutput.h"

// Include Digital Playprint header files.
#include "mle/DppDatatype.h"
#include "mle/DppActorGroupOutput.h"


/**
 * This is the datatype object for a MleDwpVector3.  This 3-vector of scalars
 * is written to a workprint as three floating point numbers delimited by
 * white space.  For example:
 *
 *	1.2 -2.7 3.6
 *
 * Decimal points and leading zeroes may be omitted.
 *
 * @see MleDwpVector3
 */
class MleDppVector3 : public MleDwpVector3, public MleDppDatatype
{
	MLE_DWP_DATATYPE_HEADER(MleDppVector3);

  public:

	virtual int write(MleDppActorGroupOutput *out,MleDwpDataUnion *data) const;

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

#endif /* __MLE_DPP_VECTOR3_H_ */

/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppArray.h
 * @ingroup MleDPPType
 *
 * This file defines the array data type used by the Magic Lantern Digital
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

#ifndef __MLE_DPP_ARRAY_H_
#define __MLE_DPP_ARRAY_H_

// Include system header files.
// #include <stdio.h>
// #include <malloc.h>
#include <string.h>

// Include Magic Lantern utility header files.
// #include "mle/mlAssert.h"
#include "mle/mlArray.h"

// Include Digital Workprint header files.
#include "mle/DwpArray.h"
#include "mle/DwpDatatype.h"
#include "mle/DwpDataUnion.h"
#include "mle/DwpInput.h"
#include "mle/DwpOutput.h"

// Include Digital Playprint header files.
#include "mle/DppDatatype.h"
#include "mle/DppActorGroupOutput.h"


/**
 * @brief This is an abstract class that defines a datatype for arrays of data
 * elements.
 *
 * The general syntax for arrays in the workprint is the entire array is
 * within square brackets ([]) and individual elements are separated by
 * commas (,).  For example:
 *
 *     [ 1 , 2 , 3 , 4 ]
 *
 * The final value may have a comma after it or it may be omitted.
 *
 * BUGS
 * There is currently a bug in many of the array types that causes input
 * errors when the commas separating array elements are not delimited by
 * spaces.  That is
 *
 *     [ 1,2,3,4 ]        will be read incorrectly,
 *     [ 1 , 2 , 3 , 4 ]  will be read correctly.
 *
 * @see MleDwpArray, MleDppIntArray, MleDppScalarArray, MleDppVector3Array
 *
 */
class MleDppArray : public MleDwpArray, public MleDppDatatype
{
	MLE_DWP_DATATYPE_ABSTRACT_HEADER(MleDwpArray);

  public:

    virtual int writeElement(MleDppActorGroupOutput *,void *) const = 0;

    virtual int write(MleDppActorGroupOutput *out, MleDwpDataUnion *data) const;
	
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

#endif /* __MLE_DPP_ARRAY_H_ */

/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppInt.h
 * @ingroup MleDPPType
 *
 * This file defines the int data type used by the Magic Lantern Digital
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

#ifndef __MLE_DPP_INT_H_
#define __MLE_DPP_INT_H_

// Include Digital Workprint header files.
#include "mle/DwpInt.h"
#include "mle/DwpDatatype.h"
#include "mle/DwpDataUnion.h"
#include "mle/DwpInput.h"
#include "mle/DwpOutput.h"

// Include Digital Playprint header files.
#include "mle/DppDatatype.h"
#include "mle/DppActorGroupOutput.h"

/**
 * @brief This is the datatype object for an int.
 *
 * @see MleDwpInt, MleDppDatatype
 */
class MleDppInt : public MleDwpInt, public MleDppDatatype
{
	MLE_DWP_DATATYPE_HEADER(MleDppInt);

  public:

	/**
	 * @brief Write out to a Digital Playprint Actor Group file.
	 */
	virtual int write(MleDppActorGroupOutput *out, MleDwpDataUnion *data) const;

	/**
	 * Override operator new.
	 *
	 * @param tSize The size, in bytes, to allocate.
	 */
	void* operator new(size_t tSize);

	/**
     * Override operator new.
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
     * Override operator delete.
     *
     * @param p A pointer to the memory to delete.
     */
	void  operator delete[](void* p);
};

#endif /* __MLE_DPP_INT_H_ */

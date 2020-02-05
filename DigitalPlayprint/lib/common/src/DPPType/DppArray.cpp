/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppArray.cpp
 * @ingroup MleDPPType
 *
 * This file defines the array data type used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @created September 12, 2004
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
//  For information concerning this source file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Digital Playprint header files.
#include "mle/DppArray.h"


MLE_DWP_DATATYPE_ABSTRACT_SOURCE(MleDppArray,MleDwpArray);

int MleDppArray::write(MleDppActorGroupOutput * out,MleDwpDataUnion * data) const
{
	MleArray<char> * array = (MleArray<char> *) data->m_u.v;

	int size = getElementSize();

    out->writeArrayOffsetLength(size,array->size());

	for ( int i = 0; i < array->size(); i++ )
	{
	    writeElement ( out, *array + i * size );
	}

	return 0;
}

void *
MleDppArray::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppArray::operator delete(void *p)
{
	mlFree(p);
}

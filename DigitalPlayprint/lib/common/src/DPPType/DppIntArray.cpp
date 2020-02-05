/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppIntArray.cpp
 * @ingroup MleDPPType
 *
 * This file implements the int array data type used by the Magic Lantern Digital
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
#include "mle/DppIntArray.h"

//
// int array class
//
MLE_DWP_DATATYPE_SOURCE(MleDppIntArray,"IntArray",MleDppArray);


int
MleDppIntArray::readElement(MleDwpInput *in, void *data) const
{
	return in->readInt((int *)data);
}

int
MleDppIntArray::writeElement(MleDwpOutput *out, void *data) const
{
	return out->writeInt(*(int *)data);
}

int
MleDppIntArray::writeElement(MleDppActorGroupOutput *out, void *data) const
{
	return out->writeInt(*(int *)data);
}

int
MleDppIntArray::getElementSize(void) const
{
	return sizeof(int);
}

void
MleDppIntArray::setElement(MleDwpDataUnion *data, void *src) const
{
	*(int *)(data->m_u.v) = *(int *)src;
}

void
MleDppIntArray::getElement(MleDwpDataUnion *data, void *dst) const
{
	*(int *)dst = *(int *)(data->m_u.v);
}

void MleDppIntArray::set(MleDwpDataUnion *data, void *src) const
{
    data->setDatatype(this);

    data->m_u.v = new MleArray<int> ( *(MleArray<int> *)src );
}

void MleDppIntArray::get(MleDwpDataUnion *data, void *dst) const
{
    *(MleArray<int> *)dst = *(MleArray<int> *)data->m_u.v;
}

void *
MleDppIntArray::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppIntArray::operator delete(void *p)
{
	mlFree(p);
}

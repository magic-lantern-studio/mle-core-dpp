/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppFloatArray.cpp
 * @ingroup MleDPPType
 *
 * This file implements the float array data type used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @created February 6, 2004
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

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Digital Playprint header files.
#include "mle/DppFloatArray.h"

//
// float array class
//
MLE_DWP_DATATYPE_SOURCE(MleDppFloatArray,"FloatArray",MleDppArray);

int
MleDppFloatArray::readElement(MleDwpInput *in,void *data) const
{
	return in->readFloat((float *)data);
}

int
MleDppFloatArray::writeElement(MleDwpOutput *out,void *data) const
{
	return out->writeFloat(*(float *)data);
}

int
MleDppFloatArray::writeElement(MleDppActorGroupOutput *out,void *data) const
{
	return out->writeFloat(*(float *)data);
}

int
MleDppFloatArray::getElementSize(void) const
{
	return sizeof(float);
}

void
MleDppFloatArray::setElement(MleDwpDataUnion *data,void *src) const
{
	*(float *)(data->m_u.v) = *(float *)src;
}

void
MleDppFloatArray::getElement(MleDwpDataUnion *data,void *dst) const
{
	*(float *)dst = *(float *)(data->m_u.v);
}

void MleDppFloatArray::set(MleDwpDataUnion *data,void *src) const
{
    data->setDatatype(this);

    data->m_u.v = new MleArray<float> ( *(MleArray<float> *)src );
}

void MleDppFloatArray::get(MleDwpDataUnion *data,void *dst) const
{
    *(MleArray<float> *)dst = *(MleArray<float> *)data->m_u.v;
}

void *
MleDppFloatArray::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppFloatArray::operator delete(void *p)
{
	mlFree(p);
}

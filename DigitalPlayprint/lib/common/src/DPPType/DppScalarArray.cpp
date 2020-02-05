/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppScalarArray.h
 * @ingroup MleDPPType
 *
 * This file implements the scalar array data type used by the Magic Lantern Digital
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
#include "mle/DppScalar.h"
#include "mle/DppScalarArray.h"

//
// int array class
//
MLE_DWP_DATATYPE_SOURCE(MleDppScalarArray,"ScalarArray",MleDppArray);

//
// The MlScalar type might be a float or a long 16.16 fixed point
// number.  Or it might be a class encapsulating one of those.
// Read/write the thing as a floating point number for the sake
// of numerics and having a fixed interchange format (esp if we
// go to 16.16 AND 20.12 fixed point numbers).
//
// Read into the data union as a float, then convert to fixed/float
// scalar type on the get()/set().

int
MleDppScalarArray::readElement(MleDwpInput *in,void *data) const
{
	return in->readFloat((float *)data);
}

int
MleDppScalarArray::writeElement(MleDwpOutput *out,void *data) const
{
	return out->writeFloat(*(float *)data);
}

int
MleDppScalarArray::writeElement(MleDppActorGroupOutput *out,void *data) const
{
	return out->writeScalar(*(float *)data);
}

int
MleDppScalarArray::getElementSize(void) const
{
	return sizeof(MlScalar);
}

void
MleDppScalarArray::setElement(MleDwpDataUnion *data,void *src) const
{
	SET_FLOAT_FROM_SCALAR(*(float *)(data->m_u.v),src);
}

void
MleDppScalarArray::getElement(MleDwpDataUnion *data,void *dst) const
{
	SET_SCALAR_FROM_FLOAT(dst,*(float *)(data->m_u.v));
}

void 
MleDppScalarArray::set(MleDwpDataUnion *data,void *src) const
{
    data->setDatatype(this);

    data->m_u.v = new MleArray<MlScalar> ( *(MleArray<MlScalar> *)src );
}

void 
MleDppScalarArray::get(MleDwpDataUnion *data,void *dst) const
{
    *(MleArray<MlScalar> *)dst = *(MleArray<MlScalar> *)data->m_u.v;
}

void *
MleDppScalarArray::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppScalarArray::operator delete(void *p)
{
	mlFree(p);
}

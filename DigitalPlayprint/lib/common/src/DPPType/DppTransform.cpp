/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppTransform.h
 * @ingroup MleDPPType
 *
 * This file defines the Transform data type used by the Magic Lantern Digital
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
//  For information concerning this source file, contact Mark S. Millard,
//  of Wizzer Works at msm@wizzerworks.com.
//
//  More information concerning Wizzer Works may be found at
//
//      http://www.wizzerworks.com
//
// COPYRIGHT_END

// Include system header files.
#include <stdio.h>

// Include Magic Lantern header files.
#include "math/transfrm.h"
#include "mle/mlMalloc.h"
#include "mle/mlAssert.h"

// Include Digital Workprint header files.
#include "mle/DppType.h"
#include "mle/DppTransform.h"


MLE_DWP_DATATYPE_SOURCE(MleDppTransform,"MlTransform",MleDwpTransform);

//
// The MlScalar type might be a float or a long 16.16 fixed point
// number.  Or it might be a class encapsulating one of those.
// Read/write the thing as a floating point number for the sake
// of numerics and having a fixed interchange format (esp if we
// go to 16.16 AND 20.12 fixed point numbers).
//

int
MleDppTransform::write(MleDppActorGroupOutput *out,MleDwpDataUnion *data) const
{
	int i, j;

	out->writeOffsetLength(48);

	switch (out->getScalarFormat())
	{
		default:
		case ML_SCALAR_FLOAT:
			for (i=0; i<4; i++) {
				for (j=0; j<3; j++) {
					if (out->writeFloat(((float(*)[3])data->m_u.v)[i][j])) return 1;
				}
			}
			return 0;
		case ML_SCALAR_FIXED_16_16:
			for (i=0; i<4; i++) {
				for (j=0; j<3; j++) {
					if (out->writeInt(mlDppFixed1616(((float(*)[3])data->m_u.v)[i][j]))) return 1;
				}
			}
			return 0;
		case ML_SCALAR_FIXED_20_12:
			for (i=0; i<4; i++) {
				for (j=0; j<3; j++) {
					if (out->writeInt(mlDppFixed2012(((float(*)[3])data->m_u.v)[i][j]))) return 1;
				}
			}
			return 0;
	}
}

void *
MleDppTransform::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppTransform::operator delete(void *p)
{
	mlFree(p);
}

void*
MleDppTransform::operator new[](size_t tSize)
{
	void* p = mlMalloc(tSize);
	return p;
}

void
MleDppTransform::operator delete[](void* p)
{
	mlFree(p);
}

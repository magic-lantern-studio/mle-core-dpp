/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppRotation.h
 * @ingroup MleDWPType
 *
 * This file defines the Rotation data type used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @created September 13, 2004
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

// Inclde system header files.
#include <stdio.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Magic Lantern header files.
#include "math/rotation.h"
#include "mle/mlAssert.h"

// Inlclude Digital Workprint header files.
#include "mle/DppType.h"
#include "mle/DppRotation.h"


MLE_DWP_DATATYPE_SOURCE(MleDppRotation,"MlRotation",MleDwpRotation);

//
// The MlScalar type might be a float or a long 16.16 fixed point
// number.  Or it might be a class encapsulating one of those.
// Read/write the thing as a floating point number for the sake
// of numerics and having a fixed interchange format (esp if we
// go to 16.16 AND 20.12 fixed point numbers).
//

int
MleDppRotation::write(MleDppActorGroupOutput *out,MleDwpDataUnion *data) const
{
	out->writeOffsetLength(16);

	switch (out->getScalarFormat()) {
	default:
	case ML_SCALAR_FLOAT:
		if (out->writeFloat(((float*)data->m_u.v)[0])) return 1;
		if (out->writeFloat(((float*)data->m_u.v)[1])) return 1;
		if (out->writeFloat(((float*)data->m_u.v)[2])) return 1;
		if (out->writeFloat(((float*)data->m_u.v)[3])) return 1;
		return 0;
	case ML_SCALAR_FIXED_16_16:
		if (out->writeInt(mlDppFixed1616(((float*)data->m_u.v)[0]))) return 1;
		if (out->writeInt(mlDppFixed1616(((float*)data->m_u.v)[1]))) return 1;
		if (out->writeInt(mlDppFixed1616(((float*)data->m_u.v)[2]))) return 1;
		if (out->writeInt(mlDppFixed1616(((float*)data->m_u.v)[3]))) return 1;
		return 0;
	case ML_SCALAR_FIXED_20_12:
		if (out->writeInt(mlDppFixed2012(((float*)data->m_u.v)[0]))) return 1;
		if (out->writeInt(mlDppFixed2012(((float*)data->m_u.v)[1]))) return 1;
		if (out->writeInt(mlDppFixed2012(((float*)data->m_u.v)[2]))) return 1;
		if (out->writeInt(mlDppFixed2012(((float*)data->m_u.v)[3]))) return 1;
		return 0;
	}
}

void *
MleDppRotation::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppRotation::operator delete(void *p)
{
	mlFree(p);
}

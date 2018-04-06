/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppMatrix34.h
 * @ingroup MleDPPType
 *
 * This file defines the Matrix34 data type used by the Magic Lantern Digital
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


// Include system header files.
#include <stdio.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Digital Workprint header files.
#include "mle/DppMatrix34.h"

MLE_DWP_DATATYPE_SOURCE(MleDppMatrix34,"MlMatrix34",MleDwpMatrix34);


int MleDppMatrix34::write(MleDppActorGroupOutput *out,MleDwpDataUnion *data) const
{
	if ( data->m_u.v == NULL )
		return 1;
	
	out->writeOffsetLength(sizeof(float[4][3]));
#define ELEM(I)	out->writeFloat(((float *)data->m_u.v)[I])
	ELEM(0); ELEM(1); ELEM(2); ELEM(3);
	ELEM(4); ELEM(5); ELEM(6); ELEM(7);
	ELEM(8); ELEM(9); ELEM(10); ELEM(11);
#undef ELEM

	return 0;
}

void *
MleDppMatrix34::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppMatrix34::operator delete(void *p)
{
	mlFree(p);
}

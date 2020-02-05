/** @defgroup MleDPPType Magic Lantern Digital Playprint Library API - Type */

/**
 * @file DppVoidStarType.h
 * @ingroup MleDPPType
 *
 * This file implements the void * data type used by the Magic Lantern Digital
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

// Inlucde system header files.
#include <string.h>
#include <stdlib.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Inlclude Digital Workprint header files.
#include "mle/DwpMediaRef.h"
#include "mle/DppVoidStarType.h"
//#include "mle/DppMediaRef.h"
//#include "mle/DppActorGroupOutput.h"

MLE_DWP_DATATYPE_SOURCE(MleDppMediaRef,"MediaRef",MleDppVoidStar);

MLE_DWP_DATATYPE_SOURCE(MleDppVoidStar,"void*",MleDwpVoidStar);

// Used by bsearch to find actor with name key
static int FindMediaRefs(const void* key, const void* mediaRef)
{
	return strcmp((const char*) key, (*(const MleDwpMediaRef**) mediaRef)->getName());
}

int
MleDppVoidStar::write(MleDppActorGroupOutput *out,MleDwpDataUnion *data) const
{
	out->writeOffsetLength(0);
	MleDwpMediaRef **mediaPtr = (MleDwpMediaRef**) bsearch((char *) data->m_u.v,
		out->m_references, out->m_numReferences,
		sizeof(out->m_references[0]), FindMediaRefs);
	return mediaPtr != NULL ? out->writeIndex(mediaPtr - out->m_references) : 1;
}

void *
MleDppVoidStar::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppVoidStar::operator delete(void *p)
{
	mlFree(p);
}

void *
MleDppMediaRef::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppMediaRef::operator delete(void *p)
{
	mlFree(p);
}

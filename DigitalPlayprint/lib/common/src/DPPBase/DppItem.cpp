/** @defgroup MleDPPBase Magic Lantern Digital Playprint Library API - Base */

/**
 * @file DwpItem.cpp
 * @ingroup MleDPPBase
 *
 * This file implements the digital workprint item used by the Magic Lantern Digital
 * Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 14, 2003
 */

// COPYRIGHT_BEGIN
//
// The MIT License (MIT)
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
#include "mle/mlAssert.h"

// Include Digital Playprint header files.
#include "mle/DppItem.h"


int MleDppItem::writeContents(MleDppActorGroupOutput *)
{
	return 0;
}

int MleDppItem::writeContents(MleDppSceneOutput *)
{
	return 0;
}

#if 0
int MleDppItem::writeContents(MleDppMediaOutput *)
{
	return 0;
}
#endif /* 0 */

int MleDppItem::write(MleDppActorGroupOutput *out)
{
	MLE_ASSERT(out);

	writeContents(out);

	/* Write any children. */
	MleDwpItem *child = m_dwpItem->getFirstChild();
	while ( child )
	{
		((MleDppItem *)child)->write(out);
		child = child->getNext();
	}
	return 0;
}

int MleDppItem::write(MleDppSceneOutput *out)
{
	MLE_ASSERT(out);

	writeContents(out);

	/* Write any children. */
	MleDwpItem *child = m_dwpItem->getFirstChild();
	while ( child )
	{
		((MleDppItem *)child)->write(out);
		child = child->getNext();
	}
	return 0;
}

void *
MleDppItem::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppItem::operator delete(void *p)
{
	mlFree(p);
}

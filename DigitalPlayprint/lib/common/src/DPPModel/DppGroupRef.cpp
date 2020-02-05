/** @defgroup MleDPPModel Magic Lantern Digital Playprint Library API - Model */

/**
 * @file DppGroupRef.h
 * @ingroup MleDPPModel
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 14, 2004
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
#include <stdlib.h>
#include <string.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Digital Workprint header files.
#include "mle/DwpDatatype.h"
#include "mle/DwpFinder.h"
#include "mle/DwpGroup.h"
#include "mle/DwpGroupRef.h"
#include "mle/DwpInput.h"
#include "mle/DwpOutput.h"

// Include Digital Workprint header files.
#include "mle/DppGroupRef.h"
#include "mle/DppActorGroupOutput.h"


MLE_DWP_SOURCE(MleDppGroupRef,MleDwpGroupRef);

MleDppGroupRef::MleDppGroupRef(void)
{
	m_dwpItem = this;
}

MleDppGroupRef::~MleDppGroupRef(void)
{
    // Do nothing for now.
}

int
MleDppGroupRef::writeContents(MleDppActorGroupOutput *)
{
    // Our name is written by the base class write() method
    // so we have nothing to add.

    return 0;
}

int
MleDppGroupRef::write(MleDppActorGroupOutput *)
{
    // Must start our search from the root to find the group we refer to.
    MleDwpItem *root = this;
    while ( root->getParent() )
	    root = root->getParent();

    // Find all the groups we refer to.
    MleDwpFinder groupFinder(MleDwpGroup::typeId, getName(), 1);
    groupFinder.find(root);

    MleDwpGroup *group = (MleDwpGroup *)groupFinder.getItem();
    if (NULL == group) {
	    fprintf(stderr,"Error: finding Group %s from GroupRef. Expecting 1 result, found %d\n", getName(), groupFinder.getNumItems());
	    return -1;
    }
    MLE_ASSERT(NULL != group);

    // The group will already be written to a playprint chunk from
    // elsewhere in the workprint, so there is really nothing we have
    // to write.

    return 0;
}

void *
MleDppGroupRef::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppGroupRef::operator delete(void *p)
{
	mlFree(p);
}

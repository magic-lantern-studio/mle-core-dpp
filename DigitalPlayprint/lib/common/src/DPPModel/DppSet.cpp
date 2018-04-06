/** @defgroup MleDPPModel Magic Lantern Digital Playprint Library API - Model */

/**
 * @file DppSet.cpp
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
#include <stdio.h>
#include <string.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

//Include Digital Workprint header files.
#include "mle/DwpSet.h"
#include "mle/DwpInput.h"
#include "mle/DwpOutput.h"

//Include Digital Playprint header files.
#include "mle/DppSet.h"
#include "mle/DppProperty.h"
#include "mle/DppItem.h"
#include "mle/DppActorGroupOutput.h"


MLE_DWP_SOURCE(MleDppSet,MleDwpSet);

MleDppSet::MleDppSet(void)
{
	m_dwpItem = this;
}

MleDppSet::~MleDppSet(void)
{
    // Do nothing for now.
}

int
MleDppSet::writeContents(MleDppActorGroupOutput *out)
{
    int setNumber = out->m_table->setRuntimeName(this->getName());

    out->writeOpcode(createSetOpcode);
    out->writeIndex(setNumber);
    out->m_currentSetClass = this->getType();

    return 0;
}

int
MleDppSet::write(MleDppActorGroupOutput *out)
{
    char setChunkFile[FILENAME_MAX];
    MleDwpItem *child;

    /*
     * Open set chunk file.
     */
    sprintf(setChunkFile, "%s.chk", getName());
    out->openSetChunkFile(setChunkFile);

    /*
     * Write out the createSetOpcode and set index.
     */
    writeContents(out);

    /*
     * Write out set (properties) configuration information.
     */
    child = getFirstChild();
    while( child)
	{
		if (child->isa(MleDppProperty::typeId))
		{
			MleDppProperty *dppItem = (MleDppProperty *)child;
			MleDppItem *item = dppItem;
			item->write(out);
		}

	    //((MleDppItem *)child)->write(out);
	    child = child->getNext();
    }

    /*
     * Close set chunk file.
     */
    out->closeSetChunkFile();

    return 0;
}

void *
MleDppSet::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppSet::operator delete(void *p)
{
	mlFree(p);
}

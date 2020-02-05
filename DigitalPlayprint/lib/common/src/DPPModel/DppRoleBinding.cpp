/** @defgroup MleDPPModel Magic Lantern Digital Playprint Library API - Model */

/**
 * @file DppRoleBinding.h
 * @ingroup MleDPPModel
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 14, 2003
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

// Include Digital Workprint header files.
#include "mle/DwpRoleBinding.h"
#include "mle/DwpInput.h"
#include "mle/DwpOutput.h"

// Include Digital Playprint header files.
#include "mle/DppRoleBinding.h"
#include "mle/DppActorGroupOutput.h"
#include "mle/DppTblMgr.h"

MLE_DWP_SOURCE(MleDppRoleBinding,MleDwpRoleBinding);

MleDppRoleBinding::MleDppRoleBinding()
{
	m_dwpItem = this;
}

MleDppRoleBinding::~MleDppRoleBinding()
{
    // Do nothing for now.
}

int
MleDppRoleBinding::writeContents(MleDppActorGroupOutput *out)
{
	// Write out the set id.
	out->writeOpcode(setSetOpcode);
	int setNumber = out->m_table->setRuntimeName(getSet());
	out->writeIndex(setNumber);

	// Write out the role class id.
	out->writeOpcode(bindRoleOpcode);
	int roleClassNumber =
		out->m_table->roleClassRuntimeName(getName()); 
	out->writeIndex(roleClassNumber);
	return 0;
}

void *
MleDppRoleBinding::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppRoleBinding::operator delete(void *p)
{
	mlFree(p);
}

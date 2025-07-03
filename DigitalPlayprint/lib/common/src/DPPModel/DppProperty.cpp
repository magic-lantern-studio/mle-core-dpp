/** @defgroup MleDPPModel Magic Lantern Digital Playprint Library API - Model */

/**
 * @file DppProperty.cpp
 * @ingroup MleDPPModel
 *
 * Magic Lantern Digital Playprint Library API.
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
#include <string.h>

// Include Magic Lantern header files.
#include "mle/mlMalloc.h"

// Include Digital Workprint header files.
#include "mle/DwpDatatype.h"

// Include Digital Playprint header files.
#include "mle/DppProperty.h"
#include "mle/DppDatatype.h"
#include "mle/DppInt.h"
#include "mle/DppIntArray.h"
#include "mle/DppFloat.h"
#include "mle/DppFloat3.h"
#include "mle/DppFloat4.h"
#include "mle/DppFloatArray.h"
#include "mle/DppScalar.h"
#include "mle/DppScalarArray.h"
#include "mle/DppString.h"
#include "mle/DppMatrix34.h"
#include "mle/DppRotation.h"
#include "mle/DppTransform.h"
#include "mle/DppVector2.h"
#include "mle/DppVector3.h"
#include "mle/DppVector4.h"
#include "mle/DppVector3Array.h"
#include "mle/DppEnum.h"
#include "mle/DppVoidStarType.h"



// Include Runtime Player header files.
//#include "mle/actorclass.h"


MLE_DWP_SOURCE(MleDppProperty,MleDwpProperty);

MleDppProperty::MleDppProperty()
{
	m_dwpItem = this;
}

MleDppProperty::~MleDppProperty()
{
    // Do nothig for now.
}

int
MleDppProperty::writeContents(MleDppActorGroupOutput *out)
{
	if ( m_data.m_datatype == NULL )
		return 1;
    
	out->m_currentProperty = this->getName(); // This is used later to find the property offset.
	//((MleDppDatatype *)m_data.m_datatype)->write(out,&m_data);
	MleDppDatatype *datatype = NULL;
	if ((m_data.m_datatype)->isa(MleDppInt::typeId))
	{
		MleDppInt *type = (MleDppInt *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppIntArray::typeId))
	{
		MleDppIntArray *type = (MleDppIntArray *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppFloat::typeId))
	{
		MleDppFloat *type = (MleDppFloat *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppFloat3::typeId))
	{
		MleDppFloat3 *type = (MleDppFloat3 *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppFloat4::typeId))
	{
		MleDppFloat4 *type = (MleDppFloat4 *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppFloatArray::typeId))
	{
		MleDppFloatArray *type = (MleDppFloatArray *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppScalar::typeId))
	{
		MleDppScalar *type = (MleDppScalar *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppScalarArray::typeId))
	{
		MleDppScalarArray *type = (MleDppScalarArray *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppMatrix34::typeId))
	{
		MleDppMatrix34 *type = (MleDppMatrix34 *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppRotation::typeId))
	{
		MleDppRotation *type = (MleDppRotation *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppTransform::typeId))
	{
		MleDppTransform *type = (MleDppTransform *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppString::typeId))
	{
		MleDppString *type = (MleDppString *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppEnum::typeId))
	{
		MleDppEnum *type = (MleDppEnum *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppVector2::typeId))
	{
		MleDppVector2 *type = (MleDppVector2 *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppVector3::typeId))
	{
		MleDppVector3 *type = (MleDppVector3 *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppVector4::typeId))
	{
		MleDppVector4 *type = (MleDppVector4 *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppVector3Array::typeId))
	{
		MleDppVector3Array *type = (MleDppVector3Array *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppMediaRef::typeId))
	{
		MleDppMediaRef *type = (MleDppMediaRef *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	} else if ((m_data.m_datatype)->isa(MleDppVoidStar::typeId))
	{
		MleDppVoidStar *type = (MleDppVoidStar *)m_data.m_datatype;
        datatype = (MleDppDatatype *)type;
	}

	if (datatype != NULL)
		datatype->write(out,&m_data);

	return 0;
}

void *
MleDppProperty::operator new(size_t tSize)
{
	void *p = mlMalloc(tSize);
	return p;
}

void
MleDppProperty::operator delete(void *p)
{
	mlFree(p);
}

void*
MleDppProperty::operator new[](size_t tSize)
{
	void* p = mlMalloc(tSize);
	return p;
}

void
MleDppProperty::operator delete[](void* p)
{
	mlFree(p);
}

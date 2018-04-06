/** @defgroup MleDPPBase Magic Lantern Digital Playprint Library API - Base */

/**
 * @file Dpp.cpp
 * @ingroup MleDPPBase
 *
 * Magic Lantern Digital Playprint Library API.
 *
 * @author Mark S. Millard
 * @date September 15, 2004
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

// Include system header files
#include <stdio.h>

// Include Magic Lantern header files.
#include "mle/mlAssert.h"

//Include Digital Playprint header files.
#include "mle/Dpp.h"

#include "mle/DppActor.h"
#include "mle/DwpActorDef.h"
#include "mle/DwpActorTemplate.h"
//#include "mle/DppForum.h"
//#include "mle/DwpForumDef.h"
//#include "mle/DppDelegateAttachment.h"
//#include "mle/DppDelegateBinding.h"
//#include "mle/DwpDelegateDef.h"
#include "mle/DppSet.h"
#include "mle/DwpSetDef.h"
#include "mle/DppRoleAttachment.h"
#include "mle/DppRoleBinding.h"
#include "mle/DwpRoleDef.h"
#include "mle/DwpDSOFile.h"
#include "mle/DppGroup.h"
#include "mle/DppScene.h"
#include "mle/DwpBoot.h"
#include "mle/DwpHeaderFile.h"
#include "mle/DwpInclude.h"
#include "mle/DwpMediaRef.h"
#include "mle/DwpMediaRefSource.h"
#include "mle/DwpMediaRefTarget.h"
#include "mle/DppProperty.h"
#include "mle/DwpPropertyDef.h"
#include "mle/DwpSourceFile.h"
#include "mle/DwpStage.h"
#include "mle/DwpStageDef.h"
#include "mle/DwpContainer.h"
#include "mle/DwpPackage.h"

#include "mle/DppInt.h"
#include "mle/DppFloat.h"
#include "mle/DppScalar.h"
#include "mle/DppVector2.h"
#include "mle/DppVector3.h"
#include "mle/DppVector4.h"
#include "mle/DppRotation.h"
#include "mle/DppTransform.h"
#include "mle/DppString.h"
#include "mle/DppIntArray.h"
#include "mle/DppFloatArray.h"
#include "mle/DppVoidStarType.h"
#include "mle/DppEnum.h"
#include "mle/DwpLimits.h"
//#include "mle/DwpDelegateForumMapping.h"
#include "mle/DwpRoleSetMapping.h"
#include "mle/DwpConverter.h"
#include "mle/DwpMedia.h"
#include "mle/DwpMediaRefClass.h"
#include "mle/DwpFiletype.h"

/*
 * This function initializes Magic Lantern system types.  It is possible
 * to link these in on demand using a DSO, but since they will be
 * commonly used it is simpler not to use that mechanism.
 */
void mleDppInit(void)
{
	static int called = 0;

	// Don't initialize more than once.
	//   Datatypes should be unique.  If they are created more
	//   than once, datatype comparison may fail.
	MLE_ASSERT(called == 0);
	called = 1;

	// Initialize Workprint items
	MleDwpActor::initClass();
	MleDwpActorDef::initClass();
	MleDwpActorTemplate::initClass();
	MleDwpBoot::initClass();
	MleDwpContainer::initClass();
	//MleDwpDelegateAttachment::initClass();
	//MleDwpDelegateBinding::initClass();
	MleDwpRoleAttachment::initClass();
	MleDwpRoleBinding::initClass();
	//MleDwpDelegateDef::initClass();
	MleDwpRoleDef::initClass();
	MleDwpDSOFile::initClass();
	//MleDppForum::initClass();
	//MleDwpForumDef::initClass();
	MleDppSet::initClass();
	MleDwpSetDef::initClass();
	MleDwpGroup::initClass();
	MleDwpScene::initClass();
	MleDwpHeaderFile::initClass();
	MleDwpInclude::initClass();
	MleDwpMediaRef::initClass();
	MleDwpSourceFile::initClass();
	MleDwpMediaRefTarget::initClass();
	MleDwpMediaRefSource::initClass();
	MleDwpPackage::initClass();
	MleDppProperty::initClass();
	MleDwpPropertyDef::initClass();
	MleDwpStage::initClass();
	MleDwpStageDef::initClass();

	// Initialize Playprint items
	MleDppActor::initClass();
	//MleDppDelegateAttachment::initClass();
	//MleDppDelegateBinding::initClass();
	MleDppRoleAttachment::initClass();
	MleDppRoleBinding::initClass();
	//MleDppForum::initClass();
	MleDppSet::initClass();
	MleDppGroup::initClass();
	MleDppScene::initClass();
	MleDppProperty::initClass();
	MleDwpPropertyDef::initClass();

	// Initialize property data types
	MleDppInt::initClass();
	MleDppFloat::initClass();
	MleDppScalar::initClass();
	MleDppVector2::initClass();
	MleDppVector3::initClass();
	MleDppVector4::initClass();
	MleDppRotation::initClass();
	MleDppTransform::initClass();
	MleDppIntArray::initClass();
	MleDppFloatArray::initClass();
	MleDppString::initClass();
	MleDwpMediaRefType::initClass();
	MleDppMediaRef::initClass();

	MleDppEnum::initClass();
	MleDwpLimits::initClass();
	//MleDwpDelegateForumMapping::initClass();
	MleDwpRoleSetMapping::initClass();
	MleDwpConverter::initClass();
	MleDwpMedia::initClass();
	MleDwpMediaRefClass::initClass();
	MleDwpFiletype::initClass();
}

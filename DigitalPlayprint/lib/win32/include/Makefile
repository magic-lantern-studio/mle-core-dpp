# -*- Makefile -*-
#
#  Copyright (C) 2000-2007  Wizzer Works
#
#  Wizzer Works makes available all content in this file ("Content").
#  Unless otherwise indicated below, the Content is provided to you
#  under the terms and conditions of the Common Public License Version 1.0
#  ("CPL"). A copy of the CPL is available at
#
#      http://opensource.org/licenses/cpl1.0.php
#
#  For purposes of the CPL, "Program" will mean the Content.
#
#  For information concerning this Makefile, contact Mark S. Millard,
#  of Wizzer Works at msm@wizzerworks.com.
#
#*************************************************************************
#
# Makefile for installing header files.
#
# $Id$
#
###########################################################################

ISM_DEPTH = ..
include $(ISM_DEPTH)/ismdefs

VPATH = mle:$(ISM_DEPTH)/../common/include/mle

BASE_COMMON_HEADERS = \
	agopcode.h \
	DppActorGroupOutput.h \
	DppItem.h \
	DppSceneOutput.h \
	DppTblMgr.h \
	$(NULL)

BASE_WIN32_HEADERS = \
	$(NULL)

TYPE_COMMON_HEADERS = \
	DppArray.h \
	DppDatatype.h \
	DppEnum.h \
	DppFloat.h \
	DppFloat3.h \
	DppFloat4.h \
	DppFloatArray.h \
	DppInt.h \
	DppIntArray.h \
	DppMatrix34.h \
	DppRotation.h \
	DppScalar.h \
	DppScalarArray.h \
	DppString.h \
	DppTransform.h \
	DppType.h \
	DppVector2.h \
	DppVector3.h \
	DppVector3Array.h \
	DppVector4.h \
	DppVoidStarType.h \
	$(NULL)

MODEL_COMMON_HEADERS = \
	DppActor.h \
	DppActorSet.h \
	DppGroup.h \
	DppGroupRef.h \
	DppProperty.h \
	DppRoleAttachment.h \
	DppRoleBinding.h \
	DppScene.h \
	DppSet.h \
	$(NULL)

DPP_COMMON_HEADERS = \
	Dpp.h \
	$(NULL)

COMMON_HEADERS = \
	$(BASE_COMMON_HEADERS) \
	$(TYPE_COMMON_HEADERS) \
	$(MODEL_COMMON_HEADERS) \
	$(DPP_COMMON_HEADERS) \
	$(NULL)

WIN32_HEADERS = \
	$(BASE_WIN32_HEADERS) \
	$(NULL)

HEADERS = \
	$(COMMON_HEADERS) \
	$(WIN32_HEADERS) \
	$(NULL)

install: $(HEADERS)
	$(INSTALL) -d $(MLE_ROOT)/include/mle
	$(INSTALL) $^ $(MLE_ROOT)/include/mle

include $(ISMRULES)

collect:
	$(IBCOLLECT) -baseDir $(ISM_DEPTH)/.. \
                     -log ../build/collect.raw \
                     -srcDir $(ISM_DEPTH)/../common/include/mle \
                     -destDir "include/mle" \
                     -tags "INCLUDE" \
                     $(COMMON_HEADERS)
	$(IBCOLLECT) -baseDir $(ISM_DEPTH)/.. \
                     -log ../build/collect.raw \
                     -srcDir mle \
                     -destDir "include/mle" \
                     -tags "INCLUDE" \
                     $(WIN32_HEADERS)

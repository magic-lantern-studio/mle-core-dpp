#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libDPP.la

#######################################
# Build information for each library

# Sources for libDPP
libDPP_la_SOURCES = \
	../../common/src/DPP/Dpp.cpp \
	../../common/src/DPPBase/DppItem.cpp \
	../../common/src/DPPBase/DppActorGroupOutput.cpp \
	../../common/src/DPPBase/DppSceneOutput.cpp \
	../../common/src/DPPBase/DppTblMgr.cpp \
	../../common/src/DPPType/DppArray.cpp \
	../../common/src/DPPType/DppEnum.cpp \
	../../common/src/DPPType/DppFloat.cpp \
	../../common/src/DPPType/DppFloat3.cpp \
	../../common/src/DPPType/DppFloat4.cpp \
	../../common/src/DPPType/DppFloatArray.cpp \
	../../common/src/DPPType/DppInt.cpp \
	../../common/src/DPPType/DppIntArray.cpp \
	../../common/src/DPPType/DppMatrix34.cpp \
	../../common/src/DPPType/DppRotation.cpp \
	../../common/src/DPPType/DppScalar.cpp \
	../../common/src/DPPType/DppScalarArray.cpp \
	../../common/src/DPPType/DppString.cpp \
	../../common/src/DPPType/DppTransform.cpp \
	../../common/src/DPPType/DppType.cpp \
	../../common/src/DPPType/DppVector2.cpp \
	../../common/src/DPPType/DppVector3.cpp \
	../../common/src/DPPType/DppVector3Array.cpp \
	../../common/src/DPPType/DppVector4.cpp \
	../../common/src/DPPType/DppVoidStarType.cpp \
	../../common/src/DPPModel/DppActor.cpp \
	../../common/src/DPPModel/DppActorSet.cpp \
	../../common/src/DPPModel/DppGroup.cpp \
	../../common/src/DPPModel/DppGroupRef.cpp \
	../../common/src/DPPModel/DppProperty.cpp \
	../../common/src/DPPModel/DppRoleAttachment.cpp \
	../../common/src/DPPModel/DppRoleBinding.cpp \
	../../common/src/DPPModel/DppScene.cpp \
	../../common/src/DPPModel/DppSet.cpp

# Linker options libTestProgram
libDPP_la_LDFLAGS =

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libDPP_la_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/../common/include \
	-I$(MLE_HOME)/DigitalWorkprint/lib/common/include \
	-I$(MLE_HOME)/DigitalWorkprint/lib/linux/include \
	-I$(MLE_HOME)/Core/util/common/include \
	-I$(MLE_HOME)/Core/util/linux/include \
	-I$(MLE_HOME)/Core/math/common/include


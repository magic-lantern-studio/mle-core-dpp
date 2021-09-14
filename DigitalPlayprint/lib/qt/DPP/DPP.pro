QT -= gui

TEMPLATE = lib
DEFINES += DPP_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += MLE_NOT_DLL DEBUG

INCLUDEPATH += $$PWD/../../common/include $$PWD/../../linux/include /opt/MagicLantern/include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/../../common/src/DPP/Dpp.cpp

SOURCES += \
    $$PWD/../../common/src/DPPBase/DppActorGroupOutput.cpp \
    $$PWD/../../common/src/DPPBase/DppItem.cpp \
    $$PWD/../../common/src/DPPBase/DppSceneOutput.cpp \
    $$PWD/../../common/src/DPPBase/DppTblMgr.cpp

SOURCES += \
    $$PWD/../../common/src/DPPType/DppArray.cpp \
    $$PWD/../../common/src/DPPType/DppEnum.cpp \
    $$PWD/../../common/src/DPPType/DppFloat.cpp \
    $$PWD/../../common/src/DPPType/DppFloat3.cpp \
    $$PWD/../../common/src/DPPType/DppFloat4.cpp \
    $$PWD/../../common/src/DPPType/DppFloatArray.cpp \
    $$PWD/../../common/src/DPPType/DppInt.cpp \
    $$PWD/../../common/src/DPPType/DppIntArray.cpp \
    $$PWD/../../common/src/DPPType/DppMatrix34.cpp \
    $$PWD/../../common/src/DPPType/DppRotation.cpp \
    $$PWD/../../common/src/DPPType/DppScalar.cpp \
    $$PWD/../../common/src/DPPType/DppScalarArray.cpp \
    $$PWD/../../common/src/DPPType/DppString.cpp \
    $$PWD/../../common/src/DPPType/DppTransform.cpp \
    $$PWD/../../common/src/DPPType/DppType.cpp \
    $$PWD/../../common/src/DPPType/DppVector2.cpp \
    $$PWD/../../common/src/DPPType/DppVector3.cpp \
    $$PWD/../../common/src/DPPType/DppVector3Array.cpp \
    $$PWD/../../common/src/DPPType/DppVector4.cpp \
    $$PWD/../../common/src/DPPType/DppVoidStarType.cpp

SOURCES += \
    $$PWD/../../common/src/DPPModel/DppActor.cpp \
    $$PWD/../../common/src/DPPModel/DppActorSet.cpp \
    $$PWD/../../common/src/DPPModel/DppGroup.cpp \
    $$PWD/../../common/src/DPPModel/DppGroupRef.cpp \
    $$PWD/../../common/src/DPPModel/DppProperty.cpp \
    $$PWD/../../common/src/DPPModel/DppRoleAttachment.cpp \
    $$PWD/../../common/src/DPPModel/DppRoleBinding.cpp \
    $$PWD/../../common/src/DPPModel/DppScene.cpp \
    $$PWD/../../common/src/DPPModel/DppSet.cpp

HEADERS += \
    $$PWD/../../common/include/mle/Dpp.h \
    $$PWD/../../common/include/mle/DppActor.h \
    $$PWD/../../common/include/mle/DppActorGroupOutput.h \
    $$PWD/../../common/include/mle/DppActorSet.h \
    $$PWD/../../common/include/mle/DppArray.h \
    $$PWD/../../common/include/mle/DppDatatype.h \
    $$PWD/../../common/include/mle/DppEnum.h \
    $$PWD/../../common/include/mle/DppFloat.h \
    $$PWD/../../common/include/mle/DppFloat3.h \
    $$PWD/../../common/include/mle/DppFloat4.h \
    $$PWD/../../common/include/mle/DppFloatArray.h \
    $$PWD/../../common/include/mle/DppGroup.h \
    $$PWD/../../common/include/mle/DppGroupRef.h \
    $$PWD/../../common/include/mle/DppInt.h \
    $$PWD/../../common/include/mle/DppIntArray.h \
    $$PWD/../../common/include/mle/DppItem.h \
    $$PWD/../../common/include/mle/DppMatrix34.h \
    $$PWD/../../common/include/mle/DppProperty.h \
    $$PWD/../../common/include/mle/DppRoleAttachment.h \
    $$PWD/../../common/include/mle/DppRoleBinding.h \
    $$PWD/../../common/include/mle/DppRotation.h \
    $$PWD/../../common/include/mle/DppScalar.h \
    $$PWD/../../common/include/mle/DppScalarArray.h \
    $$PWD/../../common/include/mle/DppScene.h \
    $$PWD/../../common/include/mle/DppSceneOutput.h \
    $$PWD/../../common/include/mle/DppSet.h \
    $$PWD/../../common/include/mle/DppString.h \
    $$PWD/../../common/include/mle/DppTblMgr.h \
    $$PWD/../../common/include/mle/DppTransform.h \
    $$PWD/../../common/include/mle/DppType.h \
    $$PWD/../../common/include/mle/DppVector2.h \
    $$PWD/../../common/include/mle/DppVector3.h \
    $$PWD/../../common/include/mle/DppVector3Array.h \
    $$PWD/../../common/include/mle/DppVector4.h \
    $$PWD/../../common/include/mle/DppVoidStarType.h \
    $$PWD/../../common/include/mle/agopcode.h

unix {
    LIBS += -L/opt/MagicLantern/lib -lDWP -lmlmath -lmlutil
}

# Default rules for deployment.
unix {
    target.path = /opt/MagicLantern/lib
    headers.path = /opt/MagicLantern/include/mle
    headers.files = $$HEADERS
    INSTALLS += target headers
}
!isEmpty(target.path): INSTALLS += target

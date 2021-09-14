QT -= gui

TEMPLATE = lib
DEFINES += DPPGEN_LIBRARY

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
        $$PWD/../../common/src/DPPGenerator/DppGenCode.cpp \
        $$PWD/../../common/src/DPPGenerator/DppGenMakefile.cpp

HEADERS += \
        $$PWD/../../common/include/mle/DppGenCode.h
        $$PWD/../../common/include/mle/DppGenMakefile.h

TEMPLATES = \
        $$PWD/../../common/src/DPPGenerator/TitleCppCode.tmpl \
        $$PWD/../../common/src/DPPGenerator/TitleJavaCode.tmpl \
        $$PWD/../../common/src/layout/TOCCppGlobals.tmpl \
        $$PWD/../../common/src/layout/TOCJavaGlobals.tmpl

unix {
    LIBS += -L/opt/MagicLantern/lib -lDPP -lDWP -lmlmath -lmlutil
}

# Default rules for deployment.
unix {
    target.path = /opt/MagicLantern/lib
    headers.path = /opt/MagicLantern/include/mle
    headers.files = $$HEADERS
    templates.path = /opt/MagicLantern/lib
    templates.files = $$TEMPLATES
    INSTALLS += target headers templates
}
!isEmpty(target.path): INSTALLS += target

QT -= gui

TEMPLATE = lib
DEFINES += PLAYPRINT_LIBRARY

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
    $$PWD/../../common/src/agchk.cxx \
    $$PWD/../../common/src/chkfile.cxx \
    $$PWD/../../common/src/chktable.cxx \
    $$PWD/../../common/src/mrefchk.cxx \
    $$PWD/../../common/src/playprnt.c \
    $$PWD/../../common/src/pp.cxx \
    $$PWD/../../common/src/ppinput.cxx \
    $$PWD/../../common/src/ppoutput.cxx \
    $$PWD/../../common/src/scenechk.cxx

HEADERS += \
    $$PWD/../../common/include/mle/agchk.h \
    $$PWD/../../common/include/mle/chkfile.h \
    $$PWD/../../common/include/mle/chktable.h \
    $$PWD/../../common/include/mle/chunk.h \
    $$PWD/../../common/include/mle/mrefchk.h \
    $$PWD/../../common/include/mle/playprnt.h \
    $$PWD/../../common/include/mle/pp.h \
    $$PWD/../../common/include/mle/ppinput.h \
    $$PWD/../../common/include/mle/ppoutput.h \
    $$PWD/../../common/include/mle/scenechk.h

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

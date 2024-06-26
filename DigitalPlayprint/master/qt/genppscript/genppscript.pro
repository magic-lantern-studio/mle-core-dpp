QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += USE_PYTHON MLE_NOT_DLL MLE_DEBUG DEBUG

INCLUDEPATH += $$PWD/../../common/include $$PWD/../../linux/include /opt/MagicLantern/include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        $$PWD/../../common/src/layout/genppscript.cpp

unix: QMAKE_LFLAGS_RPATH=
unix: QMAKE_LFLAGS += \
    "-Wl,-rpath,/opt/MagicLantern/lib"

unix:!macx {
    LIBS += -L/opt/MagicLantern/lib -lDPPGen -lDPP -lDWP -lplayprint -lmlmath -lmlutil -Wl,--no-as-needed -ldl
}

macx {
    LIBS += -L/opt/MagicLantern/lib -lDPPGen -lDPP -lDWP -lplayprint -lmlmath -lmlutil
}

# Default rules for deployment.
unix {
    target.path = /opt/MagicLantern/bin
    INSTALLS += target
}
#!isEmpty(target.path): INSTALLS += target

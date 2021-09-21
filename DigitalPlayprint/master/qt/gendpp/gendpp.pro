QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += MLE_NOT_DLL DEBUG


unix:!macx {
    INCLUDEPATH += /usr/include/python3.6
}

macx {
    INCLUDEPATH += /Library/Frameworks/Python.framework/Versions/Current/include/python3.6
}

INCLUDEPATH += $$PWD/../../common/include $$PWD/../../linux/include /opt/MagicLantern/include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        $$PWD/../../common/src/layout/pymain.cpp \
        $$PWD/../../common/src/layout/dppmodule.cpp

unix:!macx {
    LIBS += -L/opt/MagicLantern/lib -lDPPGen -lDPP -lDWP -lplayprint -lmlmath -lmlutil
    LIBS += -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m
    LIBS += -Wl,--no-as-needed -ldl
}

macx {
    LIBS += -L/opt/MagicLantern/lib -lDPPGen -lDPP -lDWP -lplayprint -lmlmath -lmlutil
    LIBS += -L/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/config/ -lpython3.6
}

# Default rules for deployment.
unix {
    target.path = /opt/MagicLantern/bin
    INSTALLS += target
}
!isEmpty(target.path): INSTALLS += target

copydata.commands = $(COPY_DIR) \"$$PWD/test\" \"$$OUT_PWD\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

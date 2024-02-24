bin_PROGRAMS = genppscript

genppscript_SOURCES = \
	../../../common/src/layout/genppscript.cpp

genppscript_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

genppscript_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-DUSE_PYTHON \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include \
	-std=c++17

genppscript_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed

if HAVE_LOG4CXX
#genppscript_SOURCES += \
#    source.h \
#    source.cpp
	
genppscript_CPPFLAGS += $(LOG4CXX_CFLAGS)
#genppscript_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

genppscript_LDFLAGS += -Wl,-rpath=/usr/local/lib
genppscript_LDADD += $(LOG4CXX_LIBS)
#genppscript_LDADD += -L/usr/local/lib -llog4cxx
endif
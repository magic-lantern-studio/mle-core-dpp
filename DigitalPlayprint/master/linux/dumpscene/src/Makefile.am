bin_PROGRAMS = dumpscene

dumpscene_SOURCES = ../../../common/src/dumpscene/dumpscene.cpp

dumpscene_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

dumpscene_CPPFLAGS = \
	-DMLE_NOT_UTIL_DLL \
	-DMLE_NOT_MATH_DLL \
	-DMLE_NOT_DWP_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include

dumpscene_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed

if HAVE_LOG4CXX
#dumpscene_SOURCES += \
#    source.h \
#    source.cpp
	
dumpscene_CPPFLAGS += $(LOG4CXX_CFLAGS)
#dumpscene_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

dumpscene_LDFLAGS += -Wl,-rpath=/usr/local/lib
dumpscene_LDADD += $(LOG4CXX_LIBS)
#dumpscene_LDADD += -L/usr/local/lib -llog4cxx
endif
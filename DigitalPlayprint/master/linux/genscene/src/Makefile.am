bin_PROGRAMS = genscene

genscene_SOURCES = ../../../common/src/genscene/genscene.cpp

genscene_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

genscene_CPPFLAGS = \
	-DMLE_NOT_UTIL_DLL \
	-DMLE_NOT_MATH_DLL \
	-DMLE_NOT_DWP_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include \
	-std=c++17

genscene_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed


if HAVE_LOG4CXX
#genscene_SOURCES += \
#    source.h \
#    source.cpp
	
genscene_CPPFLAGS += $(LOG4CXX_CFLAGS)
#genscene_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

genscene_LDFLAGS += -Wl,-rpath=/usr/local/lib
genscene_LDADD += $(LOG4CXX_LIBS)
#genscene_LDADD += -L/usr/local/lib -llog4cxx
endif
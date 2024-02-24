bin_PROGRAMS = gengroup

gengroup_SOURCES = ../../../common/src/gengroup/gengroup.cpp

gengroup_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

gengroup_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include \
	-std=c++17

gengroup_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed

if HAVE_LOG4CXX
#gengroup_SOURCES += \
#    source.h \
#    source.cpp
	
gengroup_CPPFLAGS += $(LOG4CXX_CFLAGS)
#gengroup_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

gengroup_LDFLAGS += -Wl,-rpath=/usr/local/lib
gengroup_LDADD += $(LOG4CXX_LIBS)
#gengroup_LDADD += -L/usr/local/lib -llog4cxx
endif

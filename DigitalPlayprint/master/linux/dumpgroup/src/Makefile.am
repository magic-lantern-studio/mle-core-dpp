bin_PROGRAMS = dumpgroup

dumpgroup_SOURCES = ../../../common/src/dumpgroup/dumpgroup.cpp

dumpgroup_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

dumpgroup_CPPFLAGS = \
	-DMLE_NOT_UTIL_DLL \
	-DMLE_NOT_MATH_DLL \
	-DMLE_NOT_DWP_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include

dumpgroup_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed
	
if HAVE_LOG4CXX
#dumpgroup_SOURCES += \
#    source.h \
#    source.cpp
	
dumpgroup_CPPFLAGS += $(LOG4CXX_CFLAGS)
#dumpgroup_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

dumpgroup_LDFLAGS += -Wl,-rpath=/usr/local/lib
dumpgroup_LDADD += $(LOG4CXX_LIBS)
#dumpgroup_LDADD += -L/usr/local/lib -llog4cxx
endif
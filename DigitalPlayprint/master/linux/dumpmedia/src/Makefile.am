bin_PROGRAMS = dumpmedia

dumpmedia_SOURCES = ../../../common/src/dumpmedia/dumpmedia.cpp

#AM_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -ldl

dumpmedia_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.a \
	$(MLE_ROOT)/lib/libDPP.a \
	$(MLE_ROOT)/lib/libDWP.a \
	$(MLE_ROOT)/lib/libplayprint.a \
	$(MLE_ROOT)/lib/libmlutil.a \
	$(MLE_ROOT)/lib/libmlmath.a \
	-ldl

dumpmedia_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include

dumpmedia_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed
	
if HAVE_LOG4CXX
#dumpmedia_SOURCES += \
#    source.h \
#    source.cpp
	
dumpmedia_CPPFLAGS += $(LOG4CXX_CFLAGS)
#dumpmedia_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

dumpmedia_LDFLAGS += -Wl,-rpath=/usr/local/lib
dumpmedia_LDADD += $(LOG4CXX_LIBS)
#dumpmedia_LDADD += -L/usr/local/lib -llog4cxx
endif
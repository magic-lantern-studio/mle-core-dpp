bin_PROGRAMS = dumpdpp

dumpdpp_SOURCES = ../../../common/src/dumpdpp/dumpdpp.cpp

dumpdpp_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

dumpdpp_LDFLAGS = -Wl,--no-as-needed

dumpdpp_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include

dumpdpp_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed

if HAVE_LOG4CXX
#dumpdpp_SOURCES += \
#    source.h \
#    source.cpp
	
dumpdpp_CPPFLAGS += $(LOG4CXX_CFLAGS)
#dumpdpp_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

dumpdpp_LDFLAGS += -Wl,-rpath=/usr/local/lib
dumpdpp_LDADD += $(LOG4CXX_LIBS)
#gentables_LDADD += -L/usr/local/lib -llog4cxx
endif
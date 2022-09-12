bin_PROGRAMS = genmedia

genmedia_SOURCES = ../../../common/src/genmedia/genmedia.cpp

genmedia_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

genmedia_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_HOME)/DigitalWorkprint/lib/common/include \
	-I$(MLE_HOME)/DigitalWorkprint/lib/linux/include \
	-I$(MLE_HOME)/DigitalPlayprint/lib/common/include \
	-I$(MLE_HOME)/DigitalPlayprint/lib/linux/include \
	-I$(MLE_HOME)/Core/util/common/include \
	-I$(MLE_HOME)/Core/util/linux/include \
	-I$(MLE_HOME)/Core/math/common/include \
	-I$(MLE_HOME)/Core/math/linux/include

genmedia_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed

if HAVE_LOG4CXX
#genmedia_SOURCES += \
#    source.h \
#    source.cpp
	
genmedia_CPPFLAGS += $(LOG4CXX_CFLAGS)
#genmedia_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

genmedia_LDFLAGS += -Wl,-rpath=/usr/local/lib
genmedia_LDADD += $(LOG4CXX_LIBS)
#genmedia_LDADD += -L/usr/local/lib -llog4cxx
endif
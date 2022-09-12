bin_PROGRAMS = gentables

gentables_SOURCES = ../../../common/src/gentables/gentables.cpp

gentables_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

gentables_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(MLE_HOME)/DigitalWorkprint/lib/common/include \
	-I$(MLE_HOME)/DigitalWorkprint/lib/linux/include \
	-I$(MLE_HOME)/DigitalPlayprint/lib/common/include \
	-I$(MLE_HOME)/DigitalPlayprint/lib/linux/include \
	-I$(MLE_HOME)/Core/util/common/include \
	-I$(MLE_HOME)/Core/util/linux/include

gentables_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed

if HAVE_LOG4CXX
#gentables_SOURCES += \
#    source.h \
#    source.cpp
	
gentables_CPPFLAGS += $(LOG4CXX_CFLAGS)
#gentables_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

gentables_LDFLAGS += -Wl,-rpath=/usr/local/lib
gentables_LDADD += $(LOG4CXX_LIBS)
#gentables_LDADD += -L/usr/local/lib -llog4cxx
endif

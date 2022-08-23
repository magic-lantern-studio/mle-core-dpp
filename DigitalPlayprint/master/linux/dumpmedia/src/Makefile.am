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
	-I$(MLE_HOME)/DigitalWorkprint/lib/common/include \
	-I$(MLE_HOME)/DigitalWorkprint/lib/linux/include \
	-I$(MLE_HOME)/DigitalPlayprint/lib/common/include \
	-I$(MLE_HOME)/DigitalPlayprint/lib/linux/include \
	-I$(MLE_HOME)/Core/util/common/include \
	-I$(MLE_HOME)/Core/util/linux/include \
	-I$(MLE_HOME)/Core/math/common/include \
	-I$(MLE_HOME)/Core/math/linux/include

dumpmedia_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed
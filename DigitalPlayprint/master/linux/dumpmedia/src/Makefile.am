bin_PROGRAMS = dumpmedia

dumpmedia_SOURCES = ../../../common/src/dumpmedia/dumpmedia.cpp

#AM_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -ldl

dumpmedia_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.a \
	$(MLE_ROOT)/lib/libDPP.a \
	$(MLE_ROOT)/lib/libDWP.a \
	$(MLE_ROOT)/lib/libplayprint.a \
	$(MLE_ROOT)/lib/libmlutil.a \
	$(MLE_ROOT)/lib/libmlmath.a

dumpmedia_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(top_srcdir)/../../../../DigitalWorkprint/lib/common/include \
	-I$(top_srcdir)/../../../../DigitalWorkprint/lib/linux/include \
	-I$(top_srcdir)/../../../../DigitalPlayprint/lib/common/include \
	-I$(top_srcdir)/../../../../DigitalPlayprint/lib/linux/include \
	-I$(top_srcdir)/../../../../Core/util/common/include \
	-I$(top_srcdir)/../../../../Core/util/linux/include \
	-I$(top_srcdir)/../../../../Core/math/common/include \
	-I$(top_srcdir)/../../../../Core/math/linux/include
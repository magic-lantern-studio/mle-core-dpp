bin_PROGRAMS = genmakefile

genmakefile_SOURCES = ../../../common/src/genmakefile/genmakefile.cpp

genmakefile_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so

genmakefile_CPPFLAGS = \
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
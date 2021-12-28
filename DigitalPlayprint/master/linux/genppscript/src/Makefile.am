bin_PROGRAMS = genppscript

genppscript_SOURCES = \
	../../../common/src/layout/genppscript.cpp

genppscript_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl

genppscript_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-DUSE_PYTHON \
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

genppscript_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib
bin_PROGRAMS = gendpp

gendpp_SOURCES = \
	../../../common/src/layout/main.cpp \
	../../../common/src/layout/gencode.cpp

gendpp_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	$(TCL_HOME)/lib/libtcl8.6.a -ldl -lz -lpthread

gendpp_CPPFLAGS = \
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

gendpp_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib

if TCL_HOME_SET
## Add Tcl header files
gendpp_CPPFLAGS += 	-I$(TCL_HOME)/include
else
## Report error
Error: TCL_HOME environment variable must be set.
endif

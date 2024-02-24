bin_PROGRAMS = gendpp

gendpp_SOURCES = \
	../../../common/src/layout/pymain.cpp \
	../../../common/src/layout/gencode.cpp \
	../../../common/src/layout/dppmodule.cpp \
	../../../common/src/layout/DppLayoutManager.cpp

#gendpp_HEADERS = \
#    ../../../common/src/layout/DppLayoutManager.h

gendpp_LDADD = \
	$(MLE_ROOT)/lib/libDPPGen.so \
	$(MLE_ROOT)/lib/libDPP.so \
	$(MLE_ROOT)/lib/libDWP.so \
	$(MLE_ROOT)/lib/libplayprint.so \
	$(MLE_ROOT)/lib/libmlutil.so \
	$(MLE_ROOT)/lib/libmlmath.so \
	-ldl -lz -lpthread

# 	-L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -lpython3.6m

gendpp_CPPFLAGS = \
	-DMLE_NOT_DLL \
	-I$(top_srcdir)/../../common/include \
	-I$(top_srcdir)/../../../runtime/common/include \
	-I$(MLE_ROOT)/include \
	-std=c++17

# -I/usr/include/python3.6

gendpp_LDFLAGS = \
	-Wl,-rpath=$(MLE_ROOT)/lib \
	-Wl,--no-as-needed

if HAVE_LOG4CXX
#gendpp_SOURCES += \
#    source.h \
#    source.cpp
	
gendpp_CPPFLAGS += $(LOG4CXX_CFLAGS)
#gendpp_CPPFLAGS += -DLINUX -D_REENTRANT -D_GNU_SOURCE -I/usr/local/include -I/usr/include/apr-1.0

gendpp_LDFLAGS += -Wl,-rpath=/usr/local/lib
gendpp_LDADD += $(LOG4CXX_LIBS)
#gendpp_LDADD += -L/usr/local/lib -llog4cxx
endif

if HAVE_PYTHON
gendpp_CPPFLAGS += $(PYTHON_CPPFLAGS)
gendpp_LDFLAGS += $(PYTHON_LDFLAGS)
gendpp_LDADD += $(PYTHON_LIBS)
endif

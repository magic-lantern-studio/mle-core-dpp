#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libDPPGen.la

#######################################
# Build information for each library

# Sources for libDPPGen
libDPPGen_la_SOURCES = \
	../../../common/src/DPPGenerator/DppGenCode.cpp \
	../../../common/src/DPPGenerator/DppGenMakefile.cpp

# Linker options libTestProgram
libDPPGen_la_LDFLAGS = -version-info 1:0:0

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libDPPGen_la_CPPFLAGS = \
	-DMLE_NOT_UTIL_DLL \
	-DMLE_NOT_MATH_DLL \
	-DMLE_NOT_DWP_DLL \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/../../common/include \
	-I$(MLE_ROOT)/include \
	-std=c++17

#templates_dir = $(pkgdatadir)/lib
templates_dir = $(prefix)/lib
templates__DATA = \
	../../../common/src/DPPGenerator/TitleCppCode.tmpl \
	../../../common/src/DPPGenerator/TitleJavaCode.tmpl \
	../../../common/src/layout/TOCCppGlobals.tmpl \
	../../../common/src/layout/TOCJavaGlobals.tmpl

#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libplayprint.la

#######################################
# Build information for each library

# Sources for libplayprint
libplayprint_la_SOURCES = \
	../../common/src/agchk.cxx \
	../../common/src/chkfile.cxx \
	../../common/src/chktable.cxx \
	../../common/src/mrefchk.cxx \
	../../common/src/playprnt.c \
	../../common/src/pp.cxx \
	../../common/src/ppinput.cxx \
	../../common/src/ppoutput.cxx \
	../../common/src/scenechk.cxx

# Linker options libTestProgram
libplayprint_la_LDFLAGS = \
    -L/opt/MagicLantern/lib -lDWP -lmlmath -lmlutil \
    -version-info 1:0:0

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
libplayprint_la_CPPFLAGS = \
	-DMLE_NOT_UTIL_DLL \
	-DMLE_NOT_MATH_DLL \
	-DMLE_NOT_DWP_DLL \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/../common/include \
	-I$(top_srcdir)/../../lib/common/include \
	-I$(top_srcdir)/../../lib/linux/include \
	-I$(MLE_ROOT)/include \
	-std=c++17

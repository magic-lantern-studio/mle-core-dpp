dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(gengroup, 1.0)

AC_PREFIX_DEFAULT(/opt/MagicLantern)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_PROG_CXX

AC_SUBST([LOG4CXX_CFLAGS])
AC_SUBST([LOG4CXX_LIBS])

AC_ARG_WITH([log4cxx],
	[AS_HELP_STRING([--with-log4cxx], [enable log4cxx logging])],
	[],
	[with_log4cxx=no])

AS_IF([test "x$with_log4cxx" != xno],
	[
	PKG_CHECK_MODULES([LOG4CXX], [liblog4cxx])
	AM_CONDITIONAL(HAVE_LOG4CXX, true)
	AC_DEFINE(HAVE_LOG4CXX,[],[Use log4cxx])
	], [
	AM_CONDITIONAL(HAVE_LOG4CXX, false)
	])
	
AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT


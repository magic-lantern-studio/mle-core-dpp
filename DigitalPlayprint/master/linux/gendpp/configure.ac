dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(gendpp, 1.0)

AC_PREFIX_DEFAULT(/opt/MagicLantern)

AC_SUBST([TCL_HOME])
AM_CONDITIONAL([TCL_HOME_SET], [test ! -z "$TCL_HOME"])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_PROG_CXX

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT


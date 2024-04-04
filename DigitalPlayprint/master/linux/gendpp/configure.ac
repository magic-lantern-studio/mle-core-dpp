dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(gendpp, 1.0)

AC_PREFIX_DEFAULT(/opt/MagicLantern)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CXX

AC_SUBST([LOG4CXX_CFLAGS])
AC_SUBST([LOG4CXX_LIBS])

dnl Check if we are using log4cxx
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

dnl Python configuration

dnl Present configure with a command-line option to disable the python bindings.
AC_ARG_ENABLE([python],[AS_HELP_STRING([--disable-python],[build without the python bindings])],[enable_python=$enableval],[enable_python=check])

dnl Default behavior is to install the python bindings into subfolders of $prefix.
dnl However, this may require the user to set the PYTHONPATH environment variable.
dnl In order to avoid this, invoke configure with the --enable-python-integration option.
AC_ARG_ENABLE([python-integration],[AS_HELP_STRING([--enable-python-integration],[install the python bindings in the interpreters site-packages folder])],[enable_python_integration=$enableval],[enable_python_integration=check])

dnl Verify that we have a valid version of Python (>= version 2.5).
VALID_PYTHON=
dnl AM_PATH_PYTHON([3.6],, [:])
AM_PATH_PYTHON([3.6],[PYTHON_FOUND=true],[PYTHON_FOUND=false])
if test "x$PYTHON_FOUND" = xtrue ; then
    PYTHON_CPPFLAGS=
    PYTHON_LDFLAGS=
    AX_PYTHON_DEVEL
    if test "x$PYTHON" = x ; then
        if test "x$enable_python" = xyes ; then
            AC_MSG_ERROR([Incomplete Python development package])
        else
            AC_MSG_WARN([Incomplete Python development package])
        fi
        VALID_PYTHON=no
    else
        VALID_PYTHON=yes
    fi
fi

if test "x$VALID_PYTHON" = xyes ; then
	AC_MSG_NOTICE([Building with Python bindings])

	if test "x$enable_python_integration" = xyes ; then
        	pythondir=$PYTHON_SITE_PKG
        	pyexecdir=$PYTHON_SITE_PKG_EXEC
	fi

	AC_SUBST(PYTHONDIR,$pythondir)
	AC_SUBST(PKGPYTHONDIR,$pkgpythondir)
	AC_SUBST(PYEXECDIR,$pyexecdir)
	AC_SUBST(PKGPYEXECDIR,$pkgpyexecdir)
fi

AM_CONDITIONAL([HAVE_PYTHON],[test x$VALID_PYTHON = xyes])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT


dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.69)
AC_INIT(ghfaxwidgets, 0.0.0)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_HOST
AC_ISC_POSIX
AC_PROG_CC
AC_LANG_C
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE()
AM_MAINTAINER_MODE

LT_INIT
AC_SUBST([LIBTOOL_DEPS])

GHFW_MAJOR_VERSION=0
GHFW_MINOR_VERSION=0
GHFW_MICRO_VERSION=0
GHFW_INTERFACE_AGE=0
GHFW_BINARY_AGE=0

dnl # libtool versioning
dnl LT_RELEASE=$GHFW_MAJOR_VERSION.$GHFW_MINOR_VERSION
dnl LT_CURRENT=`expr $GHFW_MICRO_VERSION - $GHFW_INTERFACE_AGE`
dnl LT_REVISION=$GHFW_INTERFACE_AGE
dnl LT_AGE=`expr $GHFW_BINARY_AGE - $GHFW_INTERFACE_AGE`
dnl AC_SUBST(LT_RELEASE)
dnl AC_SUBST(LT_CURRENT)
dnl AC_SUBST(LT_REVISION)
dnl AC_SUBST(LT_AGE)

dnl AM_PROG_CC_STDC
dnl AC_HEADER_STDC
dnl AC_C_CONST
dnl AC_C_INLINE

AC_EXEEXT

case $host_os in
  *cygwin* ) CYGWIN=yes;
             CYGWIN=no;;
  *mingw32* ) MINGW32=yes;
              MINGW32=no;;
          * ) MINGW32=no;
              CYGWIN=no;;
esac

AC_PROG_INSTALL

WINDOWS_DETECTED=no

ALL_LINGUAS="de fr pl zh_TW.Big5"

dnl MANPAGES=ghfaxviewer.1
MANPAGES=

# are we configured from halifax parent dir or from independent ghfaxviewer
# subdir ?
if test "x$cache_file" == "x.././config.cache"; then
  SUPPSUBDIRS= 
  docsubdir=no
  if test ! -f docs/Makefile.in; then
    mkdir docs
    echo "install clean distclean dist:" > docs/Makefile.in
  fi
else
  SUPPSUBDIRS=docs
  docsubdir=yes
fi

AC_SUBST(SUPPSUBDIRS)

AC_ARG_ENABLE(debug,
  [  --enable-debug          warnings, debugging codes and messages],
    debug="$enableval", debug="no")

dnl # FreeBSD hacks
dnl if test "`uname`" = "FreeBSD"; then
dnl   AC_MSG_WARN([Compiling for FreeBSD])
dnl   if test -x /usr/X11R6/bin/gtk12-config; then
dnl     GTK_CONFIG=/usr/X11R6/bin/gtk12-config
dnl   fi
dnl   if test -x /usr/local/bin/glib12-config; then
dnl     GLIB_CONFIG=/usr/local/bin/glib12-config
dnl   fi 
dnl fi

dnl if test "$ac_cv_exeext" = ".exe"; then
dnl   WINDOWS_DETECTED=yes;
dnl fi

dnl case $ac_cv_build in
dnl   *cygwin*)
dnl     CYGWIN=yes
dnl     WINDOWS_DETECTED=yes;
dnl     ;;
dnl   *mingw*)
dnl     MINGW32=yes
dnl     WINDOWS_DETECTED=yes;
dnl     ;;
dnl esac

dnl OS=
dnl USING_GNOME=

GCC_CFLAGS=
DEBUG_CFLAGS=
DEBUG_LIBS=

WIN32_CFLAGS=
WIN32_LIBS=

GHFW_VERSION="$VERSION"
GHFW_CFLAGS=
GHFW_LIBS=
GHFW_INCLUDEDIR=
GHFW_LIBDIR=

dnl if test "$WINDOWS_DETECTED" = yes; then
dnl # under Windows
dnl OS=WIN32
dnl AC_MSG_WARN([It seems we are compiling for WIN32...])
dnl window_system=yes;

dnl AC_MSG_CHECKING([how we adapt to gcc])
dnl GCC_VERSION="`gcc --version`"
dnl if test $? != 0; then
dnl   AC_MSG_ERROR([Your gcc executable couldn't be found.])
dnl else
dnl   case $GCC_VERSION in
dnl     2.95*) AC_MSG_RESULT([-fnative-struct will be used])
dnl            GCC_CFLAGS="-fnative-struct"
dnl 	   ;;
dnl     egcs*) AC_MSG_RESULT([using egcs (GCC 2.95.2 is recommended)])
dnl            ;;
dnl     *) AC_MSG_ERROR([What version of GCC are you using dude?!])
dnl   esac
dnl fi

dnl AC_MSG_CHECKING([which version of the GCC port you are using])
dnl if test "$CYGWIN" = yes; then
dnl 	AC_MSG_RESULT([using cygwin, CFLAGS+=-mno-cygwin])
dnl 	GCC_CFLAGS="-mno-cygwin -I$includedir/w32api -I$includedir/mingw"
dnl 	WINDRES_INCLUDE_DIR="$includedir/w32api"
dnl else
dnl 	AC_MSG_RESULT([using mingw])
dnl 	WINDRES_INCLUDE_DIR=$includedir
dnl fi

dnl AC_CHECK_TOOL(WINDRES, windres, false)
dnl WINDRES="$WINDRES"

dnl AC_MSG_WARN(*** libtiff and GTK assumed to be present)
dnl GTK_LIBS="-L${libdir} -lgtk -lgdk -lglib"
dnl WIN32_LIBS="-L${libdir} -lcomdlg32 -lgnu-intl"

dnl USE_INCLUDED_LIBINTL="no"
dnl BUILD_INCLUDED_LIBINTL="no"
dnl USE_NLS="yes"
dnl CATALOGS="`for cats in $ALL_LINGUAS; do echo -n "$cats.gmo "; done`"
dnl GMSGFMT="msgfmt"

dnl AC_PATH_PROG(XGETTEXT, "xgettext")
dnl AC_PATH_PROG(GETTEXT, "gettext")
dnl AC_PATH_PROG(MSGFMT, "msgfmt")
dnl AC_PATH_PROG(GENCAT, "gencat")
dnl AC_PATH_PROG(MSGMERGE, "msgmerge")

dnl AC_SUBST(WINDRES_INCLUDE_DIR)
dnl AC_SUBST(USE_INCLUDED_LIBINTL)
dnl AC_SUBST(BUILD_INCLUDED_LIBINTL)
dnl AC_SUBST(USE_NLS)
dnl AC_SUBST(CATALOGS)
dnl AC_SUBST(XGETTEXT)
dnl AC_SUBST(GETTEXT)
dnl AC_SUBST(MSGFMT)
dnl AC_SUBST(GMSGFMT)
dnl AC_SUBST(GENCAT)
dnl AC_SUBST(MSGMERGE)

dnl AC_DEFINE(ENABLE_NLS)
dnl AC_DEFINE(HAVE_GETTEXT)
dnl AC_DEFINE(HAVE_LC_MESSAGES)

dnl dnl #undef HAVE_CATGETS
dnl dnl #undef HAVE_STPCPY
dnl dnl #undef HAVE_LIBSM


dnl else
dnl # under GNU
dnl OS=GNU

AM_GNU_GETTEXT([external])

dnl AC_PATH_X
dnl window_system=$have_x;

dnl AC_MSG_CHECKING([whether debugging is requested])
dnl if test "$debug" = "yes"; then
dnl     AC_MSG_RESULT([yes])
dnl     DEBUG_CFLAGS="-Wall -O0 -ggdb3"
dnl else
dnl     AC_MSG_RESULT([no])
dnl fi

AC_SUBST(GCC_CFLAGS)
AC_SUBST(DEBUG_CFLAGS)
AC_SUBST(DEBUG_LIBS)
dnl AC_SUBST(WIN32_CFLAGS)
dnl AC_SUBST(WIN32_LIBS)
dnl AC_SUBST(GTK_LIBS)
dnl AC_SUBST(GTK_CFLAGS)

PKG_CHECK_MODULES(GNOME, libgnomeui-2.0 >= 0.14.0)
AC_SUBST(GNOME_CFLAGS)
AC_SUBST(GNOME_LIBS)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_SUBST(PIXMAPS_DIR)
AC_SUBST(MANPAGES)
dnl AC_SUBST(RC_ICON_DIRECTIVE)

dnl Computing GHFW_values
GHFW_INCLUDEDIR="-I${includedir}"
GHFW_CFLAGS="$GNOMEUI_CFLAGS $GTK_CFLAGS $WIN32_CFLAGS"
GHFW_LIBDIR="-L${libdir}"
GHFW_LIBS="-lghfaxwidgets $GTK_LIBS $GNOMEUI_LIBS $WIN32_LIBS"
AC_SUBST(GHFW_VERSION)
AC_SUBST(GHFW_INCLUDEDIR)
AC_SUBST(GHFW_CFLAGS)
AC_SUBST(GHFW_LIBDIR)
AC_SUBST(GHFW_LIBS)

AC_SUBST(GHFW_MAJOR_VERSION)
AC_SUBST(GHFW_MINOR_VERSION)
AC_SUBST(GHFW_MICRO_VERSION)
AC_SUBST(GHFW_INTERFACE_AGE)
AC_SUBST(GHFW_BINARY_AGE)

dnl AC_SUBST(CFLAGS)

dnl AM_CONDITIONAL(TARGET_GNU, test $OS = GNU)
dnl AM_CONDITIONAL(TARGET_WIN32, test $OS = WIN32)
dnl AM_CONDITIONAL(GNOME, test x"$USING_GNOME" = xyes)

dnl to replace acconfig.h
dnl AC_DEFINE([HAVE_LIBSM], 0, [Autodefined if libSM is present.])

AC_OUTPUT([
Makefile
docs/Makefile
ghfaxwidgets.pc
ghfaxwidgets-uninstalled.pc
src/Makefile
src/pixmaps/Makefile
tests/Makefile
po/Makefile.in
po/Makefile
])

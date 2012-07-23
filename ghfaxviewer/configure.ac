dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.69)
AC_INIT(ghfaxviewer, 0.29.10)
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

AC_SUBST([LIBTOOL_DEPS])

dnl AC_LIBTOOL_WIN32_DLL
dnl AC_PROG_LIBTOOL
dnl AC_SUBST(LIBTOOL_DEPS)

dnl AM_PROG_CC_STDC
dnl AC_HEADER_STDC
dnl AC_C_CONST
dnl AC_C_INLINE

dnl AC_EXEEXT

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

MANPAGES=ghfaxviewer.1
PIXMAPS_DIR="$datadir/pixmaps/ghfaxviewer"

AM_GNU_GETTEXT([external])

dnl Test for libtiff
  
AC_CHECK_LIB(tiff, TIFFReadScanline,
            AC_CHECK_HEADER(tiffio.h, TIFF='tiff'; TIFF_LIBS="${LIBS} -ltiff",
                            AC_MSG_WARN(*** please install libtiff (TIFF header files not found) ***)),
            AC_CHECK_LIB(tiff, TIFFWriteScanline,
                         AC_CHECK_HEADER(tiffio.h, TIFF='tiff'; TIFF_LIBS="${LIBS} -ltiff -ljpeg -lz",
                                         AC_MSG_WARN(*** please install libtiff (TIFF header files not found) ***)),
                         AC_CHECK_LIB(tiff34, TIFFFlushData,
                                      AC_CHECK_HEADER(tiffio.h, TIFF='tiff'; TIFF_LIBS="${LDFLAGS} -ltiff34 -ljpeg -lz",
                                                      AC_MSG_WARN(*** please install libtiff (TIFF header files not found) ***)),
                                      AC_MSG_WARN(*** please install libtiff (TIFF library not found) ***),
                                     -ljpeg -lz -lm),
                         -ljpeg -lz -lm),
            -lm)

deps="$TIFF_LIBS"

AC_ARG_ENABLE(debug,
  [  --enable-debug          warnings, debugging codes and messages],
      debug="$enableval", debug="no")

AC_MSG_CHECKING([whether debugging is requested])
if test x"$debug" = x"yes"; then
    AC_MSG_RESULT([yes])
    CFLAGS="$CFLAGS -Wall -O0 -ggdb3"
else
    AC_MSG_RESULT([no])
fi

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

PKG_CHECK_MODULES(GNOME, libgnomeui-2.0 >= 0.14.0  libgnomeprintui-2.2 >= 0.14.0)
PKG_CHECK_MODULES(GHFAXWIDGETS, ghfaxwidgets >= 0.0.0)
AC_SUBST(GNOME_CFLAGS)
AC_SUBST(GNOME_LIBS)
AC_SUBST(GNOME_PRINT_CFLAGS)
AC_SUBST(GNOME_PRINT_LIBS)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_SUBST(GHFAXWIDGETS_CFLAGS)
AC_SUBST(GHFAXWIDGETS_LIBS)

AC_SUBST(SUPPSUBDIRS)

AC_SUBST(PIXMAPS_DIR)
AC_SUBST(MANPAGES)
AC_SUBST(TIFF_LIBS)
dnl AC_SUBST(RC_ICON_DIRECTIVE)

dnl AM_CONDITIONAL(TARGET_GNU, test $OS = GNU)
dnl AM_CONDITIONAL(TARGET_WIN32, test $OS = WIN32)
dnl AM_CONDITIONAL(GNOME, test x"$USING_GNOME" = xyes)
dnl AM_CONDITIONAL(WITHOUT_GNOMEPRINT, test "x$no_gnome_print" = xyes)

AC_OUTPUT([
Makefile
ghfaxviewer.spec
debian/Makefile
docs/Makefile
src/Makefile
src/ghfaxviewer-res.rc
src/pixmaps/Makefile
po/Makefile.in
po/Makefile
])

dnl if test x"$XPM2WICO" = x && test $OS = WIN32; then
dnl  AC_MSG_WARN([The xpm2wico utility was not found. As a result,
dnl	the application icon won't be generated, which won't arm anything
dnl	anyway.
dnl	To benefit from its functionalities, please get the latest version
dnl	of xpm2wico from:
  
dnl	ftp://ftp.gnu.org/gnu/halifax/tools])
dnl fi

# Copyright (C) 2001 Wolfgang Sourdeau <wolfgang@gnu.org>
#
# This file is free software; you may copy and/or distribute it with
# or without modifications, as long as this notice is preserved.
# This software is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.

# This file is part of the GNU HaliFAX Widgets library.
# Please send any bugs to <halifax-bugs@gnu.org>

dnl So that ghfaxwidgets.m4 can stands both in $(topdir)/macros and in
dnl /usr/share/aclocal...

dnl Test for GHfaxWidgets, and define GHFW_CFLAGS and GHFW_LIBS
dnl
AC_DEFUN(AM_PATH_GHFW,
[dnl 
dnl Get the cflags and libraries from the ghfw-config script
dnl
AC_ARG_WITH(ghfw-prefix,[  --with-ghfw-prefix=PFX   Prefix where GHfaxWidgets is installed (optional)],
            ghfw_config_prefix="$withval", ghfw_config_prefix="")
AC_ARG_WITH(ghfw-exec-prefix,[  --with-ghfw-exec-prefix=PFX Exec prefix where GHfaxWidgets is installed (optional)],
            ghfw_config_exec_prefix="$withval", ghfw_config_exec_prefix="")
AC_ARG_ENABLE(ghfwtest, [  --disable-ghfwtest       Do not try to compile and run a test GHfaxWidgets program],
		    , enable_ghfwtest=yes)

  if test x$ghfw_config_exec_prefix != x ; then
     ghfw_config_args="$ghfw_config_args --exec-prefix=$ghfw_config_exec_prefix"
     if test x${GHFW_CONFIG+set} != xset ; then
        GHFW_CONFIG=$ghfw_config_exec_prefix/bin/ghfw-config
     fi
  fi
  if test x$ghfw_config_prefix != x ; then
     ghfw_config_args="$ghfw_config_args --prefix=$ghfw_config_prefix"
     if test x${GHFW_CONFIG+set} != xset ; then
        GHFW_CONFIG=$ghfw_config_prefix/bin/ghfw-config
     fi
  fi

  AC_PATH_PROG(GHFW_CONFIG, ghfw-config, no)
  min_ghfw_version=ifelse([$1], ,0.0.0,$1)
  AC_MSG_CHECKING(for GHfaxWidgets - version >= $min_ghfw_version)
  no_ghfw=""
  if test "$GHFW_CONFIG" = "no" ; then
    no_ghfw=yes
  else
    GHFW_CFLAGS=`$GHFW_CONFIG $ghfw_config_args --cflags`
    GHFW_LIBS=`$GHFW_CONFIG $ghfw_config_args --libs`
    ghfw_config_major_version=`$GHFW_CONFIG $ghfw_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ghfw_config_minor_version=`$GHFW_CONFIG $ghfw_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ghfw_config_micro_version=`$GHFW_CONFIG $ghfw_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_ghfwtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GHFW_CFLAGS"
      LIBS="$GHFW_LIBS $LIBS"
dnl
dnl Now check if the installed GHfaxWidgets is sufficiently new. (Also sanity
dnl checks the results of ghfw-config to some extent
dnl
      rm -f conf.ghfwtest
      AC_TRY_RUN([
#include <glib.h>
#include <ghfaxwidgets/ghfwversion.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.ghfwtest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_ghfw_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_ghfw_version");
     exit(1);
   }

  if ((ghfw_major_version != $ghfw_config_major_version) ||
      (ghfw_minor_version != $ghfw_config_minor_version) ||
      (ghfw_micro_version != $ghfw_config_micro_version))
    {
      printf("\n*** 'ghfw-config --version' returned %d.%d.%d, but GHfaxWidgets (%d.%d.%d)\n", 
             $ghfw_config_major_version, $ghfw_config_minor_version, $ghfw_config_micro_version,
             ghfw_major_version, ghfw_minor_version, ghfw_micro_version);
      printf ("*** was found! If ghfw-config was correct, then it is best\n");
      printf ("*** to remove the old version of GHfaxWidgets. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If ghfw-config was wrong, set the environment variable GHFW_CONFIG\n");
      printf("*** to point to the correct copy of ghfw-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
#if defined (GHFW_MAJOR_VERSION) && defined (GHFW_MINOR_VERSION) && defined (GHFW_MICRO_VERSION)
  else if ((ghfw_major_version != GHFW_MAJOR_VERSION) ||
	   (ghfw_minor_version != GHFW_MINOR_VERSION) ||
           (ghfw_micro_version != GHFW_MICRO_VERSION))
    {
      printf("*** GHfaxWidgets header files (version %d.%d.%d) do not match\n",
	     GHFW_MAJOR_VERSION, GHFW_MINOR_VERSION, GHFW_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     ghfw_major_version, ghfw_minor_version, ghfw_micro_version);
    }
#endif /* defined (GHFW_MAJOR_VERSION) ... */
  else
    {
      if ((ghfw_major_version > major) ||
        ((ghfw_major_version == major) && (ghfw_minor_version > minor)) ||
        ((ghfw_major_version == major) && (ghfw_minor_version == minor) && (ghfw_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GHfaxWidgets (%d.%d.%d) was found.\n",
               ghfw_major_version, ghfw_minor_version, ghfw_micro_version);
        printf("*** You need a version of GHfaxWidgets newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GHfaxWidgets is always available from ftp://ftp.gnu.org/gnu/halifax/.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the ghfw-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GHfaxWidgets, but you can also set the GHFW_CONFIG environment to point to the\n");
        printf("*** correct copy of ghfw-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_ghfw=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_ghfw" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GHFW_CONFIG" = "no" ; then
       echo "*** The ghfw-config script installed by GHfaxWidgets could not be found"
       echo "*** If GHfaxWidgets was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GHFW_CONFIG environment variable to the"
       echo "*** full path to ghfw-config."
     else
       if test -f conf.ghfwtest ; then
        :
       else
          echo "*** Could not run GHfaxWidgets test program, checking why..."
          CFLAGS="$CFLAGS $GHFW_CFLAGS"
          LIBS="$LIBS $GHFW_LIBS"
          AC_TRY_LINK([
#include <glib.h>
#include <ghfaxwidgets/ghfwversion.h>
#include <stdio.h>
],      [ return ((ghfw_major_version) || (ghfw_minor_version) || (ghfw_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GHfaxWidgets or finding the wrong"
          echo "*** version of GHfaxWidgets. If it is not finding GHfaxWidgets, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GHfaxWidgets package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps ghfw ghfw-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GHfaxWidgets was incorrectly installed"
          echo "*** or that you have moved GHfaxWidgets since it was installed. In the latter case, you"
          echo "*** may want to edit the ghfw-config script: $GHFW_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GHFW_CFLAGS=""
     GHFW_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GHFW_CFLAGS)
  AC_SUBST(GHFW_LIBS)
  rm -f conf.ghfwtest
])

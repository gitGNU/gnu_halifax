noinst_PROGRAMS = ghfwtest dlg_test

AM_CFLAGS = @DEBUG_CFLAGS@ -I$(top_srcdir) -I$(top_srcdir)/src @GNOME_CFLAGS@
AM_LDFLAGS = $(top_builddir)/src/libghfaxwidgets.la @GNOME_LIBS@

ghfwtest_SOURCES = ghfwtest.c
dlg_test_SOURCES = dlg_test.c


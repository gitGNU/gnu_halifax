noinst_PROGRAMS = ghfwtest dlg_test

CFLAGS = @DEBUG_CFLAGS@ @WIN32_CFLAGS@ -I$(top_srcdir) -I$(top_srcdir)/src @GNOMEUI_CFLAGS@ @GTK_CFLAGS@
LDFLAGS = $(top_builddir)/src/libghfax_widgets.la 

ghfwtest_SOURCES = ghfwtest.c
dlg_test_SOURCES = dlg_test.c


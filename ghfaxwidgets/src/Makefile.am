SUBDIRS = pixmaps

lib_LTLIBRARIES = libghfax_widgets.la
noinst_PROGRAMS = ghfwtest

if TARGET_WIN32
LIBS = $(GTK_LIBS)
else
LIBS =
endif

# libghfax_widgets.la

libghfax_widgetsincludedir = $(includedir)/ghfaxwidgets

libghfax_widgets_la_SOURCES = ghfwdialogwindow.c ghfwgtkutils.c \
				ghfwurlzone.c ghfwgdkutils.c ghfwprogress.c \
				ghfwthumbbox.c
libghfax_widgets_la_LDFLAGS = -version-info 0:0:0 -export-dynamic $(LIBS)

libghfax_widgetsinclude_HEADERS = ghfwdialogwindow.h ghfwprogress.h \
				ghfwurlzone.h ghfwgtkutils.h ghfwthumbbox.h

# ghfwtest
ghfwtest_SOURCES = ghfwtest.c
ghfwtest_INCLUDES = -I$(top_srcdir) $(GLIB_CFLAGS) $(GTK_CFLAGS)
ghfwtest_LDADD = $(top_builddir)/src/libghfax_widgets.la @LDFLAGS@ @GTK_LIBS@

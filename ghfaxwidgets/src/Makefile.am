SUBDIRS = pixmaps

lib_LTLIBRARIES = libghfax_widgets.la
noinst_PROGRAMS = ghfwtest

# libghfax_widgets.la

libghfax_widgetsincludedir = $(includedir)/ghfaxwidgets

libghfax_widgets_la_SOURCES = ghfwdialogwindow.c ghfwprogress.c \
				ghfwgtkutils.c ghfwgdkutils.c \
				ghfwurlzone.c ghfwthumbbox.c
libghfax_widgets_la_CFLAGS = @DEBUG_CFLAGS@ @WIN32_CFLAGS@ @GNOMEUI_CFLAGS@ @GTK_CFLAGS@ @WIN32_CFLAGS@
libghfax_widgets_la_LDFLAGS = -release $(LT_RELEASE) \
		-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		-export-dynamic \
		-no-undefined \
		@DEBUG_LIBS@ \
		@GNOMEUI_LIBS@ \
		@GTK_LIBS@ \
		@WIN32_LIBS@

libghfax_widgetsinclude_HEADERS = ghfwdialogwindow.h ghfwprogress.h \
				ghfwurlzone.h ghfwthumbbox.h \
				ghfwgtkutils.h ghfwgdkutils.h \
				i18n.h

# ghfwtest
ghfwtest_SOURCES = ghfwtest.c
ghfwtest_INCLUDES = @DEBUG_CFLAGS@ @WIN32_CFLAGS@ -I$(top_srcdir) @GNOMEUI_CFLAGS@ @GTK_CFLAGS@
ghfwtest_LDADD = $(top_builddir)/src/libghfax_widgets.la 

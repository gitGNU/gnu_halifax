SUBDIRS = pixmaps

lib_LTLIBRARIES = libghfax_widgets.la

noinst_HEADERS = i18n.h

# libghfax_widgets.la

libghfax_widgetsincludedir = $(includedir)/ghfaxwidgets

libghfax_widgets_la_SOURCES = ghfwdlgwindow.c ghfwprogress.c \
				ghfwgtkutils.c ghfwgdkutils.c \
				ghfwurlzone.c ghfwthumbbox.c \
				ghfaxwidgets.c

libghfax_widgets_la_CFLAGS = @DEBUG_CFLAGS@ @WIN32_CFLAGS@ @GNOMEUI_CFLAGS@ @GTK_CFLAGS@ @WIN32_CFLAGS@
libghfax_widgets_la_LDFLAGS = -release $(LT_RELEASE) \
		-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		-export-dynamic \
		-no-undefined \
		@DEBUG_LIBS@ \
		@GNOMEUI_LIBS@ \
		@GTK_LIBS@ \
		@WIN32_LIBS@

libghfax_widgetsinclude_HEADERS = ghfwdlgwindow.h ghfwprogress.h \
				ghfwurlzone.h ghfwthumbbox.h \
				ghfwgtkutils.h ghfwgdkutils.h \
				ghfaxwidgets.h

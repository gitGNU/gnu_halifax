SUBDIRS = pixmaps

GHFW_MAJOR_VERSION=@GHFW_MAJOR_VERSION@
GHFW_MINOR_VERSION=@GHFW_MINOR_VERSION@
GHFW_MICRO_VERSION=@GHFW_MICRO_VERSION@
GHFW_BINARY_AGE=@GHFW_BINARY_AGE@
GHFW_INTERFACE_AGE=@GHFW_INTERFACE_AGE@

lib_LTLIBRARIES = libghfaxwidgets.la

noinst_HEADERS = i18n.h

CPPFLAGS = -DGHFW_MAJOR_VERSION=$(GHFW_MAJOR_VERSION) \
	-DGHFW_MINOR_VERSION=$(GHFW_MINOR_VERSION) \
	-DGHFW_MICRO_VERSION=$(GHFW_MICRO_VERSION) \
	-DGHFW_INTERFACE_AGE=$(GHFW_INTERFACE_AGE) \
	-DGHFW_BINARY_AGE=$(GHFW_BINARY_AGE) \
	-DNEED_GNOMESUPPORT_H \
	-DGNOME_DISABLE_DEPRECATED \
	-DGTK_DISABLE_DEPRECATED \
	-DG_DISABLE_DEPRECATED 

CFLAGS = @CFLAGS@ @DEBUG_CFLAGS@ @GNOME_CFLAGS@

# libghfaxwidgets.la

libghfaxwidgetsincludedir = $(includedir)/ghfaxwidgets

libghfaxwidgets_la_SOURCES = ghfwdlgwindow.c ghfwprogress.c \
				ghfwgtkutils.c ghfwgdkutils.c \
				ghfwurlzone.c ghfwthumbbox.c \
				ghfaxwidgets.c

libghfaxwidgets_la_LDFLAGS = -release $(LT_RELEASE) \
		-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		-export-dynamic \
		-no-undefined \
		@DEBUG_LIBS@ \
		@GNOMEUI_LIBS@

libghfaxwidgetsinclude_HEADERS = ghfwdlgwindow.h ghfwprogress.h \
				ghfwurlzone.h ghfwthumbbox.h \
				ghfwgtkutils.h ghfwgdkutils.h \
				ghfaxwidgets.h ghfwversion.h

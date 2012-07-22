PACKAGE = @PACKAGE@
VERSION = @VERSION@
SUBDIRS = @SUPPSUBDIRS@ po src tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ghfaxwidgets.pc

$(pkgconfig_DATA): config.status

EXTRA_DIST = config.rpath m4/ChangeLog README ghfaxwidgets.pc.in ghfaxwidgets-uninstalled.pc.in

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

ACLOCAL_AMFLAGS = -I m4

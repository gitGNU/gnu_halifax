PACKAGE = @PACKAGE@
VERSION = @VERSION@
SUBDIRS = @SUPPSUBDIRS@ intl po src tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ghfaxwidgets.pc

$(pkgconfig_DATA): config.status

EXTRA_DIST = README ghfaxwidgets.pc.in ghfaxwidgets-uninstalled.pc.in

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

PACKAGE = @PACKAGE@
VERSION = @VERSION@
SUBDIRS = @SUPPSUBDIRS@ intl po src debian

EXTRA_DIST = README ghfaxviewer.spec Makefile.win32.in gfv_gtkrc

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck


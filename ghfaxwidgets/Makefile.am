PACKAGE = @PACKAGE@
VERSION = @VERSION@
SUBDIRS = @SUPPSUBDIRS@ intl po src

bin_SCRIPTS = ghfw-config

EXTRA_DIST = README ghfw-config.in

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck


PACKAGE = @PACKAGE@
VERSION = @VERSION@
SUBDIRS = @SUPPSUBDIRS@ intl po src macros

EXTRA_DIST = README 

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck


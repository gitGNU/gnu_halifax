SUBDIRS = pixmaps

gnulocaledir = $(datadir)/locale
pixmapsdir = @PIXMAPS_DIR@
gnome_graph_app_dir = @GNOME_GRAPH_APP_DIR@

LIBS = @GNOME_LIBS@ @GNOME_PRINT_LIBS@ @TIFF_LIBS@

INCLUDES = -I.. -I. @GHFW_CFLAGS@ @GNOME_CFLAGS@ @GNOME_PRINT_CFLAGS@

bin_PROGRAMS = ghfaxviewer

SYSDEPDEFS = -DLOCALEDIR="\"$(gnulocaledir)\"" -DPIXMAP_DIR="\"$(pixmapsdir)\""  -DNEED_GNOMESUPPORT_H -DGTK_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED -DGNOME_DISABLE_DEPRECATED

.rc.o:
	@WINDRES@ --include-dir=$(prefix)/include $< $@

endif
endif

DEFS = @DEFS@ $(SYSDEPDEFS)

SUFFIXES = .rc .o

noinst_HEADERS = setup.h tiffimages.h tiffinfo.h viewer.h cursors.h print.h \
		 menu.h toolbar.h errors.h i18n.h \
		 callbcks.h draw_page.h zoom.h winprint.c ps_print.c

ghfaxviewer_SOURCES = setup.c fileopen.c cursors.c draw_page.c \
		 thumbnails.c prefs.c ps_export.c \
		 tiffimages.c tiffinfo.c zoom.c viewer.c callbcks.c toolbar.c \
		 menu.c about.c errors.c info.c gn_print.c

ghfaxviewer_LDADD = @GHFW_LIBS@

EXTRA_DIST = ghfaxviewer.desktop ghfaxviewer-res.rc

if GNOME
install-data-local:
	$(mkinstalldirs) $(gnome_graph_app_dir)
	$(INSTALL_DATA) $(srcdir)/ghfaxviewer.desktop \
		$(gnome_graph_app_dir)

uninstall-local:
	test -f $(gnome_graph_app_dir)/ghfaxviewer.desktop && \
		rm -f $(gnome_graph_app_dir)/ghfaxviewer.desktop
	test -d $(gnome_graph_app_dir) && \
		rmdir --ignore-fail-on-non-empty $(gnome_graph_app_dir)

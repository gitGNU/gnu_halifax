SUBDIRS = pixmaps

gnulocaledir = $(datadir)/locale
pixmapsdir = @PIXMAPS_DIR@
gnome_graph_app_dir = @GNOME_GRAPH_APP_DIR@

LIBS = @GTK_LIBS@ @TIFF_LIBS@ @WIN32_LIBS@ @GNOME_PRINT_LIBS@ @GCONF_LIBS@

INCLUDES = -I.. -I. @GTK_CFLAGS@ @GNOME_PRINT_CFLAGS@ @GCONF_CFLAGS@

bin_PROGRAMS = ghfaxviewer

if TARGET_GNU
if WITHOUT_GNOMEPRINT
PRINTSRC = ps_print.c
NOPRINTSRC = winprint.c gn_print.c
else
PRINTSRC = gn_print.c
NOPRINTSRC = winprint.c ps_print.c
endif
RCSRC =
SYSDEPDEFS = -DLOCALEDIR="\"$(gnulocaledir)\"" -DPIXMAP_DIR="\"$(pixmapsdir)\""
else
if TARGET_WIN32
NOPRINTSRC = ps_print.c gn_print.c
PRINTSRC = winprint.c
RCSRC = ghfaxviewer-res.rc

.rc.o:
	@WINDRES@ --include-dir=$(prefix)/include $< $@

endif
endif

DEFS = @DEFS@ $(SYSDEPDEFS)

SUFFIXES = .rc .o

noinst_HEADERS = setup.h tiffimages.h tiffinfo.h viewer.h cursors.h print.h \
		 menu.h toolbar.h errors.h progress.h i18n.h gtkutils.h \
		 urlzone.h dialog_window.h \
		 thumblayout.h callbcks.h draw_page.h zoom.h $(NOPRINTSRC)

ghfaxviewer_SOURCES = setup.c fileopen.c cursors.c draw_page.c \
		 thumbnails.c thumblayout.c \
		 tiffimages.c tiffinfo.c zoom.c viewer.c callbcks.c toolbar.c \
		 menu.c about.c errors.c progress.c gtkutils.c info.c \
		 urlzone.c dialog_window.c \
		 $(PRINTSRC) $(RCSRC)

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
endif

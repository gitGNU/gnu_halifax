SUBDIRS = pixmaps

gnulocaledir = $(datadir)/locale

LIBS = @GTK_LIBS@ @TIFF_LIBS@ @WIN32_LIBS@ @GNOME_PRINT_LIBS@ @GCONF_LIBS@

DEFS = @DEFS@ -DLOCALEDIR="\"$(gnulocaledir)\"" -DPIXMAP_DIR="\"$(datadir)/pixmaps/@PACKAGE@\""

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
else
if TARGET_WIN32
NOPRINTSRC = ps_print.c gn_print.c
PRINTSRC = winprint.c
RCSRC = ghfaxviewer-res.rc

.rc.o:
	@WINDRES@ --include-dir=$(prefix)/include $< $@

endif
endif

SUFFIXES = .rc .o

noinst_HEADERS = setup.h tiffimages.h tiffinfo.h viewer.h cursors.h print.h \
		 menu.h toolbar.h errors.h progress.h i18n.h gtkutils.h \
		 callbcks.h draw_page.h zoom.h $(NOPRINTSRC)

ghfaxviewer_SOURCES = setup.c fileopen.c cursors.c draw_page.c thumbnails.c \
		 tiffimages.c tiffinfo.c zoom.c viewer.c callbcks.c toolbar.c \
		 menu.c about.c errors.c progress.c gtkutils.c info.c \
		 $(PRINTSRC) $(RCSRC)

EXTRA_DIST = ghfaxviewer.desktop ghfaxviewer-res.rc

if GNOME
GNOME_UTILITIES_DIR = $(prefix)/share/gnome/apps/Graphics

install-data-local:
	$(mkinstalldirs) $(GNOME_UTILITIES_DIR)
	$(INSTALL_DATA) $(srcdir)/ghfaxviewer.desktop \
		$(GNOME_UTILITIES_DIR)

uninstall-local:
	test -f $(GNOME_UTILITIES_DIR)/ghfaxviewer.desktop && \
		rm -f $(GNOME_UTILITIES_DIR)/ghfaxviewer.desktop
	test -d $(GNOME_UTILITIES_DIR) && \
		rmdir --ignore-fail-on-non-empty $(GNOME_UTILITIES_DIR)
endif

if TARGET_WIN32
if HAS_XPM2WICO
ghfaxviewer-icon.ico: ghfaxviewer-icon.xpm
	@XPM2WICO@ $<

all: ghfaxviewer-icon.ico
endif
endif

pixmapdir = $(prefix)/share/pixmaps/ghfaxviewer

if GNOME
pixmap_DATA = info.xpm ghfaxviewer-logo.xpm ghfaxviewer-icon.xpm
else
pixmap_DATA = info.xpm ghfaxviewer-logo.xpm ghfaxviewer-icon.xpm \
	      printer.xpm left90.xpm right90.xpm reverse.xpm \
              right_arrow.xpm left_arrow.xpm zoom_in.xpm zoom_out.xpm
endif

EXTRA_DIST = hand-closed-data.xbm hand-closed-mask.xbm \
	     hand-open-data.xbm hand-open-mask.xbm \
	     stock-zoom-out.xpm stock-zoom-in.xpm \
	     stock-zoom-out-menu.xpm stock-zoom-in-menu.xpm \
	     printer.xpm \
	     left90.xpm right90.xpm reverse.xpm \
             right_arrow.xpm left_arrow.xpm \
             zoom_in.xpm zoom_out.xpm info.xpm \
	     ghfaxviewer-logo.xpm ghfaxviewer-icon.xpm

uninstall-local:
	test -d $(pixmapdir) && \
	  rmdir --ignore-fail-on-non-empty $(pixmapdir)

/* about.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000-2001 Wolfgang Sourdeau
 *
 * Author: Wolfgang Sourdeau <wolfgang@contre.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

/* This file creates the about boxes */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define HALIFAX_URL "http://www.gnu.org/software/halifax/"
#ifdef __WIN32__
#define TEXT_FONT "-*-MS Sans Serif-medium-r-normal-*-*-100-*-*-p-*-*-*"
#else
#define TEXT_FONT "-adobe-helvetica-medium-r-normal-*-*-180-*-*-p-*-*-*"
#endif

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#include "urlzone.h"
#include "gtkutils.h"
#include "setup.h"
#include "i18n.h"

static
void about_pixmap_realize_cb (GtkWidget *ref_window, GtkWidget *pixmap)
{
  UrlZone *url_zone;
  GdkFont *gdk_font;
  gchar *text;
  gint text_len;

  url_zone = url_zone_new (HALIFAX_URL, 150, 280, 195, 17);
  url_zone_attach (url_zone, pixmap);

  gdk_font = gdk_font_load (TEXT_FONT);

  if (gdk_font)
    {
      text = _("The GNU HaliFAX Viewer");
      text_len = strlen (text);
      gdk_draw_text (GTK_PIXMAP (pixmap)->pixmap, gdk_font,
		     pixmap->style->black_gc,
		     128, 58,
		     text, text_len);
    }
  else
    g_print ("error loading font\n");
}

static GtkWidget*
about_content (GtkWidget *ref_window)
{
  GtkWidget *layout, *pixmap;

  layout = gtk_layout_new (NULL, NULL);
  gtk_widget_set_usize (layout, 350, 300);

  pixmap = pixmap_from_xpm (ref_window,
			    PIXMAP ("ghfaxviewer-logo.xpm"));
  gtk_layout_put (GTK_LAYOUT (layout), pixmap, 0, 0);

  gtk_signal_connect_after (GTK_OBJECT (pixmap), "realize",
			    (GtkSignalFunc) about_pixmap_realize_cb,
			    pixmap);

  return layout;
}

void
about_cb (GtkWidget *irrelevant, gpointer viewer_window)
{
  DialogWindow *about_dialog;
  GtkWidget *content, *ok_button;

  about_dialog = dialog_window_new (_("About..."));
  content = about_content (viewer_window);

  dialog_window_set_content_with_frame (about_dialog, content);

  ok_button = gtk_button_new_with_label (_("Close"));
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
		      dialog_window_destroy_from_signal,
		      about_dialog);

  dialog_window_set_button (about_dialog, ok_button);
  dialog_window_set_escapable (about_dialog);

  dialog_window_show (about_dialog, viewer_window);
}

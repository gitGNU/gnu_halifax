/* about.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000-2001 Wolfgang Sourdeau
 *
 * Time-stamp: <2003-03-07 02:01:08 wolfgang>
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

#include <string.h>
#include <ghfaxwidgets/ghfaxwidgets.h>

#include "setup.h"
#include "i18n.h"

static
void about_pixmap_realize_cb (GtkWidget *ref_window, GtkWidget *pixmap)
{
  UrlZone *url_zone;
  GdkFont *gdk_font;
  gchar *text;
  gint text_len;

  url_zone = url_zone_new (HALIFAX_URL);
  url_zone_attach (url_zone, pixmap, 150, 280, 195, 17);

  gdk_font = gdk_font_load (TEXT_FONT);

  if (gdk_font)
    {
      text = _("The GNU HaliFAX Viewer");
      text_len = strlen (text);
/*       gdk_draw_text (GTK_PIXMAP (pixmap)->pixmap, gdk_font, */
/* 		     pixmap->style->black_gc, */
/* 		     128, 58, */
/* 		     text, text_len); */
    }
  else
    g_print ("error loading font\n");
}

static GtkWidget*
about_content (GtkWidget *ref_window)
{
  GtkWidget *layout, *image;

  layout = gtk_layout_new (NULL, NULL);
  gtk_widget_set_size_request (layout, 350, 300);

  image = gtk_image_new_from_file (PIXMAP ("ghfaxviewer-logo.xpm"));
  gtk_layout_put (GTK_LAYOUT (layout), image, 0, 0);

  g_signal_connect_after (G_OBJECT (image), "realize",
			  G_CALLBACK (about_pixmap_realize_cb),
			  image);

  return layout;
}

void
about_cb (GtkWidget *irrelevant, gpointer viewer_window)
{
  GtkWidget *content, *ok_button;
  GhfwDlgWindow *about_dialog;

  about_dialog = ghfw_dlg_window_new (_("About..."));
  content = about_content (viewer_window);

  ghfw_dlg_window_set_content_with_frame ((GhfwDlgWindow*) about_dialog,
					  content);

  ok_button = gtk_button_new_with_label (_("Close"));
  g_signal_connect_swapped (G_OBJECT (ok_button), "clicked",
			    G_CALLBACK (gtk_widget_destroy),
			    G_OBJECT (about_dialog));

  ghfw_dlg_window_set_button ((GhfwDlgWindow*) about_dialog, ok_button);
  ghfw_dlg_window_set_escapable ((GhfwDlgWindow*) about_dialog);

  transient_window_show (GTK_WIDGET (about_dialog), viewer_window);
}

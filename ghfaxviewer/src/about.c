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

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#ifdef __WIN32__
#include <windows.h>
#endif

#include "setup.h"
#include "gtkutils.h"
#include "i18n.h"

static void
url_btn_clicked_cb (GtkObject *widget, gpointer user_data)
{
#ifdef __WIN32__
  ShellExecute (NULL, "open", HALIFAX_URL,
	        NULL, NULL, SW_SHOWNORMAL);
#else
#ifdef NEED_GNOMESUPPORT_H
  gnome_url_show (HALIFAX_URL);
#else
  gchar *browser, *params[3];
  gchar *test_dirs[] =
    {
      "/usr/bin",
      "/opt/bin",
      "/usr/local/bin",
      "/usr/local/netscape",
      "/opt/netscape",
      NULL
    };

  params[1] = HALIFAX_URL;
  params[2] = NULL;

  browser = where_is (test_dirs, "galeon");
  if (browser)
    params[0] = "-w";
  else
    {
      browser = where_is (test_dirs, "mozilla");
      params[0] = "--remote";

      if (!browser)
	browser = where_is (test_dirs, "netscape");
    }

  if (browser)
    {
      launch_program (browser, params);
      g_free (browser);
    }

#endif
#endif
}

static GtkWidget*
about_content (GtkWidget *ref_window)
{
  GtkWidget *layout, *pixmap, *link_btn;

  layout = gtk_layout_new (NULL, NULL);
  gtk_widget_set_usize (layout, 467, 393);

  link_btn = gtk_button_new_with_label (HALIFAX_URL);
  gtk_button_set_relief (GTK_BUTTON (link_btn), GTK_RELIEF_NONE);

  gtk_widget_set_usize (link_btn, -1, 20);
  gtk_layout_put (GTK_LAYOUT (layout), link_btn, 10, 363);
  gtk_signal_connect (GTK_OBJECT (link_btn),
		      "clicked",
		      url_btn_clicked_cb,
		      NULL);

  pixmap = pixmap_from_xpm (ref_window,
			    PIXMAP ("ghfaxviewer-logo.xpm"));
  gtk_layout_put (GTK_LAYOUT (layout), pixmap, 0, 0);

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

/* prefs.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2001 Wolfgang Sourdeau
 *
 * Author: Wolfgang Sourdeau <wolfgang@contre.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#include <ghfaxwidgets/ghfaxwidgets.h>
#include "setup.h"

static GtkWidget *
create_mixed_btn (GtkWidget *ref_widget,
		  const gchar *pixmap_path,
		  const gchar *text)
{
  GtkWidget *button, *vbox;
  GtkWidget *pixmap, *label;
  GtkRcStyle *bg_style;

  button = gtk_button_new ();
  vbox = gtk_vbox_new (FALSE, 3);
  pixmap = pixmap_from_xpm (ref_widget, pixmap_path);
  label = gtk_label_new (text);

  gtk_box_pack_start (GTK_BOX (vbox), pixmap, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);

  gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
  gtk_widget_set_usize (button, 76, 70);
  gtk_container_add (GTK_CONTAINER (button), vbox);

/*   bg_style = gtk_rc_style_new (); */
/*   back_gtkstyle (bg_style, GTK_STATE_ACTIVE, 56320, 56320, 65535); */
/*   back_gtkstyle (bg_style, GTK_STATE_PRELIGHT, 56320, 56320, 65535); */
/*   gtk_widget_modify_style (button, bg_style); */
/*   gtk_rc_style_unref (bg_style); */

  return button;
}

static GtkWidget *
create_pref_content (GtkWidget *ref_widget)
{
  GtkWidget *content_hbox, *viewport, *icon_box, *button, *fixed;

  content_hbox = gtk_hbox_new (FALSE, 5);

  viewport = gtk_viewport_new (NULL, NULL);
  icon_box = layout_new (ref_widget, GTK_ORIENTATION_VERTICAL, 0, 76);
  gtk_widget_show (icon_box);
/*   layout_set_bg_color (icon_box, 56320, 56320, 65535); */

  gtk_container_add (GTK_CONTAINER (viewport), icon_box);
  gtk_box_pack_start (GTK_BOX (content_hbox), viewport, FALSE, FALSE, 0);

  button = create_mixed_btn (ref_widget, PIXMAP ("viewer_prefs.xpm"), _("Viewer"));
  layout_add_widget (icon_box, button);

  button = create_mixed_btn (ref_widget, PIXMAP ("printer_prefs.xpm"), _("Printer"));
  layout_add_widget (icon_box, button);

  fixed = gtk_fixed_new ();
  gtk_widget_set_usize (fixed, 300, 400);
  gtk_box_pack_start (GTK_BOX (content_hbox), fixed, FALSE, FALSE, 0);

  return content_hbox;
}

void
prefs_cb (GtkWidget *widget, GtkWidget *viewer_window)
{
  GtkWidget *pref_win, *pref_content, *close_btn;

  pref_win = ghfw_dlg_window_new (_("Preferences..."));
/*   gtk_window_set_default_size (GTK_WINDOW (pref_win), 300, 400); */
  gtk_widget_show (pref_win);

  close_btn = gtk_button_new_with_label ("Close");

  pref_content = create_pref_content (pref_win);
  ghfw_dlg_window_set_content (GHFW_DLG_WINDOW (pref_win), pref_content);
  ghfw_dlg_window_set_button (GHFW_DLG_WINDOW (pref_win), close_btn);

  gtk_signal_connect_object (GTK_OBJECT (close_btn), "clicked",
			     gtk_widget_destroy,
			     GTK_OBJECT (pref_win));
  ghfw_dlg_window_set_escapable (GHFW_DLG_WINDOW (pref_win));

  transient_window_show (pref_win, viewer_window);
}

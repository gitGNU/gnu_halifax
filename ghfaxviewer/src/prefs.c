/* prefs.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2001 Wolfgang Sourdeau
 *
 * Time-stamp: <2003-03-07 02:03:49 wolfgang>
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
#include "i18n.h"

typedef struct _PrefBtnData PrefBtnData;
typedef struct _IconBoxData IconBoxData;

struct _PrefBtnData {
  GtkWidget *notebook;
  guint page_nbr;
};

struct _IconBoxData {
  GtkWidget *ref_widget, *notebook;
};

static void
tab_switch_cb (GtkWidget *button, gpointer data)
{
  PrefBtnData *pref_btn_data;
  GtkWidget *cur_selected_btn;

  pref_btn_data = data;
  gtk_notebook_set_current_page (GTK_NOTEBOOK (pref_btn_data->notebook),
				 pref_btn_data->page_nbr);


  cur_selected_btn = g_object_get_data (G_OBJECT (pref_btn_data->notebook),
					  "cur_selected_btn");	  
  gtk_widget_set_sensitive (cur_selected_btn, TRUE);
  gtk_widget_set_sensitive (button, FALSE);
  g_object_set_data (G_OBJECT (pref_btn_data->notebook),
		       "cur_selected_btn", button);
}

static GtkWidget *
create_pref_notebook ()
{
  GtkWidget *notebook, *label;

  notebook = gtk_notebook_new ();
  gtk_widget_set_size_request (notebook, 300, 200);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook), FALSE);

  label = gtk_label_new ("Test 1");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), label, NULL);

  label = gtk_label_new ("Test 2");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), label, NULL);

  return notebook;
}

static GtkWidget *
create_mixed_btn (const gchar *pixmap_path,
		  const gchar *text,
		  GtkWidget *notebook,
		  guint page_nbr)
{
  GtkWidget *button, *vbox;
  GtkWidget *image, *label;
  GtkRcStyle *bg_style;
  PrefBtnData *pref_btn_data;

  button = gtk_button_new ();
  vbox = gtk_vbox_new (FALSE, 3);
  image = gtk_image_new_from_file (pixmap_path);
  label = gtk_label_new (text);

  gtk_box_pack_start (GTK_BOX (vbox), image, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);

  pref_btn_data = g_new (PrefBtnData, 1);
  pref_btn_data->notebook = notebook;
  pref_btn_data->page_nbr = page_nbr;

  gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
  gtk_widget_set_size_request (button, 60, 70);
  g_signal_connect (G_OBJECT (button), "clicked",
		      G_CALLBACK (tab_switch_cb),
		      pref_btn_data);
  g_signal_connect (G_OBJECT (button), "destroy",
		      G_CALLBACK (free_data_on_destroy_cb),
		      pref_btn_data);

  gtk_container_add (GTK_CONTAINER (button), vbox);

  bg_style = gtk_rc_style_new ();
  back_gtkstyle (bg_style, GTK_STATE_NORMAL, 20000, 20000, 20000);
  gtk_widget_modify_style (image, bg_style);
  gtk_rc_style_unref (bg_style);

  bg_style = gtk_rc_style_new ();
  back_gtkstyle (bg_style, GTK_STATE_ACTIVE, 45000, 45000, 45000);
  back_gtkstyle (bg_style, GTK_STATE_PRELIGHT, 45000, 45000, 45000);
  gtk_widget_modify_style (button, bg_style);
  gtk_rc_style_unref (bg_style);

  gtk_widget_show_all (button);

  return button;
}

static void
icon_box_realize_cb (GtkWidget *icon_box, IconBoxData *icon_box_data)
{
  GtkWidget *button;

/*   while (gtk_events_pending ()) */
/*     gtk_main_iteration (); */

  button = create_mixed_btn (PIXMAP ("viewer_prefs.xpm"), _("Viewer"),
			     icon_box_data->notebook, 0);
  layout_add_widget (icon_box, button);
  gtk_widget_set_sensitive (button, FALSE);
  g_object_set_data (G_OBJECT (icon_box_data->notebook),
		       "cur_selected_btn", button);

  button = create_mixed_btn (PIXMAP ("printer_prefs.xpm"), _("Printer"),
			     icon_box_data->notebook, 1);
  layout_add_widget (icon_box, button);

  button = create_mixed_btn (PIXMAP ("info.xpm"), _("Printer"),
			     icon_box_data->notebook, 1);
  layout_add_widget (icon_box, button);

  button = create_mixed_btn (PIXMAP ("printer_prefs.xpm"), _("Printer"),
			     icon_box_data->notebook, 1);
  layout_add_widget (icon_box, button);

  g_free (icon_box_data);
}

static GtkWidget *
create_pref_content (GtkWidget *ref_widget)
{
  GtkWidget *content_hbox, *viewport, *notebook, *icon_box;
  IconBoxData *icon_box_data;

  content_hbox = gtk_hbox_new (FALSE, 5);

  viewport = gtk_viewport_new (NULL, NULL);
  icon_box = layout_new (ref_widget, GTK_ORIENTATION_VERTICAL, 0, 76);
  layout_set_bg_color (icon_box, 45000, 45000, 45000);
  gtk_container_add (GTK_CONTAINER (viewport), icon_box);
  gtk_box_pack_start (GTK_BOX (content_hbox), viewport, FALSE, FALSE, 0);

  notebook = create_pref_notebook ();
  gtk_box_pack_start (GTK_BOX (content_hbox), notebook, TRUE, FALSE, 0);

  icon_box_data = g_new (IconBoxData, 1);
  icon_box_data->ref_widget = ref_widget;
  icon_box_data->notebook = notebook;

  g_signal_connect (G_OBJECT (icon_box), "realize",
		    G_CALLBACK (icon_box_realize_cb),
		    icon_box_data);

  return content_hbox;
}

void
prefs_cb (GtkWidget *widget, GtkWidget *viewer_window)
{
  GtkWidget *pref_content, *close_btn;
  GhfwDlgWindow *pref_win;

  pref_win = ghfw_dlg_window_new (_("Preferences..."));
/*   gtk_window_set_default_size (GTK_WINDOW (pref_win), 300, 400); */
  gtk_widget_show (GTK_WIDGET (pref_win));

  close_btn = gtk_button_new_with_label ("Close");

  pref_content = create_pref_content (GTK_WIDGET (pref_win));
  ghfw_dlg_window_set_content_with_frame (GHFW_DLG_WINDOW (pref_win), pref_content);
  ghfw_dlg_window_set_button (GHFW_DLG_WINDOW (pref_win), close_btn);

  g_signal_connect_swapped (G_OBJECT (close_btn), "clicked",
			   G_CALLBACK (gtk_widget_destroy),
			   G_OBJECT (pref_win));
  ghfw_dlg_window_set_escapable (GHFW_DLG_WINDOW (pref_win));

  transient_window_show (GTK_WIDGET (pref_win), viewer_window);
}

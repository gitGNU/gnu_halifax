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

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#include "setup.h"
#include "gtkutils.h"
#include "i18n.h"

static gchar *copyright, *description, *licensing;
static gboolean i18n_initted = FALSE;

static
void about_i18n_init ()
{
  if (!i18n_initted)
    {
      copyright =
	_("Copyright (C) 2000,2001 Wolfgang Sourdeau");
      description =
	_("This program displays image files received on a HylaFAX system.");
      licensing =
	_("This program is free software, you"
	  " are welcome to use it, modify it"
	  " and redistribute it under certain"
	  " conditions. See the file COPYING"
	  " for further informations. There is"
	  " NO warranty; not even for"
	  " MERCHANTABILITY or FITNESS FOR A"
	  " PARTICULAR PURPOSE.");
      i18n_initted = TRUE;
    }
}

#ifdef NEED_GNOMESUPPORT_H
void
about_cb (GtkWidget *irrelevant, gpointer viewer_window)
{
  gchar *message;
  const gchar *authors[] = {"Wolfgang Sourdeau <wolfgang@gnu.org>",
			    "George Farris", "Tilman Bubeck",
			    "Thomas Bartschies", "Kevin Chen",
			    "Zbigniew Baniewski",
			    NULL};
  GtkWidget *about_dialog;

  about_i18n_init ();

  message = g_strdup_printf ("%s %s",
			     _(description),
			     _(licensing));
 
  about_dialog =
    gnome_about_new (_("GNU HaliFAX - Viewer"),
		     VERSION,
		     copyright,
		     authors,
		     message,
		     PIXMAP ("ghfaxviewer-logo.xpm"));
		       
  transient_window_show (GTK_WINDOW (about_dialog),
			 GTK_WINDOW (viewer_window));

  g_free (message);
}
#else /* NEED_GNOMESUPPORT_H */

static GtkWidget *
about_content (GtkWidget *window)
{
  GtkWidget *logo, *table, *label, *separator;

  table = gtk_table_new (8, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 20);
  gtk_container_set_border_width (GTK_CONTAINER (table), 5);

  logo = pixmap_from_xpm (window, PIXMAP ("ghfaxviewer-logo.xpm"));
  gtk_misc_set_alignment (GTK_MISC (logo), 0.5, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), logo, 0, 1, 0, 1);

  label = gtk_label_new (_("About the GNU HaliFAX - Viewer..."));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.7);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 0, 1);
  
  separator = gtk_hseparator_new ();
  gtk_table_attach_defaults (GTK_TABLE (table), separator, 0, 2, 1, 2);

  label = gtk_label_new (_("Version :"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 2, 3);
  label = gtk_label_new (_("The GNU HaliFAX team :"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 3, 4);
  label = gtk_label_new (_("The GNU HaliFAX - Viewer was enhanced"
			   " in various ways thanks to those"
			   " people :"));
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 4, 5);
  label = gtk_label_new (_("What is the GNU HaliFAX - Viewer ?"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 5, 6);
  label = gtk_label_new (_("Copyright :"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 6, 7);
  label = gtk_label_new (_("Licensing :"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 7, 8);
  
  label = gtk_label_new (VERSION);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 2, 3);

  label = gtk_label_new ("Wolfgang Sourdeau <Wolfgang@contre.com>\n"
			 "George Farris\n"
			 "Tilman Bubeck");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 3, 4);

  label =
    gtk_label_new (_("Thomas Bartschies, German translation\n"
		     "Kevin Chen, Traditional Chinese translation\n"
		     "Zbigniew Baniewski, Polish translation"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 4, 5);

  label = gtk_label_new (_(description));
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 5, 6);

  label = gtk_label_new (copyright);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 6, 7);

  label = gtk_label_new (_(licensing));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 7, 8);

  return table;
}

void
about_cb (GtkWidget *irrelevant, gpointer viewer_window)
{
  DialogWindow *about_dialog;
  GtkWidget *content, *ok_button;

  about_i18n_init ();

  about_dialog = dialog_window_new (_("About..."));
  content = about_content (viewer_window);
  dialog_window_set_content_with_frame (about_dialog, content);

  ok_button = gtk_button_new_with_label (_("Close"));
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
		      dialog_window_destroy_from_signal,
		      about_dialog);

  dialog_window_set_button (about_dialog, ok_button);
  dialog_window_set_escapable (about_dialog);

  dialog_window_show (about_dialog, GTK_WINDOW (viewer_window));
}
#endif /* NEED_GNOMESUPPORT_H */

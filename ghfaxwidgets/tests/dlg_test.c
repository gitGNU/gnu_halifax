/* dlg_test.c - this file is part of the GNU HaliFAX Widgets library
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

#include <gtk/gtk.h>

#include <ghfwdlgwindow.h>
#include <ghfwgtkutils.h>

void
escaped_cb (GtkWidget *dlg_window, gpointer null)
{
  g_print ("escaped\n");
}

GtkWidget *dlg_content ()
{
  GtkWidget *fixed, *label;

  fixed = gtk_fixed_new ();
  label = gtk_label_new ("Dialog content");
  gtk_fixed_put (GTK_FIXED (fixed), label, 50, 50);
  gtk_widget_set_usize (fixed, 140, 100);

  return fixed;
}

int
main (int argc, char *argv[])
{
  GtkWidget *main_window, *dlg_window, *content;
  gboolean escapable;

  gtk_init (&argc, &argv);
  
  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (main_window), "Main Window");
  gtk_signal_connect (GTK_OBJECT (main_window), "destroy",
		      G_CALLBACK (gtk_main_quit), NULL);

  dlg_window = GTK_WIDGET (ghfw_dlg_window_new ("Dialog Window"));

  gtk_signal_connect (GTK_OBJECT (dlg_window), "escaped",
		      G_CALLBACK (escaped_cb), NULL);
  gtk_widget_show (main_window);

  escapable = TRUE;
  gtk_object_set (GTK_OBJECT (dlg_window), "escapable", escapable);
  content = dlg_content ();
  g_object_set (G_OBJECT (dlg_window), "content", content);

  transient_window_show (dlg_window, main_window);

  gtk_main ();

  return 0;
}

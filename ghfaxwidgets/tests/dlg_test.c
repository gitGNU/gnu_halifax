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

int main (int argc, char *argv[])
{
  GtkWidget *main_window, *dlg_window;
  gboolean escapable;

  gtk_init (&argc, &argv);
  
  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (main_window), "Main Window");

  dlg_window = ghfw_dlg_window_new ("Dialog Window");

  gtk_signal_connect (GTK_OBJECT (dlg_window), "escaped",
		      escaped_cb, NULL);
  gtk_widget_show (main_window);

  escapable = TRUE;
  gtk_object_set (GTK_OBJECT (dlg_window), "escapable", escapable);
  transient_window_show (dlg_window, main_window);

  gtk_main ();

  return 0;
}

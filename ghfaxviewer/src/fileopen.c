/* fileopen.c - this file is part of the GNU HaliFAX Viewer
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <ghfaxwidgets/ghfaxwidgets.h>

#include "tiffimages.h"
#include "viewer.h"

#include "setup.h"

static void
file_dlg_ok_cb (GtkWidget *ok_button, ViewerData *viewer_data)
{
  gchar *file_name;
#ifdef CAN_SAVE_CONFIG
  gchar *last_directory;
#endif

  file_name = g_strdup (gtk_file_selection_get_filename
			(GTK_FILE_SELECTION (viewer_data->file_dlg)));

  if (file_name)
    {
      gtk_widget_destroy (viewer_data->file_dlg);
#ifdef CAN_SAVE_CONFIG
      last_directory = g_dirname (file_name);
      save_last_directory (last_directory);
      g_free (last_directory);
#endif
      fax_viewer_open_file (viewer_data, file_name);
      g_free (file_name);
    }
}

static void
file_dialog (ViewerData *viewer_data)
{
  GtkWidget *open_dialog;
#ifdef CAN_SAVE_CONFIG
  gchar *last_dir;
#endif

  open_dialog =
    gtk_file_selection_new (_("Please choose a FAX "
			      "G3 file to open..."));
  gtk_window_set_escapable (open_dialog);
  viewer_data->file_dlg = open_dialog;

#ifdef CAN_SAVE_CONFIG
  last_dir = load_last_directory ();
  gtk_file_selection_set_filename (GTK_FILE_SELECTION
				   (open_dialog), last_dir);
  g_free (last_dir);
#endif

  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION
				  (open_dialog)->ok_button),
		      "clicked",
		      GTK_SIGNAL_FUNC (file_dlg_ok_cb),
		      viewer_data);
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
					 (open_dialog)->cancel_button),
			     "clicked",
			     GTK_SIGNAL_FUNC (gtk_widget_destroy),
			     GTK_OBJECT (open_dialog));

  transient_window_show (open_dialog,
			 viewer_data->viewer_window);
  window_set_icon (open_dialog,
                   PIXMAP ("ghfaxviewer-icon.xpm"));
}

void
file_dialog_cb (GtkWidget *irrelevant, ViewerData *viewer_data)
{
  file_dialog (viewer_data);
}

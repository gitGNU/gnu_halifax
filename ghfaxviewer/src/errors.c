/* errors.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000-2001 Wolfgang Sourdeau
 *
 * Time-stamp: <2003-02-17 22:39:03 wolfgang>
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

/* This file contains functions to display error messages. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <gtk/gtk.h>
#include <ghfaxwidgets/ghfaxwidgets.h>

#include "tiffimages.h"
#include "viewer.h"

/*  static gchar* */
/*  list_to_text (GSList *string_list) */
/*  { */
/*    gchar *ret_str, *cur_pos; */
/*    GSList *cur_str; */
/*    guint count, str_c, length; */
  
/*    length = 0; */
/*    str_c = 0; */
  
/*    cur_str = string_list->next; */
  
/*    while (cur_str) */
/*      { */
/*        length += strlen ((char*) cur_str->data) + 1; */
/*        str_c++; */
/*        cur_str = cur_str->next; */
/*      } */

/*    if (str_c) */
/*      { */
/*        ret_str = g_malloc (length); */
/*        cur_pos = ret_str; */
/*        cur_str = string_list->next; */

/*        for (count = 1; count < str_c; count++) */
/*  	{ */
/*  	  strcpy (cur_pos, (char*) cur_str->data); */
/*  	  cur_pos += strlen ((char*) cur_str->data); */
/*  	  sprintf (cur_pos, "\n"); */
/*  	  cur_pos ++; */
/*  	  cur_str = cur_str->next; */
/*  	} */

/*        sprintf (cur_pos, "%s", (char*) cur_str->data); */
/*      } */
/*    else */
/*      ret_str = NULL; */

/*    return ret_str; */
/*  } */

void
display_failure (GtkWidget *window,
		 gchar *title, gchar *message, gchar *but_text)
{
  GtkWidget  *fail_dialog, *vbox, *msg_lbl, *ok_but;

  fail_dialog = ghfw_dlg_window_new (title);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);

  msg_lbl = gtk_label_new (message);
  gtk_label_set_justify (GTK_LABEL (msg_lbl),
			 GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (vbox), msg_lbl,
		      FALSE, FALSE, 0);

  ok_but = gtk_button_new_with_label (but_text); 
  g_signal_connect_object (G_OBJECT (ok_but), "clicked",
			   G_CALLBACK (gtk_widget_destroy),
			   G_OBJECT (fail_dialog));

  ghfw_dlg_window_set_content_with_frame ((GhfwDlgWindow *) fail_dialog, vbox);
  ghfw_dlg_window_set_button ((GhfwDlgWindow *) fail_dialog, ok_but);
  ghfw_dlg_window_set_escapable ((GhfwDlgWindow *) fail_dialog);

  transient_window_show (fail_dialog,
			 window);
}

static void
error_dialog (GtkWidget *window, gchar *file_name, gchar *error_string)
{
  gchar *title, *message, *but_text;

  title = _("Something strange happened...");
  message =
    g_strdup_printf (_("The specified file, %s, could not be opened\n"
		       "because %s.\n"
		       "Please fix this and try again later."),
		     file_name, error_string);;
  but_text = _("I am sorry");

  display_failure (window, title, message, but_text);
}

void
file_open_error (GtkWidget *window, gchar *file_name)
{
  gchar *error_str;

  error_str = ti_error_string (file_name);  
  error_dialog (window, file_name, error_str);
  g_free (error_str);
}

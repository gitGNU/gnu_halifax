/* errors.c - this file is part of the GNU HaliFAX Viewer
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

#include <stdio.h>
#include <gtk/gtk.h>

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

static gint
fail_win_destroy_cb (GtkWidget* window, gpointer null)
{
  decrease_win_count ();

  return FALSE;
}

static void
display_failure (gchar *title, gchar *message, gchar *but_text)
{
  GtkWidget *fail_window, *vbox, *msg_lbl, *ok_but; 

  fail_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_window_set_modal (GTK_WINDOW (fail_window), TRUE);
  gtk_window_set_title (GTK_WINDOW (fail_window), title);
  gtk_window_set_policy (GTK_WINDOW (fail_window),
			 FALSE, FALSE, TRUE);

  gtk_container_set_border_width (GTK_CONTAINER (fail_window),
				  10);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (fail_window), vbox);
  gtk_widget_show (vbox);

  msg_lbl = gtk_label_new (message);
  gtk_label_set_justify (GTK_LABEL (msg_lbl),
			 GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (vbox), msg_lbl,
		      FALSE, FALSE, 0);
  gtk_widget_show (msg_lbl);

  ok_but = gtk_button_new_with_label (but_text);
  gtk_box_pack_start (GTK_BOX (vbox), ok_but,
			FALSE, FALSE, 2);
  
  gtk_signal_connect_object (GTK_OBJECT (ok_but), "clicked",
			     GTK_SIGNAL_FUNC
			     (gtk_widget_destroy),
			     GTK_OBJECT (fail_window));
  gtk_widget_show (ok_but);

  increase_win_count ();
  gtk_signal_connect (GTK_OBJECT (fail_window), "destroy",
		      GTK_SIGNAL_FUNC (fail_win_destroy_cb),
		      NULL);

  gtk_widget_show (fail_window);
}

static void
error_dialog (gchar *file_name, gchar *error_string)
{
  gchar *title, *message, *but_text;

  title = _("Something strange happened...");
  message =
    g_strdup_printf (_("The specified file, %s, could not be opened\n"
		       "because %s.\n"
		       "Please fix this and try again later."),
		     file_name, error_string);;
  but_text = _("I am sorry");

  display_failure (title, message, but_text);
}

void
file_open_error (gchar *file_name)
{
  gchar *error_str;

  error_str = ti_error_string (file_name);  
  error_dialog (file_name, error_str);
  g_free (error_str);
}

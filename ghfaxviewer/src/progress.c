/* progress.c - this file is part of the GNU HaliFAX Viewer
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

#include "i18n.h"
#include "progress.h"
#include "gtkutils.h"

static void
progress_abort (GtkWidget *widget, GfvProgressData *progress_data)
{
  progress_data->aborted = TRUE;
}

GfvProgressData *
gfv_progress_new (GtkWindow *parent_window,
		  gchar *title, gchar *action_string,
		  GfvProgressTag tag)
{
  GtkWidget *window, *frame, *vbox, *progress, *abort_btn, *label;
  GtkObject *adjustment;
  GfvProgressData *prog_data;

  prog_data = g_malloc (sizeof (GfvProgressData));

  adjustment = gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 1.0);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), title);
  gtk_window_set_modal (GTK_WINDOW (window), TRUE);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);

  frame = gtk_frame_new (_("Processing"));
  gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
  gtk_container_add (GTK_CONTAINER (window), frame);

  vbox = gtk_vbox_new (FALSE, 3);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
  gtk_container_add (GTK_CONTAINER (frame), vbox);
	
  label = gtk_label_new (action_string);
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, FALSE, 3);

  progress = gtk_progress_bar_new_with_adjustment
    (GTK_ADJUSTMENT(adjustment));
  gtk_box_pack_start (GTK_BOX (vbox), progress, FALSE, FALSE, 3);

  if (tag & ABORT_BTN)
    {
      abort_btn = gtk_button_new_with_label (_("Cancel"));
      gtk_signal_connect (GTK_OBJECT (abort_btn), "clicked",
			  (GtkSignalFunc) progress_abort, prog_data);
      gtk_box_pack_start (GTK_BOX (vbox), abort_btn, FALSE,
			  FALSE, 3);
	    
      prog_data->abort_btn = abort_btn;
    }

  if (!(tag & DISPLAY_WHEN_NEEDED))
    transient_window_show (GTK_WINDOW (window),
			   parent_window);

  prog_data->aborted = FALSE;
  prog_data->done = FALSE;
  if (action_string)
    prog_data->action_string = g_strdup (action_string);
  else
    prog_data->action_string = NULL;
  prog_data->label = label;
  prog_data->tag = tag;
  prog_data->progress_bar = progress;
  prog_data->progress_win = window;
  prog_data->parent_window = parent_window;

  return prog_data;
}

void
gfv_progress_set_action (GfvProgressData *progress_data,
			 gchar *new_action)
{
  if (progress_data->action_string)
    g_free (progress_data->action_string);

  if (new_action)
    progress_data->action_string = g_strdup (new_action);
  else
    progress_data->action_string = NULL;

  gtk_label_set_text ((GtkLabel *) progress_data->label,
		      new_action);

  while (gtk_events_pending ())
    gtk_main_iteration ();
}

gboolean
gfv_progress_update_with_percentage (guint value, guint total,
				     guint percentage, gpointer data)
{
  gfloat p_perc;
  GfvProgressData *prog_data;

  prog_data = data;

  if ((prog_data->tag & DISPLAY_WHEN_NEEDED)
      && !GTK_WIDGET_VISIBLE (prog_data->progress_win))
    transient_window_show (GTK_WINDOW (prog_data->progress_win),
			   prog_data->parent_window);

  if (!prog_data->done)
    {
      p_perc = (gfloat) percentage / 100;

      if (p_perc > 1.0)
	{
	  p_perc = 1.0;
	  if (prog_data->tag & ABORT_BTN)
	    gtk_widget_set_sensitive
	      (prog_data->abort_btn, FALSE);
	  prog_data->done = TRUE;
	}
      gtk_progress_set_percentage
	((GtkProgress*) (prog_data->progress_bar), p_perc);

      while (gtk_events_pending ())
	gtk_main_iteration ();
    }


  return (prog_data->aborted);
}

gboolean
gfv_progress_update_with_value (guint value, guint total,
				guint percentage, gpointer data)
{
  gboolean aborted;
  gfloat p_perc;
  GfvProgressData *prog_data;

  prog_data = data;

  p_perc = value * 100 / total;
  aborted = gfv_progress_update_with_percentage (0, 0, p_perc, data);

  return aborted;
}

void
gfv_progress_destroy (GfvProgressData *prog_data)
{
  gtk_widget_destroy (prog_data->progress_win);
  if (prog_data->action_string)
    g_free (prog_data->action_string);
  g_free (prog_data);
}

void
gfv_progress_set_done (GfvProgressData *progress_data)
{
  gchar *action_str, *done_str;

  action_str = progress_data->action_string;
  done_str = g_strdup_printf (_("%s (done)"), action_str);

  gfv_progress_set_action (progress_data, done_str);
  g_free (done_str);
}

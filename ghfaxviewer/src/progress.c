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

/* This file provides facilities to create progress bars and their
   windows */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else
#include <gtk/gtk.h>
#endif

#include "i18n.h"
#include "gtkutils.h"

typedef struct _GfvProgressData GfvProgressData;

typedef enum
{
  ABORT_BTN = 1,
  DISPLAY_WHEN_NEEDED = 2,
} GfvProgressTag;

struct _GfvProgressData
{
  gboolean aborted, done, is_visible;
  gchar *action_string;
  GfvProgressTag tag;
  GtkWidget *label, *progress_bar, *abort_btn;
  DialogWindow *progress_win;
  GtkWindow *parent_window;
};

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
  DialogWindow *dlg_window;
  GtkWidget *vbox, *progress, *abort_btn, *label;
  GtkObject *adjustment;
  GfvProgressData *prog_data;

  prog_data = g_malloc (sizeof (GfvProgressData));

  adjustment = gtk_adjustment_new (0.0, 0.0, 1.0, 0.1, 0.1, 1.0);

  dlg_window = dialog_window_new (title);

  vbox = gtk_vbox_new (FALSE, 3);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
  dialog_window_set_content_with_frame (dlg_window, vbox);
	
  label = gtk_label_new (action_string);
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, FALSE, 3);

  progress = gtk_progress_bar_new_with_adjustment
    (GTK_ADJUSTMENT(adjustment));
  gtk_box_pack_start (GTK_BOX (vbox), progress, FALSE, FALSE, 3);

  if (tag & ABORT_BTN)
    {
#ifdef NEED_GNOMESUPPORT_H
      abort_btn = gnome_stock_button (GNOME_STOCK_BUTTON_CANCEL);
#else
      abort_btn = gtk_button_new_with_label (_("Cancel"));
#endif
      gtk_signal_connect (GTK_OBJECT (abort_btn), "clicked",
			  (GtkSignalFunc) progress_abort, prog_data); 
      dialog_window_set_button (dlg_window, abort_btn);
      dialog_window_set_escapable_with_callback (dlg_window, 
						 (GtkSignalFunc) progress_abort,
						 prog_data);

      prog_data->abort_btn = abort_btn;
    }

  if (!(tag & DISPLAY_WHEN_NEEDED))
    {
      dialog_window_show (dlg_window, parent_window);
      prog_data->is_visible = TRUE;
    }
  else
    prog_data->is_visible = FALSE;

  prog_data->aborted = FALSE;
  prog_data->done = FALSE;
  if (action_string)
    prog_data->action_string = g_strdup (action_string);
  else
    prog_data->action_string = NULL;
  prog_data->label = label;
  prog_data->tag = tag;
  prog_data->progress_bar = progress;
  prog_data->progress_win = dlg_window;
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
      && !(prog_data->is_visible))
    {
      dialog_window_show (prog_data->progress_win,
			  prog_data->parent_window);
      prog_data->is_visible = TRUE;
    }

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
  dialog_window_destroy (prog_data->progress_win);
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

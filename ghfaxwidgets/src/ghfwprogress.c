/* ghfwprogress.c - this file is part of the GNU HaliFAX Widgets library
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

#include "ghfwprogress.h"
#include "ghfwgtkutils.h"
#include "i18n.h"

enum {
  ARG_0,
  ARG_DONE,
  ARG_ABORTED,
  ARG_ABORTABLE
};

static guint progress_window_aborted_signal;

/* Callbacks */
static void ghfw_progress_window_real_aborted (GhfwProgressWindow *progress_window)
{
  progress_window->aborted = TRUE;  
}

static void progress_abort_cb (GhfwProgressWindow *progress_window, gpointer null)
{
  gtk_signal_emit (GTK_OBJECT (progress_window),
		   progress_window_aborted_signal);
}

/* Args */
static void
ghfw_progress_window_set_arg (GtkObject *object,
			      GtkArg    *arg,
			      guint      arg_id)
{
  GhfwProgressWindow *progress_window;
  GhfwDlgWindow *dlg_window;

  progress_window = GHFW_PROGRESS_WINDOW (object);
  dlg_window = GHFW_DLG_WINDOW (object);

  switch (arg_id)
    {
    case ARG_DONE:
      ghfw_progress_window_set_done (progress_window, GTK_VALUE_BOOL (*arg));
      break;
    case ARG_ABORTABLE:
      ghfw_progress_window_set_abortable (progress_window, GTK_VALUE_BOOL (*arg));
      break;
    case ARG_ABORTED:
      gtk_signal_emit (GTK_OBJECT (progress_window),
		       progress_window_aborted_signal);
      break;
    default:
      break;
    }
}

static void
ghfw_progress_window_get_arg (GtkObject *object,
			      GtkArg    *arg,
			      guint      arg_id)
{
  GhfwProgressWindow *progress_window;

  progress_window = GHFW_PROGRESS_WINDOW (object);

  switch (arg_id)
    {
    case ARG_DONE:
      GTK_VALUE_BOOL (*arg) = progress_window->done;
      break;
    case ARG_ABORTED:
      GTK_VALUE_BOOL (*arg) = progress_window->aborted;
      break;
    case ARG_ABORTABLE:
      GTK_VALUE_BOOL (*arg) = progress_window->abortable;
      break;
    default:
      arg->type = GTK_TYPE_INVALID;
      break;
    }
}


/* Widget creation stuff */

static void
ghfw_progress_window_class_init (GhfwProgressWindowClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;

  gtk_object_add_arg_type ("GhfwDlgWindow::done",
			   GTK_TYPE_BOOL, GTK_ARG_READWRITE,
			   ARG_DONE);
  gtk_object_add_arg_type ("GhfwDlgWindow::aborted",
			   GTK_TYPE_BOOL, GTK_ARG_READWRITE,
			   ARG_ABORTED);
  gtk_object_add_arg_type ("GhfwDlgWindow::abortable",
			   GTK_TYPE_BOOL, GTK_ARG_READWRITE,
			   ARG_ABORTABLE);

  progress_window_aborted_signal =
    gtk_signal_new ("aborted",
		    GTK_RUN_FIRST,
		    object_class->type,
		    GTK_SIGNAL_OFFSET (GhfwProgressWindowClass, aborted),
		    gtk_marshal_NONE__NONE,
		    GTK_TYPE_NONE, 0);

  gtk_object_class_add_signals (object_class, &progress_window_aborted_signal, 1);

  object_class->set_arg = ghfw_progress_window_set_arg;
  object_class->get_arg = ghfw_progress_window_get_arg;

  klass->aborted = ghfw_progress_window_real_aborted;
}

static void
ghfw_progress_window_init (GhfwProgressWindow *progress_window)
{
  GtkObject *adjustment;
  GtkWidget *vbox;

  adjustment = gtk_adjustment_new (0.0, 0.0, 1.0, 0.1, 0.1, 1.0);

  vbox = gtk_vbox_new (FALSE, 3);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
  ghfw_dlg_window_set_content_with_frame (GHFW_DLG_WINDOW (progress_window), vbox);
	
  progress_window->label = gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (vbox), progress_window->label, TRUE, FALSE, 3);

  progress_window->progress_bar = gtk_progress_bar_new_with_adjustment
    (GTK_ADJUSTMENT(adjustment));
  gtk_box_pack_start (GTK_BOX (vbox), progress_window->progress_bar, FALSE, FALSE, 3);

#ifdef NEED_GNOMESUPPORT_H
  progress_window->abort_btn = gnome_stock_button (GNOME_STOCK_BUTTON_CANCEL);
#else
  progress_window->abort_btn = gtk_button_new_with_label (_("Cancel"));
#endif
  gtk_signal_connect (GTK_OBJECT (progress_window->abort_btn), "clicked",
		      (GtkSignalFunc) progress_abort_cb, progress_window); 
  ghfw_dlg_window_set_button (GHFW_DLG_WINDOW (progress_window), progress_window->abort_btn);
  gtk_signal_connect_object (GTK_OBJECT (progress_window), "escaped",
			     (GtkSignalFunc) progress_abort_cb, NULL);

  progress_window->aborted = FALSE;
  progress_window->abortable = FALSE;
  progress_window->done = FALSE;
  progress_window->action_string = NULL;
}

GtkType
ghfw_progress_window_get_type (void)
{
  static GtkType progress_window_type = 0;

  if (!progress_window_type)
    {
      static const GtkTypeInfo progress_window_info =
	{
	  "GhfwDlgWindow",
	  sizeof (GhfwProgressWindow),
	  sizeof (GhfwProgressWindowClass),
	  (GtkClassInitFunc) ghfw_progress_window_class_init,
	  (GtkObjectInitFunc) ghfw_progress_window_init,
	  /* reserved_1 */ NULL,
	  /* reserved_2 */ NULL,
	  (GtkClassInitFunc) NULL,
	};

      progress_window_type = gtk_type_unique (GTK_TYPE_WINDOW, &progress_window_info);
    }

  return progress_window_type;
}

/* Regular public stuff */

GtkWidget *
ghfw_progress_window_new (gchar *title, gchar *action)
{
  GtkWidget *progress_window;
  
  progress_window = GTK_WIDGET (gtk_type_new (ghfw_progress_window_get_type ()));
  gtk_window_set_title (GTK_WINDOW (progress_window), title);
  ghfw_progress_window_set_action ((GhfwProgressWindow *) progress_window, action);

  return progress_window;
}

/* update callbacks */
gboolean
ghfw_progress_window_update_with_percentage (GhfwProgressWindow *progress_window,
					     guint percentage)
{
  gfloat p_perc;
 
  if (!progress_window->done)
    {
      p_perc = (gfloat) percentage / 100;

      if (p_perc > 1.0)
	{
	  p_perc = 1.0;
	  ghfw_progress_window_set_done (progress_window, TRUE);
	}
      gtk_progress_set_percentage
	((GtkProgress*) (progress_window->progress_bar), p_perc);
    }

  return (progress_window->aborted);
}

gboolean
ghfw_progress_window_update_with_value (GhfwProgressWindow *progress_window,
					guint value, guint total)
{
  gboolean aborted;
  gfloat p_perc;

  p_perc = value * 100 / total;
  aborted = ghfw_progress_window_update_with_percentage (progress_window,
							 p_perc);

  return aborted;
}

/* Parameters */
void
ghfw_progress_window_set_action (GhfwProgressWindow *progress_window,
				 gchar *action)
{
  if (progress_window->action_string)
    g_free (progress_window->action_string);

  if (action)
    progress_window->action_string = g_strdup (action);
  else
    progress_window->action_string = NULL;

  gtk_label_set_text ((GtkLabel *) progress_window->label,
		      action);
}

void
ghfw_progress_window_set_done (GhfwProgressWindow *progress_window,
			       gboolean done)
{
  gchar *action_str, *done_str;

  if (done)
    {
      if (progress_window->abortable)
	gtk_widget_set_sensitive
	  (progress_window->abort_btn, FALSE);
      
      action_str = progress_window->action_string;
      done_str = g_strdup_printf (_("%s (done)"), action_str);
      
      ghfw_progress_window_set_action (progress_window, done_str);
      g_free (done_str);
    }

  progress_window->done = done;
}

void
ghfw_progress_window_set_abortable (GhfwProgressWindow *progress_window,
				    gboolean abortable)
{
  GhfwDlgWindow *dlg_window;

  dlg_window = GHFW_DLG_WINDOW (progress_window);

  progress_window->abortable = abortable;
  if (abortable)
    gtk_widget_show_all (dlg_window->button_box);
  else
    gtk_widget_hide (dlg_window->button_box);
}

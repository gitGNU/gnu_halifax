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

#include "ghfwdlgwindow.h"
#include "ghfwprogress.h"
#include "ghfwgtkutils.h"
#include "i18n.h"

enum {
  PARAM_0,
  DONE,
  ABORTED,
  ABORTABLE
};

static GhfwDlgWindowClass *parent_class;
static guint progress_window_aborted_signal;

/* Callbacks */
static void
ghfw_progress_window_real_aborted (GhfwProgressWindow *progress_window)
{
  progress_window->aborted = TRUE;  
}

static void
ghfw_progress_window_destroy (GtkObject *object)
{
  GhfwProgressWindow *progress_window;

  progress_window = GHFW_PROGRESS_WINDOW (object);

  if (progress_window->action_string)
    g_free (progress_window->action_string);

  if (GTK_OBJECT_CLASS (parent_class)->destroy)
    (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
ghfw_progress_window_show (GtkWidget *widget)
{
  GhfwProgressWindow *progress_window;
  GhfwDlgWindow *dlg_window;

  progress_window = GHFW_PROGRESS_WINDOW (widget);
  dlg_window = GHFW_DLG_WINDOW (widget);

  if (GTK_WIDGET_CLASS (parent_class)->show)
    (* GTK_WIDGET_CLASS (parent_class)->show) (widget);

  if (!progress_window->abortable)
    gtk_widget_hide (dlg_window->button_box);
}

static void
ghfw_progress_window_show_all (GtkWidget *widget)
{
  GhfwProgressWindow *progress_window;
  GhfwDlgWindow *dlg_window;

  progress_window = GHFW_PROGRESS_WINDOW (widget);
  dlg_window = GHFW_DLG_WINDOW (widget);

  if (GTK_WIDGET_CLASS (parent_class)->show_all)
    (* GTK_WIDGET_CLASS (parent_class)->show_all) (widget);

  if (!progress_window->abortable)
    gtk_widget_hide (dlg_window->button_box);
}

static void
progress_abort_cb (GhfwProgressWindow *progress_window, gpointer null)
{
  g_signal_emit (G_OBJECT (progress_window),
		 progress_window_aborted_signal, 0);
}

/* Propertys */
static void
ghfw_progress_window_set_property (GObject *object,
				   guint property_id,
				   const GValue *value,
				   GParamSpec *pspec)
{
  GhfwProgressWindow *progress_window;
  GhfwDlgWindow *dlg_window;
  gboolean bool_value;

  progress_window = GHFW_PROGRESS_WINDOW (object);
  dlg_window = GHFW_DLG_WINDOW (object);

  switch (property_id)
    {
    case DONE:
      bool_value = g_value_get_boolean (value);
      ghfw_progress_window_set_done (progress_window, bool_value);
      break;
    case ABORTABLE:
      bool_value = g_value_get_boolean (value);
      ghfw_progress_window_set_abortable (progress_window, bool_value);
      break;
    case ABORTED:
      bool_value = g_value_get_boolean (value);
      g_signal_emit (G_OBJECT (progress_window),
		     progress_window_aborted_signal, 0);
      break;
    }
}

static void
ghfw_progress_window_get_property (GObject *object,
				   guint property_id,
				   GValue *value,
				   GParamSpec *pspec)
{
  GhfwProgressWindow *progress_window;

  progress_window = GHFW_PROGRESS_WINDOW (object);

  switch (property_id)
    {
    case DONE:
      g_value_set_boolean (value, progress_window->done);
      break;
    case ABORTED:
      g_value_set_boolean (value, progress_window->aborted);
      break;
    case ABORTABLE:
      g_value_set_boolean (value, progress_window->abortable);
      break;
/*     default: */
/*       property->type = G_TYPE_INVALID; */
/*       break; */
    }
}

/* Widget creation stuff */

static void
ghfw_progress_window_class_init (GhfwProgressWindowClass *klass)
{
  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = (GObjectClass *) klass;
  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;

  g_object_class_install_property
    (gobject_class, DONE,
     g_param_spec_boolean ("done",
			   _("Done"),
			   _("Whether the process is done."),
			   0, G_PARAM_READWRITE));
  g_object_class_install_property
    (gobject_class, ABORTED,
     g_param_spec_boolean ("aborted",
			   _("Aborted"),
			   _("Whether the process was aborted."),
			   0, G_PARAM_READWRITE));
  g_object_class_install_property
    (gobject_class, ABORTABLE,
     g_param_spec_boolean ("abortable",
			   _("Abortable"),
			   _("Whether the dialog can be aborted."),
			   0, G_PARAM_READWRITE));

  progress_window_aborted_signal =
    g_signal_new ("aborted",
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GhfwProgressWindowClass, aborted),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

/*   gtk_object_class_add_signals (object_class, */
/* 				&progress_window_aborted_signal, 1); */

  gobject_class->set_property = ghfw_progress_window_set_property;
  gobject_class->get_property = ghfw_progress_window_get_property;

  klass->aborted = ghfw_progress_window_real_aborted;
  object_class->destroy = ghfw_progress_window_destroy;
  widget_class->show = ghfw_progress_window_show;
  widget_class->show_all = ghfw_progress_window_show_all;
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

  progress_window->progress_bar = gtk_progress_bar_new ();
  gtk_box_pack_start (GTK_BOX (vbox), progress_window->progress_bar, FALSE, FALSE, 3);

  progress_window->abort_btn = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  g_signal_connect (G_OBJECT (progress_window->abort_btn), "clicked",
		    G_CALLBACK (progress_abort_cb), progress_window); 
  ghfw_dlg_window_set_button (GHFW_DLG_WINDOW (progress_window),
			      progress_window->abort_btn);
  g_signal_connect_swapped (G_OBJECT (progress_window), "escaped",
			    G_CALLBACK (progress_abort_cb), NULL);

  progress_window->aborted = FALSE;
  progress_window->abortable = FALSE;
  progress_window->done = FALSE;
  progress_window->action_string = NULL;

  progress_window->value = 0;
  progress_window->max = 0;

  gtk_widget_show_all (vbox);
  gtk_widget_hide (GHFW_DLG_WINDOW (progress_window)->button_box);
}

GType
ghfw_progress_window_get_type (void)
{
  static GType progress_window_type = 0;

  if (!progress_window_type)
    {
      GTypeInfo progress_window_info =
	{
	  sizeof (GhfwProgressWindowClass),
	  (GBaseInitFunc) NULL,
	  (GBaseFinalizeFunc) NULL,
	  (GClassInitFunc) ghfw_progress_window_class_init,
	  (GClassFinalizeFunc) NULL,
	  NULL,
	  sizeof (GhfwProgressWindow),
	  0,
	  (GInstanceInitFunc) ghfw_progress_window_init,
	};

      progress_window_type = g_type_register_static (GHFW_DLG_WINDOW_TYPE,
						     "GhfwProgressWindow",
						     &progress_window_info,
						     0);
    }

  return progress_window_type;
}

/* Regular public stuff */

GhfwProgressWindow *
ghfw_progress_window_new (gchar *title, gchar *action)
{
  GhfwProgressWindow *progress_window;

  progress_window = g_object_new (GHFW_PROGRESS_WINDOW_TYPE, NULL);
  gtk_window_set_title (GTK_WINDOW (progress_window), title);
  ghfw_progress_window_set_action ((GhfwProgressWindow *) progress_window,
				   action);

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
      gtk_progress_bar_set_fraction
	(GTK_PROGRESS_BAR (progress_window->progress_bar), p_perc);
    }

  while (gtk_events_pending())
    gtk_main_iteration ();

  return (progress_window->aborted);
}

gboolean
ghfw_progress_window_update_with_value (GhfwProgressWindow *progress_window,
					guint value, guint total)
{
  gboolean aborted;
  gfloat p_perc;

  p_perc = value * 100 / total;
  progress_window->value = value;
  aborted = ghfw_progress_window_update_with_percentage (progress_window,
							 p_perc);

  return aborted;
}

void
ghfw_progress_window_increment (GhfwProgressWindow *progress_window,
				guint increment)
{
  ghfw_progress_window_update_with_value (progress_window,
					  progress_window->value + increment,
					  progress_window->max);
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

void
ghfw_progress_window_set_max (GhfwProgressWindow *progress_window,
			      guint max)
{
  progress_window->max = max;
}

/* ghfwdlgwindow.c - this file is part of the GNU HaliFAX Widgets library
 *
 * Copyright (C) 2001 Wolfgang Sourdeau
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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "ghfwdlgwindow.h"
#include "ghfwgtkutils.h"

enum {
  ARG_0,
  ARG_ESCAPABLE,
  ARG_CONTENT,
  ARG_BUTTON_BOX,
  ARG_VBOX
};

static guint dlg_window_escaped_signal;
static GtkWindowClass *parent_class = NULL;

/* callbacks */

static gboolean
ghfw_dlg_window_key_press_event (GtkWidget *widget,
				 GdkEventKey *event)
{
  GhfwDlgWindow *dlg_window;
  gboolean ret_code;

  dlg_window = GHFW_DLG_WINDOW (widget);

  if (event->type == GDK_KEY_PRESS &&
      event->keyval == GDK_Escape &&
      dlg_window->escapable)
    {
      gtk_signal_emit (GTK_OBJECT (widget), dlg_window_escaped_signal);
      gtk_signal_emit_stop_by_name (GTK_OBJECT (widget), "key_press_event");
      gtk_widget_destroy (widget);

      ret_code = TRUE;
    }
  else
    ret_code = FALSE;

  return ret_code;
}

static GtkWidget *
ghfw_dlg_window_bbox ()
{
  GtkWidget *button_box;

  button_box = gtk_hbutton_box_new ();
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (button_box), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (button_box), 5);

  gtk_widget_show (button_box);

  return button_box;
}

static void
ghfw_dlg_window_set_arg (GtkObject *object,
			 GtkArg    *arg,
			 guint      arg_id)
{
  GhfwDlgWindow *dlg_window;

  dlg_window = GHFW_DLG_WINDOW (object);

  switch (arg_id)
    {
    case ARG_ESCAPABLE:
      dlg_window->escapable = GTK_VALUE_BOOL (*arg);
      break;
    case ARG_CONTENT:
      ghfw_dlg_window_set_content (dlg_window, GTK_VALUE_POINTER (*arg));
      break;
    case ARG_BUTTON_BOX:
      ghfw_dlg_window_set_button_box (dlg_window, GTK_VALUE_POINTER (*arg));
      break;
    default:
      break;
    }
}

static void
ghfw_dlg_window_get_arg (GtkObject *object,
			 GtkArg    *arg,
			 guint      arg_id)
{
  GhfwDlgWindow *dlg_window;

  dlg_window = GHFW_DLG_WINDOW (object);

  switch (arg_id)
    {
    case ARG_ESCAPABLE:
      GTK_VALUE_BOOL (*arg) = dlg_window->escapable;
      break;
    case ARG_CONTENT:
      GTK_VALUE_POINTER (*arg) = dlg_window->content;
      break;
    case ARG_BUTTON_BOX:
      if (!dlg_window->button_box)
	GTK_VALUE_POINTER (*arg) = ghfw_dlg_window_bbox ();
      else
	GTK_VALUE_POINTER (*arg) = dlg_window->button_box;
      break;
    case ARG_VBOX:
      GTK_VALUE_POINTER (*arg) = dlg_window->vbox;
      break;
    default:
      arg->type = GTK_TYPE_INVALID;
      break;
    }
}

/* Widget creation stuff */

static void
ghfw_dlg_window_class_init (GhfwDlgWindowClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;

  parent_class = gtk_type_class (GTK_TYPE_WINDOW);

  gtk_object_add_arg_type ("GhfwDlgWindow::escapable",
			   GTK_TYPE_BOOL, GTK_ARG_READWRITE,
			   ARG_ESCAPABLE);
  gtk_object_add_arg_type ("GhfwDlgWindow::content",
			   GTK_TYPE_POINTER, GTK_ARG_READWRITE,
			   ARG_CONTENT);
  gtk_object_add_arg_type ("GhfwDlgWindow::button_box",
			   GTK_TYPE_POINTER, GTK_ARG_READWRITE,
			   ARG_BUTTON_BOX);
  gtk_object_add_arg_type ("GhfwDlgWindow::vbox",
			   GTK_TYPE_POINTER, GTK_ARG_READABLE,
			   ARG_VBOX);

  dlg_window_escaped_signal =
    gtk_signal_new ("escaped",
		    GTK_RUN_FIRST,
		    object_class->type,
		    GTK_SIGNAL_OFFSET (GhfwDlgWindowClass, escaped),
		    gtk_marshal_NONE__NONE,
		    GTK_TYPE_NONE, 0);

  gtk_object_class_add_signals (object_class, &dlg_window_escaped_signal, 1);

  object_class->set_arg = ghfw_dlg_window_set_arg;
  object_class->get_arg = ghfw_dlg_window_get_arg;

  widget_class->key_press_event = ghfw_dlg_window_key_press_event;

  klass->escaped = NULL;
}

static void
ghfw_dlg_window_init (GhfwDlgWindow *dlg_window)
{
  dlg_window->escapable = FALSE;

  dlg_window->vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (dlg_window),
		     dlg_window->vbox);

  dlg_window->content = NULL;
  dlg_window->button_box = NULL;

  gtk_widget_show (dlg_window->vbox);
}

GtkType
ghfw_dlg_window_get_type (void)
{
  static GtkType dlg_window_type = 0;

  if (!dlg_window_type)
    {
      static const GtkTypeInfo dlg_window_info =
	{
	  "GhfwDlgWindow",
	  sizeof (GhfwDlgWindow),
	  sizeof (GhfwDlgWindowClass),
	  (GtkClassInitFunc) ghfw_dlg_window_class_init,
	  (GtkObjectInitFunc) ghfw_dlg_window_init,
	  /* reserved_1 */ NULL,
	  /* reserved_2 */ NULL,
	  (GtkClassInitFunc) NULL,
	};

      dlg_window_type = gtk_type_unique (GTK_TYPE_WINDOW, &dlg_window_info);
    }

  return dlg_window_type;
}

/* Regular public stuff */

GtkWidget*
ghfw_dlg_window_new (gchar *title)
{
  GtkWidget *dlg_window;
  
  dlg_window = GTK_WIDGET (gtk_type_new (ghfw_dlg_window_get_type ()));
  GTK_WINDOW (dlg_window)->type = GTK_WINDOW_TOPLEVEL;
  gtk_window_set_title (GTK_WINDOW (dlg_window), title);

  return dlg_window;
}

void
ghfw_dlg_window_set_escapable (GhfwDlgWindow *dlg_window)
{
  dlg_window->escapable = TRUE;
}

void
ghfw_dlg_window_set_content (GhfwDlgWindow *window, GtkWidget *content)
{
  if (!window->content)
    {
      gtk_box_pack_start (GTK_BOX (window->vbox), content,
			  TRUE, FALSE, 2);
      window->content = content;
    }
  else
    g_print ("GhfwDlgWindow->content (%x->%x) is not NULL\n",
	     (int) window, (int) window->content);
}

void
ghfw_dlg_window_set_content_with_frame (GhfwDlgWindow *window, GtkWidget *content)
{
  GtkWidget *frame;

  frame = gtk_frame_new (NULL);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
  gtk_container_add (GTK_CONTAINER (frame), content);
  gtk_widget_show (frame);

  ghfw_dlg_window_set_content (window, frame);

  window->content = content;
}

void
ghfw_dlg_window_set_button_box (GhfwDlgWindow *window,
				GtkWidget *button_box)
{
  if (!window->button_box)
    {
      gtk_box_pack_end (GTK_BOX (window->vbox), button_box,
			FALSE, FALSE, 0);
      window->button_box = button_box;
    }
  else
    g_print ("GhfwDlgWindow->button_box (%x->%x) is not NULL\n",
	     (int) window, (int) window->button_box);
}

void
ghfw_dlg_window_set_button (GhfwDlgWindow *window,
			    GtkWidget *button)
{
  GtkWidget *button_box;

  button_box = ghfw_dlg_window_bbox ();
  gtk_box_pack_start (GTK_BOX (button_box), button,
		      FALSE, FALSE, 0);

  ghfw_dlg_window_set_button_box (window,
				button_box);

  GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
}

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

#include "i18n.h"

enum {
  PARAM_0,
  ESCAPABLE,
  CONTENT,
  BUTTON_BOX,
  VBOX
};

static guint dlg_window_escaped_signal;
static GObjectClass *parent_class = NULL;

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
      g_signal_emit (G_OBJECT (widget), dlg_window_escaped_signal, 0);
      g_signal_stop_emission_by_name (G_OBJECT (widget), "key_press_event");
      gtk_widget_destroy (widget);

      ret_code = TRUE;
    }
  else
    ret_code = FALSE;

  return ret_code;
}

/* local functions */

GtkWidget *
ghfw_dlg_window_button_box ()
{
  GtkWidget *button_box;

  button_box = gtk_hbutton_box_new ();
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (button_box), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (button_box), 5);

  gtk_widget_show (button_box);

  return button_box;
}

static void
ghfw_dlg_window_set_property (GObject *object,
			      guint property_id,
			      const GValue *value,
			      GParamSpec *pspec)
{
  GhfwDlgWindow *dlg_window;
  gboolean bool_value;
  gpointer ptr_value;

  dlg_window = GHFW_DLG_WINDOW (object);

  switch (property_id)
    {
    case ESCAPABLE:
      bool_value = g_value_get_boolean (value);
      dlg_window->escapable = bool_value;
      break;
    case CONTENT:
      ptr_value = g_value_get_pointer (value);
      ghfw_dlg_window_set_content (dlg_window, ptr_value);
      break;
    case BUTTON_BOX:
      ptr_value = g_value_get_pointer (value);
      ghfw_dlg_window_set_button_box (dlg_window, ptr_value);
      break;
    }
}

static void
ghfw_dlg_window_get_property (GObject *object,
			      guint property_id,
			      GValue *value,
			      GParamSpec *pspec)
{
  GhfwDlgWindow *dlg_window;

  dlg_window = GHFW_DLG_WINDOW (object);

  switch (property_id)
    {
    case ESCAPABLE:
      g_value_set_boolean (value, dlg_window->escapable);
      break;
    case CONTENT:
      g_value_set_pointer (value, dlg_window->content);
      break;
    case BUTTON_BOX:
      if (!dlg_window->button_box)
	g_value_set_pointer (value, ghfw_dlg_window_button_box ());
      else
	g_value_set_pointer (value, dlg_window->button_box);
      break;
    case VBOX:
      g_value_set_pointer (value, dlg_window->vbox);
      break;
/*     default: */
/*       property->type = G_TYPE_INVALID; */
/*       break; */
    }
}

/* Widget creation stuff */

static void
ghfw_dlg_window_class_init (GhfwDlgWindowClass *klass)
{
  GObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  g_object_class_install_property
    (object_class, ESCAPABLE,
     g_param_spec_boolean ("escapable",
			   _("Escapable"),
			   _("Whether the dialog can be close with the"
			     " Esc key."),
			   0, G_PARAM_READWRITE));
  g_object_class_install_property
     (object_class, CONTENT,
      g_param_spec_object ("content",
			   _("Content"),
			   _("The content appearing within the frame."),
			   GTK_TYPE_WIDGET, G_PARAM_READWRITE));

  g_object_class_install_property
     (object_class, BUTTON_BOX,
     g_param_spec_object ("button_box",
			   _("Button Box"),
			   _("The button box at the botton of the window."),
			   GTK_TYPE_BUTTON_BOX, G_PARAM_READWRITE));
  g_object_class_install_property
     (object_class, VBOX,
     g_param_spec_object ("vbox",
			   _("Vbox"),
			   _("A random vbox...."),
			   GTK_TYPE_VBOX, G_PARAM_READWRITE));

  dlg_window_escaped_signal =
    g_signal_new ("escaped",
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GhfwDlgWindowClass, escaped),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

/*   g_object_class_add_signals (object_class, &dlg_window_escaped_signal, 1); */

  object_class->set_property = ghfw_dlg_window_set_property;
  object_class->get_property = ghfw_dlg_window_get_property;

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

GType
ghfw_dlg_window_get_type (void)
{
  static GType dlg_window_type = 0;

  if (!dlg_window_type)
    {
      GTypeInfo dlg_window_info =
	{
	  sizeof (GhfwDlgWindowClass),
	  (GBaseInitFunc) NULL,
	  (GBaseFinalizeFunc) NULL,
	  (GClassInitFunc) ghfw_dlg_window_class_init,
	  (GClassFinalizeFunc) NULL,
	  NULL,
	  sizeof (GhfwDlgWindow),
	  0,
	  (GInstanceInitFunc) ghfw_dlg_window_init,
	};

      dlg_window_type = g_type_register_static (GTK_TYPE_WINDOW,
						"GhfwDlgWindow",
						&dlg_window_info,
						0);
    }

  return dlg_window_type;
}

/* Regular public stuff */

GhfwDlgWindow*
ghfw_dlg_window_new (gchar *title)
{
  GhfwDlgWindow *dlg_window;
  
  dlg_window = g_object_new (GHFW_DLG_WINDOW_TYPE, NULL);
/*   GTK_WINDOW (dlg_window)->type = GTK_WINDOW_DIALOG; */
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
ghfw_dlg_window_set_content_with_frame (GhfwDlgWindow *window,
					GtkWidget *content)
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

  button_box = ghfw_dlg_window_button_box ();
  gtk_box_pack_start (GTK_BOX (button_box), button,
		      FALSE, FALSE, 0);

  ghfw_dlg_window_set_button_box (window,
				button_box);

  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
}

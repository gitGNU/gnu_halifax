/* dialog_window.c - this file is part of the GNU HaliFAX Viewer
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

#include "gtkutils.h"

typedef struct _DialogWindow DialogWindow;

struct _DialogWindow
{
  GtkWidget *window, *vbox;
  GtkWidget *content, *button_box;
};

/* dialog windows */

DialogWindow *
dialog_window_new (gchar *title)
{
  DialogWindow *window;

  window = g_malloc (sizeof (DialogWindow));
  window->window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_window_set_title (GTK_WINDOW (window->window), title);
  gtk_window_set_position (GTK_WINDOW (window->window), GTK_WIN_POS_CENTER);
  window->vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (window->window),
		     window->vbox);

  window->content = NULL;
  window->button_box = NULL;

  return window;
}

void
dialog_window_set_escapable (DialogWindow *window)
{
  gtk_window_set_escapable ((GtkWindow*) window->window);
}

void
dialog_window_set_escapable_with_callback (DialogWindow *window,
					   GtkSignalFunc callback,
					   gpointer user_data)
{
  EscCallbackData *esc_cb_data;

  esc_cb_data = g_malloc (sizeof (EscCallbackData));
  esc_cb_data->callback = callback;
  esc_cb_data->user_data = user_data;
  gtk_signal_connect (GTK_OBJECT (window->window), "destroy",
		      (GtkSignalFunc) free_data_on_destroy_cb,
		      esc_cb_data);
  gtk_signal_connect (GTK_OBJECT (window->window), "key-press-event",
		      GTK_SIGNAL_FUNC (key_press_event_cb), esc_cb_data);
}

GtkWidget *
dialog_window_bbox ()
{
  GtkWidget *button_box;

  button_box = gtk_hbutton_box_new ();
  gtk_container_set_border_width (GTK_CONTAINER (button_box), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (button_box), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (button_box), 5);

  return button_box;
}

void
dialog_window_set_content (DialogWindow *window, GtkWidget *content)
{
  if (!window->content)
    {
      gtk_box_pack_start (GTK_BOX (window->vbox), content,
			  TRUE, FALSE, 2);
      window->content = content;
    }
  else
    g_print ("DialogWindow->content (%x->%x) is not free\n",
	     (int) window, (int) window->content);
}

void
dialog_window_set_content_with_frame (DialogWindow *window, GtkWidget *content)
{
  GtkWidget *frame;

  frame = gtk_frame_new (NULL);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
  gtk_container_add (GTK_CONTAINER (frame), content);
  dialog_window_set_content (window, frame);
  
  window->content = content;
}

void
dialog_window_add_destroy_callback (DialogWindow *window,
				    GtkSignalFunc callback,
				    gpointer data)
{
  gtk_signal_connect (GTK_OBJECT (window->window),
		      "destroy",
		      callback, data);
}

void
dialog_window_set_button_box (DialogWindow *window,
			      GtkWidget *button_box)
{
  if (!window->button_box)
    {
      gtk_box_pack_end (GTK_BOX (window->vbox), button_box,
			FALSE, FALSE, 0);
      window->button_box = button_box;
    }
  else
    g_print ("DialogWindow->button_box (%x->%x) is not free\n",
	     (int) window, (int) window->button_box);
}

void
dialog_window_set_button (DialogWindow *window,
			  GtkWidget *button)
{
  GtkWidget *button_box;

  button_box = dialog_window_bbox ();
  gtk_box_pack_start (GTK_BOX (button_box), button,
		      FALSE, FALSE, 0);

  dialog_window_set_button_box (window,
				button_box);

  GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
}

GtkWidget *
dialog_window_get_gtkwin (DialogWindow *window)
{
  return (window->window);
}

void
dialog_window_show (DialogWindow *dialog, GtkWidget *parent)
{
  transient_window_show (dialog->window, parent);
}

void
dialog_window_destroy (DialogWindow *dialog)
{
  gtk_widget_destroy (dialog->window);
  g_free (dialog);
}

void
dialog_window_destroy_from_signal (GtkWidget *widget,
				   DialogWindow *dialog)
{
  dialog_window_destroy (dialog);
}

/* gtkutils.c - this file is part of the GNU HaliFAX Viewer
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

/* This file implements some useful GTK+-related functions used all
   around the program */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "setup.h"

typedef struct _DialogWindow DialogWindow;

struct _DialogWindow
{
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *content;
  GtkHButtonBox *button_box;
};

/* Menu creation */

GtkWidget*
menu_separator_new (GtkWidget *menu)
{
  GtkWidget *separator;

  separator = gtk_menu_item_new ();
  gtk_widget_set_sensitive (separator, FALSE);
  gtk_container_add (GTK_CONTAINER (menu), separator);

  return separator;
}

void
menu_item_new (GtkWidget *menu, gchar *label,
	       GtkSignalFunc callback, gpointer data)
{
  GtkWidget *new_menu_item;

  new_menu_item = gtk_menu_item_new_with_label (label);
  gtk_container_add (GTK_CONTAINER (menu), new_menu_item);

  if (callback)
    gtk_signal_connect (GTK_OBJECT (new_menu_item), "activate",
			callback, data);
}

/* Pixmaps and icons */

GtkWidget *
pixmap_from_xpm (GtkWidget *ref_widget, gchar *file_name)
{
  GtkWidget *gtk_pixmap;
  GdkPixmap *pixmap;
  GdkBitmap *mask;

  pixmap = gdk_pixmap_create_from_xpm
    (ref_widget->window, &mask,
     &(ref_widget->style->bg[GTK_STATE_NORMAL]),
     file_name);

  gtk_pixmap = gtk_pixmap_new (pixmap, mask);

  return gtk_pixmap;
}

#ifndef __WIN32__
void
window_set_icon (GtkWidget* ref_widget, gchar *file_name)
{
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  
  pixmap = gdk_pixmap_create_from_xpm
    (ref_widget->window, &mask,
     &(ref_widget->style->bg[GTK_STATE_NORMAL]),
     file_name);
  
  gdk_window_set_icon (ref_widget->window, ref_widget->window, pixmap, mask);
}
#endif /* __WIN32__ */


/* transient windows */

static void
transient_destroy_cb (GtkWidget *window, gpointer data)
{
  GtkWindow *parent;

  parent = gtk_object_get_data (GTK_OBJECT (window), "parent_was_modal");
  if (parent)
    gtk_window_set_modal (parent, TRUE);
    
  decrease_win_count ();
}

void
transient_window_show (GtkWindow *transient, GtkWindow *parent)
{
  gtk_window_set_transient_for (transient, parent);

  if (parent->modal)
    {
      gtk_object_set_data (GTK_OBJECT (transient), "parent_was_modal",
			   parent);
      gtk_window_set_modal (parent, FALSE);
    }

  gtk_window_set_modal (transient, TRUE);
  gtk_window_set_policy (GTK_WINDOW (transient), FALSE, FALSE, TRUE);

  gtk_signal_connect (GTK_OBJECT (transient), "destroy",
		      transient_destroy_cb, NULL);

  gtk_widget_show_all (GTK_WIDGET (transient));
#ifndef __WIN32__
  window_set_icon (GTK_WIDGET (transient),
		   PIXMAP ("ghfaxviewer-icon.xpm"));
#endif

  increase_win_count ();
}

/* ESC-key handling */

static gboolean
key_press_event_cb (GtkWidget *window, GdkEventKey *event,
		    gpointer user_data)
{
  gboolean ret_code;

  if (event->keyval == GDK_Escape)
    {
      gtk_widget_destroy (window);
      ret_code = TRUE;
    }
  else
    ret_code = FALSE;

  return ret_code;
}

void
gtk_window_set_escapable (GtkWindow *window)
{
  gtk_signal_connect (GTK_OBJECT (window), "key-press-event",
		      GTK_SIGNAL_FUNC (key_press_event_cb), NULL);
}

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
  gtk_signal_connect (GTK_OBJECT (window->window), "destroy",
		      GTK_SIGNAL_FUNC (callback), data);
}

void
dialog_window_set_button_box (DialogWindow *window,
			      GtkHButtonBox *button_box)
{
  if (!window->button_box)
    {
      dialog_window_set_escapable (window);
      gtk_box_pack_end (GTK_BOX (window->vbox), GTK_WIDGET (button_box),
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
				GTK_HBUTTON_BOX (button_box));

  GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button);
}

GtkWindow *
dialog_window_get_gtkwin (DialogWindow *window)
{
  return ((GtkWindow*) window->window);
}

void
dialog_window_show (DialogWindow *dialog, GtkWindow *parent)
{
  transient_window_show (GTK_WINDOW (dialog->window), parent);
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

/* very simple GtkRcStyle stuff */

void
back_gtkstyle (GtkRcStyle *style, GtkStateType state,
	       gushort red, gushort green, gushort blue)
{
  GdkColor color;

  color.red = red;
  color.green = green;
  color.blue = blue;
  style->bg[state] = color;
  style->color_flags[state] = GTK_RC_BG;
}

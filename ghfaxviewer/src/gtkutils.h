/* gtkutils.h - this file is part of the GNU HaliFAX Viewer
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

/* Some useful GTK+-related functions used all around the program */

#ifndef GTKUTILS_H
#define GTKUTILS_H

typedef gpointer DialogWindow;                         /* private */

GtkWidget* menu_separator_new (GtkWidget *menu);
void menu_item_new (GtkWidget *menu, gchar *label,
		    GtkSignalFunc callback, gpointer data);

void gtk_window_set_escapable (GtkWindow *window);

void dialog_window_set_escapable (DialogWindow *window);
GtkWidget *dialog_window_bbox ();

DialogWindow *dialog_window_new (gchar *title);
void dialog_window_set_content (DialogWindow *window, GtkWidget *content);
void dialog_window_set_content_with_frame (DialogWindow *window,
					   GtkWidget *content);
void dialog_window_set_button_box (DialogWindow *window,
				   GtkHButtonBox *button_box);
void dialog_window_set_button (DialogWindow *window,
			       GtkWidget *button);
GtkWindow *dialog_window_get_gtkwin (DialogWindow *window);
void dialog_window_show (DialogWindow *dialog, GtkWindow *parent);
void dialog_window_destroy (DialogWindow *dialog);
void dialog_window_destroy_from_signal (GtkWidget *widget, gpointer dialog);
void dialog_window_add_destroy_callback (DialogWindow *window,
					 GtkSignalFunc callback,
					 gpointer data);

void transient_window_show (GtkWindow *transient, GtkWindow *parent);

GtkWidget *pixmap_from_xpm (GtkWidget *ref_widget, gchar *file_name);

void back_gtkstyle (GtkRcStyle *style, GtkStateType state,
		    gushort red, gushort green, gushort blue);

#ifndef __WIN32__
void window_set_icon (GtkWidget *window, gchar *file_name);
#endif /* __WIN32__ */
	
#endif /* GTKUTILS_H */

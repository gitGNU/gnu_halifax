/* gtkutils.h - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000, 2001, 2002, 2003 Wolfgang Sourdeau
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

void free_data_on_destroy_cb (GtkWidget *widget, gpointer data);

GtkWidget *menu_separator_new (GtkWidget *menu);
GtkWidget *menu_item_new (GtkWidget *menu, gchar *label,
			  GtkSignalFunc callback, gpointer data);

void gtk_window_set_escapable (GtkWidget *window);
void transient_window_show (GtkWidget *transient, GtkWidget *parent);

/* GtkWidget *pixmap_from_xpm (GtkWidget *ref_widget, gchar *file_name); */
GtkWidget *image_from_xpm_data (GtkWidget *ref_widget, gchar **xpm_data);

void back_gtkstyle (GtkRcStyle *style, GtkStateType state,
		    gushort red, gushort green, gushort blue);

/* Handle-box transientization (for menus, toolbars, ...) */

void handle_box_transient_cb (GtkHandleBox *handle_box,
			      GtkWidget *hb_child,
			      GdkWindow *parent);

void launch_program (const gchar *program, gchar *argv[]);

#ifndef __WIN32__
void window_set_icon (GtkWidget *window, gchar *file_name);
#endif /* __WIN32__ */
	
#endif /* GTKUTILS_H */

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

/* A pseudo-widget to create the thumbnail area */

#ifndef THUMBLAYOUT_H
#define THUMBLAYOUT_H

GtkWidget *layout_new (GtkWidget *ref_widget, GtkOrientation orientation,
		       gint spacing, gint width);
void layout_reset (GtkWidget *layout);

void layout_add_widget (GtkWidget *layout, GtkWidget *widget);
void layout_set_bg_color (GtkWidget *layout,
			  gushort red, gushort green, gushort blue);

#endif

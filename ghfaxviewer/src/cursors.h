/* cursors.h - this file is part of the GNU HaliFAX Viewer
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

/* Code taken from the Eye of Gnome image viewer
 *
 * Copyright (C) Federico Mena-Quintero <federico@gimp.org>
 *
 */

#ifndef CURSORS_H
#define CURSORS_H

#include <gdk/gdk.h>



typedef enum {
	CURSOR_HAND_OPEN,
	CURSOR_HAND_CLOSED,
	CURSOR_NUM_CURSORS
} CursorType;

void cursor_set (GdkWindow *window, CursorType type);
void gdkcursor_set (GdkWindow *window, GdkCursorType type);



#endif

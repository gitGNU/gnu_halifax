/* cursors.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000 The Free Software Foundation, inc.
 * Copyright (C) 2000, 2001, 2002, 2003 Wolfgang Sourdeau
 *
 * Time-stamp: <2003-03-07 10:19:24 wolfgang>
 *
 * Author: Wolfgang Sourdeau <wolfgang@contre.com>
 * 
 * NOTE: This code was originally taken from the Eye of GNOME image viewer,
 *       whose author is Federico Mena-Quintero <federico@gimp.org>
 *	 copyright holder is the Free Software Foundation, inc.
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

/* This file implement non-builtin cursor handling */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "cursors.h"



/* Cursor definitions.  Keep in sync with the CursorType enumeration in
 * cursors.h.
 */

#include "pixmaps/hand-open-data.xbm"
#include "pixmaps/hand-open-mask.xbm"
#include "pixmaps/hand-closed-data.xbm"
#include "pixmaps/hand-closed-mask.xbm"

static struct {
	char *data;
	char *mask;
	int data_width;
	int data_height;
	int mask_width;
	int mask_height;
	int hot_x, hot_y;
} cursors[] = {
	{ hand_open_data_bits, hand_open_mask_bits,
	  hand_open_data_width, hand_open_data_height,
	  hand_open_mask_width, hand_open_mask_height,
	  hand_open_data_width / 2, hand_open_data_height / 2 },
	{ hand_closed_data_bits, hand_closed_mask_bits,
	  hand_closed_data_width, hand_closed_data_height,
	  hand_closed_mask_width, hand_closed_mask_height,
	  hand_closed_data_width / 2, hand_closed_data_height / 2 },
	{ NULL, NULL, 0, 0, 0, 0 }
};



void cursor_set (GdkWindow *window, CursorType type)
{
	GdkBitmap *data;
	GdkBitmap *mask;
	GdkColor black, white;
	GdkCursor *cursor;

	g_assert (window != NULL);
	g_assert (type >= 0 && type < CURSOR_NUM_CURSORS);

	g_assert (cursors[type].data_width == cursors[type].mask_width);
	g_assert (cursors[type].data_height == cursors[type].mask_height);

	data = gdk_bitmap_create_from_data (window,
					    cursors[type].data,
					    cursors[type].data_width,
					    cursors[type].data_height);
	mask = gdk_bitmap_create_from_data (window,
					    cursors[type].mask,
					    cursors[type].mask_width,
					    cursors[type].mask_height);

	g_assert (data != NULL && mask != NULL);

	gdk_color_black (gdk_window_get_colormap (window), &black);
	gdk_color_white (gdk_window_get_colormap (window), &white);

	cursor = gdk_cursor_new_from_pixmap (data, mask, &white, &black,
					     cursors[type].hot_x, cursors[type].hot_y);
	g_assert (cursor != NULL);

	gdk_window_set_cursor (window, cursor);

	gdk_bitmap_unref (data);
	gdk_bitmap_unref (mask);
	gdk_cursor_destroy (cursor);
}

void gdkcursor_set (GdkWindow *window, GdkCursorType type)
{
  GdkCursor *cursor;

  cursor = gdk_cursor_new (type);
  gdk_window_set_cursor (window, cursor);
  gdk_cursor_destroy (cursor);
}

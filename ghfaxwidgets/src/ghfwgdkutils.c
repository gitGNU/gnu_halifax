/* gdkfontutils.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2001 Wolfgang Sourdeau
 *
 * Author: Wolfgang Sourdeau <wolfgang@contre.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gdk/gdk.h>

typedef enum
{
  JUSTIFY_LEFT,
  JUSTIFY_RIGHT,
} Justification;

typedef struct _TextBox TextBox;

struct _TextBox
{
  gint x, y;
  gint width, height;
  gchar *string;
  Justification justification;
  GdkFont *font;
  GdkDrawable *drawable;
};

TextBox *
text_box_new (gchar *string, Justification justification,
	      gint x, gint y, gint width, gint height)
{
  TextBox *text_box;

  text_box = g_malloc (sizeof (TextBox));

  text_box->x = x;
  text_box->y = y;
  text_box->width = width;
  text_box->height = height;
  text_box->justification = justification;
  text_box->drawable = NULL;
  if (string)
    text_box->string = g_strdup (string);
  else
    text_box->string = NULL;

  return text_box;
}

void
text_box_destroy (TextBox *text_box)
{
  if (text_box->string)
    g_free (text_box->string);
  if (text_box->drawable)
    gdk_window_unref (text_box->drawable);
  g_free (text_box);
}

gboolean
text_box_attach_to_drawable (TextBox *text_box, GdkDrawable *drawable)
{
  return FALSE;
}

/* Cursors */


void gdkcursor_set (GdkWindow *window, GdkCursorType type)
{
  GdkCursor *cursor;

  cursor = gdk_cursor_new (type);
  gdk_window_set_cursor (window, cursor);
  gdk_cursor_destroy (cursor);
}

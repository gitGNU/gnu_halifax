/* ghfwgdkutils.h - this file is part of $PROJECT_NAME_HERE$
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

#ifndef GHFWGDKUTILS_H
#define GHFWGDKUTILS_H

typedef void TextBox; /* private */

typedef enum
{
  JUSTIFY_LEFT,
  JUSTIFY_RIGHT,
} Justification;


TextBox *text_box_new (gchar *string, Justification justification,
		       gint x, gint y, gint width, gint height);

void text_box_destroy (TextBox *text_box);

gboolean text_box_attach_to_drawable (TextBox *text_box,
				      GdkDrawable *drawable);

void gdkcursor_set (GdkWindow *window, GdkCursorType type);

#endif /* GHFWGDKUTILS_H */

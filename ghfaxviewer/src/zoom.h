/* zoom.h - this file is part of the GNU HaliFAX Viewer
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

#ifndef ZOOM_H
#define ZOOM_H

typedef void (*TiDrawFunc) (FaxPage *ref_page,
			    gint x, gint height, gpointer user_data);

FaxPage *ti_zoomed_fax_page (FaxPage *orig_page,
			     guint new_width, guint new_height,
			     FaxRotationType rotation);
void ti_set_draw_func (TiDrawFunc new_draw_func);
void ti_set_draw_func_user_data (gpointer user_data);
void ti_cancel_zoom ();

#endif

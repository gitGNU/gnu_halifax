/* progress.h - this file is part of the GNU HaliFAX Viewer
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

/* Functions to create progress and manage dialogs easily */

#ifndef PROGRESS_H
#define PROGRESS_H

typedef gpointer GfvProgressData;                      /* private */

typedef enum
{
  ABORT_BTN = 1,
  DISPLAY_WHEN_NEEDED = 2,
} GfvProgressTag;

GfvProgressData *gfv_progress_new (GtkWindow *parent_window,
				   gchar *title,
				   gchar *action_string,
				   GfvProgressTag tag);
void gfv_progress_destroy (GfvProgressData *progress_data);

/* convenient callback functions */

gboolean gfv_progress_update_with_percentage (guint value, guint total,
					      guint percentage, gpointer data);
gboolean gfv_progress_update_with_value (guint value, guint total,
					 guint percentage, gpointer data);

/* convenient functions */
void gfv_progress_set_action (GfvProgressData *progress_data,
			      gchar *new_action);
void gfv_progress_set_done (GfvProgressData *progress_data);

#endif

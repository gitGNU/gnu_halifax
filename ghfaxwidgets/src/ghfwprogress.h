/* ghfwprogress.h - this file is part of the GNU HaliFAX Widgets library
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GHFW_PROGRESS_WINDOW_TYPE            (ghfw_progress_window_get_type ())
#define GHFW_PROGRESS_WINDOW(obj)		(GTK_CHECK_CAST (obj, GHFW_PROGRESS_WINDOW_TYPE, GhfwProgressWindow))
#define GHFW_PROGRESS_WINDOW_CLASS(obj)	(GTK_CHECK_CAST (obj, GHFW_PROGRESS_WINDOW_TYPE, GhfwProgressWindowClass))
#define GHFW_IS_PROGRESS_WINDOW(obj)         (GTK_CHECK_TYPE (obj, GHFW_PROGRESS_WINDOW_TYPE))
#define GHFW_IS_PROGRESS_WINDOW_CLASS(klass) (GTK_CHECK_CLASS_TYPE (klass, GHFW_PROGRESS_WINDOW_TYPEo))

typedef struct _GhfwProgressWindow GhfwProgressWindow;
typedef struct _GhfwProgressWindowClass GhfwProgressWindowClass;

struct _GhfwProgressWindow
{
  GhfwDlgWindow window;

  GtkWidget *label, *progress_bar, *abort_btn;

  gchar *action_string;
  guint value;
  guint max;

  /* flags */
  gboolean done;
  gboolean aborted;
  gboolean abortable;
};

struct _GhfwProgressWindowClass
{
  GhfwDlgWindowClass parent_class;

  void (*aborted) (GhfwProgressWindow *progress_window);
};

GtkType   ghfw_progress_window_get_type (void);

GhfwProgressWindow *ghfw_progress_window_new (gchar *title,
					      gchar *action_string);

/* convenient callback functions */

gboolean
ghfw_progress_window_update_with_percentage (GhfwProgressWindow *progress_window,
					     guint percentage);

gboolean
ghfw_progress_window_update_with_value (GhfwProgressWindow *progress_window,
					guint value, guint total);

void ghfw_progress_window_set_max (GhfwProgressWindow *progress_window,
				   guint max);
void ghfw_progress_window_increment (GhfwProgressWindow *progress_window,
				     guint increment);

/* convenient functions */
void ghfw_progress_window_set_action (GhfwProgressWindow *progress_win,
				      gchar *new_action);
void ghfw_progress_window_set_done (GhfwProgressWindow *progress_win,
				    gboolean done);
void ghfw_progress_window_set_abortable (GhfwProgressWindow *progress_window,
					 gboolean abortable);

#endif

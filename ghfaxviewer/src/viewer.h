/* viewer.h - this file is part of the GNU HaliFAX Viewer
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

#ifndef VIEWER_H
#define VIEWER_H

#include "i18n.h"

#ifdef __WIN32__
#define DEGREES "Â°"
#else
#define DEGREES "°"
#endif

#define MAX_ZOOM_INDEX 8

typedef struct _ViewerData ViewerData;
typedef struct _MotionData MotionData;

struct _ViewerData
{
  FaxFile *fax_file;
  FaxPage *current_page;
  guint zoom_index;
  
  GdkPixmap *page_pixmap;
  GtkWidget *viewer_window;
  GtkWidget *page_area;
  int rotation;
  
  /* button bar */
  GtkWidget *bb_buttons[4];
  GtkWidget *ut_buttons[5]; 

  /* print dialog */
  GtkWidget *zoom_entry;
  
  /* print dialog */
  GtkWidget *print_dialog;
  
  /* thumbnails */
  GtkWidget *thumbs_fixed;
  GtkWidget **th_buttons;
  
  /* Cursor motion data */
  MotionData *motion_data;
};

struct _MotionData
{
  gboolean dragging;
  guint orig_x, orig_y;
  gfloat orig_adj_x, orig_adj_y;
};

typedef enum
{
  NEXT_PAGE,
  PREV_PAGE,
  ZOOM_IN,
  ZOOM_OUT
} VariableButtonIndex;

typedef enum
{
  RIGHT90,
  LEFT90,
  REVERSE,
  PRINT,
  INFO,
} FixedButtonIndex;

GdkPixmap *pixmap_for_page (GtkWidget *ref_widget,
			    gint ref_width, gint ref_height,
			    FaxRotationType rotation,
			    int with_frame);

ViewerData *fax_viewer_new ();
void fax_viewer_open_file (ViewerData *viewer_data, gchar *file_name);

void increase_win_count ();
void decrease_win_count ();

/* functions from C files of which it would be a waste of time to
 * create an interface H file. */

void draw_page (ViewerData *viewer_data);
GtkWidget *thumbs_fixed_new ();
void add_thumbs (ViewerData *viewer_data);
void destroy_thumb (gpointer fixed_child, gpointer container);

void about_cb (GtkWidget *irrelevant_1, gpointer irrelevant_2);
void close_file_cb (GtkWidget *irrelevant, ViewerData *viewer_data);
void file_dialog_cb (GtkWidget* irrelevant, ViewerData *viewer_data);
void info_cb (GtkWidget *irrelevant, ViewerData *viewer_data);

#ifdef __WIN32__
gchar *win32_pixmap_file (gchar *pixmap);
#endif

void closeviewer_cb (GtkWidget *widget, ViewerData *viewer_data);

#endif

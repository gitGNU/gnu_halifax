/* callbcks.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000, 2001, 2002, 2003 Wolfgang Sourdeau
 *
 * Time-stamp: <2003-03-07 10:19:15 wolfgang>
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

/* This file contains menu and toolbar callbacks only */

#include <gtk/gtk.h>
#include <ghfaxwidgets/ghfaxwidgets.h>

#include "tiffimages.h"
#include "viewer.h"
#include "setup.h"

void
nextpage_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  FaxPage *current_page, *next_page;

  current_page = viewer_data->current_page;
  next_page = current_page->next;
  
  if (next_page)
    {
      viewer_data->current_page = current_page->next;
      draw_page (viewer_data);
    }
}

void
prevpage_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  FaxPage *current_page, *prev_page;

  current_page = viewer_data->current_page;
  prev_page = current_page->prev;
  
  if (prev_page)
    {
      viewer_data->current_page = current_page->prev;
      draw_page (viewer_data);
    }
}	

void
zoomout_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  viewer_data->zoom_index--;
      
  draw_page (viewer_data);
}

void
zoomin_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  viewer_data->zoom_index++;
      
  draw_page (viewer_data);
}

void
reverse_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  switch (viewer_data->rotation)
    {
    case ROT_NONE:
      viewer_data->rotation = ROT_180;
      break;
    case ROT_180:
      viewer_data->rotation = ROT_NONE;
      break;
    case ROT_RIGHT90:
      viewer_data->rotation = ROT_LEFT90;
      break;
    case ROT_LEFT90:
      viewer_data->rotation = ROT_RIGHT90;
      break;
    }
  
  draw_page (viewer_data);
}

void
right90_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  switch (viewer_data->rotation)
    {
    case ROT_NONE:
      viewer_data->rotation = ROT_RIGHT90;
      break;
    case ROT_180:
      viewer_data->rotation = ROT_LEFT90;
      break;
    case ROT_RIGHT90:
      viewer_data->rotation = ROT_180;
      break;
    case ROT_LEFT90:
      viewer_data->rotation = ROT_NONE;
      break;
    }

  draw_page (viewer_data);
}

void
left90_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  switch (viewer_data->rotation)
    {
    case ROT_NONE:
      viewer_data->rotation = ROT_LEFT90;
      break;
    case ROT_180:
      viewer_data->rotation = ROT_RIGHT90;
      break;
    case ROT_RIGHT90:
      viewer_data->rotation = ROT_NONE;
      break;
    case ROT_LEFT90:
      viewer_data->rotation = ROT_180;
      break;
    }
  
  draw_page (viewer_data);
}

void widget_close_cb (GtkWidget *widget, ViewerData *viewer_data)
{
#ifdef CAN_SAVE_CONFIG
  save_window_coords (viewer_data->viewer_window->window);
#endif
  gtk_widget_destroy (viewer_data->viewer_window);
}

void
close_file_cb (GtkWidget *irrelevant, ViewerData *viewer_data)
{
  fax_viewer_open_file (viewer_data, NULL);  
}

gboolean progress_update (guint value, guint total,
			  GhfwProgressWindow *progress_window)
{
  return (ghfw_progress_window_update_with_value (progress_window, value, total));
}

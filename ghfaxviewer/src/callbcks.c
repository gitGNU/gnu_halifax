/* callbcks.c - this file is part of the GNU HaliFAX Viewer
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

/* This file contains menu and toolbar callbacks only */

#include <gtk/gtk.h>

#include "tiffimages.h"
#include "viewer.h"

void
nextpage_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  FaxPage *current_page, *next_page;

  if (viewer_data->fax_file)
    {
      current_page = viewer_data->current_page;
      next_page = current_page->next;
  
      if (next_page)
	{
	  viewer_data->current_page = current_page->next;
	  draw_page (viewer_data);
	}
    }
}

void
prevpage_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  FaxPage *current_page, *prev_page;

  if (viewer_data->fax_file)
    {
      current_page = viewer_data->current_page;
      prev_page = current_page->prev;
      
      if (prev_page)
	{
	  viewer_data->current_page = current_page->prev;
	  draw_page (viewer_data);
	}
    }
}	

void
zoomout_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  if (viewer_data->fax_file
      && viewer_data->zoom_index != 0)
    {
      viewer_data->zoom_index--;
      
      draw_page (viewer_data);
    }
}

void
zoomin_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  if (viewer_data->fax_file
      && viewer_data->zoom_index != MAX_ZOOM_INDEX)
    {
      viewer_data->zoom_index++;
      
      draw_page (viewer_data);
    }
}

void
reverse_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  if (viewer_data->fax_file)
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
	default:
	}
  
      draw_page (viewer_data);
    }
}

void
right90_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  if (viewer_data->fax_file)
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
	default:
	}

      draw_page (viewer_data);
    }
}

void
left90_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  if (viewer_data->fax_file)
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
	default:
	}
      
      draw_page (viewer_data);
    }
}


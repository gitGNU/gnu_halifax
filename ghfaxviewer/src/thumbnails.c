/* thumbnails.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000-2001 Wolfgang Sourdeau
 *
 * Time-stamp: <2002-10-25 01:08:13 wolfgang>
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

/* These functions handle the thumbnails and thumbnail area of the
   viewer */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <ghfaxwidgets/ghfaxwidgets.h>

#include "tiffimages.h"
#include "zoom.h"
#include "viewer.h"
#include "draw_page.h"
#include "errors.h"
#include "callbcks.h"

typedef struct _DrawReqData DrawReqData;

struct _DrawReqData
{
  ViewerData *viewer_data;
  FaxPage *requested_page;
};

inline static gint
get_thumbed_height (FaxFile *fax_file,
		    FaxPage *image,
		    gint width)
{
  gfloat factor;
  gint height;

  factor = (gfloat) image->width / (gfloat) width;
  height = image->height / factor;
  height *= (gfloat) fax_file->x_res / (gfloat) fax_file->y_res;

  return height;
}

static gint
draw_req_page_cb (GtkWidget *widget, DrawReqData *request)
{
  FaxPage *rqd_page;
  
  rqd_page = request->requested_page;
  
  request->viewer_data->current_page = rqd_page;
  draw_page (request->viewer_data);
  gtk_widget_grab_focus (widget->parent);
  
  return FALSE;
}

GtkWidget *
thumb_button (ViewerData *viewer_data, FaxPage *cur_page,
	      gint th_height, gint th_width)
{
  GtkWidget *button, *gtk_pixmap;
  GdkPixmap *gdk_pixmap;
  GtkTooltips *cur_tooltips;
  GtkRcStyle *bg_style; /*  , *bg_prelight; */
  FaxPage *thumbnail;
  DrawReqData *draw_request;
  DrawData *draw_data;
  gchar *tttext;

  gdk_pixmap = pixmap_for_page (viewer_data->viewer_window,
				th_width, th_height,
				ROT_NONE, TRUE);
  draw_data = prepare_drawing (viewer_data->viewer_window,
			       gdk_pixmap,
			       ROT_NONE);

  ti_set_draw_func (draw_page_func);
  ti_set_draw_func_user_data (draw_data);

  gtk_pixmap = gtk_pixmap_new (gdk_pixmap, NULL);

  thumbnail = ti_zoomed_fax_page (cur_page, th_width, th_height,
				  ROT_NONE);
  
  bg_style = gtk_rc_style_new ();
  back_gtkstyle (bg_style, GTK_STATE_NORMAL, 65535, 65535, 0);
  gtk_widget_modify_style (gtk_pixmap, bg_style);
  gtk_rc_style_unref (bg_style);

  button = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
  gtk_widget_set_usize (button, 60, th_height + 12);
  gtk_container_add (GTK_CONTAINER (button), gtk_pixmap);

  bg_style = gtk_rc_style_new ();
  back_gtkstyle (bg_style, GTK_STATE_ACTIVE, 31488, 32000, 31488);
  back_gtkstyle (bg_style, GTK_STATE_PRELIGHT, 31488, 32000, 31488);
  gtk_widget_modify_style (button, bg_style);
  gtk_rc_style_unref (bg_style);

  draw_request = g_malloc (sizeof (DrawReqData));
  draw_request->viewer_data = viewer_data;
  draw_request->requested_page = cur_page;

  gtk_signal_connect (GTK_OBJECT(button), "destroy",
		      GTK_SIGNAL_FUNC (free_data_on_destroy_cb),
		      draw_request);
  gtk_signal_connect (GTK_OBJECT(button), "clicked",
		      GTK_SIGNAL_FUNC (draw_req_page_cb),
		      draw_request);
  
  cur_tooltips = gtk_tooltips_new();
  tttext = g_strdup_printf (_("Page %d"), cur_page->nbr + 1);
  gtk_tooltips_set_tip (cur_tooltips,
			button,
			tttext, NULL);
  g_free (tttext);
  
  ti_destroy_fax_page (thumbnail);
  g_free (draw_data);      

  gtk_widget_show (gtk_pixmap);
		
  return button;
}

void
add_thumbs (ViewerData *viewer_data)
{
  GtkWidget *progress, *cur_button;
  GtkWidget **b_arr_ptr;
  FaxPage *cur_page;
  gint pos_in_fixed;
  gchar *p_action;
  gint th_height;
  
  progress = ghfw_progress_window_new (_("Please wait..."), NULL);
  ghfw_progress_window_set_abortable (GHFW_PROGRESS_WINDOW (progress), FALSE);

  transient_window_show (progress, viewer_data->viewer_window);

  ti_set_progress_func ((TiProgressFunc) progress_update, progress);
  
  cur_page = viewer_data->fax_file->first;
  
  viewer_data->th_buttons = g_malloc (sizeof (GtkWidget*)
				      *(viewer_data->fax_file->nbr_pages + 1));
  b_arr_ptr = viewer_data->th_buttons;
  pos_in_fixed = 10;
  
  while (cur_page)
    {
      p_action = g_strdup_printf (_("Computing index for page "
				    "%d of %d..."), cur_page->nbr + 1,
				  viewer_data->fax_file->nbr_pages + 1);
      ghfw_progress_window_set_action (GHFW_PROGRESS_WINDOW (progress),
				       p_action);
      g_free (p_action);

      th_height = get_thumbed_height (viewer_data->fax_file,
				      cur_page, 48);

      ti_load_fax_page (viewer_data->fax_file, cur_page);
      cur_button = thumb_button (viewer_data,
				 cur_page, th_height, 48);
      ti_unload_fax_page (cur_page);

      layout_add_widget (viewer_data->thumbs_layout, cur_button);
      gtk_widget_show (cur_button);

      *b_arr_ptr++ = cur_button;
      pos_in_fixed += th_height + 24;
      cur_page = cur_page->next;
    }

  ti_set_progress_func (NULL, NULL);
  gtk_widget_destroy (progress);
}

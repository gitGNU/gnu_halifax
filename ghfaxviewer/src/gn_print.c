/* gn_print.c - this file is part of the GNU HaliFAX Viewer
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

/* This file implement the functions needed to interface with
   gnome-print */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <libgnome/gnome-paper.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-dialog.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>

#include "tiffimages.h"
#include "zoom.h"
#include "viewer.h"
#include "progress.h"
#include "gtkutils.h"

/* Damn, I hate those
   very-long-names-that-just-serve-no-purpose-at-all-except-filling-a-whole-line-just-for-pleasure */
#define GPD(dlg) GNOME_PRINT_DIALOG(dlg)


static double *
create_matrix_from_page (FaxPage *gray_page)
{
  double *matrix;

  matrix = g_malloc (sizeof (gdouble) * 6);
  *matrix = gray_page->width;
  *(matrix + 1) = 0;
  *(matrix + 2) = 0;
  *(matrix + 3) = -gray_page->height;
  *(matrix + 4) = 0;
  *(matrix + 5) = gray_page->height;

  return matrix;
}

static void
print_page (GnomePrintContext *context,
	    FaxFile *fax_file,
	    FaxPage *gray_page)
{
  gchar *page_name;
  double *matrix, scale_x, scale_y;

  page_name = g_strdup_printf ("%d", gray_page->nbr + 1);
  gnome_print_beginpage (context, page_name);

  matrix = create_matrix_from_page (gray_page);

  scale_x = 72.0 / fax_file->x_res;
  scale_y = 72.0 / fax_file->y_res;

  gnome_print_gsave (context);
  gnome_print_concat (context, matrix);
  gnome_print_moveto (context, 0, 0);
  gnome_print_scale (context, scale_x, -scale_y);
  gnome_print_translate (context, 0, -(1 /scale_y));
  gnome_print_grayimage (context, gray_page->image,
			 gray_page->width, gray_page->height,
			 gray_page->width);
  gnome_print_grestore (context);
  gnome_print_showpage (context);

  g_free (matrix);
  g_free (page_name);
}

static gboolean
send_pages_to_pc (GnomePrintContext *context,
		  FaxFile *fax_file,
		  gint from, gint to,
		  gint copies, gint collate,
		  GfvProgressData *p_data)
{
  FaxPage *orig_page, *gray_page;
  gint page_nbr, copy_nbr, count, max_page_nbr;
  gboolean aborted;
  gchar *p_action;

  ti_set_draw_func (NULL);
  aborted = FALSE;
  count = 0;
  max_page_nbr = copies * (to - from + 1);

  if (collate)
    for (copy_nbr = 0; copy_nbr < copies; copy_nbr++)
      for (page_nbr = from - 1; page_nbr < to; page_nbr++)
	{
	  while (!aborted)
	    {
	      count++;
	      p_action = g_strdup_printf (_("Printing page"
					    " %d (%d left)"),
					  count,
					  max_page_nbr
					  - count);
	      gfv_progress_set_action (p_data, p_action);
	      g_free (p_action);
	      
	      orig_page = ti_seek_fax_page (fax_file, page_nbr);
	      ti_load_fax_page (fax_file, orig_page);
	      gray_page = ti_zoomed_fax_page (orig_page,
					      orig_page->width,
					      orig_page->height,
					      ROT_NONE);
	      print_page (context, fax_file, gray_page);
	      ti_destroy_fax_page (gray_page);
	      ti_unload_fax_page (orig_page);
	      
	      aborted = gfv_progress_update_with_value (count, max_page_nbr,
							0, p_data);
	    }
	}
  else
    for (page_nbr = from - 1; page_nbr < to && !aborted; page_nbr++)
      {
	orig_page = ti_seek_fax_page (fax_file, page_nbr);
	ti_load_fax_page (fax_file, orig_page);
	gray_page = ti_zoomed_fax_page (orig_page,
					orig_page->width,
					orig_page->height,
					ROT_NONE);
	for (copy_nbr = 0; copy_nbr < copies && !aborted; copy_nbr++)
	  {
	    count++;
	    p_action = g_strdup_printf (_("Printing page"
					  " %d (%d left)"),
					count,
					max_page_nbr
					- count);
	    gfv_progress_set_action (p_data, p_action);
	    g_free (p_action);
	    
	    print_page (context, fax_file, gray_page);
	    aborted = gfv_progress_update_with_value (count, max_page_nbr,
						      0, p_data);
	    
	  }
	ti_destroy_fax_page (gray_page);
	ti_unload_fax_page (orig_page);
      }

  if (!aborted)
    gfv_progress_set_done (p_data);

  return aborted;
}

static GnomePrintMaster *
prepare_print_master (GtkWidget *print_dlg,
		      FaxFile *fax_file,
		      FaxPage *current_page)
{
  GnomePrintContext *print_context;
  GnomePrintMaster *print_master;
  GnomeFont *def_font;
  gint copies, collate, range, aborted;
  gint from, to;
  GfvProgressData *p_data;

  p_data = gfv_progress_new
    (GTK_WINDOW (print_dlg),
     _("Please wait..."), NULL, ABORT_BTN);

  print_master =
    gnome_print_master_new_from_dialog (GPD (print_dlg));
  print_context =
    gnome_print_master_get_context (print_master);

  def_font = gnome_font_new_closest  ("times", GNOME_FONT_BOOK, 0, 10);
  gnome_print_setfont (print_context, def_font);

  gnome_print_dialog_get_copies (GPD (print_dlg), &copies, &collate);
  range = gnome_print_dialog_get_range_page (GPD (print_dlg), &from,
					     &to);
  if (range == GNOME_PRINT_RANGE_CURRENT)
    from = to = current_page->nbr + 1;
  else if (range == GNOME_PRINT_RANGE_ALL)
    {
      from = 1;
      to = fax_file->nbr_pages + 1;
    }

  aborted = send_pages_to_pc (print_context, fax_file,
			      from, to, copies, collate,
			      p_data);
  gnome_print_context_close (print_context);

  if (aborted)
    {
      gnome_print_master_close (print_master);
      print_master = NULL;
    }

  gfv_progress_destroy (p_data);

  return print_master;
}

static void
print_or_preview (GtkWidget *print_dlg,
		  ViewerData *viewer_data,
		  gint button)
{
  GnomePrintMaster *print_master;
  GtkWidget *preview;

  print_master = prepare_print_master (print_dlg,
				       viewer_data->fax_file,
				       viewer_data->current_page);

  if (print_master)
    {
      if (button == GNOME_PRINT_PRINT)
	gnome_print_master_print (print_master);
      else
	{
	  preview =
	    GTK_WIDGET (gnome_print_master_preview_new (print_master,
							_("Print"
							  " preview...")));
	  
	  transient_window_show (GTK_WINDOW (preview),
				 GTK_WINDOW (print_dlg));
	  
	  gtk_widget_show (preview);
	}

      gnome_print_master_close (print_master);
    }
}

static void
print_dlg_clicked_cb (GtkWidget *print_dlg, gint button,
		      ViewerData *viewer_data)
{
  gboolean destroy_dlg;

  destroy_dlg = TRUE;

  switch (button)
    {
    case GNOME_PRINT_PREVIEW:
      destroy_dlg = FALSE;
    case GNOME_PRINT_PRINT:
      print_or_preview (print_dlg, viewer_data, button);
      break;
    case GNOME_PRINT_CANCEL:
    }

  if (destroy_dlg)
    gtk_widget_destroy (print_dlg);
}

void
print_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  if (viewer_data->fax_file)
    {
      viewer_data->print_dialog =
	gnome_print_dialog_new (_("Print..."),
				GNOME_PRINT_DIALOG_RANGE
				| GNOME_PRINT_DIALOG_COPIES);

      gnome_print_dialog_construct_range_page 
	(GPD (viewer_data->print_dialog),
	 GNOME_PRINT_RANGE_CURRENT
	 | GNOME_PRINT_RANGE_ALL
	 | GNOME_PRINT_RANGE_RANGE,
	 1,
	 viewer_data->fax_file->nbr_pages + 1,
	 _("Current page only"),
	 _("Range"));
					   
      gtk_signal_connect (GTK_OBJECT (viewer_data->print_dialog),
			  "clicked", print_dlg_clicked_cb,
			  viewer_data);

      transient_window_show (GTK_WINDOW (viewer_data->print_dialog),
			     GTK_WINDOW (viewer_data->viewer_window));
    }
}

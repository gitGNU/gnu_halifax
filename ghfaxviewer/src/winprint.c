/* winprint.c - this file is part of the GNU HaliFAX Viewer
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

/* This file interfaces with the Windows printing system */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include <windows.h>
#include <tiffio.h>

#include "tiffimages.h"
#include "viewer.h"
#include "progress.h"

static gboolean aborted;

static void
print_page (HDC dev_cont, FaxPage *cur_page, gint y_res)
{
  guint bpl, pr_width, pr_height;
  BITMAP *bm_struct;
  HBITMAP bm_handler;
  HDC compat_dc;
  
  if (StartPage (dev_cont) > 0)
    {
      bpl = cur_page->width / 8;
      if (cur_page->width % 8)
	bpl++;
      if (bpl % 2)
	bpl++;
      
      pr_width = cur_page->width
	* ((gfloat) GetDeviceCaps (dev_cont, LOGPIXELSX)
	   / 204);
      pr_height = cur_page->height
	* ((gfloat) GetDeviceCaps (dev_cont, LOGPIXELSY)
	   / y_res);
      
      bm_struct = alloca (sizeof (BITMAP));
      bm_struct->bmType = 0;
      bm_struct->bmWidth = cur_page->width;
      bm_struct->bmHeight = cur_page->height;
      bm_struct->bmWidthBytes = bpl;
      bm_struct->bmPlanes = 1;
      bm_struct->bmBitsPixel = 1;
      bm_struct->bmBits = cur_page->image;
      
      bm_handler = CreateBitmapIndirect (bm_struct);
      compat_dc = CreateCompatibleDC (dev_cont);
      SelectObject (compat_dc, bm_handler);
/*        SetStretchBltMode (compat_dc, WHITEONBLACK); */
      
      StretchBlt (dev_cont,
		      
		      0,
		      0,
		      pr_width,
		      pr_height,
		      
		      compat_dc,
		      
		      0,
		      0,
		      cur_page->width,
		      cur_page->height,
		      
		      NOTSRCCOPY);
      
      if (!aborted)
	{
	  EndPage (dev_cont);
	}
      
      DeleteDC(compat_dc);
      DeleteObject (bm_handler);
    }
}

static guint
size_of_output (ViewerData *viewer_data,
		guint min_page_nbr, guint max_page_nbr)
{
  FaxPage *page, *max_page;
  guint result, c_page_nbr;
  
  result = 0;
  page = ti_seek_fax_page (viewer_data->fax_file, min_page_nbr - 1);
  max_page = ti_seek_fax_page (viewer_data->fax_file, max_page_nbr);
  
  while (page != max_page)
    {
      result += page->height;
      page = page->next;
    }

  return result;
}

BOOL CALLBACK abort_proc (HDC hdc, int nCode)
{
  return !aborted;
}

static void
windows_print (ViewerData *viewer_data, PRINTDLG *prt_params)
{
  DOCINFO *doc_info;
  FaxFile *document;
  FaxPage *cur_page;
  GfvProgressData *p_data;
  gchar *p_action;
  guint count, min_page_nbr, max_page_nbr, p_step, p_val, p_max;
  
  if (!(GetDeviceCaps(prt_params->hDC, RASTERCAPS) 
	& RC_BITBLT))
    { 
      MessageBox(NULL, 
		 _("Printer cannot display bitmaps."), 
		 _("Device Error"), 
		 MB_OK); 
    }
  else
    {
      document = viewer_data->fax_file;
      
      min_page_nbr = prt_params->nFromPage;
      max_page_nbr = prt_params->nToPage;
      cur_page = ti_seek_fax_page (document, min_page_nbr - 1);
      
      doc_info = alloca (sizeof (DOCINFO));
      doc_info->cbSize = sizeof (DOCINFO); 
      doc_info->lpszDocName =
	g_strdup_printf ("%s - %s",
			 _("The GNU HaliFAX Viewer"),
			 document->file_name);
      doc_info->lpszOutput = (LPTSTR) NULL; 
      doc_info->lpszDatatype = (LPTSTR) NULL; 
      doc_info->fwType = 0;
      
      StartDoc(prt_params->hDC, doc_info);
      
      p_data = gfv_progress_new
	(GTK_WINDOW (viewer_data->viewer_window),
	 _("Please wait..."), NULL, ABORT_BTN);
      p_max = size_of_output (viewer_data,
			      min_page_nbr, max_page_nbr);
      p_val = 0;
      p_step = p_max;
      
      for (count = min_page_nbr; count <= max_page_nbr
	     && !aborted; count++)
	{
	  p_action = g_strdup_printf (_("Printing page"
					" %d (%d left)"),
				      count,
				      max_page_nbr
				      - count);
	  gfv_progress_set_action (p_data, p_action);
	  g_free (p_action);
	  ti_load_fax_page (document, cur_page);
	  print_page (prt_params->hDC, cur_page, document->y_res);
	  ti_unload_fax_page (cur_page);
	  cur_page = cur_page->next;
	}
      gfv_progress_destroy (p_data);
      if (!aborted)
	EndDoc (prt_params->hDC);
      g_free (doc_info);
    }
}

void print_cb (void *ignored, ViewerData *viewer_data)
{
  int result;
  PRINTDLG *fax_pdlg;
  
  fax_pdlg = g_malloc0 (sizeof (PRINTDLG));
      
  fax_pdlg->hwndOwner = NULL;
  fax_pdlg->hDevNames = NULL;
  fax_pdlg->Flags = PD_RETURNDC | PD_PAGENUMS | PD_NOSELECTION;
  fax_pdlg->nMinPage = 1;
  fax_pdlg->nMaxPage = viewer_data->fax_file->nbr_pages + 1;
  fax_pdlg->nFromPage = 1;
  fax_pdlg->nToPage = viewer_data->fax_file->nbr_pages + 1;
  fax_pdlg->nCopies = 1;
  fax_pdlg->lStructSize = sizeof (PRINTDLG);
  
  result = PrintDlg (fax_pdlg);
  
  if (result)
    {
      SetAbortProc (fax_pdlg->hDC, abort_proc);
      windows_print (viewer_data, fax_pdlg);
    }
  
  DeleteDC(fax_pdlg->hDC); 
  g_free (fax_pdlg);
}


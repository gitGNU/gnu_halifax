/* tiffimages.c - this file is part of the GNU HaliFAX Viewer
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

/* This file implements the handling of TIFF g3/g4 files */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <glib.h>
#include <math.h>
#include <tiffio.h>
#include <unistd.h>

#include "tiffimages.h"
#include "i18n.h"

static FaxOpenError fax_open_error;
static TiProgressFunc progress_func;
static gpointer progress_func_data;

static FaxOpenError
diagnoze_open_error (char *file_name)
{
  gchar tiff_sig[5];
  gchar *c_ptr;
  FaxOpenError err_code;
  FILE *file;
  
  if (!access(file_name, F_OK))
    {
      file = fopen (file_name, "r");
      if (file)
	{
	  fgets (tiff_sig, 5, file);
	  fclose (file);
	  
	  c_ptr = tiff_sig;
	  if ((*c_ptr == 'M'
	       && *(c_ptr + 1) == 'M'
	       && *(c_ptr + 2) == 0
	       && *(c_ptr + 3) == 0x2a)
	      ||
	      (*c_ptr == 'I'
	       && *(c_ptr + 1) == 'M'
	       && *(c_ptr + 2) == 0x2a
	       && *(c_ptr + 3) == 0))
	    err_code = FILE_OK;
	  else
	    err_code = FILE_NOT_TIFF;
	}
      else
	err_code = FILE_NOT_ACCESSIBLE;
    }
  else
    err_code = FILE_NOT_FOUND;
  
  return err_code;	
}

gchar* 
ti_error_string (gchar *file_name)
{
  gchar *err_string;
  
  switch (fax_open_error)
    {
    case FILE_NOT_FAX:
      err_string =
	g_strdup_printf (_("%s is not a G3/G4 Fax File"), file_name);
      break;
    case FILE_NOT_TIFF:
      err_string =
	g_strdup_printf (_("%s is not a TIFF file"), file_name);
      break;
    case FILE_TOO_WEIRD:
      err_string =
	g_strdup_printf (_("%s seems corrupted or incomplete"),
			 file_name);
      break;
    case FILE_NOT_FOUND:
      err_string = g_strdup_printf (_("%s could not be found"),
				    file_name);
      break;
    case FILE_NOT_ACCESSIBLE:
      err_string =
	g_strdup_printf (_("permission to read %s is missing"),
			 file_name);
      break;
    default:
      err_string = g_strdup ("You should not have to read this string"
			     " so it has not been translated");
    }

  return (err_string);
}


gint
rounded_width (gint width)
{
  gint r_width;

  r_width = width + ((8 - (width % 8)) & ~8);

  return r_width;
}

FaxPage*
ti_page_new (int nbr,
	     gint width, gint height,
	     FaxPageDepth depth,
	     guchar *image,
	     FaxPage *prev, FaxPage *next)
{
  FaxPage *new_page;
  gint cwidth;

  if (depth == FP_DEPTH_MONO)
    cwidth = rounded_width (width) / 8;
  else
    cwidth = width;
  
  new_page = g_malloc (sizeof (FaxPage));
  new_page->prev = prev;
  new_page->nbr = nbr;
  new_page->height = height;
  new_page->width = width;
  new_page->cwidth = cwidth;
  new_page->depth = depth;
  new_page->image = image;
  new_page->old_image = NULL;
  new_page->image_index = NULL;

  if (image)
    new_page->limit = image + cwidth * height;
  else
    new_page->limit = NULL;

  return new_page;
}

static gboolean
values_ok (uint32 width, uint32 height, 
	   gfloat x_res, gfloat y_res)
{
  gboolean ret_code;
  
  /* Here we test if the page values seems "normal". The mins and maxs
     are not really normal either, but they can be tolerated. */

  if (width == 0 || width > 10000
      || height == 0 || height > 10000
      || x_res < 50 || x_res > 500
      || y_res < 50 || y_res > 500)
    ret_code = FALSE;
  else
    ret_code = TRUE;
  
  return ret_code;
}

FaxFile*
ti_open_fax_file (char* file_name)
{
  TIFF *tiff_file;
  FaxFile *fax_file;
  FaxPage *current_page, *new_page;
  gboolean is_fax;
  guint count;
  gfloat x_res, y_res;
  uint32 width, height;
  
  is_fax = 0;
  fax_open_error = FILE_OK;
  
  tiff_file = TIFFOpen (file_name, "r");
  
  if (tiff_file)
    {
      TIFFGetField (tiff_file, TIFFTAG_FAXMODE, &is_fax);
      
      if (is_fax)
	{
	  width = 0;
	  height = 0;
	  x_res = 0;
	  y_res = 0;
	  
	  TIFFGetField (tiff_file, TIFFTAG_XRESOLUTION, &x_res);
	  TIFFGetField (tiff_file, TIFFTAG_YRESOLUTION, &y_res);
	  
	  TIFFGetField (tiff_file, TIFFTAG_IMAGEWIDTH, &width);
	  TIFFGetField (tiff_file, TIFFTAG_IMAGELENGTH, &height);

	  if (values_ok (width, height, x_res, y_res))
	    {
	      fax_file = (FaxFile*) g_malloc(sizeof(FaxFile));
	      fax_file->file_name = g_strdup (g_basename (file_name));
	      fax_file->tiff_file = tiff_file;
		  
	      fax_file->x_res = (guint) x_res;
	      fax_file->y_res = (guint) y_res;
		  
	      count = 0;
		  
	      current_page = ti_page_new (0, width, height,
					  FP_DEPTH_MONO,
					  NULL, NULL, NULL);
		  
	      fax_file->first = current_page;
		  
	      while (TIFFReadDirectory (tiff_file))
		{
		  count++;
			  
		  TIFFGetField (tiff_file, TIFFTAG_IMAGEWIDTH, &width);
		  TIFFGetField (tiff_file, TIFFTAG_IMAGELENGTH, &height);
			  
		  new_page = ti_page_new (count, width, height,
					  FP_DEPTH_MONO,
					  NULL, current_page, NULL);
			  
		  current_page->next = new_page;
		  current_page = new_page;
		}
	      current_page->next = NULL;
		  
	      fax_file->nbr_pages = count;
	    }
	  else
	    {
	      fax_file = NULL;
	      fax_open_error = FILE_TOO_WEIRD;	
	      TIFFClose (tiff_file);
	    }
	}
      else
	{
	  fax_file = NULL;
	  fax_open_error = FILE_NOT_FAX;
	  TIFFClose (tiff_file);
	}
    }
  else
    {
      fax_file = NULL;
      fax_open_error = diagnoze_open_error (file_name);
    }
  
  return fax_file;
}

static void
adapt_index (FaxPage *fax_page)
{
  gint delta;
  gint steps;
  GSList *current_elem;

  delta = (guint) fax_page->image - (guint) fax_page->old_image;

  if (delta)
    {
      current_elem = fax_page->image_index->next;
      steps = 0;
      
      while (current_elem)
	{
	  current_elem->data += delta;
	  current_elem = g_slist_next (current_elem);
	  steps++;
	}
    }
}

static void
fill_index (FaxPage *fax_page)
{
  GSList *index;
  gboolean is_open;
  guchar *char_ptr, *prev_prog_step;
  guint prog_min_rel_step;

  char_ptr = fax_page->image;
  index = g_slist_alloc ();
  is_open = FALSE;

  if (progress_func)
    {
      /* This is so that we can spare some time instead of updating
	 the  progress bar every new byte processed. A 10% step is
	 sufficient... */
      prev_prog_step = NULL;
      prog_min_rel_step = (fax_page->limit - char_ptr) / 10;
    }

  while (char_ptr < fax_page->limit)
    {
      if (!is_open && *char_ptr) 
	{
	  index = g_slist_append (index, char_ptr);
	  is_open = TRUE;

	  if (progress_func
	      && char_ptr > (prev_prog_step + prog_min_rel_step))
	    {
	      progress_func (char_ptr - fax_page->image,
			     fax_page->limit - fax_page->image,
			     progress_func_data);
	      prev_prog_step = char_ptr;
	    }
	}
      else if (is_open && !(*char_ptr))
	is_open = FALSE;
	
      char_ptr++;
    }
  
  fax_page->image_index = index;
}

void
ti_load_fax_page (FaxFile *fax_file, FaxPage *fax_page)
{
  tstrip_t strip, nbr_strips;
  tsize_t stripsize;

  TIFFSetDirectory (fax_file->tiff_file, fax_page->nbr);
		
  stripsize = TIFFStripSize(fax_file->tiff_file);
  nbr_strips = TIFFNumberOfStrips(fax_file->tiff_file);
  fax_page->image = _TIFFmalloc(stripsize * nbr_strips);
  fax_page->limit = fax_page->image + fax_page->cwidth * fax_page->height;

  for (strip = 0; strip < nbr_strips; strip++) 
    {       
      TIFFReadEncodedStrip (fax_file->tiff_file, strip,
			    fax_page->image
			    + (strip * stripsize),
			    (tsize_t) - 1);
    }

  if (!fax_page->old_image)
    fill_index (fax_page);
  else
    adapt_index (fax_page);
}

void
ti_destroy_fax_page (FaxPage *fax_page)
{
  if (fax_page->image)
    _TIFFfree (fax_page->image);

  if (fax_page->image_index)
    g_slist_free (fax_page->image_index);
		
  g_free (fax_page);
}

void
ti_unload_fax_page (FaxPage *fax_page)
{
  _TIFFfree (fax_page->image);
  fax_page->old_image = fax_page->image;
  fax_page->image = NULL;
}

FaxPage *
ti_seek_fax_page (FaxFile *fax_file, guint number)
{
  FaxPage *page;
  gint count;

  page = fax_file->first;

  for (count = 0; count < number; count++)
    page = page->next;

  return page;
}

void
ti_destroy_fax_file (FaxFile *fax_file)
{
  gint pcounter;
  FaxPage *cur_page, *next_page;

  cur_page = fax_file->first;
  TIFFClose (fax_file->tiff_file);
  g_free (fax_file->file_name);

  for (pcounter = 0; pcounter < fax_file->nbr_pages + 1; pcounter++)
    {
      next_page = cur_page->next;
      ti_destroy_fax_page (cur_page);
      cur_page = next_page;
    }

  g_free (fax_file);
}

void
ti_set_progress_func (TiProgressFunc function, gpointer data)
{
  progress_func = function;
  progress_func_data = data;
}

TiProgressFunc
ti_get_progress_func ()
{
  return (progress_func);
}



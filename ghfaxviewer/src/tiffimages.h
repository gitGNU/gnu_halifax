/* tiffimages.h - this file is part of the GNU HaliFAX Viewer
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

#ifndef TIFFIMAGE_H
#define TIFFIMAGE_H

#ifdef __MINGW32__
/* Mingw32's Win32 header files are buggy */
#undef NO_ERROR
#endif

typedef struct _FaxFile FaxFile;
typedef struct _FaxPage FaxPage;

/* A hook to permit displaying progress bars or any progress display
   while tiffimage is busy... */
typedef gboolean (*TiProgressFunc) (guint value, guint total, gpointer data);

typedef enum
{
  FILE_OK = 0,
  FILE_NOT_FAX,
  FILE_NOT_TIFF,
  FILE_TOO_WEIRD,
  FILE_NOT_FOUND,
  FILE_NOT_ACCESSIBLE,
  NUM_ERRORS
} FaxOpenError;

typedef enum
{
  ROT_NONE,
  ROT_RIGHT90,
  ROT_180,
  ROT_LEFT90
} FaxRotationType;

typedef enum
{
  FP_DEPTH_MONO,
  FP_DEPTH_GRAY
} FaxPageDepth;

struct _FaxPage
{
  gint nbr; /* real page # - 1 */
  gint width, height;
  gint cwidth;
  FaxPageDepth depth;
  guchar *image, *old_image, *limit;
  GSList *image_index;
  FaxPage *next;
  FaxPage *prev;
};

struct _FaxFile
{
  gchar *file_name;
  guint nbr_pages, x_res, y_res;
  gpointer tiff_file; /* TIFF* */
  FaxPage *first;
};

FaxFile *ti_open_fax_file (gchar* file_name);
void ti_destroy_fax_file (FaxFile *fax_file);

FaxPage* ti_page_new (int nbr,
		      gint width, gint height,
		      FaxPageDepth depth,
		      guchar *image,
		      FaxPage *prev, FaxPage *next);

FaxPage *ti_seek_fax_page (FaxFile *fax_file, guint number);
void ti_load_fax_page (FaxFile *fax_file, FaxPage *fax_page);
void ti_unload_fax_page (FaxPage *fax_page);
void ti_destroy_fax_page (FaxPage *fax_page);
gchar *ti_error_string (gchar *file_name);

void ti_set_progress_func (TiProgressFunc function, gpointer data);
TiProgressFunc ti_get_progress_func ();

gint rounded_width (gint width);

#endif

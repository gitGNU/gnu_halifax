/* tiffinfo.c - this file is part of the GNU HaliFAX Viewer
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

/* Various informations about the G3/G4 TIFF files are gathered here
   from libtiff */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <tiffio.h>

#include "tiffimages.h"
#include "tiffinfo.h"

#define CHAR_FIELD(f,v)				\
  if (!TIFFGetField (fax_file->tiff_file,	\
		     TIFFTAG_##f, &v))		\
    v = NULL

#define INT_FIELD(f,v)            		\
  if (!TIFFGetField (fax_file->tiff_file,	\
		     TIFFTAG_##f, &v))		\
    v = 0

#define FREE_OR_NOT(f)				\
  if (f)					\
    g_free (f)

TiffInfo *
ti_get_file_info (FaxFile *fax_file)
{
  gchar *host_computer, *software, *sender_id, *date_time;
  gchar *sub_address, *make, *model;
  uint16 fill_order, compression, clean_fax_data;
  TiffInfo *file_info;

  file_info = g_malloc0 (sizeof (TiffInfo));

  CHAR_FIELD (HOSTCOMPUTER, host_computer);
  CHAR_FIELD (SOFTWARE, software);
  CHAR_FIELD (IMAGEDESCRIPTION, sender_id);
  CHAR_FIELD (DATETIME, date_time);
  CHAR_FIELD (FAXSUBADDRESS, sub_address);
  CHAR_FIELD (MAKE, make);
  CHAR_FIELD (MODEL, model);

  INT_FIELD (FILLORDER, fill_order);
  INT_FIELD (COMPRESSION, compression);
  INT_FIELD (CLEANFAXDATA, clean_fax_data);
/*    INT_FIELD (BADFAXLINES, bad_fax_lines); */
/*    CHAR_FIELD (FAXSUBADDRESS, subaddress); */

  if (host_computer)
    file_info->host_computer = g_strdup (host_computer);
  if (software)
    file_info->software = g_strdup (software);
  if (sender_id)
    file_info->sender_id = g_strdup (sender_id);
  if (date_time)
    file_info->date_time = g_strdup (date_time);
  if (sub_address)
    file_info->sub_address = g_strdup (sub_address);
  if (make && model)
    file_info->make_model = g_strdup_printf ("%s %s", make, model);

  file_info->fill_order = fill_order;

  switch (compression)
    {
    case COMPRESSION_CCITTFAX3:
      file_info->compression = G3_TYPE;
      INT_FIELD (GROUP3OPTIONS, file_info->g3g4options);
      break;
    case COMPRESSION_CCITTFAX4:
      file_info->compression = G4_TYPE;
      INT_FIELD (GROUP4OPTIONS, file_info->g3g4options);
      break;
    default:
      file_info->compression = UNKNOWN_TYPE;
    }

  switch (fax_file->y_res)
    {
    case 98:
      file_info->resolution = LOW_RES;
      break;
    case 196:
      file_info->resolution = HIGH_RES;
      break;
    default:
      file_info->resolution = WEIRD_RES;
    }

  return file_info; 
}

void
ti_destroy_file_info (TiffInfo *file_info)
{
  FREE_OR_NOT (file_info->host_computer);
  FREE_OR_NOT (file_info->software);
  FREE_OR_NOT (file_info->sender_id);
  FREE_OR_NOT (file_info->date_time);
  FREE_OR_NOT (file_info->sub_address);
  FREE_OR_NOT (file_info->make_model);

  g_free (file_info);
}

/* tiffinfo.h - this file is part of the GNU HaliFAX Viewer
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

#ifndef TIFFINFO_H
#define TIFFINFO_H

/* Resolution */
#define LOW_RES		1
#define HIGH_RES	2
#define WEIRD_RES	3

/* Bit order */
#define MSB_TO_LSB 	1
#define LSB_TO_MSB 	2

/* Fax cleanliness */
#define CLEAN		0
#define REGENERATED	1
#define UNCLEAN		2

/* Compression type */
#define UNKNOWN_TYPE	1
#define G3_TYPE		2
#define G4_TYPE		3

/* G3/G4 options */
#define ENC_2D		1   /* invalid with G4 */
#define UNCOMPRESSED	2
#define FILLBITS	4   /* invalid with G4 */

typedef struct _TiffInfo TiffInfo;

struct _TiffInfo
{
  char *host_computer;
  char *software;
  char *sender_id;
  char *date_time;
  char *sub_address;
  char *make_model;
  unsigned int clean_fax_data;
  unsigned int fill_order;
  unsigned int compression;
  unsigned int resolution;
  unsigned int g3g4options;
};

TiffInfo *ti_get_file_info (FaxFile *fax_file);
void ti_destroy_file_info (TiffInfo *file_info);

#endif

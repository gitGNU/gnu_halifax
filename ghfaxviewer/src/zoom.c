/* zoom.c - this file is part of the GNU HaliFAX Viewer
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

/* This file implements the zoom-related functions */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <tiffio.h>
#include <string.h>

#include "tiffimages.h"

/* max number of lines waiting to be drawn for progressive drawing */
#define MAX_LINES_TO_DRAW 200

typedef struct ir_data_s ir_data_t;

/* An external hook to provide progressive drawing */
typedef void (*TiDrawFunc) (FaxPage *ref_page,
			    gint x, gint height,
			    gpointer user_data);

/* A drawing function choosen following the rotation */
typedef void (*PixelFunc) (ir_data_t *, gint x, gint y, gint intensity);

struct ir_data_s                   /* ir stands for "index record" */
{
  FaxPage *orig_page, *new_page;
  gint max_intensity;              /* max intensity of a pixel / line */
  gfloat fact_x, fact_y;           /* transform. factor */
  gint x_pix_size, y_pix_size;     /* number of bits per pixel for
				      new page */
  gfloat x_rel_step, y_rel_step;   /* number of bits to next
				      relevant series of bits */
  PixelFunc put_pixel;             /* pixel drawing function */

  gint last_drawn_line;	           /* for progressive drawing */
};

static guchar
bit_and_table[] =
{
  255, 127, 63, 31, 15, 7, 3, 1
};

static guchar
rev_bit_and_table[] =
{
  0, 128, 192, 224, 240, 248, 252, 254
};

static guchar
bit_nbr_table[] =
{
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

static TiDrawFunc draw_func = NULL;
static FaxPage *current_page;
static gboolean zoom_cancelled;
static gpointer draw_func_user_data;

static gint
unfloat (gfloat value)
{
  gint ret_val;

  ret_val = (gint) value;
  if ((gfloat) ret_val < value)
    ret_val++;

  return ret_val;
}

static inline void
set_pixel (char *bitmap, gint number, gint intensity)
{
  guchar *byte;

  byte = bitmap + number;

  if ((gint) *byte > intensity)
    *byte -= (char) intensity;
  else
    *byte = 0;
}

static inline int
get_pixel (guchar *image, gint bit_offset, gint size)
{
  gint char_counter, max_count, pix_val, last_bit_offset;
  guchar cur_pix_batch; /* batch of relevant bits in the current
			   character */

  cur_pix_batch = *image & *(bit_and_table + bit_offset);

  if (bit_offset + size < 8)
    {
      cur_pix_batch &= *(rev_bit_and_table + bit_offset + size);
      pix_val = *(bit_nbr_table + cur_pix_batch);
    }
  else
    {
      char_counter = 1;
      pix_val = *(bit_nbr_table + cur_pix_batch);
      max_count = (bit_offset + size) / 8 - 1;

      while (char_counter < max_count)
	{
	  cur_pix_batch = *(image + char_counter);
	  pix_val += *(bit_nbr_table + cur_pix_batch);
	  char_counter++;
	}

      last_bit_offset = (bit_offset + size) % 8;
      cur_pix_batch = *(image + char_counter)
	& *(rev_bit_and_table + last_bit_offset);
      pix_val += *(bit_nbr_table + cur_pix_batch);
    }

  return pix_val;
}

static inline gint
dest_coord (gint orig_coord, gfloat factor)
{
  gint value;

  value = orig_coord * factor;

  return value;
}

void
commit_draw (ir_data_t *ir_data, gint line)
{
  draw_func (ir_data->new_page,
	     ir_data->last_drawn_line,
	     line - ir_data->last_drawn_line,
	     draw_func_user_data);
  
  ir_data->last_drawn_line = line;
}

/* Zoom functions start here */

/* The put_pixel_* functions are designed to provide an
   orientation-independent way of putting pixels on the generated
   image. Actually only put_pixel_rot_none and put_pixel_rot_right90
   are doing the job. The latters just translate virtual
   coordinates to real one before giving the rest of the job to the
   formers. */

static void
put_pixel_rot_none_zoom (ir_data_t *ir_data, gint column,
			 gint line, gint intensity)
{
  gint x_rpix_max, cwidth;
  gchar *coord, *coord_max;

  x_rpix_max = ir_data->x_pix_size;
  cwidth = ir_data->new_page->cwidth;
  coord = ir_data->new_page->image + line * cwidth + column;
  coord_max = coord + ir_data->y_pix_size * cwidth;

  while (coord < coord_max)
    {
      memset (coord, 255 - intensity, x_rpix_max);
      coord += cwidth;
    }
}

static void
put_pixel_rot_180_zoom (ir_data_t *ir_data, gint column,
			gint line, gint intensity)
{
  gint ab_col, ab_line;

  ab_col = ir_data->new_page->width - 1 - column;
  ab_line = ir_data->new_page->height - 1 - line;

  put_pixel_rot_none_zoom (ir_data, ab_col, ab_line, intensity);
}

static void
put_pixel_rot_right90_zoom (ir_data_t *ir_data, gint column,
			    gint line, gint intensity)
{
  gint x_rpix, y_rpix, y_rpix_max, x_rpix_max;
  gint tr_line;
  gchar *line_coord;

  line_coord = ir_data->new_page->image
    + column * ir_data->new_page->height;
  tr_line = ir_data->new_page->height - 1 - line;
  y_rpix_max = ir_data->x_pix_size;
  x_rpix_max = tr_line + ir_data->y_pix_size;

  for (y_rpix = 0; y_rpix < y_rpix_max; y_rpix++)
    {
      for (x_rpix = tr_line; x_rpix < x_rpix_max; x_rpix++)
	set_pixel (line_coord, x_rpix, intensity);

      line_coord += ir_data->new_page->height;
    }
}

static void
put_pixel_rot_left90_zoom (ir_data_t *ir_data, gint column,
			   gint line, gint intensity)
{
  gint ab_col, ab_line;

  ab_col = ir_data->new_page->width - 1 - column;
  ab_line = ir_data->new_page->height - 1 - line;

  put_pixel_rot_right90_zoom (ir_data, ab_col, ab_line, intensity);
}

static void
process_ir_zoom (guchar *ir, ir_data_t *ir_data)
{
  gint dest_pix_val;
  gint dest_col, dest_line;
  gint offset, line;
  gint base_column, byte_offset, bit_offset;
  guchar *byte_ptr;
  gint real_offset;

  offset = (gint) ir - (gint) ir_data->orig_page->image;
  line = offset / ir_data->orig_page->cwidth;
  base_column = (offset % ir_data->orig_page->cwidth) * 8;
  dest_line = dest_coord (line, ir_data->fact_x);

  real_offset = 0;
  bit_offset = 0;
  byte_ptr = ir;

  while (*byte_ptr && (byte_ptr < ir_data->orig_page->limit))
    {
      dest_pix_val = get_pixel (byte_ptr, bit_offset, 1) * 255;

      /* we try to replace x_pix_size with fact_x */
      dest_col = dest_coord (base_column + real_offset,
			     ir_data->fact_x);

      ir_data->put_pixel (ir_data,
			  dest_col, dest_line,
			  dest_pix_val);

      real_offset++;
      byte_offset = (gint) real_offset / 8;
      bit_offset = (gint) real_offset % 8;
      byte_ptr = ir + byte_offset;
    }

  if (draw_func
      && ( dest_line > (ir_data->last_drawn_line
			+ MAX_LINES_TO_DRAW)))
    commit_draw (ir_data, dest_line);
}

/* Unzoom functions start here */

static void
put_pixel_rot_none_unzoom (ir_data_t *ir_data, gint column,
			   gint line, gint intensity)
{
  gint cwidth;
  gchar *line_coord;

  cwidth = ir_data->new_page->cwidth;
  line_coord = ir_data->new_page->image + line * cwidth;

  set_pixel (line_coord, column, intensity);
}

static void
put_pixel_rot_180_unzoom (ir_data_t *ir_data, gint column,
		   gint line, gint intensity)
{
  gint ab_col, ab_line;

  ab_col = ir_data->new_page->width - 1 - column;
  ab_line = ir_data->new_page->height - 1 - line;

  put_pixel_rot_none_unzoom (ir_data, ab_col, ab_line, intensity);
}

static void
put_pixel_rot_right90_unzoom (ir_data_t *ir_data, gint column,
			      gint line, gint intensity)
{
  gint tr_line;
  gchar *line_coord;

  line_coord = ir_data->new_page->image
    + column * ir_data->new_page->height;
  tr_line = ir_data->new_page->height - 1 - line;

  set_pixel (line_coord, tr_line, intensity);
}

static void
put_pixel_rot_left90_unzoom (ir_data_t *ir_data, gint column,
			     gint line, gint intensity)
{
  gint ab_col, ab_line;

  ab_col = ir_data->new_page->width - 1 - column;
  ab_line = ir_data->new_page->height - 1 - line;

  put_pixel_rot_right90_unzoom (ir_data, ab_col, ab_line, intensity);
}

/* FIXME: some zoom factors have problems and are not rendered
   correctly */

static void
process_ir_unzoom (guchar *ir, ir_data_t *ir_data)
{
  gint pix_val, dest_pix_val, dest_pix_size;
  gint base_column, column, dest_col, line, dest_line;
  gint offset, bit_offset, column_delta;
  guchar *byte_ptr;

  /* number of bytes between ir and orig_page->image */
  offset = (gint) ir - (gint) ir_data->orig_page->image;

  base_column = (offset % ir_data->orig_page->cwidth) * 8;
  dest_col = base_column / ir_data->x_rel_step;

  /* FIXME: This is somewhat buggy but it should do 99.9999999% of the
     time.
     We assume that each index record only covers pixels of the same
     line, but of course this may not be true. */
  line = offset / ir_data->orig_page->cwidth;
  dest_line = line * ir_data->fact_y;

  bit_offset = 0;
  byte_ptr = ir;
  dest_pix_size = unfloat (ir_data->x_rel_step);

  do
    {
      column = dest_col * ir_data->x_rel_step;
      column_delta = column - base_column;
      byte_ptr = ir + column_delta / 8;
      bit_offset = column_delta % 8;

      pix_val = get_pixel (byte_ptr, bit_offset, dest_pix_size);
      dest_pix_val = (pix_val * ir_data->max_intensity) / dest_pix_size;
      
      ir_data->put_pixel (ir_data,
			  dest_col, dest_line,
			  dest_pix_val);

      dest_col++;
    }
  while (*byte_ptr && (byte_ptr < ir_data->orig_page->limit));

  if (draw_func
      && ( dest_line > (ir_data->last_drawn_line
			+ MAX_LINES_TO_DRAW)))
    commit_draw (ir_data, dest_line);
}

static PixelFunc
choose_pixel_func (FaxRotationType rotation, gboolean unzoom)
{
  PixelFunc chosen_func;

  switch (rotation)
    {
    case ROT_NONE:
      if (unzoom)
	chosen_func = put_pixel_rot_none_unzoom;
      else
	chosen_func = put_pixel_rot_none_zoom;
      break;
    case ROT_180:
      if (unzoom)
	chosen_func = put_pixel_rot_180_unzoom;
      else
	chosen_func = put_pixel_rot_180_zoom;
      break;
    case ROT_RIGHT90:
      if (unzoom)
	chosen_func = put_pixel_rot_right90_unzoom;
      else
	chosen_func = put_pixel_rot_right90_zoom;
      break;
    case ROT_LEFT90:
      if (unzoom)
	chosen_func = put_pixel_rot_left90_unzoom;
      else
	chosen_func = put_pixel_rot_left90_zoom;
      break;
    }

  return chosen_func;
}

FaxPage *
ti_zoomed_fax_page (FaxPage *orig_page,
		    guint new_width, guint new_height,
		    FaxRotationType rotation)
{
  FaxPage *new_page;
  guchar *new_page_img;
  guint img_size;
  ir_data_t *ir_data;

  zoom_cancelled = FALSE;

  img_size = new_width * new_height;

  new_page_img = _TIFFmalloc (img_size);
  _TIFFmemset (new_page_img, 255, img_size);

  new_page = ti_page_new (orig_page->nbr,
			  new_width, new_height,
			  FP_DEPTH_GRAY,
			  new_page_img,
			  NULL, NULL);

  /* This is dirty */
  current_page = new_page;

  ir_data = g_malloc (sizeof (ir_data_t));
  ir_data->orig_page = orig_page;
  ir_data->new_page = new_page;

  ir_data->fact_x = (gfloat) new_width / orig_page->width;
  ir_data->fact_y = (gfloat) new_height / orig_page->height;
  ir_data->x_rel_step = (gfloat) orig_page->width / new_width;
  ir_data->y_rel_step = (gfloat) orig_page->height / new_height;

  ir_data->last_drawn_line = 0;

  if (new_width > orig_page->width)
    {
      ir_data->max_intensity = 255;
      ir_data->x_pix_size = unfloat (ir_data->fact_x);
      ir_data->y_pix_size = unfloat (ir_data->fact_y);
      ir_data->put_pixel = choose_pixel_func (rotation, FALSE);

      g_slist_foreach (orig_page->image_index->next,
		       (GFunc) process_ir_zoom, ir_data);
    }
  else
    {
      ir_data->max_intensity = 255 / ir_data->y_rel_step;
      ir_data->put_pixel = choose_pixel_func (rotation, TRUE);

      g_slist_foreach (orig_page->image_index->next,
		       (GFunc) process_ir_unzoom, ir_data);
    }

  if (draw_func)
    commit_draw (ir_data, new_height);

  g_free (ir_data);
  
  return new_page;
}

void
ti_set_draw_func (TiDrawFunc new_draw_func)
{
  draw_func = new_draw_func;
}

void
ti_set_draw_func_user_data (gpointer user_data)
{
  draw_func_user_data = user_data;
}

void
ti_cancel_zoom ()
{
  zoom_cancelled = TRUE;
}

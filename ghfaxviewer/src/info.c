/* info.c - this file is part of the GNU HaliFAX Viewer
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

/* This file creates the info dialog and interfaces with tiffinfo.c */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#include "gtkutils.h"
#include "setup.h"
#include "tiffimages.h"
#include "tiffinfo.h"
#include "viewer.h"

static GtkWidget *
info_clean_fax_data (unsigned int clean_fax_data)
{
  GtkWidget *label;
  gchar *text;

  switch (clean_fax_data)
    {
    case CLEAN:
      text = _("this fax is clean");
      break;
    case REGENERATED:
      text = _("some lines have been regenerated");
      break;
    case UNCLEAN:
      text = _("this fax is not clean");
      break;
    default:
      text = _("invalid data");
    }

  label = gtk_label_new (text);

  return label;
}

static GtkWidget *
info_resolution (unsigned int resolution)
{
  GtkWidget *label;
  gchar *text;

  switch (resolution)
    {
    case LOW_RES:
      text = _("low resolution (204 dpi x 98 dpi)");
      break;
    case HIGH_RES:
      text = _("high resolution (204 dpi x 196 dpi)");
      break;
    case WEIRD_RES:
      text = _("strange resolution");
      break;
    default:
      text = _("invalid data");
    }

  label = gtk_label_new (text);

  return label;
}

static GtkWidget *
info_compression (unsigned int compression)
{
  GtkWidget *label;
  gchar *text;

  switch (compression)
    {
    case G3_TYPE:
      text = "CCITT Group 3";
      break;
    case G4_TYPE:
      text = "CCITT Group 4";
      break;
    case UNKNOWN_TYPE:
      text = _("strange compression type");
      break;
    default:
      text = _("invalid data");
    }

  label = gtk_label_new (text);

  return label;
}

static inline void
try_free (gchar *text)
{
  if (text)
    g_free (text);
}

static gchar *
text_append (gchar *old_text, gchar *new_text)
{
  gchar *ret_text;

  if (old_text)
    {
      ret_text = g_strdup_printf ("%s, %s", old_text, new_text);
      g_free (old_text);
    }
  else
    ret_text = g_strdup (new_text);

  return ret_text;
}

static GtkWidget *
info_g3g4options (unsigned int compression, unsigned int g3g4options)
{
  GtkWidget *label;
  gchar *text;

  text = NULL;

  if (compression == G4_TYPE
      && (g3g4options & (ENC_2D | FILLBITS)))
    text = g_strdup (_("invalid data"));
  else
    {
      if (g3g4options & ENC_2D)
	text = g_strdup (_("2D encoding"));

      if (g3g4options & UNCOMPRESSED)
	text = text_append (text, _("uncompressed"));

      if (g3g4options & FILLBITS)
	text = text_append (text, _("fill bits"));
    }

  label = gtk_label_new (text);

  g_free (text);

  return label;
}

static GtkWidget *
info_fill_order (unsigned int fill_order)
{
  GtkWidget *label;
  gchar *text;

  switch (fill_order)
    {
    case LSB_TO_MSB:
      text = _("least to most significant bit (big endian)");
      break;
    case MSB_TO_LSB:
      text = _("most to least significant bit (little endian)");
      break;
    default:
      text = _("invalid data");
    }

  label = gtk_label_new (text);

  return label;
}

static GtkWidget *
info_pixmap (GtkWidget *window)
{
  GtkWidget *gtk_pixmap;
  GdkPixmap *gdk_pixmap;
  GdkBitmap *mask; 
  
  gdk_pixmap = gdk_pixmap_create_from_xpm
    (window->window, &mask,
     &(window->style->bg[GTK_STATE_NORMAL]),
     PIXMAP ("info.xpm"));
  gtk_pixmap = gtk_pixmap_new (gdk_pixmap, mask);
  gtk_misc_set_alignment (GTK_MISC (gtk_pixmap), 0.0, 0.0);

  return gtk_pixmap;
}

static GtkWidget *
prepare_table (GtkWidget *window)
{
  GtkWidget *table;
  GtkWidget *pixmap, *label, *separator;

  table = gtk_table_new (13, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 20);
  gtk_container_set_border_width (GTK_CONTAINER (table), 5);

  pixmap = info_pixmap (window);
  gtk_table_attach_defaults (GTK_TABLE (table), pixmap, 0, 1, 0, 1);

  separator = gtk_hseparator_new ();
  gtk_table_attach_defaults (GTK_TABLE (table), separator, 0, 2, 1, 2);

/*    label = gtk_label_new (_("Fax properties")); */
/*    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0); */
/*    gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 0, 1); */

  label = gtk_label_new (_("Sender ID"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 2, 3);
  label = gtk_label_new (_("Destination subaddress"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 3, 4);
  label = gtk_label_new (_("Date & time of reception"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 4, 5);
  label = gtk_label_new (_("Fax data cleanliness"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 5, 6);
  label = gtk_label_new (_("Resolution"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 6, 7);
  label = gtk_label_new (_("Compression type"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 7, 8);
  label = gtk_label_new (_("G3/G4 Options"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 8, 9);
  label = gtk_label_new (_("Pixel fill order"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 9, 10);
  label = gtk_label_new (_("Make and model of receiving fax"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 10, 11);
  label = gtk_label_new (_("Software"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 11, 12);
  label = gtk_label_new (_("Receiving computer hostname"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 12, 13);

  return table;
}

static GtkWidget *
create_info_table (GtkWidget *window, TiffInfo *file_info)
{
  GtkWidget *table, *label;

  table = prepare_table (window);

  label = gtk_label_new (file_info->sender_id);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 2, 3);
  label = gtk_label_new (file_info->sub_address);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 3, 4);
  label = gtk_label_new (file_info->date_time);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 4, 5);
  label = info_clean_fax_data (file_info->clean_fax_data);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 5, 6);
  label = info_resolution (file_info->resolution);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 6, 7);
  label = info_compression (file_info->compression);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 7, 8);
  label = info_g3g4options (file_info->compression,
			    file_info->g3g4options);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 8, 9);
  label = info_fill_order (file_info->fill_order);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 9, 10);
  label = gtk_label_new (file_info->make_model);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 10, 11);
  label = gtk_label_new (file_info->software);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 11, 12);
  label = gtk_label_new (file_info->host_computer);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 12, 13);

  return table;
}

static DialogWindow *
create_info_dialog (GtkWidget *viewer_window, TiffInfo *file_info)
{
  DialogWindow *info_dialog;
  GtkWidget *table, *ok_button;

  info_dialog = dialog_window_new (_("Fax properties"));
  table = create_info_table (viewer_window, file_info);
  dialog_window_set_content_with_frame (info_dialog, table);

#ifdef NEED_GNOMESUPPORT_H
  ok_button = gnome_stock_button (GNOME_STOCK_BUTTON_OK);
#else /* NEED_GNOMESUPPORT_H */
  ok_button = gtk_button_new_with_label (_("Close"));
#endif
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
		      dialog_window_destroy_from_signal, info_dialog);

  dialog_window_set_button (info_dialog,
			    ok_button);
  dialog_window_set_escapable (info_dialog);

  return info_dialog;
}

void
info_cb (GtkWidget *irrelevant, ViewerData *viewer_data)
{
  DialogWindow *info_dialog;
  TiffInfo *file_info;

  if (viewer_data->fax_file)
    {
      file_info = ti_get_file_info (viewer_data->fax_file);
      info_dialog = create_info_dialog (viewer_data->viewer_window,
					file_info);
      dialog_window_show (info_dialog,
			  GTK_WINDOW (viewer_data->viewer_window));

      ti_destroy_file_info (file_info);
    }
}

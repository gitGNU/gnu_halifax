/* setup.h - this file is part of the GNU HaliFAX Viewer
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

#ifdef __WIN32__
#define PIXMAP(p) win32_pixmap_file (p)
gchar *win32_pixmap_file (gchar *pixmap);
#else
#define PIXMAP(p) PIXMAP_DIR "/" p
#endif

void app_setup (gint *argc, gchar **argv[]);
void vwindow_set_def_coords (GdkWindow *window);

void increase_win_count ();
void decrease_win_count ();

#if defined(NEED_GNOMESUPPORT_H) || defined(__WIN32__)
#define CAN_SAVE_CONFIG 1
#endif

#ifdef CAN_SAVE_CONFIG
void save_window_coords (GdkWindow *window);
gchar *load_last_directory ();
void save_last_directory (gchar *path);
#endif

gchar *where_is (gchar *test_dirs[], gchar *command);

#define STOCK_ZOOM_IN       "GHFV_stock_zoom_in"
#define STOCK_ZOOM_OUT	    "GHFV_stock_zoom_out"
#define STOCK_ZOOM_IN_MENU  "GHFV_stock_zoom_in_menu"
#define STOCK_ZOOM_OUT_MENU "GHFV_stock_zoom_out_menu"

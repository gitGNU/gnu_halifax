/* setup.c - this file is part of the GNU HaliFAX Viewer
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#include <gconf/gconf-client.h>

#include "pixmaps/stock-zoom-in.xpm"
#include "pixmaps/stock-zoom-out.xpm"
#include "pixmaps/stock-zoom-in-menu.xpm"
#include "pixmaps/stock-zoom-out-menu.xpm"
#else
#include <gtk/gtk.h>
#endif

#ifdef __WIN32__
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#endif

#include <tiffio.h>

#include "i18n.h"
#include "setup.h"

#ifdef CAN_SAVE_CONFIG
#define KEY_DEF_DIR "default_dir"

#define KEY_DEF_X "viewer_def_x"
#define KEY_DEF_Y "viewer_def_y"
#define KEY_DEF_WIDTH "viewer_def_width"
#define KEY_DEF_HEIGHT "viewer_def_height"
#endif

static gint windows_count;

#ifdef NEED_GNOMESUPPORT_H
#define CONFIG_KEY "/apps/ghfaxviewer/viewer"
GConfClient *gc_client;
#else
#ifdef __WIN32__
#define CONFIG_KEY "Software\\GNU\\HaliFAX"
#endif /* __WIN32__ */
#endif /* NEED_GNOMESUPPORT_H */

#ifdef CAN_SAVE_CONFIG
static gint viewer_def_x, viewer_def_y;
#endif

static gint viewer_def_width, viewer_def_height;

/* Window accounting */

void
increase_win_count ()
{
  windows_count++;
}

void
decrease_win_count ()
{
  windows_count--;

  if (windows_count == 0)
    gtk_main_quit();
}


/* default dimensions */

static void
gtk_screen_setup ()
{
  gint scr_width, scr_height;

  /* Not a perfect mechanism, but acceptable for now. */
  scr_width = gdk_screen_width ();
  scr_height = gdk_screen_height ();

  if (scr_width > 800 && scr_height > 600)
    {
      viewer_def_width = 800;
      viewer_def_height = 700;
    }
  else
    {
      viewer_def_width = scr_width - 10;
      viewer_def_height = scr_height - 50;
    }
}

void
vwindow_set_def_coords (GdkWindow *window)
{
#ifdef CAN_SAVE_CONFIG
  gdk_window_move_resize (window,
			  viewer_def_x, viewer_def_y,
			  viewer_def_width, viewer_def_height);
#else
  gdk_window_resize (window,
		     viewer_def_width,
		     viewer_def_height);
#endif
}

#if !defined (__WIN32__) && defined (ENABLE_NLS)
static void
locale_setup ()
{
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
}
#endif /* ENABLE_NLS && !WIN32*/

#ifdef NEED_GNOMESUPPORT_H
void
save_window_coords (GdkWindow *window)
{
  gint x, y, width, height;
  GError *gerror;

  gerror = NULL;

  gdk_window_get_root_origin (window, &x, &y);
  gdk_window_get_size (window, &width, &height);

  gconf_client_set_int (gc_client,
			CONFIG_KEY KEY_DEF_X,
			x,
			&gerror);
  gconf_client_set_int (gc_client,
			CONFIG_KEY KEY_DEF_Y,
			y,
			&gerror);
  gconf_client_set_int (gc_client,
			CONFIG_KEY KEY_DEF_WIDTH,
			width,
			&gerror);
  gconf_client_set_int (gc_client,
			CONFIG_KEY KEY_DEF_HEIGHT,
			height,
			&gerror);
}

gchar *load_last_directory ()
{
  GError *gerror;
  gchar *last_dir, *last_dir_slash;

  gerror = NULL;

  last_dir = gconf_client_get_string (gc_client,
				      CONFIG_KEY KEY_DEF_DIR,
				      &gerror);
 
  if (!last_dir)
    last_dir = g_get_current_dir ();

  last_dir_slash = g_strdup_printf ("%s/", last_dir);

  g_free (last_dir);

  return last_dir_slash;
}

void save_last_directory (gchar *path)
{
  GError *gerror;

  gerror = NULL;

  gconf_client_set_string (gc_client,
			   CONFIG_KEY KEY_DEF_DIR,
			   path,
			   &gerror);
}

void
stock_init (void)
{
  static GnomeStockPixmapEntry entries[4];

  entries[0].data.type = GNOME_STOCK_PIXMAP_TYPE_DATA;
  entries[0].data.width = 24;
  entries[0].data.height = 24;
  entries[0].data.xpm_data = stock_zoom_in_xpm;

  entries[1].data.type = GNOME_STOCK_PIXMAP_TYPE_DATA;
  entries[1].data.width = 24;
  entries[1].data.height = 24;
  entries[1].data.xpm_data = stock_zoom_out_xpm;

  entries[2].data.type = GNOME_STOCK_PIXMAP_TYPE_DATA;
  entries[2].data.width = 16;
  entries[2].data.height = 16;
  entries[2].data.xpm_data = stock_zoom_in_menu_xpm;

  entries[3].data.type = GNOME_STOCK_PIXMAP_TYPE_DATA;
  entries[3].data.width = 16;
  entries[3].data.height = 16;
  entries[3].data.xpm_data = stock_zoom_out_menu_xpm;

  gnome_stock_pixmap_register (STOCK_ZOOM_IN, GNOME_STOCK_PIXMAP_REGULAR, &entries[0]);
  gnome_stock_pixmap_register (STOCK_ZOOM_OUT, GNOME_STOCK_PIXMAP_REGULAR, &entries[1]);
  gnome_stock_pixmap_register (STOCK_ZOOM_IN_MENU, GNOME_STOCK_PIXMAP_REGULAR, &entries[2]);
  gnome_stock_pixmap_register (STOCK_ZOOM_OUT_MENU, GNOME_STOCK_PIXMAP_REGULAR, &entries[3]);
}

static void
gnome_screen_setup ()
{
  GError *gerror;

  gerror = NULL;

  viewer_def_x = gconf_client_get_int (gc_client,
				       CONFIG_KEY KEY_DEF_X,
				       &gerror);
  viewer_def_y = gconf_client_get_int (gc_client,
				       CONFIG_KEY KEY_DEF_Y,
				       &gerror);
  viewer_def_width = gconf_client_get_int (gc_client,
					   CONFIG_KEY KEY_DEF_WIDTH,
					   &gerror);
  viewer_def_height = gconf_client_get_int (gc_client,
					    CONFIG_KEY KEY_DEF_HEIGHT,
					    &gerror);

  if (viewer_def_width == 0 && viewer_def_height == 0)
    gtk_screen_setup ();
}
#else
#ifdef __WIN32__
void
save_window_coords (GdkWindow *window)
{
  gint x, y, width, height;
  DWORD buf_size, reg_vtype;
  HKEY reg_key;
  
  gdk_window_get_origin (window, &x, &y);
  gdk_window_get_size (window, &width, &height);
  
  buf_size = sizeof (gint);
  reg_vtype = REG_DWORD;
  
  RegCreateKey (HKEY_CURRENT_USER,
		CONFIG_KEY,
		&reg_key);
  RegSetValueEx (reg_key,
		 KEY_DEF_X, 0,
		 reg_vtype,
		 (LPBYTE) &x, buf_size);
  RegSetValueEx (reg_key,
		 KEY_DEF_Y, 0,
		 reg_vtype,
		 (LPBYTE) &y, buf_size);
  RegSetValueEx (reg_key,
		 KEY_DEF_WIDTH, 0,
		 reg_vtype,
		 (LPBYTE) &width, buf_size);
  RegSetValueEx (reg_key,
		 KEY_DEF_HEIGHT, 0,
		 reg_vtype,
		 (LPBYTE) &height, buf_size);
  RegCloseKey (reg_key);  
}

void save_last_directory (gchar *path)
{
  HKEY reg_key;
  
  RegCreateKey (HKEY_CURRENT_USER,
		CONFIG_KEY,
		&reg_key);
  RegSetValueEx (reg_key,
		 KEY_DEF_DIR, 0,
		 REG_SZ,
		 (LPBYTE) path, strlen (path));
  RegCloseKey (reg_key); 
}

gchar *load_last_directory ()
{
  gint rc;
  DWORD key_vtype, buf_size;
  gchar *last_dir, *last_dir_slash;
  HKEY reg_key;

  rc = RegOpenKey (HKEY_CURRENT_USER,
		   CONFIG_KEY,
		   &reg_key);

  if (rc)
    last_dir = g_get_current_dir ();
  else
    {
      key_vtype = REG_SZ;

      RegQueryValueEx (reg_key,
		       KEY_DEF_DIR, 0,
		       &key_vtype,
		       NULL, &buf_size);
      
      /* The buffer concept is a stupid and dirty idea */
      if (buf_size)
	{
	  last_dir = g_malloc (buf_size + 1);
	  RegQueryValueEx (reg_key,
			   KEY_DEF_DIR, 0,
			   &key_vtype,
			   last_dir, &buf_size);	  
	}
      else
	last_dir = g_get_current_dir ();   
      
      RegCloseKey (reg_key);
    }
  
  last_dir_slash = g_strdup_printf ("%s\\", last_dir);
  
  g_free (last_dir);
  
  return last_dir_slash;
}

static gchar*
win32_local_dir ()
{
  gboolean inited = FALSE;
  static gchar file_name[MAX_PATH];
  gchar *sep;

  if (!inited) 
    {
      GetModuleFileName (NULL, file_name, sizeof (file_name));
      sep = strrchr (file_name, '\\');
      *sep = '\0';
    }

  return file_name;
}

gchar *
win32_pixmap_file (gchar *pixmap)
{
  gchar *pixmap_file;

  pixmap_file = g_strdup_printf ("%s/icons/%s",
				win32_local_dir (),
				pixmap);

  return pixmap_file;
}

static void
win32_screen_setup ()
{
  gint rc;
  DWORD key_vtype, buf_size;
  HKEY reg_key;

  rc = RegOpenKey (HKEY_CURRENT_USER,
		   CONFIG_KEY,
		   &reg_key);

  buf_size = sizeof (int);
  if (rc)
    gtk_screen_setup ();
  else
    {
      key_vtype = REG_DWORD; /* I just don't understand the absurdity
				in this API */
      rc = RegQueryValueEx (reg_key,
			    KEY_DEF_X, 0,
			    &key_vtype,
			    (LPBYTE) &viewer_def_x, &buf_size);
      if (rc)
	gtk_screen_setup ();
      else
	{
	  RegQueryValueEx (reg_key,
			   KEY_DEF_Y, 0,
			   &key_vtype,
			   (LPBYTE) &viewer_def_y, &buf_size);
	  RegQueryValueEx (reg_key,
			   KEY_DEF_WIDTH, 0,
			   &key_vtype,
			   (LPBYTE) &viewer_def_width, &buf_size);
	  RegQueryValueEx (reg_key,
			   KEY_DEF_HEIGHT, 0,
			   &key_vtype,
			   (LPBYTE) &viewer_def_height, &buf_size);
	}

      RegCloseKey (reg_key);
    }
}

#ifndef DEBUG
static void
dummy_output_handler (const gchar* log_domain,
		      GLogLevelFlags log_level,
		      const gchar *message,
		      gpointer user_data)
{
}
#endif

static void
win32_app_setup ()
{
  gchar *local_dir, *gtkrc_file;

#ifndef DEBUG
  g_log_set_handler ("Gtk",
		     G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING,
		     (GLogFunc) dummy_output_handler,
		     NULL);
  g_log_set_handler ("Gdk",
		     G_LOG_LEVEL_WARNING,
		     (GLogFunc) dummy_output_handler,
		     NULL);
#endif /* DEBUG */

  local_dir = win32_local_dir ();
  gtkrc_file = g_strdup_printf ("%s/%s", local_dir, "gfv_gtkrc");

  gtk_rc_parse (gtkrc_file);
  g_free (gtkrc_file);
  g_free (local_dir);
}

static void
locale_setup ()
{
  gchar *local_dir, *locale_dir;

  local_dir = win32_local_dir ();

  locale_dir = g_strdup_printf ("%s/%s", local_dir, "locale");
  bindtextdomain (PACKAGE, locale_dir);
  textdomain (PACKAGE);
}
#endif /* __WIN32__ */
#endif /* NEED_GNOMESUPPORT_H */

void
app_setup (gint *argc, gchar **argv[])
{
  TIFFSetErrorHandler (NULL);
  TIFFSetWarningHandler (NULL);

  windows_count = 0;
#ifdef __WIN32__
  win32_app_setup ();
#endif /* WIN32 */
#ifdef ENABLE_NLS
  locale_setup ();
#endif /* ENABLE_NLS */

#ifdef NEED_GNOMESUPPORT_H
  gconf_init (*argc, *argv, NULL);
  gnome_init (PACKAGE, VERSION, *argc, *argv);
  stock_init ();

  gc_client = gconf_client_get_default ();
  gconf_client_set_error_handling (gc_client,
				   GCONF_CLIENT_HANDLE_NONE);
  gnome_screen_setup ();
#else
  gtk_set_locale ();
  gtk_init (argc, argv);
  gdk_rgb_init ();
#ifdef __WIN32__
  win32_screen_setup ();
#else
  gtk_screen_setup ();
#endif /* __WIN32__ */
#endif /* NEED_GNOMESUPPORT_H */
}

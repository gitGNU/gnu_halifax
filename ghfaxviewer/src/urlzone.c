/* urlzone.c - this file is part of the GNU HaliFAX Viewer
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

/* A URL Zone is a zone attached to a GtkPixmap widget that is
   sensitive to mouse moves and presses, and which underscores URL
   with a certain colour as well as launch the default browser
   whenever the URL is clicked. Due to its specific use, it would be
   useless to turn it to a GTK+ Widget. */

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else
#include <gtk/gtk.h>
#ifdef __WIN32__
#include <windows.h>
#endif
#endif

#include "cursors.h"

typedef struct _UrlZone UrlZone;

struct _UrlZone
{
  GtkPixmap *zone_pixmap;
  guint x, y, width, height;
  gchar *url;

  /* mouse-related status data */
  gboolean mouse_over;
};

/* callback helpers */
static void
browse_url (gchar *url)
{
#ifdef __WIN32__
  ShellExecute (NULL, "open", url,
	        NULL, NULL, SW_SHOWNORMAL);
#else
#ifdef NEED_GNOMESUPPORT_H
  gnome_url_show (url);
#else
  gchar *browser, *params[3];
  gchar *test_dirs[] =
    {
      "/usr/bin",
      "/opt/bin",
      "/usr/local/bin",
      "/usr/local/netscape",
      "/opt/netscape",
      NULL
    };

  params[1] = url;
  params[2] = NULL;

  browser = where_is (test_dirs, "galeon");
  if (browser)
    params[0] = "-w";
  else
    {
      browser = where_is (test_dirs, "mozilla");
      params[0] = "--remote";

      if (!browser)
	browser = where_is (test_dirs, "netscape");
    }

  if (browser)
    {
      launch_program (browser, params);
      g_free (browser);
    }
#endif
#endif
}

/* Callbacks */
static gboolean
motion_notify_event_cb (GtkWidget *pixmap_widget,
			GdkEventMotion *event,
			UrlZone *url_zone)
{
  guint mouse_x, mouse_y;

  mouse_x = (guint) event->x;
  mouse_y = (guint) event->y;

  if (url_zone->mouse_over)
    {
      if ((mouse_x < url_zone->x)
	  || (mouse_x > url_zone->x + url_zone->width - 1)
	  || (mouse_y < url_zone->y)
	  || (mouse_y > url_zone->y + url_zone->height - 1))
	{
	  gdkcursor_set (pixmap_widget->parent->window,
			 GDK_LEFT_PTR);
	  url_zone->mouse_over = FALSE;
	}
    }
  else
    {
      if ((mouse_x > url_zone->x)
	  && (mouse_x < url_zone->x + url_zone->width - 1)
	  && (mouse_y > url_zone->y)
	  && (mouse_y < url_zone->y + url_zone->height - 1))
	{
	  gdkcursor_set (pixmap_widget->parent->window,
			 GDK_HAND2);
	  url_zone->mouse_over = TRUE;
	}      
    }

  return FALSE;
}

static gboolean
button_release_event_cb (GtkWidget *pixmap_widget,
			 GdkEventButton *event,
			 UrlZone *url_zone)
{
  if (event->button == 1 && url_zone->mouse_over)
    browse_url (url_zone->url);

  return FALSE;
}

UrlZone *
url_zone_new (gchar *url, gint x, gint y, gint width, gint height)
{
  UrlZone *url_zone;

  url_zone = g_malloc (sizeof (UrlZone));
  url_zone->zone_pixmap = NULL;
  url_zone->url = g_strdup (url);
  url_zone->x = x;
  url_zone->y = y;
  url_zone->width = width;
  url_zone->height = height;
  url_zone->mouse_over = FALSE;

  return url_zone;
}

void
url_zone_destroy (UrlZone *url_zone)
{
  g_free (url_zone->url);
  g_free (url_zone);
}

void
url_zone_attach (UrlZone *url_zone, GtkWidget *pixmap_widget)
{
  GtkPixmap *pixmap;

  pixmap = GTK_PIXMAP (pixmap_widget);
  url_zone->zone_pixmap = pixmap;

  gdk_window_set_events (pixmap_widget->window,
			 (gdk_window_get_events (pixmap_widget->window)
			  | GDK_POINTER_MOTION_MASK
			  | GDK_EXPOSURE_MASK
			  | GDK_BUTTON_PRESS_MASK
			  | GDK_BUTTON_RELEASE_MASK));

  gtk_signal_connect (GTK_OBJECT (pixmap_widget->parent),
		      "button-release-event",
		      GTK_SIGNAL_FUNC (button_release_event_cb),
		      url_zone);
  gtk_signal_connect (GTK_OBJECT (pixmap_widget->parent),
		      "motion-notify-event",
		      GTK_SIGNAL_FUNC (motion_notify_event_cb),
		      url_zone);
}

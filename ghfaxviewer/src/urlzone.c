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

typedef struct _UrlZone UrlZone;

struct _UrlZone
{
  GtkPixmap *zone_pixmap;
  GdkPixmap *zone_copy;
  gint x, y, width, height;
  gchar *url;

  /* mouse-related status data */
  gboolean mouse_over, button_pressed;
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
button_press_event_cb (GtkWidget *pixmap_widget,
		       GdkEventButton *event,
		       UrlZone *url_zone)
{
  if (event->button == 1)
    url_zone->button_pressed = TRUE;
}

static gboolean
button_release_event_cb (GtkWidget *pixmap_widget,
			 GdkEventButton *event,
			 UrlZone *url_zone)
{
  if (event->button == 1)
    {
      if (url_zone->mouse_over)
	browse_url (url_zone->url);

      url_zone->button_pressed = FALSE;
    }
}

UrlZone *
url_zone_new (gchar *url, gint x, gint y, gint width, gint height)
{
  UrlZone *url_zone;

  url_zone = g_malloc (sizeof (UrlZone));
  url_zone->zone_pixmap = NULL;
  url_zone->zone_copy = NULL;
  url_zone->url = g_strdup (url);
  url_zone->x = x;
  url_zone->y = y;
  url_zone->width = width;
  url_zone->height = height;

  return url_zone;
}

void
url_zone_attach (UrlZone *url_zone, GtkWidget *pixmap_widget)
{
  GtkPixmap *pixmap;

  pixmap = GTK_PIXMAP (pixmap_widget);
  url_zone->zone_pixmap = pixmap;

  gdk_window_set_events (pixmap_widget->window,
			 (gdk_window_get_events (pixmap_widget->pixmap)
			  | GDK_EXPOSURE_MASK
			  | GDK_BUTTON_RELEASE_MASK));

  gtk_signal_connect (GTK_OBJECT (pixmap_widget),
		      "button-press-event",
		      GTK_SIGNAL_FUNC (button_press_event_cb),
		      url_zone);
  gtk_signal_connect (GTK_OBJECT (pixmap_widget),
		      "button-release-event",
		      GTK_SIGNAL_FUNC (button_release_event_cb),
		      url_zone);
  gtk_signal_connect (GTK_OBJECT (pixmap_widget),
		      "motion-notify-event",
		      GTK_SIGNAL_FUNC (motion_notify_event_cb),
		      url_zone);
}

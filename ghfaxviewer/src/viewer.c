/* viewer.c - this file is part of the GNU HaliFAX Viewer
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

/* This file implements the viewer window mechanisms and the main
   function */

/* FIXME: This file contains the main routines plus a couple of
   utility routines which should be separated at some point so as to
   make it cleaner */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#include <stdio.h>

#include "tiffimages.h"
#include "cursors.h"
#include "errors.h"
#include "viewer.h"
#include "toolbar.h"
#include "menu.h"
#include "viewer.h"
#include "setup.h"
#include "gtkutils.h"

static gint
motion_notify_event_cb (GtkWidget *widget,
			GdkEventMotion *event,
			ViewerData *viewer_data)
{
  GtkAdjustment *adj_x, *adj_y;
  GdkModifierType mods;
  gint x, y, valx, valy, valx_up_lim, valy_up_lim;
  gint ret_code;

  if (viewer_data->motion_data->dragging)
    {
      adj_x = gtk_viewport_get_hadjustment
	(GTK_VIEWPORT (viewer_data->page_area->parent->parent));
      adj_y = gtk_viewport_get_vadjustment
	(GTK_VIEWPORT (viewer_data->page_area->parent->parent));

      if (event->is_hint) 
	gdk_window_get_pointer
	  (widget->window, &x, &y, &mods);
      else
	{
	  x = event->x;
	  y = event->y;
	}

      valx = viewer_data->motion_data->orig_adj_x
	+ viewer_data->motion_data->orig_x - x;
      valy = viewer_data->motion_data->orig_adj_y
	+ viewer_data->motion_data->orig_y - y;

      valx_up_lim = adj_x->upper - 2 * adj_x->page_increment;
      valy_up_lim = adj_y->upper - 2 * adj_y->page_increment;

      if (valx < 0)
	{
	  valx = 0;
	  viewer_data->motion_data->orig_x =
	    x - viewer_data->motion_data->orig_adj_x;
	}
      else if (valx > valx_up_lim)
	{
	  valx = valx_up_lim;
	  viewer_data->motion_data->orig_x =
	    x + valx_up_lim - viewer_data->motion_data->orig_adj_x;
	}

      if (valy < 0)
	{
	  valy = 0;
	  viewer_data->motion_data->orig_y =
	    y - viewer_data->motion_data->orig_adj_y;
	}
      else if (valy > valy_up_lim)
	{
	  valy = valy_up_lim;
	  viewer_data->motion_data->orig_y =
	    y + valy_up_lim - viewer_data->motion_data->orig_adj_y;
	}

      gtk_adjustment_set_value
	(adj_x, valx); 
      gtk_adjustment_set_value
	(adj_y, valy);

      ret_code = TRUE;
    }
  else
    ret_code = FALSE;

  return ret_code;
}

static gint
button_release_event_cb (GtkWidget *widget,
			 GdkEventButton *event,
			 ViewerData *viewer_data)
{
  guint ret_code;

  if (viewer_data->motion_data->dragging
      && event->button == 1)
    {
      viewer_data->motion_data->dragging = FALSE;
      cursor_set (viewer_data->page_area->window,
		  CURSOR_HAND_OPEN);
      gdk_pointer_ungrab (event->time);

      ret_code = TRUE;
    }
  else
    ret_code = FALSE;

  return ret_code;
}

static gint
button_press_event_cb (GtkWidget *widget,
		       GdkEventButton *event,
		       ViewerData *viewer_data)
{
  GtkAdjustment *adj_x, *adj_y;
  guint ret_code;
  GdkModifierType mods;

  if (!viewer_data->motion_data->dragging
      && event->button == 1)
    {
      adj_x = gtk_viewport_get_hadjustment
	(GTK_VIEWPORT (viewer_data->page_area->parent->parent));
      adj_y = gtk_viewport_get_vadjustment
	(GTK_VIEWPORT (viewer_data->page_area->parent->parent));

      viewer_data->motion_data->dragging = TRUE;
      gdk_window_get_pointer
	(viewer_data->viewer_window->window,
	 &viewer_data->motion_data->orig_x,
	 &viewer_data->motion_data->orig_y,
	 &mods);

      viewer_data->motion_data->orig_adj_x = adj_x->value;
      viewer_data->motion_data->orig_adj_y = adj_y->value;

      cursor_set (viewer_data->page_area->window,
		  CURSOR_HAND_CLOSED);
			    
      gdk_pointer_grab (viewer_data->viewer_window->window,
			FALSE,
			(GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_BUTTON_RELEASE_MASK),
			NULL,
			NULL,
			event->time);

      ret_code = TRUE;
    }
  else
    ret_code = FALSE;

  return ret_code;
}

#ifdef CAN_SAVE_CONFIG
/* When the close button is pressed */
gint
precloseviewer_cb (GtkWidget *widget, GdkEventAny *event,
		   gpointer nothing)
{
  save_window_coords (event->window);

  return FALSE;
}
#endif

void
closeviewer_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  if (viewer_data->fax_file)
    {
      ti_destroy_fax_file (viewer_data->fax_file);
      g_free (viewer_data->th_buttons);
      g_free (viewer_data->motion_data);
    }

/*   if (viewer_data->page_pixmap) */
/*     gdk_pixmap_unref (viewer_data->page_pixmap); */

  decrease_win_count ();
}

#ifndef __WIN32__
static void
page_area_destroy_cb (GtkWidget *widget, GdkWindow *page_area_window)
{
  gdk_window_set_back_pixmap (page_area_window, NULL, TRUE);
}
#endif /* __WIN32__ */

static void
viewer_set_cmd_widgets_sensitive (ViewerData *viewer_data, gboolean state)
{
  gint wid_count;

  for (wid_count = FIRST_BUTTON; wid_count < NBR_ACTIONS; wid_count++)
    gtk_widget_set_sensitive (viewer_data->cmd_buttons [wid_count],
			      state);
  for (wid_count = FIRST_MENU; wid_count < NBR_ACTIONS; wid_count++)
    gtk_widget_set_sensitive (viewer_data->cmd_menus [wid_count],
			      state);
}

static void
viewer_window_unset_file (ViewerData *viewer_data)
{
  GList *fixed_children;

  fixed_children =
    gtk_container_children (GTK_CONTAINER
			    (viewer_data->thumbs_fixed));
  g_list_foreach (fixed_children, destroy_thumb,
		  viewer_data->thumbs_fixed);

  viewer_set_cmd_widgets_sensitive (viewer_data, FALSE);

  ti_destroy_fax_file (viewer_data->fax_file);

  gdk_pixmap_unref (viewer_data->page_pixmap);
  gdk_window_set_back_pixmap
    (viewer_data->page_area->window,
     NULL, FALSE);
  gtk_widget_hide (viewer_data->page_area);

  viewer_data->fax_file = NULL;
  viewer_data->current_page = NULL;
  viewer_data->zoom_index = 4;
  viewer_data->rotation = ROT_NONE;
  viewer_data->page_pixmap = NULL;
}

void
fax_viewer_open_file (ViewerData *viewer_data, gchar *file_name)
{
  FaxFile *fax_file;
  gchar *title, *app_title;

  app_title = _("GNU HaliFAX - Viewer");

  if (file_name)
    {
      fax_file = ti_open_fax_file (file_name);

      if (!fax_file)
	file_open_error (viewer_data->viewer_window,
			 g_basename (file_name));
      else
	{
	  if (viewer_data->fax_file)
	    viewer_window_unset_file (viewer_data);

	  viewer_data->fax_file = fax_file;

	  title = g_strdup_printf ("%s %s (%s)", app_title,
				   VERSION, fax_file->file_name);
	  gtk_window_set_title (GTK_WINDOW (viewer_data->viewer_window),
				title);
	  g_free (title);

	  viewer_set_cmd_widgets_sensitive (viewer_data, TRUE);
	  gtk_widget_show (viewer_data->page_area);

	  add_thumbs (viewer_data);  
	  draw_page (viewer_data);
	}
    }
  else
    {
      fax_file = NULL;
      if (viewer_data->fax_file)
	viewer_window_unset_file (viewer_data);
      title = g_strdup_printf ("%s %s", app_title,  VERSION);
      gtk_window_set_title (GTK_WINDOW (viewer_data->viewer_window),
			    title);
      g_free (title);
    }
}

static void
page_area_realize_cb (GtkWidget *drawing_area,
		      ViewerData *viewer_data)
{
  gtk_drawing_area_size
    (GTK_DRAWING_AREA (drawing_area), 0, 0);

  gdk_window_set_events
    (drawing_area->window,
     gdk_window_get_events (viewer_data->page_area->window)
     | GDK_BUTTON_PRESS_MASK);

  gtk_signal_connect (GTK_OBJECT (drawing_area),
		      "button_press_event",
		      GTK_SIGNAL_FUNC (button_press_event_cb),
		      viewer_data);

#ifndef __WIN32__
  gtk_signal_connect (GTK_OBJECT (drawing_area),
		      "destroy",
		      GTK_SIGNAL_FUNC (page_area_destroy_cb),
		      drawing_area->window);
#endif
}

static void
viewer_window_realize_cb (GtkWidget *viewer_window, ViewerData *viewer_data)
{
#ifndef NEED_GNOMESUPPORT_H
  GtkWidget *vbox, *menu_bar;
  GtkWidget *tool_bar;
#endif
  GtkWidget *view_hbox;
  GtkWidget *page_table, *page_sc_win;
  GtkWidget *thumbs_sc_win;

  view_hbox = gtk_hbox_new (FALSE, 2);

#ifdef NEED_GNOMESUPPORT_H
  gnome_menu_bar_new (viewer_data);
  gnome_toolbar_new (viewer_data);
  gnome_app_set_contents (GNOME_APP (viewer_window),
			  view_hbox);
#else
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_box_set_spacing (GTK_BOX (vbox), 0);
  gtk_container_add (GTK_CONTAINER (viewer_window),
		     vbox);

  menu_bar = menu_bar_new (viewer_data);

  gtk_box_pack_start (GTK_BOX (vbox), menu_bar,
		      FALSE, FALSE, 0);

  tool_bar = toolbar_new (viewer_data);

  gtk_box_pack_start (GTK_BOX (vbox), tool_bar,
		      FALSE, FALSE, 0);

  gtk_box_pack_start (GTK_BOX (vbox), view_hbox, TRUE, TRUE, 0);
#endif

  viewer_set_cmd_widgets_sensitive (viewer_data, FALSE);
  gtk_widget_hide (viewer_data->page_area);

  thumbs_sc_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW
				  (thumbs_sc_win),
				  GTK_POLICY_NEVER,
				  GTK_POLICY_ALWAYS);

  gtk_box_pack_start (GTK_BOX (view_hbox), thumbs_sc_win,
		      FALSE, FALSE, 0);

  viewer_data->thumbs_fixed = thumbs_fixed_new (viewer_data->fax_file);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW
					 (thumbs_sc_win),
					 viewer_data->thumbs_fixed);

  page_sc_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (page_sc_win),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);

  gtk_box_pack_start (GTK_BOX (view_hbox), page_sc_win,
		      TRUE, TRUE, 2);

  page_table = gtk_table_new (1, 1, FALSE);
  gtk_scrolled_window_add_with_viewport
    (GTK_SCROLLED_WINDOW (page_sc_win),
     page_table);

  gtk_table_attach (GTK_TABLE (page_table),
		    viewer_data->page_area,
		    0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK,
		    GTK_EXPAND | GTK_SHRINK,
		    1, 1);

  gdk_window_set_events
    (viewer_window->window,
     gdk_window_get_events
     (viewer_window->window)
     | GDK_EXPOSURE_MASK
     | GDK_BUTTON_RELEASE_MASK);

  gtk_signal_connect (GTK_OBJECT (viewer_window),
		      "button_release_event",
		      GTK_SIGNAL_FUNC (button_release_event_cb),
		      viewer_data);
  gtk_signal_connect (GTK_OBJECT (viewer_window),
		      "motion_notify_event",
		      GTK_SIGNAL_FUNC (motion_notify_event_cb),
		      viewer_data);
}

static gboolean
viewer_window_cfg_event_cb (GtkWidget *viewer_window,
			    GdkEventConfigure *event, gpointer nothing)
{
  /* the signal has to be caught only once */
  gtk_signal_disconnect_by_func (GTK_OBJECT (viewer_window),
				 GTK_SIGNAL_FUNC (viewer_window_cfg_event_cb),
				 nothing);
  while (gtk_events_pending ())
    gtk_main_iteration ();
  
  vwindow_set_def_coords (viewer_window->window);

  return FALSE;
}

static void
viewer_window_new (ViewerData *viewer_data)
{
#ifdef NEED_GNOMESUPPORT_H
  viewer_data->viewer_window =
    gnome_app_new (PACKAGE, _("GNU HaliFAX - Viewer"));
#else
  viewer_data->viewer_window =
    gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (viewer_data->viewer_window),
			_("GNU HaliFAX - Viewer"));
#endif

#ifdef CAN_SAVE_CONFIG
  gtk_signal_connect (GTK_OBJECT (viewer_data->viewer_window),
		      "delete-event",
		      GTK_SIGNAL_FUNC (precloseviewer_cb),
		      NULL);
#endif

  gtk_signal_connect (GTK_OBJECT (viewer_data->viewer_window),
		      "destroy",
		      GTK_SIGNAL_FUNC (closeviewer_cb),
		      viewer_data);

  gtk_signal_connect (GTK_OBJECT (viewer_data->viewer_window),
		      "realize",
		      GTK_SIGNAL_FUNC (viewer_window_realize_cb),
		      viewer_data);

  /* This signal is handled only once, to resize and reposition the
     window according to our settings. */
  gtk_signal_connect_after (GTK_OBJECT (viewer_data->viewer_window),
			    "configure-event",
			    GTK_SIGNAL_FUNC (viewer_window_cfg_event_cb),
			    NULL);

  viewer_data->page_area = gtk_drawing_area_new ();
  gtk_signal_connect (GTK_OBJECT (viewer_data->page_area),
		      "realize",
		      GTK_SIGNAL_FUNC (page_area_realize_cb),
		      viewer_data);

  gtk_widget_show_all (viewer_data->viewer_window);

  increase_win_count ();
}

ViewerData *
fax_viewer_new ()
{
  ViewerData *new_viewer;

  new_viewer = g_malloc (sizeof(ViewerData));

  new_viewer->fax_file = NULL;
  new_viewer->current_page = NULL;
  new_viewer->page_area = NULL;
  new_viewer->page_pixmap = NULL;
  new_viewer->zoom_index = 4;
  new_viewer->rotation = ROT_NONE;
  new_viewer->motion_data = g_malloc (sizeof (MotionData));
  new_viewer->motion_data->dragging = FALSE;

  viewer_window_new (new_viewer);

#ifndef __WIN32__
  window_set_icon (new_viewer->viewer_window, PIXMAP ("ghfaxviewer-icon.xpm"));
#endif /* __WIN32__ */

  gtk_widget_show_all (new_viewer->viewer_window);

  return new_viewer;
}

static gchar *
parse_params (gint argc, gchar *argv[])
{
  gint counter;
  gchar *file_name;

  counter = 1;
  file_name = NULL;

  while (!file_name && counter < argc)
    if (*argv [counter] != '-')
      file_name = g_strdup (argv [counter]);
    else
      counter++;

  return (file_name);
}

gint
main (gint argc, gchar *argv[])
{
  ViewerData *viewer;
  gchar *file_name;

  app_setup (&argc, &argv);

  viewer = fax_viewer_new ();
  file_name = parse_params (argc, argv);
  fax_viewer_open_file (viewer, file_name);
  if (file_name)
    g_free (file_name);

  gtk_main();

  return 0;
}


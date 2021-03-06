/* toolbar.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000, 2001, 2002, 2003 Wolfgang Sourdeau
 *
 * Time-stamp: <2003-03-07 10:27:17 wolfgang>
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

/* This file is the collection of functions needed to create and
   manage the toolbar */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef NEED_GNOMESUPPORT_H
#include <gnome.h>

#include "setup.h"
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#include <ghfaxwidgets/ghfaxwidgets.h>
#include <stdio.h>

#include "tiffimages.h"
#include "viewer.h"
#include "callbcks.h"
#include "print.h"
#include "setup.h"

#ifdef NEED_GNOMESUPPORT_H
static void
remember_buttons (ViewerData *viewer_data, GnomeUIInfo uiinfo[])
{
  viewer_data->cmd_buttons[VIEW_PREV_PAGE] = uiinfo[0].widget;
  viewer_data->cmd_buttons[VIEW_NEXT_PAGE] = uiinfo[1].widget;
  viewer_data->cmd_buttons[VIEW_ZOOM_IN] = uiinfo[3].widget;
  viewer_data->cmd_buttons[VIEW_ZOOM_OUT] = uiinfo[4].widget;

  viewer_data->cmd_buttons[VIEW_ROTATE_LEFT90] = uiinfo[6].widget;
  viewer_data->cmd_buttons[VIEW_ROTATE_REVERSE] = uiinfo[7].widget;
  viewer_data->cmd_buttons[VIEW_ROTATE_RIGHT90] = uiinfo[8].widget;

  viewer_data->cmd_buttons[FILE_PRINT] = uiinfo[10].widget;
  viewer_data->cmd_buttons[FILE_PROPERTIES] = uiinfo[12].widget;
}

void
gnome_toolbar_new (ViewerData *viewer_data)
{
  GtkWidget *toolbar;

  GnomeUIInfo toolbar_uiinfo[] =
  {
    {
      GNOME_APP_UI_ITEM, NULL, N_("Go to previous page"),
      (gpointer) prevpage_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_GO_BACK,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, NULL, N_("Go to next page"),
      (gpointer) nextpage_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_GO_FORWARD,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_SEPARATOR,
    {
      GNOME_APP_UI_ITEM, NULL, N_("Zoom in"),
      (gpointer) zoomin_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ZOOM_IN,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, NULL, N_("Zoom out"),
      (gpointer) zoomout_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ZOOM_OUT,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_SEPARATOR,
    {
      GNOME_APP_UI_ITEM, NULL, N_("Rotate 90 degrees counter-clockwise"),
      (gpointer) left90_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_UNDO,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, NULL, N_("Reverse image"),
      (gpointer) reverse_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_REFRESH,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, NULL, N_("Rotate 90 degrees clockwise"),
      (gpointer) right90_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_REDO,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_SEPARATOR,
    {
      GNOME_APP_UI_ITEM, NULL, N_("Print fax"),
      (gpointer) print_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_PRINT,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_SEPARATOR,
    {
      GNOME_APP_UI_ITEM, NULL, N_("Properties..."),
      (gpointer) info_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GTK_STOCK_PROPERTIES,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_END
  };

  toolbar = gtk_toolbar_new ();
  gtk_toolbar_set_orientation (GTK_TOOLBAR (toolbar), GTK_ORIENTATION_HORIZONTAL);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);

  gnome_app_fill_toolbar (GTK_TOOLBAR (toolbar),
			  toolbar_uiinfo,
			  NULL);
  gnome_app_set_toolbar (GNOME_APP (viewer_data->viewer_window),
			 GTK_TOOLBAR (toolbar));

  remember_buttons (viewer_data, toolbar_uiinfo);
}

#else /* NEED_GNOMESUPPORT_H */

static GtkWidget*
toolbar_button_new (GtkWidget *window, gchar *xpm_path,
		    GtkSignalFunc callback, gpointer cb_data)
{
  GtkWidget *new_button, *icon;

  new_button = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (new_button), GTK_RELIEF_NONE);
  icon = pixmap_from_xpm (window, xpm_path);
  gtk_container_add (GTK_CONTAINER (new_button), icon);
  g_signal_connect (G_OBJECT (new_button), "clicked",
		      callback, cb_data);

  /* buttons are insensitive when created, draw_page will make them
     sensitive appropriately */
  gtk_widget_set_sensitive (new_button, FALSE);

  return new_button;
}

GtkWidget*
toolbar_new (ViewerData *viewer_data)
{
  GtkWidget *new_bbar, *bbar_handle_box;

  /* To give a GNOMEish look to those poor users without GNOME */
  bbar_handle_box = gtk_handle_box_new ();
  g_signal_connect (G_OBJECT (bbar_handle_box), "child-detached",
		      G_CALLBACK (handle_box_transient_cb),
		      viewer_data->viewer_window->window);

  new_bbar = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL,
			      GTK_TOOLBAR_ICONS);

  gtk_container_add (GTK_CONTAINER (bbar_handle_box), new_bbar);
  gtk_container_set_border_width (GTK_CONTAINER (bbar_handle_box), 2);

  gtk_toolbar_set_space_size (GTK_TOOLBAR (new_bbar), 20);

  viewer_data->cmd_buttons[VIEW_PREV_PAGE] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("left_arrow.xpm"),
			prevpage_cb, viewer_data);
  gtk_toolbar_append_widget (GTK_TOOLBAR (new_bbar),
			     viewer_data->cmd_buttons[VIEW_PREV_PAGE],
			     _("Go to previous page"), NULL);

  viewer_data->cmd_buttons[VIEW_NEXT_PAGE] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("right_arrow.xpm"),
			nextpage_cb, viewer_data);
  gtk_toolbar_append_widget (GTK_TOOLBAR (new_bbar),
			     viewer_data->cmd_buttons[VIEW_NEXT_PAGE],
			     _("Go to next page"), NULL);

  gtk_toolbar_append_space (GTK_TOOLBAR (new_bbar));

  viewer_data->cmd_buttons[VIEW_ZOOM_IN] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("zoom_in.xpm"),
			zoomin_cb, viewer_data);
  gtk_toolbar_append_widget (GTK_TOOLBAR (new_bbar),
			     viewer_data->cmd_buttons[VIEW_ZOOM_IN],
			     _("Zoom in"), NULL);

  viewer_data->cmd_buttons[VIEW_ZOOM_OUT] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("zoom_out.xpm"),
			zoomout_cb, viewer_data);
  gtk_toolbar_append_widget (GTK_TOOLBAR (new_bbar),
			     viewer_data->cmd_buttons[VIEW_ZOOM_OUT],
			     _("Zoom out"), NULL);

  gtk_toolbar_append_space (GTK_TOOLBAR (new_bbar));

  viewer_data->cmd_buttons[VIEW_ROTATE_LEFT90] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("left90.xpm"),
			left90_cb, viewer_data);
  gtk_toolbar_append_widget
    (GTK_TOOLBAR (new_bbar), viewer_data->cmd_buttons[VIEW_ROTATE_LEFT90],
     _("Rotate 90 degrees counter-clockwise"), NULL);

  viewer_data->cmd_buttons[VIEW_ROTATE_REVERSE] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("reverse.xpm"),
			reverse_cb, viewer_data);
  gtk_toolbar_append_widget
    (GTK_TOOLBAR (new_bbar), viewer_data->cmd_buttons[VIEW_ROTATE_REVERSE],
     _("Reverse image"), NULL);

  viewer_data->cmd_buttons[VIEW_ROTATE_RIGHT90] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("right90.xpm"),
			right90_cb, viewer_data);
  gtk_toolbar_append_widget
    (GTK_TOOLBAR (new_bbar), viewer_data->cmd_buttons[VIEW_ROTATE_RIGHT90],
     _("Rotate 90 degrees clockwise"), NULL);

  gtk_toolbar_append_space (GTK_TOOLBAR (new_bbar));

  viewer_data->cmd_buttons[FILE_PRINT] =
    toolbar_button_new (viewer_data->viewer_window, PIXMAP ("printer.xpm"),
			G_CALLBACK (print_cb), viewer_data);
  gtk_toolbar_append_widget
    (GTK_TOOLBAR (new_bbar), viewer_data->cmd_buttons[FILE_PRINT],
     _("Print fax"), NULL);

  gtk_toolbar_append_space (GTK_TOOLBAR (new_bbar));

  viewer_data->cmd_buttons[FILE_PROPERTIES] =
    toolbar_button_new (viewer_data->viewer_window,
			PIXMAP ("info.xpm"), info_cb, viewer_data);
			
  gtk_toolbar_append_widget
    (GTK_TOOLBAR (new_bbar), viewer_data->cmd_buttons[FILE_PROPERTIES],
     _("Properties..."), NULL);

  return bbar_handle_box;
}

#endif  /* NEED_GNOMESUPPORT_H */

/* menu.c - this file is part of the GNU HaliFAX Viewer
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

#include <stdio.h>

#ifdef NEED_GNOMESUPPORT_H
#include "pixmaps/stock-zoom-in-menu.xpm"
#include "pixmaps/stock-zoom-out-menu.xpm"

#include <gnome.h>
#else /* NEED_GNOMESUPPORT_H */
#include <gtk/gtk.h>
#endif /* NEED_GNOMESUPPORT_H */

#include "tiffimages.h"
#include "viewer.h"
#include "callbcks.h"
#include "print.h"
#include "gtkutils.h"

#ifdef NEED_GNOMESUPPORT_H
void
gnome_menu_bar_new (ViewerData *viewer_data)
{
  GnomeUIInfo file_menu_uiinfo[] =
  {
    GNOMEUIINFO_MENU_OPEN_ITEM (file_dialog_cb, viewer_data),
    GNOMEUIINFO_MENU_CLOSE_ITEM (close_file_cb, viewer_data),
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_PRINT_ITEM (print_cb, viewer_data),
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_PROPERTIES_ITEM (info_cb, viewer_data),
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_EXIT_ITEM (widget_close_cb, viewer_data),
    GNOMEUIINFO_END
  };
  
  GnomeUIInfo view_menu_uiinfo[] =
  {
    {
      GNOME_APP_UI_ITEM, N_("Go to next page"),
      NULL, (gpointer) nextpage_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, N_("Go to previous page"),
      NULL, (gpointer) prevpage_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BACK,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_SEPARATOR,
    {
      GNOME_APP_UI_ITEM, N_("Zoom in"),
      NULL, (gpointer) zoomin_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_DATA, stock_zoom_in_menu_xpm,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, N_("Zoom out"),
      NULL, (gpointer) zoomout_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_DATA, stock_zoom_out_menu_xpm,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_SEPARATOR,
    {
      GNOME_APP_UI_ITEM, N_("Rotate 90" DEGREES " clockwise"),
      NULL, (gpointer) right90_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REDO,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, N_("Rotate 90" DEGREES " counter-clockwise"),
      NULL, (gpointer) left90_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_UNDO,
      0, (GdkModifierType) 0, NULL
    },
    {
      GNOME_APP_UI_ITEM, N_("Reverse image"),
      NULL, (gpointer) reverse_cb, viewer_data, NULL,
      GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_REFRESH,
      0, (GdkModifierType) 0, NULL
    },
    GNOMEUIINFO_END
  };
  
  GnomeUIInfo settings_menu_uiinfo[] =
  {
    GNOMEUIINFO_MENU_PREFERENCES_ITEM (NULL, NULL),
    GNOMEUIINFO_END
  };
  
  GnomeUIInfo help_menu_uiinfo[] =
  {
    GNOMEUIINFO_MENU_ABOUT_ITEM (about_cb, viewer_data->viewer_window),
    GNOMEUIINFO_END
  };
  
  GnomeUIInfo menubar_uiinfo[] =
  {
    GNOMEUIINFO_MENU_FILE_TREE (file_menu_uiinfo),
    GNOMEUIINFO_MENU_VIEW_TREE (view_menu_uiinfo),
    GNOMEUIINFO_MENU_SETTINGS_TREE (settings_menu_uiinfo),
    GNOMEUIINFO_MENU_HELP_TREE (help_menu_uiinfo),
    GNOMEUIINFO_END
  };

  gnome_app_create_menus (GNOME_APP (viewer_data->viewer_window),
			  menubar_uiinfo);
}

#else /* NEED_GNOMESUPPORT_H */

static GtkWidget*
file_menu_new (ViewerData *viewer_data)
{
  GtkWidget *file_menu_item, *file_menu;

  file_menu_item = gtk_menu_item_new_with_label (_("File"));
  file_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_menu_item), file_menu);

  menu_item_new (file_menu, _("Open..."), file_dialog_cb, viewer_data);
  menu_item_new (file_menu, _("Close"), close_file_cb, viewer_data);
  menu_separator_new (file_menu);
  menu_item_new (file_menu, _("Print fax"), print_cb, viewer_data);
  menu_separator_new (file_menu);
  menu_item_new (file_menu, _("Properties..."), info_cb, viewer_data);
  menu_separator_new (file_menu);
  menu_item_new (file_menu, _("Quit"), widget_close_cb, viewer_data);

  return file_menu_item;
}

static GtkWidget*
view_menu_new (ViewerData *viewer_data)
{
  GtkWidget *view_menu_item, *view_menu;

  view_menu_item = gtk_menu_item_new_with_label (_("View"));
  view_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (view_menu_item), view_menu);

  menu_item_new (view_menu, _("Go to next page"), nextpage_cb, viewer_data);
  menu_item_new (view_menu, _("Go to previous page"), prevpage_cb, viewer_data);
  menu_separator_new (view_menu);
  menu_item_new (view_menu, _("Zoom in"), zoomin_cb, viewer_data);
  menu_item_new (view_menu, _("Zoom out"), zoomout_cb, viewer_data);
  menu_separator_new (view_menu);
  menu_item_new (view_menu, _("Rotate 90" DEGREES " clockwise"),
		 right90_cb, viewer_data);
  menu_item_new (view_menu, _("Rotate 90" DEGREES " counter-clockwise"),
		 left90_cb, viewer_data);
  menu_item_new (view_menu, _("Reverse image"),
		 reverse_cb, viewer_data);

  return view_menu_item;
}

static GtkWidget*
param_menu_new (ViewerData *viewer_data)
{
  GtkWidget *param_menu_item, *param_menu;

  param_menu_item = gtk_menu_item_new_with_label (_("Settings"));
  param_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (param_menu_item), param_menu);

  menu_item_new (param_menu, _("Preferences..."), NULL, NULL);
 
  return param_menu_item;
}

static GtkWidget*
help_menu_new (ViewerData *viewer_data)
{
  GtkWidget *help_menu_item, *help_menu;

  help_menu_item = gtk_menu_item_new_with_label (_("Help"));
  help_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_menu_item), help_menu);

  menu_item_new (help_menu, _("About..."), about_cb,
		 viewer_data->viewer_window);
 
  return help_menu_item;
}

GtkWidget*
menu_bar_new (ViewerData *viewer_data)
{
  GtkWidget *menu_handle_box, *viewer_menu;
  GtkWidget *file_menu_item, *view_menu_item;
  GtkWidget *param_menu_item, *help_menu_item;

  /* To give a GNOMEish look to those poor users with only GTK+
     available */
  menu_handle_box = gtk_handle_box_new ();

  viewer_menu = gtk_menu_bar_new ();
  gtk_container_add (GTK_CONTAINER (menu_handle_box), viewer_menu);

  file_menu_item = file_menu_new (viewer_data);
  view_menu_item = view_menu_new (viewer_data);
  param_menu_item = param_menu_new (viewer_data);
  help_menu_item = help_menu_new (viewer_data);

  gtk_menu_bar_append (GTK_MENU_BAR (viewer_menu), file_menu_item);
  gtk_menu_bar_append (GTK_MENU_BAR (viewer_menu), view_menu_item);
  gtk_menu_bar_append (GTK_MENU_BAR (viewer_menu), param_menu_item);
  gtk_menu_bar_append (GTK_MENU_BAR (viewer_menu), help_menu_item);

  return menu_handle_box;
}
#endif /* NEED_GNOMESUPPORT_H */


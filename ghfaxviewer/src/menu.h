#ifndef MENU_H
#define MENU_H

#ifdef NEED_GNOMESUPPORT_H
void gnome_menu_bar_new (ViewerData *viewer_data);
#else
GtkWidget* menu_bar_new (ViewerData *viewer_data);
#endif

#endif

/* ghfwthumbbox.h - this file is part of the GNU HaliFAX Widgets library
 *
 * Copyright (C) 2001 Wolfgang Sourdeau
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

/* A pseudo-widget to create the thumbnail area */

#ifndef GHFW_THUMBBOX_H
#define GHFW_THUMBBOX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GHFW_TYPE_THUMBBOX            (ghfw_thumbbox_get_type ())
#define GHFW_THUMBBOX(obj)		(GTK_CHECK_CAST (obj, GHFW_TYPE_THUMBBOX, GhfwThumbBox))
#define GHFW_THUMBBOX_CLASS(obj)	(GTK_CHECK_CAST (obj, GHFW_TYPE_THUMBBOX, GhfwThumbBoxClass))
#define GHFW_IS_THUMBBOX(obj)         (GTK_CHECK_TYPE (obj, GHFW_TYPE_THUMBBOX))
#define GHFW_IS_THUMBBOX_CLASS(klass) (GTK_CHECK_CLASS_TYPE (klass, GHFW_TYPE_THUMBBOX))


typedef struct _GhfwThumbBox GhfwThumbBox;
typedef struct _GhfwThumbBoxClass GhfwThumbBoxClass;

struct _GhfwThumbBox
{
  GtkWindow window;

  GtkWidget *vbox, *content, *button_box;

  guint escapable:1;
};

struct _GhfwThumbBoxClass
{
  GtkWindowClass parent_class;

  void (* escaped) (GhfwThumbBox *thumbbox);
};

GtkType        ghfw_thumbbox_get_type       (void);


GtkWidget *ghfw_thumbbox_new (gchar *title);

GtkWidget *layout_new (GtkWidget *ref_widget, GtkOrientation orientation,
		       gint spacing, gint width);
void layout_reset (GtkWidget *layout);

void layout_add_widget (GtkWidget *layout, GtkWidget *widget);
void layout_set_bg_color (GtkWidget *layout,
			  gushort red, gushort green, gushort blue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GHFW_THUMBBOX_H */

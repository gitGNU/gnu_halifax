#ifndef DRAW_PAGE_H
#define DRAW_PAGE_H

typedef struct _DrawData DrawData;

struct _DrawData
{
  GtkWidget *ref_widget;
  GdkPixmap *ref_pixmap;
  FaxRotationType rotation;
};

void draw_page_func (FaxPage *ref_page, gint y, gint height,
		     gpointer references);
GdkPixmap* pixmap_for_page (GtkWidget *ref_widget,
			    gint ref_width, gint ref_height,
			    FaxRotationType rotation,
			    int with_frame);
DrawData *prepare_drawing (GtkWidget *ref_widget,
			   GdkPixmap *ref_pixmap,
			   FaxRotationType rotation);

#endif

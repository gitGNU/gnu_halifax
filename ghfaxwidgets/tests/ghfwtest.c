#include <gtk/gtk.h>

#include "ghfwthumbbox.h"

GtkWidget* layout_window (GtkOrientation orientation)
{
  GtkWidget *window, *layout, *button;
  gint count;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  layout = layout_new (window, orientation, 10, 80);
  gtk_container_add (GTK_CONTAINER (window), layout);

  for (count = 0; count < 10; count++)
    {
      button = gtk_button_new_with_label ("test");
      layout_add_widget (layout, button);
    }

  return window;
}

int main (int argc, char *argv[])
{
  GtkWidget *window;

  gtk_init (&argc, &argv);

  window = layout_window (GTK_ORIENTATION_HORIZONTAL);
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 80);
  gtk_widget_show_all (window);

  window = layout_window (GTK_ORIENTATION_VERTICAL);
  gtk_window_set_default_size (GTK_WINDOW (window), 80, 200);
  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}

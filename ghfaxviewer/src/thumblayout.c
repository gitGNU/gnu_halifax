/* gtkutils.h - this file is part of the GNU HaliFAX Viewer
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

/* A pseudo-widget to create the thumbnail area */

#include <gtk/gtk.h>

#include "gtkutils.h"

#include "pixmaps/layout_up_arrow.xpm"
#include "pixmaps/layout_down_arrow.xpm"
#include "pixmaps/layout_left_arrow.xpm"
#include "pixmaps/layout_right_arrow.xpm"

#define ADJUSTMENT_STEP 20
#define ADJUSTMENT_DELAY 50

#define MOUSE_WHEEL_UP 4
#define MOUSE_WHEEL_DOWN 5
#define MOUSE_WHEEL_ADJUSTMENT_STEP 50

typedef struct _LayoutData LayoutData;

struct _LayoutData
{
  GtkAdjustment *adjustment;

  GtkWidget *gtk_layout;
  gint height, width, spacing;

  GtkOrientation orientation;

  /* Control buttons */
  GtkWidget *up, *down;

  /* Timeout data */
  gboolean up_pressed, down_pressed;
  guint timeout_id;

  GList *children_list;
};

static gint
widget_height (GtkWidget *widget, GtkOrientation orientation)
{
  gint height;
  GtkRequisition requisition;

  if (GTK_WIDGET_VISIBLE (widget))
    {
      gtk_widget_get_child_requisition (widget, &requisition);
      if (orientation == GTK_ORIENTATION_HORIZONTAL)
	height = requisition.width;
      else
	height = requisition.height;
    }
  else
    height = 0;

  return height;
}

static void
reset_timeout (LayoutData *layout_data)
{
  GtkWidget *grab_widget;

  if (layout_data->timeout_id)
    {
      layout_data->up_pressed = FALSE;
      layout_data->down_pressed = FALSE;
      gtk_timeout_remove (layout_data->timeout_id);
      layout_data->timeout_id = 0;

      grab_widget = gtk_grab_get_current ();
      if (grab_widget)
	gtk_grab_remove (grab_widget);
      if (gdk_pointer_is_grabbed ())
	gdk_keyboard_ungrab (GDK_CURRENT_TIME);
    }
}

static void
refresh_buttons (LayoutData *layout_data)
{
  GtkAdjustment *adjustment;
  gint up_height, down_height;
  gboolean condition;

  adjustment = layout_data->adjustment;
  down_height = widget_height (layout_data->down, 
			       layout_data->orientation);
  up_height = widget_height (layout_data->up, 
			     layout_data->orientation);

  condition = (adjustment->value - up_height <= 0);

  if (GTK_WIDGET_VISIBLE (layout_data->up))
    {
      if (condition) 
	{
	  gtk_button_released ((GtkButton*) layout_data->up);
	  gtk_widget_hide (layout_data->up);
	  reset_timeout (layout_data);
	  gtk_adjustment_set_value (adjustment, adjustment->value - up_height);
	  up_height = 0;
	}
    }
  else
    if (!condition)
      {
        gtk_widget_show (layout_data->up);
        up_height = widget_height (layout_data->up, 
				   layout_data->orientation);
        gtk_adjustment_set_value (adjustment, adjustment->value + up_height);
      }

  condition = (adjustment->page_size + adjustment->value
	       + down_height - layout_data->height >= 0);

  if (GTK_WIDGET_VISIBLE (layout_data->down))
    {
      if (condition)
        {
	  gtk_button_released ((GtkButton*) layout_data->down);
	  gtk_widget_hide (layout_data->down);
	  reset_timeout (layout_data);
        }
    }
  else
    if (!condition)
      {
        gtk_widget_show (layout_data->down);
        down_height = widget_height (layout_data->down,
				     layout_data->orientation);
        gtk_adjustment_set_value (adjustment,
				  adjustment->value
				  - down_height
				  + up_height);
      }
}

static gboolean
layout_timeout_cb (LayoutData *layout_data)
{
  GtkAdjustment *adjustment;
  gint new_value;

  adjustment = layout_data->adjustment;

  if (layout_data->up_pressed)
    new_value = adjustment->value - ADJUSTMENT_STEP;
  else if (layout_data->down_pressed)
    new_value = adjustment->value + ADJUSTMENT_STEP;
  else
    {
      reset_timeout (layout_data);
      new_value = adjustment->value;
    }

  gtk_adjustment_set_value (adjustment, new_value);

  refresh_buttons (layout_data);

  return TRUE;
}

static void
up_pressed_cb (GtkWidget *button, LayoutData *layout_data)
{
  layout_data->up_pressed = TRUE;
  layout_timeout_cb (layout_data);
  layout_data->timeout_id = gtk_timeout_add (ADJUSTMENT_DELAY,
					     (GtkFunction) layout_timeout_cb,
					     layout_data);
}

static void
down_pressed_cb (GtkWidget *button, LayoutData *layout_data)
{
  layout_data->down_pressed = TRUE;
  layout_timeout_cb (layout_data);
  layout_data->timeout_id = gtk_timeout_add (ADJUSTMENT_DELAY,
					     (GtkFunction) layout_timeout_cb,
					     layout_data);
}

static void
button_released_cb (GtkWidget *button, LayoutData *layout_data)
{
  reset_timeout (layout_data);
}

static void
layout_resize_cb (GtkWidget *layout,
		  GtkAllocation *allocation,
		  LayoutData *layout_data)
{
  GtkAdjustment *adjustment;
  gint delta;

  adjustment = layout_data->adjustment;

  delta = (layout_data->height - (gint) adjustment->page_size);

  if ((adjustment->value > delta)
      && (delta >= 0))
    {
      adjustment->value = delta;
      gtk_adjustment_value_changed (adjustment);
    }

  adjustment->upper = delta;
  
  if (adjustment->upper < 0.0)
    adjustment->upper = 0.0;
  
  gtk_adjustment_changed (adjustment);

  refresh_buttons (layout_data);
}

#ifdef __WIN32__
/* This is a dirty hack waiting for GTK+ for Windows to be fixed... */
void win32_layout_changed_cb (GtkWidget *widget, LayoutData *layout_data)
{
  gtk_widget_size_allocate (layout_data->gtk_layout,
			    &(layout_data->gtk_layout->allocation));
}

static gint
mouse_scroll_event_cb (GtkWidget *ref_widget,
			GdkEventScroll *event,
			LayoutData *layout_data)
{
  gboolean modify_value;
  gint new_value;
  GtkAdjustment *adjustment;

  adjustment = layout_data->adjustment;

  if (event->direction == GDK_SCROLL_UP)
    {
      new_value = adjustment->value - MOUSE_WHEEL_ADJUSTMENT_STEP;
      modify_value = TRUE;
    }
  else if (event->direction == GDK_SCROLL_DOWN)
    {
      new_value = adjustment->value + MOUSE_WHEEL_ADJUSTMENT_STEP;
      modify_value = TRUE;
    }
  else
    {
      new_value = 0;
      modify_value = FALSE;
    }

  if (modify_value)
    {
      gtk_adjustment_set_value (adjustment, new_value);
      refresh_buttons (layout_data);
    }

  return FALSE;
}

static void
widget_mouse_scroll_prepare (GtkWidget *widget, LayoutData *layout_data)
{
  if (GTK_WIDGET_REALIZED (widget))
    {
      gdk_window_set_events (widget->window,
			     gdk_window_get_events (widget->window)
			     | GDK_SCROLL_MASK);
      
      gtk_signal_connect (GTK_OBJECT (widget),
			  "scroll-event",
			  GTK_SIGNAL_FUNC (mouse_scroll_event_cb),
			  layout_data);
    }
  else
    gtk_signal_connect ((GtkObject*) widget, "realize",
			widget_mouse_scroll_prepare,
			layout_data);			
}
#else /* __WIN32__ */
static gint
mouse_press_event_cb (GtkWidget *ref_widget,
		      GdkEventButton *event,
		      LayoutData *layout_data)
{
  gboolean modify_value;
  gint new_value;
  GtkAdjustment *adjustment;

  adjustment = layout_data->adjustment;

  if (event->button == MOUSE_WHEEL_UP)
    {
      new_value = adjustment->value - MOUSE_WHEEL_ADJUSTMENT_STEP;
      modify_value = TRUE;
    }
  else if (event->button == MOUSE_WHEEL_DOWN)
    {
      new_value = adjustment->value + MOUSE_WHEEL_ADJUSTMENT_STEP;
      modify_value = TRUE;
    }
  else
    {
      new_value = 0;
      modify_value = FALSE;
    }

  if (modify_value)
    {
      gtk_adjustment_set_value (adjustment, new_value);
      refresh_buttons (layout_data);
    }

  return FALSE;
}

static void
widget_mouse_scroll_prepare (GtkWidget *widget, LayoutData *layout_data)
{
  if (GTK_WIDGET_REALIZED (widget))
    {
      gdk_window_set_events (widget->window,
			     gdk_window_get_events (widget->window)
			     | GDK_BUTTON_PRESS_MASK);
	  
      gtk_signal_connect (GTK_OBJECT (widget),
			  "button-press-event",
			  GTK_SIGNAL_FUNC (mouse_press_event_cb),
			  layout_data);
    }
  else
    gtk_signal_connect ((GtkObject*) widget, "realize",
			widget_mouse_scroll_prepare,
			layout_data);
}
#endif /* __WIN32__ */

static LayoutData *
layout_data_create (GtkWidget *ref_widget,
		    GtkAdjustment *adjustment)
{
  LayoutData *layout_data;

  layout_data = g_malloc (sizeof (LayoutData));
  layout_data->adjustment = adjustment;

  layout_data->up = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (layout_data->up), GTK_RELIEF_HALF);
  gtk_signal_connect (GTK_OBJECT (layout_data->up), "pressed",
		      up_pressed_cb, layout_data);
  gtk_signal_connect (GTK_OBJECT (layout_data->up), "released",
		      button_released_cb, layout_data);

  layout_data->down = gtk_button_new ();
  gtk_button_set_relief (GTK_BUTTON (layout_data->down), GTK_RELIEF_HALF);
  gtk_signal_connect (GTK_OBJECT (layout_data->down), "pressed",
		      down_pressed_cb, layout_data);
  gtk_signal_connect (GTK_OBJECT (layout_data->down), "released",
		      button_released_cb, layout_data);

  return layout_data;
}

static void
put_icons_on_buttons (GtkWidget *ref_widget, LayoutData *layout_data)
{
  GtkWidget *pixmap;

  if (layout_data->orientation
      == GTK_ORIENTATION_HORIZONTAL)
    {
      pixmap = pixmap_from_xpm_data (ref_widget,
				     layout_left_arrow_xpm);
      gtk_container_add (GTK_CONTAINER (layout_data->up),
			 pixmap);
      gtk_widget_show (pixmap);
      pixmap = pixmap_from_xpm_data (ref_widget,
				     layout_right_arrow_xpm);
      gtk_container_add (GTK_CONTAINER (layout_data->down),
			 pixmap);
      gtk_widget_show (pixmap);
    }
  else
    {
      pixmap = pixmap_from_xpm_data (ref_widget,
				     layout_up_arrow_xpm);
      gtk_container_add (GTK_CONTAINER (layout_data->up),
			 pixmap);
      gtk_widget_show (pixmap);
      pixmap = pixmap_from_xpm_data (ref_widget,
				     layout_down_arrow_xpm);
      gtk_container_add (GTK_CONTAINER (layout_data->down),
			 pixmap);
      gtk_widget_show (pixmap);
    }
}

static void
layout_set_width (LayoutData *layout_data, gint width)
{
  if (layout_data->orientation
      == GTK_ORIENTATION_HORIZONTAL)
    {
      gtk_widget_set_usize (layout_data->up, -1, width);
      gtk_widget_set_usize (layout_data->down, -1, width);
      gtk_widget_set_usize (layout_data->gtk_layout,
			    -1, width);
    }
  else
    {
      gtk_widget_set_usize (layout_data->up, width, -1);
      gtk_widget_set_usize (layout_data->down, width, -1);
      gtk_widget_set_usize (layout_data->gtk_layout,
			    width, -1);
    }

  layout_data->width = width;
}

static gboolean
check_button_pos_cb (GtkWidget *button, LayoutData *layout_data)
{
  GtkAllocation allocation;
  GtkAdjustment *adjustment;
  gint delta, button_delta;

  allocation = button->allocation;
  adjustment = layout_data->adjustment;

  button_delta = allocation.height + layout_data->spacing;

  /* If a button was hidden at the top of the scrolling layout... */
  delta = allocation.y + allocation.height - adjustment->page_size;
  if (delta > 0)
    gtk_adjustment_set_value (adjustment, (adjustment->value
					   + delta
					   + button_delta));

  /* If a button was hidden at the bottom of the scrolling layout... */
  delta = allocation.y;
  if (delta < 0)
    gtk_adjustment_set_value (adjustment, (adjustment->value
					   + delta
					   - button_delta));

  refresh_buttons (layout_data);

  return FALSE;
}

static void
usr_btn_state_changed_cb (GtkWidget *button,
			  GtkStateType state, gpointer data)
{
  GdkEventMask event_mask;

  event_mask = gdk_window_get_events (button->window);

  if (GTK_WIDGET_IS_SENSITIVE (button))
    event_mask |= GDK_BUTTON_PRESS_MASK;
  else
    event_mask &= ~GDK_BUTTON_PRESS_MASK;

  gdk_window_set_events (button->window, event_mask);
}

static void
setup_usr_btn (GtkWidget *button, LayoutData *layout_data)
{
  if (GTK_WIDGET_REALIZED (button))
    {
      gtk_signal_connect (GTK_OBJECT (button),
			  "released",
			  (GtkSignalFunc) check_button_pos_cb,
			  layout_data);
      gtk_signal_connect_after (GTK_OBJECT (button),
				"state-changed",
				(GtkSignalFunc) usr_btn_state_changed_cb,
				NULL);

      widget_mouse_scroll_prepare (button, layout_data);
    }
  else
    gtk_signal_connect ((GtkObject*) button, "realize",
			setup_usr_btn, layout_data);
}

GtkWidget *
layout_new (GtkWidget *ref_widget, GtkOrientation orientation,
	    gint spacing, gint width)
{
  GtkWidget *box, *gtk_layout;
  GtkObject *adjustment;
  LayoutData *layout_data;  

  adjustment = gtk_adjustment_new (0.0, 0.0, 10.0, 1.0, 2.0, 0.0);

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      box = gtk_hbox_new (FALSE, 0);
      gtk_layout = gtk_layout_new (GTK_ADJUSTMENT (adjustment),
				   NULL);
    }
  else
    {
      box = gtk_vbox_new (FALSE, 0);
      gtk_layout = gtk_layout_new (NULL,
				   GTK_ADJUSTMENT (adjustment));
    }

  layout_data = layout_data_create (ref_widget,
				    (GtkAdjustment*) adjustment);

  gtk_box_pack_start (GTK_BOX (box), layout_data->up,
		      FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (box), gtk_layout,
		      TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (box), layout_data->down,
		      FALSE, TRUE, 0);

  layout_data->gtk_layout = gtk_layout;
  layout_data->spacing = spacing;
  layout_data->height = 0;
  layout_data->up_pressed = FALSE;
  layout_data->down_pressed = FALSE;
  layout_data->timeout_id = 0;
  layout_data->orientation = orientation;
  layout_data->children_list = g_list_alloc ();

  layout_set_width (layout_data, width);

  /* signal stuff */
  gtk_signal_connect (GTK_OBJECT (gtk_layout),
		      "size-allocate",
		      (GtkSignalFunc) layout_resize_cb,
		      layout_data);
  gtk_widget_add_events (gtk_layout, GDK_BUTTON_PRESS_MASK);
  widget_mouse_scroll_prepare (gtk_layout, layout_data);
  if (GTK_WIDGET_REALIZED (ref_widget))
    put_icons_on_buttons (ref_widget, layout_data);
  else
    gtk_signal_connect ((GtkObject*) ref_widget, "realize",
			put_icons_on_buttons, layout_data);
#ifdef __WIN32__
  gtk_signal_connect (adjustment, "value_changed",
  		      win32_layout_changed_cb, layout_data);
#endif

  gtk_object_set_data_full (GTK_OBJECT (box),
			    "_layout_data", layout_data,
			    g_free);

  gtk_widget_show (gtk_layout);

  return box;
}

static void
layout_adjust_widgets_after_child (LayoutData *layout_data,
				   GtkWidget *widget,
				   gint child_x,
				   gint child_y)
{
  GtkRequisition requisition;
  GtkWidget *cur_widget;
  GList *widget_ptr;
  gint x, y, prev_wheight;

  x = child_x;
  y = child_y;

  widget_ptr = g_list_find (layout_data->children_list, widget);
  cur_widget = widget_ptr->data;
  prev_wheight = widget_height (widget, layout_data->orientation);

  if (prev_wheight && widget_ptr->next)
    {
      widget_ptr = widget_ptr->next;

      if (layout_data->orientation == GTK_ORIENTATION_HORIZONTAL)
	while (widget_ptr)
	  {
	    cur_widget = widget_ptr->data;	    
	    gtk_widget_get_child_requisition (cur_widget, &requisition);

	    x += prev_wheight + layout_data->spacing;
	    y = (layout_data->width - requisition.height) / 2;
	    
	    gtk_layout_move ((GtkLayout*) layout_data->gtk_layout,
			     cur_widget, x, y);

	    prev_wheight = requisition.width;	    
	    widget_ptr = widget_ptr->next;
	  }
      else if (layout_data->orientation == GTK_ORIENTATION_VERTICAL)
	while (widget_ptr)
	  {
	    cur_widget = widget_ptr->data;	    
	    gtk_widget_get_child_requisition (cur_widget, &requisition);

	    x = (layout_data->width - requisition.width) / 2;
	    y += prev_wheight + layout_data->spacing;
	    
	    gtk_layout_move ((GtkLayout*) layout_data->gtk_layout,
			     cur_widget, x, y);

	    prev_wheight = requisition.height;    
	    widget_ptr = widget_ptr->next;
	  }
    }

  gtk_widget_queue_resize (layout_data->gtk_layout);
}

static gint
layout_widget_position (LayoutData *layout_data, GtkWidget *widget)
{
  GList *cur_widget_ptr, *widget_ptr;
  GtkWidget *cur_widget;
  gint position;

  position = layout_data->spacing / 2;
  widget_ptr = g_list_find (layout_data->children_list, widget);
  cur_widget_ptr = layout_data->children_list->next;

  while (cur_widget_ptr != widget_ptr)
    {
      cur_widget = cur_widget_ptr->data;
      position += (widget_height (cur_widget, layout_data->orientation)
		   + layout_data->spacing);
      cur_widget_ptr = cur_widget_ptr->next;
    }

  return position;
}

static void
layout_real_place_widget (LayoutData *layout_data,
			  GtkWidget *widget,
			  gint height)
{
  GtkWidget *gtk_layout;
  GtkRequisition requisition;
  gint x;
 
  gtk_layout = layout_data->gtk_layout;
  gtk_widget_get_child_requisition (widget, &requisition);

  if (layout_data->orientation
      == GTK_ORIENTATION_HORIZONTAL)
    {
      x = (layout_data->width - requisition.height) / 2;
      gtk_layout_move ((GtkLayout*) gtk_layout,
		       widget, height, x);
      if (GTK_WIDGET_VISIBLE (widget))
	layout_data->height += requisition.width;
    }
  else
    {
      x = (layout_data->width - requisition.width) / 2;
      gtk_layout_move ((GtkLayout*) gtk_layout,
		       widget, x, height);
      if (GTK_WIDGET_VISIBLE (widget))
	layout_data->height += requisition.height;
    }

  if (GTK_IS_BUTTON (widget))
    setup_usr_btn (widget, layout_data);
}

static void
layout_place_widget (LayoutData *layout_data, GtkWidget *widget)
{
  gint position;

  position = layout_widget_position (layout_data, widget);

  layout_real_place_widget (layout_data, widget, position);

  if (layout_data->orientation == GTK_ORIENTATION_HORIZONTAL)
    layout_adjust_widgets_after_child (layout_data, widget,
				       position, 0);
  else 
    layout_adjust_widgets_after_child (layout_data, widget,
				       0, position);
}

static void
widget_size_allocate_cb (GtkWidget *widget, GtkAllocation *allocation,
			 LayoutData *layout_data)
{
  gint position;

  gtk_signal_disconnect_by_func ((GtkObject*) widget, widget_size_allocate_cb,
				 layout_data);

  position = layout_widget_position (layout_data, widget);

  if (layout_data->orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      layout_data->height += allocation->width;

      gtk_layout_move ((GtkLayout*) layout_data->gtk_layout,
		       widget,
		       position,
		       (layout_data->width - allocation->height) / 2);
		       
      layout_adjust_widgets_after_child (layout_data, widget,
					 position, 0);
    }
  else
    {
      layout_data->height += allocation->height;

      gtk_layout_move ((GtkLayout*) layout_data->gtk_layout,
		       widget,
		       (layout_data->width - allocation->width) / 2,
		       position);
      
      layout_adjust_widgets_after_child (layout_data, widget,
					 0, position);
    }
}

void
layout_add_widget (GtkWidget *layout, GtkWidget *widget)
{
  LayoutData *layout_data;

  layout_data = gtk_object_get_data (GTK_OBJECT (layout),
				     "_layout_data");
  layout_data->children_list = 
    g_list_append (layout_data->children_list, widget);

  gtk_layout_put ((GtkLayout*) layout_data->gtk_layout, widget, 0, 0);
  layout_place_widget (layout_data, widget);

  layout_data->height += layout_data->spacing;

  if (!widget_height (widget, layout_data->orientation))
    gtk_signal_connect ((GtkObject*) widget, "size-allocate",
			(GtkSignalFunc) widget_size_allocate_cb,
			layout_data);

  gtk_widget_queue_resize (layout_data->gtk_layout);
}

static void
destroy_thumb (gpointer layout_child, gpointer container)
{
  gtk_container_remove (GTK_CONTAINER (container), layout_child);
}

void
layout_reset (GtkWidget *layout)
{
  LayoutData *layout_data;
  GtkContainer *gtk_layout;
  GList *children;

  layout_data = gtk_object_get_data (GTK_OBJECT (layout), "_layout_data");
  layout_data->height = 0;
  gtk_adjustment_set_value (layout_data->adjustment, 0.0);
  refresh_buttons (layout_data);

  gtk_layout = (GtkContainer*) layout_data->gtk_layout;
  children = gtk_container_children (gtk_layout);
  g_list_foreach (children, destroy_thumb, gtk_layout);
  g_list_free (layout_data->children_list);
  layout_data->children_list = g_list_alloc ();
}

void
layout_set_bg_color (GtkWidget *layout,
		     gushort red, gushort green, gushort blue)
{
  LayoutData *layout_data;
  GtkRcStyle *bg_style;

  layout_data = gtk_object_get_data (GTK_OBJECT (layout), "_layout_data");

  bg_style = gtk_rc_style_new ();
  back_gtkstyle (bg_style, GTK_STATE_NORMAL, red, green, blue);
  gtk_widget_modify_style (layout_data->gtk_layout, bg_style);
  gtk_rc_style_unref (bg_style);
}

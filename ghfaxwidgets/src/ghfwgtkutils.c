/* gtkutils.c - this file is part of the GNU HaliFAX Viewer
 *
 * Copyright (C) 2000 The Free Software Foundation, inc.
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

/* sigchld_handler and launch_program taken and modified from the GTK+
   FAQ at http://www.gtk.org/faq/. This code is copyright (C) 1999 by
   Erik Mouw <J.A.K.Mouw at its.tudelft.nl>. */

/* This file implements some useful GTK+-related functions used all
   around the program */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#ifdef __WIN32__
#include <gdk/win32/gdkwin32.h>
#else
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

/* #include "setup.h" */

/* This function happens to be present in more than one place. Let's
   clean up some... */
void
free_data_on_destroy_cb (GtkWidget *widget, gpointer data)
{
  g_free (data);
}

/* Menu creation */

GtkWidget*
menu_separator_new (GtkWidget *menu)
{
  GtkWidget *separator;

  separator = gtk_menu_item_new ();
  gtk_widget_set_sensitive (separator, FALSE);
  gtk_container_add (GTK_CONTAINER (menu), separator);

  return separator;
}

GtkWidget *
menu_item_new (GtkWidget *menu, gchar *label,
	       GtkSignalFunc callback, gpointer data)
{
  GtkWidget *new_menu_item;

  new_menu_item = gtk_menu_item_new_with_label (label);
  gtk_container_add (GTK_CONTAINER (menu), new_menu_item);

  if (callback)
    g_signal_connect (G_OBJECT (new_menu_item), "activate",
			callback, data);

  return new_menu_item;
}

/* Pixmaps and icons */

/* GtkWidget * */
/* pixmap_from_xpm (GtkWidget *ref_widget, gchar *file_name) */
/* { */
/*   GtkWidget *gtk_pixmap; */
/*   GdkPixmap *pixmap; */
/*   GdkBitmap *mask; */

/*   pixmap = gdk_pixmap_create_from_xpm */
/*     (ref_widget->window, &mask, */
/*      &(ref_widget->style->bg[GTK_STATE_NORMAL]), */
/*      file_name); */

/*   gtk_pixmap = gtk_pixmap_new (pixmap, mask); */

/*   return gtk_pixmap; */
/* } */

GtkWidget *
image_from_xpm_data (GtkWidget *ref_widget, gchar **xpm_data)
{
  GtkWidget *gtk_image;
  GdkPixmap *pixmap;
  GdkBitmap *mask;

  pixmap = gdk_pixmap_create_from_xpm_d
    (ref_widget->window, &mask,
     &(ref_widget->style->bg[GTK_STATE_NORMAL]),
     xpm_data);

  gtk_image = gtk_image_new_from_pixmap (pixmap, mask);

  return gtk_image;
}

void
window_set_icon (GtkWidget* ref_widget, gchar *file_name)
{
#ifndef __WIN32__
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  
  pixmap = gdk_pixmap_create_from_xpm
    (ref_widget->window, &mask,
     &(ref_widget->style->bg[GTK_STATE_NORMAL]),
     file_name);

  gdk_window_set_icon (ref_widget->window, ref_widget->window, pixmap, mask);
#endif /* __WIN32__ */
}


/* transient windows */

static void
transient_destroy_cb (GtkWidget *window, gpointer data)
{
  GtkWindow *parent;

  parent = g_object_get_data (G_OBJECT (window), "parent_was_modal");
  if (parent)
    gtk_window_set_modal (parent, TRUE);
    
/*   decrease_win_count (); */
}

#ifdef __WIN32__
void win32gdk_window_set_transient_for (GdkWindow *window, GdkWindow *parent)
{
  HWND window_id, parent_id;
  LONG style;

  window_id = GDK_DRAWABLE_XID (window);
  parent_id = GDK_DRAWABLE_XID (parent);

  style = GetWindowLong (window_id, GWL_STYLE);
  style |= WS_POPUP;
  style &= ~(WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

  SetWindowLong (window_id, GWL_STYLE, style);
  SetWindowLong (window_id, GWL_HWNDPARENT, (LONG) parent_id);

  RedrawWindow (window_id, NULL, NULL,
		RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
}
#endif

void
transient_window_show (GtkWidget *transient, GtkWidget *parent)
{
#ifndef __WIN32__
  gtk_window_set_transient_for (GTK_WINDOW (transient),
				GTK_WINDOW (parent));
#endif

  if (((GtkWindow *) parent)->modal)
    {
      g_object_set_data (G_OBJECT (transient), "parent_was_modal", parent);
      gtk_window_set_modal ((GtkWindow *) parent, FALSE);
    }

  gtk_window_set_modal ((GtkWindow *) transient, TRUE);
/*   gtk_window_set_policy ((GtkWindow *) transient, FALSE, FALSE, TRUE); */

  g_signal_connect (G_OBJECT (transient), "destroy",
		    G_CALLBACK (transient_destroy_cb), NULL);

  gtk_widget_show_all (transient);

#ifdef __WIN32__
  /* Have to call this here because the window is not realized at the
     beginning of this function. With the next version of GTK+/GDK for
     Windows we won't need this anymore. */
  win32gdk_window_set_transient_for (transient->window, parent->window);
#endif

/*   increase_win_count (); */
}

/* ESC-key handling */

static gboolean
key_press_event_cb (GtkWidget *window, GdkEventKey *event,
		    gpointer nothing)
{
  gboolean ret_code;

  if (event->keyval == GDK_Escape)
    {
      gtk_widget_destroy (window);
      ret_code = TRUE;
    }
  else
    ret_code = FALSE;

  return ret_code;
}

void
gtk_window_set_escapable (GtkWindow *window)
{
  g_signal_connect (G_OBJECT (window), "key-press-event",
		    G_CALLBACK (key_press_event_cb), NULL);
}

/* very simple GtkRcStyle stuff */

void
back_gtkstyle (GtkRcStyle *style, GtkStateType state,
	       gushort red, gushort green, gushort blue)
{
  GdkColor color;

  color.red = red;
  color.green = green;
  color.blue = blue;
  style->bg[state] = color;
  style->color_flags[state] = GTK_RC_BG;
}

/* Handle-box transientization */

void
handle_box_transient_cb (GtkHandleBox *handle_box, GtkWidget *hb_child,
			 GdkWindow *parent)
{
  g_signal_handlers_disconnect_by_func (G_OBJECT (handle_box),
					G_CALLBACK (handle_box_transient_cb),
					parent);
#ifndef __WIN32__
  gdk_window_set_transient_for (handle_box->float_window,
				parent);
#else
  win32gdk_window_set_transient_for (handle_box->float_window,
				     parent);
#endif
}

#ifndef __WIN32__
/* Launching programs from GTK+ applications */
static
void sigchld_handler (int num)
{
  sigset_t set, oldset;
  pid_t pid;
  gint status, exitstatus;

  /* block other incoming SIGCHLD signals */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, &oldset);

  /* wait for child */
  while ((pid = waitpid ((pid_t)-1, &status, WNOHANG)) > 0)
    {
      if(WIFEXITED (status))
	{
	  exitstatus = WEXITSTATUS (status);
	}
      else if (WIFSIGNALED (status))
	{
	  exitstatus = WTERMSIG (status);
	}
      else if (WIFSTOPPED (status))
	{
	  exitstatus = WSTOPSIG (status);
	}
    }

  /* re-install the signal handler (some systems need this) */
  signal(SIGCHLD, sigchld_handler);
  
  /* and unblock it */
  sigemptyset (&set);
  sigaddset (&set, SIGCHLD);
  sigprocmask (SIG_UNBLOCK, &set, &oldset);
}

void
launch_program (const gchar *program, gchar *argv[])
{
  pid_t pid;

  pid = fork();

  signal(SIGCHLD, sigchld_handler);

  if (pid == -1)
    {
      /* ouch, fork() failed */
      perror("fork");
      exit(-1);
    }
  else if (pid == 0)
    {
      execvp(program, argv);
      
      /* if exec() returns, there is something wrong */
      perror("execvp");

      /* exit child. note the use of _exit() instead of exit() */
      _exit(-1);
    }
}
#endif

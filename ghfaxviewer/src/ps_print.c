/* ps_print.c - this file is part of the GNU HaliFAX Viewer
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

/* This file implement the functions needed to get dumb UNIX printing
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tiffio.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <ghfaxwidgets/ghfwdialogwindow.h>
#include <ghfaxwidgets/ghfwgtkutils.h>
#include <ghfaxwidgets/ghfwprogress.h>

#include "tiffimages.h"
#include "viewer.h"
#include "print.h"
#include "i18n.h"
#include "errors.h"
#include "setup.h"

/* Shorthand for type of comparison functions.  */
#ifndef __GLIBC__
#define __compar_fn_t int (*) (const void *, const void *)
#endif

typedef struct _PList PList;
typedef struct _PDlgWidgets PDlgWidgets;
typedef struct _PrintData PrintData;
typedef struct _OutputData OutputData;

typedef enum
{
  PRINT_TO_FILE = 0,
  PRINT_TO_PRINTER = 1
} PrintDirection;

struct _PList
{
  int active, is_default;
  char name[17];
};

struct _PDlgWidgets
{
  GtkWidget *print_window;
  GtkWidget *printer_rb, *printer_opt_menu;
  GtkWidget *printer_cmd_lbl, *printer_cmd_en;
  GtkWidget *file_rb, *output_file_en;
  GtkWidget *all_pages_rb, *cur_page_rb;
  GtkWidget *from_to_rb, *from_sp_but, *to_sp_but;
};

struct _PrintData
{
  gchar *printer;
  FaxFile *document;
  FaxPage *current_page;
  gint first_page, last_page;
  PDlgWidgets widgets;
  DialogWindow *print_dialog;
  GtkWidget *parent_window;
};

struct _OutputData
{
  FILE *output_stream;
  FaxFile *document;
  gchar *out_file_name;
  gint from_page, to_page;
  DialogWindow *print_dialog, *err_dialog;
  GtkWidget *parent_window;
};

#define MAX_PLIST 8
static gchar *lpc_command = NULL;
static gchar *lpstat_command = NULL;
static gboolean printer_enabled;

/* Do various tests to determine how to get the list of printers.
   There are so many veriants of the lpr system... */
static gchar *
cmd_result (gchar *cmd)
{
  FILE *pfile;
  gchar *result;

  result = NULL;
  pfile = popen (cmd, "r");

  if (pfile)
    {
      result = g_malloc (129);
      fgets(result, 129, pfile);
      pclose (pfile);
    }

  return result;
}

static gboolean
test_lpc (gchar *lpc)
{
  gboolean ret_code;
  gchar *command, *result;

  ret_code = FALSE;
  command = g_strdup_printf ("%s status < /dev/null", lpc);
  result = cmd_result (command);

  /* FreeBSD lpr needs to have a printer specified or "all", while
     other versions don't */
  if (!strncmp (result, "Usage:", 6))
    {
      g_free (result);
      g_free (command);

      command = g_strdup_printf ("%s status all < /dev/null", lpc);
      result = cmd_result (command);
      
      if (strncmp (result, "unknown", 7))
	{
	  lpc_command = command;
	  ret_code = TRUE;
	}
      else
	g_free (command);

      g_free (result);
    }
  else
    {
      g_free (result);
      lpc_command = command;
      ret_code = TRUE;
    }	    
  
  return ret_code;
}

static gboolean
test_lpstat (gchar *lpstat)
{
  gboolean ret_code;
/*   gchar *command, *result; */

  /* I have yet to test multiple versions of lpstat... */
  ret_code = TRUE;
  lpstat_command = g_strdup_printf ("%s -d -p < /dev/null", lpstat);

  return ret_code;
}

static void
ensure_commands (GtkWidget *viewer_window)
{
  gchar *lpc, *lpstat;
  gchar *test_dirs[] =
    {
      "/usr/sbin",
      "/usr/bin",
      "/sbin",
      "/bin",
      NULL
    };

  if (!lpc_command && !lpstat_command)
    {
      lpc = where_is (test_dirs, "lpc");
      lpstat = where_is (test_dirs, "lpstat");

      if (lpc)
	printer_enabled = test_lpc (lpc);
      else if (lpstat)
	printer_enabled = test_lpstat (lpstat);
      else
	printer_enabled = FALSE;

      g_free (lpc);
      g_free (lpstat);
    }
}

static int
compare_printers (PList *p1, PList *p2)
{
  return (strcasecmp(p1->name, p2->name));
}

void
make_default_pr (PList *plist, int nbr_pr, char *defname)
{
  int count;
  gchar *printer_env, *new_default;

  printer_env = getenv ("PRINTER");
  if (printer_env)
    new_default = printer_env;
  else
    new_default = defname;

  if (strlen (new_default))
    {
      count = 0;
      while (count < nbr_pr)
	{
	  if (!strcmp (plist->name, new_default))
	    plist->is_default = TRUE;
	  plist++;
	  count++;
	}
    }
}

static PList*
get_printers (void)
{
  gint plist_count;
  gchar defname[17], name[17];
  gchar line[129];
  PList *plist;
  FILE *pfile;
	
  defname[0] = '\0';
  
  plist = g_malloc0 (sizeof (PList) * MAX_PLIST);
  plist_count = 0;

  if (lpc_command)
    {  
      pfile = popen (lpc_command, "r");

      if (pfile)
	{
	  while (fgets(line, sizeof (line), pfile)
		 && plist_count < MAX_PLIST)
	    if (strchr(line, ':')
		&& line[0] != ' '
		&& line[0] != '\t'
		&& strncmp(line, "Press RETURN to continue", 24))
	      {
		*strchr(line, ':') = '\0';
		strcpy(plist[plist_count].name, line);
		if (plist_count == 0)
		  strcpy(defname, line);
		plist[plist_count].active = TRUE;
		plist[plist_count].is_default = FALSE;
		plist_count++;
	      }
      
	  pclose(pfile);
	}
    }

  if (!plist_count && lpstat_command)
    {
      pfile = popen (lpstat_command, "r");
      if (pfile)
	{
	  while (fgets (line, sizeof (line), pfile) != NULL
		 && plist_count < MAX_PLIST)
	    {
	      if (sscanf (line, "printer %s", name) == 1)
		{
		  strcpy (plist[plist_count].name, name);
		  plist[plist_count].active = TRUE;
		  plist[plist_count].is_default = FALSE;
		  plist_count++;
		}
	      else
		sscanf(line, "system default destination: %s",
		       defname);
	    }
	  
	  pclose(pfile);
	}
    }
	
  if (plist_count)
    {
      make_default_pr (plist, plist_count, defname);
      if (plist_count > 1)
	qsort(plist, plist_count, sizeof (PList),
	      (__compar_fn_t) compare_printers);
    }
  else
    {
      g_free (plist);
      plist = NULL;
    }
  
  return plist;
}

static gint
activate_printer_cb (GtkWidget *widget, PrintData *print_data)
{
  print_data->printer = gtk_object_get_data (GTK_OBJECT (widget),
					     "printer_name");

  return FALSE;
}

static GtkWidget*
make_printer_menu (PrintData *print_data)
{
  GtkWidget *menu, *menuitem;
  PList *plist, *cur_printer;
  
  plist = get_printers ();
  
  if (plist)
    {
      menu = gtk_menu_new ();
      cur_printer = plist;
      
      if (cur_printer->active)
	print_data->printer = cur_printer->name;
      
      while (cur_printer->active)
	{
	  menuitem = gtk_menu_item_new_with_label (cur_printer->name);
	  gtk_menu_append (GTK_MENU (menu), menuitem);
	  gtk_object_set_data (GTK_OBJECT (menuitem), "printer_name",
			       cur_printer->name);
	  gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			      GTK_SIGNAL_FUNC (activate_printer_cb),
			      print_data);

	  if (cur_printer->is_default)
	    gtk_menu_reorder_child (GTK_MENU (menu), menuitem, 0);

	  cur_printer++;
	}

      gtk_signal_connect (GTK_OBJECT (menu), "destroy",
			  free_data_on_destroy_cb, plist);
    }
  else
    menu = NULL;
  
  return menu;
}

static gint
toggle_entries_cb (GtkWidget *widget, PDlgWidgets *widgets)
{
  if (widget == widgets->printer_rb)
    {
      gtk_widget_set_sensitive (widgets->printer_opt_menu, TRUE);
      gtk_widget_set_sensitive (widgets->printer_cmd_en, TRUE);
      gtk_widget_set_sensitive (widgets->output_file_en, FALSE);
    }
  else
    {
      gtk_widget_set_sensitive (widgets->printer_opt_menu, FALSE);
      gtk_widget_set_sensitive (widgets->printer_cmd_en, FALSE);
      gtk_widget_set_sensitive (widgets->output_file_en, TRUE);
    }

  return FALSE;
}

static gint
toggle_page_sel_entries_cb (GtkWidget *widget,
			    PDlgWidgets *widgets)
{
  if (widget == widgets->from_to_rb)
    {
      gtk_widget_set_sensitive (widgets->from_sp_but, TRUE);
      gtk_widget_set_sensitive (widgets->to_sp_but, TRUE);
    }
  else
    {
      gtk_widget_set_sensitive (widgets->from_sp_but, FALSE);
      gtk_widget_set_sensitive (widgets->to_sp_but, FALSE);
    }
  
  return FALSE;
}

static GtkWidget*
make_output_frame (PrintData *print_data)
{
  GtkWidget *output_frame, *table, *output_separator;
  GtkWidget *printer_menu;
  GtkWidget *output_file_lbl;
  GSList *rb_group;
  gchar *output_file_txt;

  output_frame = gtk_frame_new (_("Output to"));
  gtk_container_set_border_width (GTK_CONTAINER (output_frame), 5);
  table = gtk_table_new (5, 2, FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (table), 3);
  gtk_table_set_row_spacings (GTK_TABLE (table), 3);
  gtk_table_set_col_spacings (GTK_TABLE (table), 3);
  output_separator = gtk_hseparator_new ();
  
  print_data->widgets.printer_rb =
    gtk_radio_button_new_with_label (NULL, _("Printer"));

  rb_group = gtk_radio_button_group
    (GTK_RADIO_BUTTON(print_data->widgets.printer_rb));
  print_data->widgets.file_rb =
    gtk_radio_button_new_with_label (rb_group, _("File"));
  
  gtk_signal_connect (GTK_OBJECT (print_data->widgets.printer_rb),
		      "clicked", GTK_SIGNAL_FUNC (toggle_entries_cb),
		      &(print_data->widgets));			     
  gtk_signal_connect (GTK_OBJECT (print_data->widgets.file_rb),
		      "clicked", GTK_SIGNAL_FUNC (toggle_entries_cb),
		      &(print_data->widgets));			     

  print_data->widgets.printer_cmd_lbl = gtk_label_new (_("Command"));
  gtk_misc_set_alignment (GTK_MISC (print_data->widgets.printer_cmd_lbl),
			  0.0, 0.5);
  output_file_lbl = gtk_label_new (_("File"));
  gtk_misc_set_alignment (GTK_MISC (output_file_lbl), 0.0, 0.5);
  
  print_data->widgets.printer_cmd_en = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (print_data->widgets.printer_cmd_en),
		      "lpr");
  gtk_widget_set_usize (print_data->widgets.printer_cmd_en, 80, 20);
  print_data->widgets.output_file_en = gtk_entry_new ();
  output_file_txt = _("Fax Output.ps");
  gtk_entry_set_text (GTK_ENTRY (print_data->widgets.output_file_en),
		      output_file_txt);
  gtk_editable_set_position
    (GTK_EDITABLE (print_data->widgets.output_file_en),
     strlen (output_file_txt));
  gtk_widget_set_sensitive (print_data->widgets.output_file_en,
			    FALSE);
  gtk_widget_set_usize (print_data->widgets.output_file_en, 150, 20);

  print_data->widgets.printer_opt_menu = gtk_option_menu_new ();
  printer_menu = make_printer_menu (print_data);

  gtk_widget_set_usize (print_data->widgets.printer_opt_menu, 80, 25);

  gtk_table_attach_defaults (GTK_TABLE (table),
			     print_data->widgets.printer_rb,
			     0, 1, 0, 1);
  gtk_table_attach_defaults (GTK_TABLE (table),
			     print_data->widgets.printer_opt_menu,
			     1, 2, 0, 1);
  gtk_table_attach_defaults (GTK_TABLE (table),
			     print_data->widgets.printer_cmd_lbl,
			     0, 1, 1, 2);
  gtk_table_attach_defaults (GTK_TABLE (table),
			     print_data->widgets.printer_cmd_en,
			     1, 2, 1, 2);
  gtk_table_attach_defaults (GTK_TABLE (table),
			     output_separator,
			     0, 2, 2, 3);
  gtk_table_attach_defaults (GTK_TABLE (table),
			     print_data->widgets.file_rb,
			     0, 1, 3, 4);
  gtk_table_attach_defaults (GTK_TABLE (table),
			     output_file_lbl,
			     0, 1, 4, 5);
  gtk_table_attach_defaults (GTK_TABLE (table),
			     print_data->widgets.output_file_en,
			     1, 2, 4, 5);
  
  gtk_container_add (GTK_CONTAINER (output_frame), table);
  
  if (printer_menu)
    {
      gtk_widget_set_usize (printer_menu, 79, -2);
      gtk_option_menu_set_menu
	(GTK_OPTION_MENU (print_data->widgets.printer_opt_menu),
	 printer_menu);
    }
  else
    {
      gtk_button_clicked (GTK_BUTTON (print_data->widgets.file_rb));
      gtk_widget_set_sensitive (print_data->widgets.printer_opt_menu,
				FALSE);
      gtk_widget_set_sensitive (print_data->widgets.printer_cmd_lbl,
				FALSE);
      gtk_widget_set_sensitive (print_data->widgets.printer_rb,
				FALSE);
    }
  
  return output_frame;
}

static GtkWidget*
make_page_frame (PrintData *print_data)
{
  GtkWidget *page_frame, *page_table;
  GtkWidget *to_label;
  GtkObject *from_adj, *to_adj;
  gfloat max_pages;
  GSList *rb_group;

  page_frame = gtk_frame_new (_("Page selection"));
  gtk_container_set_border_width (GTK_CONTAINER (page_frame), 5);

  page_table = gtk_table_new (3, 4, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (page_table), 3);
  gtk_container_add (GTK_CONTAINER (page_frame), page_table);

  print_data->widgets.all_pages_rb = 
    gtk_radio_button_new_with_label (NULL, _("All pages"));
  rb_group = gtk_radio_button_group
    (GTK_RADIO_BUTTON (print_data->widgets.all_pages_rb));
  gtk_table_attach_defaults (GTK_TABLE (page_table),
			     print_data->widgets.all_pages_rb,
			     0, 4, 0, 1);
  gtk_signal_connect (GTK_OBJECT
		      (print_data->widgets.all_pages_rb),
		      "clicked",
		      GTK_SIGNAL_FUNC (toggle_page_sel_entries_cb),
		      &(print_data->widgets));
  
  print_data->widgets.from_to_rb =
    gtk_radio_button_new_with_label (rb_group, _("From"));
  rb_group = gtk_radio_button_group
    (GTK_RADIO_BUTTON (print_data->widgets.from_to_rb));
  gtk_table_attach_defaults (GTK_TABLE (page_table),
			     print_data->widgets.from_to_rb,
			     0, 1, 1, 2);
  gtk_signal_connect (GTK_OBJECT (print_data->widgets.from_to_rb),
		      "clicked",
		      GTK_SIGNAL_FUNC (toggle_page_sel_entries_cb),
		      &(print_data->widgets));

  max_pages = (gfloat) (print_data->document->nbr_pages + 1);
  from_adj = gtk_adjustment_new (1.0, 1.0, max_pages, 1.0, 0.0, 0.0);

  print_data->widgets.from_sp_but =
    gtk_spin_button_new (GTK_ADJUSTMENT (from_adj), 1.0, 0);
  gtk_spin_button_set_numeric
    (GTK_SPIN_BUTTON (print_data->widgets.from_sp_but), TRUE);
  gtk_table_attach_defaults (GTK_TABLE (page_table),
			     print_data->widgets.from_sp_but,
			     1, 2, 1, 2);
  
  to_label = gtk_label_new (_("to"));
  gtk_table_attach_defaults (GTK_TABLE (page_table), to_label,
			     2, 3, 1, 2);

  to_adj = gtk_adjustment_new (max_pages, 1.0, max_pages,
			       1.0, 0.0, 0.0);

  print_data->widgets.to_sp_but =
		gtk_spin_button_new (GTK_ADJUSTMENT (to_adj), 1.0, 0);
  gtk_spin_button_set_numeric
    (GTK_SPIN_BUTTON (print_data->widgets.to_sp_but), TRUE);
  gtk_table_attach_defaults (GTK_TABLE (page_table),
			     print_data->widgets.to_sp_but,
			     3, 4, 1, 2);

  print_data->widgets.cur_page_rb =
    gtk_radio_button_new_with_label (rb_group,
				     _("Current page only"));
  gtk_table_attach_defaults (GTK_TABLE (page_table),
			     print_data->widgets.cur_page_rb,
			     0, 4, 2, 3);
  gtk_signal_connect (GTK_OBJECT (print_data->widgets.cur_page_rb),
		      "clicked",
		      GTK_SIGNAL_FUNC (toggle_page_sel_entries_cb),
		      &(print_data->widgets));
  
  gtk_widget_set_sensitive (print_data->widgets.from_sp_but, FALSE);
  gtk_widget_set_sensitive (print_data->widgets.to_sp_but, FALSE);

  return page_frame;
}

static FILE*
create_temp_print_file (gchar **outfile_name)
{
  gint counter;
  FILE *file_stream;
  char *file_name, *template;
  
  file_stream = NULL;
  counter = 0;
  
  template = g_strdup ("/tmp/gfo-XXXXXX");

  while (!file_stream && counter < 64)
    {
      file_name = mktemp (template);
      file_stream = fopen (template, "wx");
      if (file_stream)
	*outfile_name = template;
      counter++;
    }

  return file_stream;
}

static guint
size_of_page (FaxPage *page)
{
  gfloat f_result;
  guint result;
  
  f_result = (gfloat) page->width * (gfloat) page->height / 8.0;
  result = (guint) f_result;

  return result;
}

static guint
size_of_output (OutputData *output_data)
{
  FaxPage *cur_page;
  guint result, pcount, from_p, to_p;
  
  result = 0;
  from_p = output_data->from_page;
  to_p = output_data->to_page;
  cur_page = output_data->document->first;

  for (pcount = from_p; pcount <= to_p; pcount++)
    {
      result += size_of_page (cur_page);
      cur_page = cur_page->next;
    }
  
  return result;
}

static guint
print_image (FILE *print_file, FaxPage *fax_image,
	     GfvProgressData *p_data,
	     guint progress, guint unit, guint total_bytes)
{
  gboolean aborted;
  guint full_length, counter, new_progress;
  guchar cur_char;
  
  full_length = size_of_page (fax_image);

  aborted = FALSE;
  new_progress = progress;
  
  for (counter = 0; (counter < full_length) && !aborted; counter++)
    {
      new_progress++;
      if ((new_progress % unit) == 0)
	aborted = gfv_progress_update_with_value (new_progress,
						  total_bytes,
						  0,
						  p_data);
      if (!aborted)
	{
	  cur_char = *(fax_image->image + counter);
	  fprintf (print_file, "%.2x", (unsigned char) ~cur_char);
		
	  if ((counter + 1) % 40 == 0)
	    fprintf (print_file, "\n");
	}
      else
	new_progress = 0;
    }

  if (!aborted && ((counter + 1) % 40 != 0))
    fprintf (print_file, "\n");

  return new_progress;
}

static gboolean
output_document (OutputData *output_data)
{
  gint cur_page_nbr;
  guint cur_byte, unit, total_bytes;
  gboolean success;
  gchar *time_string, *ps_title, *ps_creator, *p_action;
  FaxPage *cur_page;
  GfvProgressData *p_data;
  time_t cur_time;

#ifdef ENABLE_NLS
  /* If we don't do this, our floating-point numbers might
   * become floating-comma numbers which postscript doesn't
   * understand that well... */
  setlocale (LC_NUMERIC, "C");
#endif

  cur_page_nbr = output_data->from_page;
  time (&cur_time);
  time_string = ctime (&cur_time);
  
  success = TRUE;
  cur_byte = 0;
  total_bytes = size_of_output (output_data);
  unit = total_bytes / 100;

  ps_creator = g_strdup_printf(_("The GNU HaliFAX Viewer"));
  ps_title = g_strdup_printf ("%s %s - %s",
			      _("The GNU HaliFAX Viewer"),
			      VERSION,
			      output_data->document->file_name);

  fprintf (output_data->output_stream,
	   "%%!PS-Adobe-3.0\n"
	   "%%%%Creator: %s\n"
	   "%%%%Title: %s\n"
	   "%%%%CreationDate: %s"
	   "%%%%DocumentData: Clean7Bit\n"
	   "%%%%Origin: 0 0\n"
	   "%%%%BoundingBox: 0 0 612 792\n"
	   "%%%%LanguageLevel: 1\n"
	   "%%%%Pages: (atend)\n"
	   "%%%%EndComments\n"
	   "%%%%BeginSetup\n"
	   "%%%%EndSetup\n",
	   ps_creator, ps_title, time_string);
  
  g_free (ps_title);
  g_free (ps_creator);
  
  p_data = gfv_progress_new (output_data->parent_window,
			     _("Please wait..."), NULL, ABORT_BTN);
  
  while (cur_page_nbr <= output_data->to_page && success)
    {
      p_action = g_strdup_printf (_("Printing page %d (%d left)"),
				  cur_page_nbr,
				  output_data->to_page
				  - cur_page_nbr);
      gfv_progress_set_action (p_data, p_action);
      g_free (p_action);
      cur_page = ti_seek_fax_page (output_data->document,
				   cur_page_nbr - 1);

      fprintf (output_data->output_stream,
	       "%%%%Page: %d %d\n",
	       cur_page_nbr,
	       cur_page_nbr);
      fprintf (output_data->output_stream,
	       "gsave\n"
	       "100 dict begin\n"
	       "%f %f scale\n",
	       ((float) cur_page->width * 72.0
		/ (float) output_data->document->x_res),
	       ((float) cur_page->height
		* 72.0
		/ (float) output_data->document->y_res));
      fprintf (output_data->output_stream,
	       "%%ImageData: %d %d 1 1 0 1 2 \"image\"\n",
	       cur_page->width, cur_page->height);
      fprintf (output_data->output_stream,
	       "/scanLine %d string def\n",
	       (int) (cur_page->width / 8));
      fprintf (output_data->output_stream,
	       "%d %d 1\n", cur_page->width, cur_page->height);
      fprintf (output_data->output_stream,
	       "[%d 0 0 -%d 0 %d]\n", cur_page->width,
	       cur_page->height, cur_page->height);
      fprintf (output_data->output_stream,
	       "{currentfile scanLine readhexstring pop} bind\n"
	       "image\n");
      
      ti_load_fax_page (output_data->document, cur_page);
      cur_byte = print_image (output_data->output_stream,
			      cur_page, p_data,
			      cur_byte, unit, total_bytes);

      if (!cur_byte)
	success = FALSE;
      else
	{
	  fprintf (output_data->output_stream,
		   "end\n"
		   "grestore\n"
		   "showpage\n");

	  cur_page_nbr++;
	}

      ti_unload_fax_page (cur_page);
    }

  if (success)
    fprintf (output_data->output_stream,
	     "%%%%Trailer\n"
	     "%%%%Pages: %d\n"
	     "%%%%EOF\n", (cur_page_nbr - output_data->from_page));

#ifdef ENABLE_NLS
  setlocale (LC_NUMERIC, "");
#endif

  gfv_progress_destroy (p_data);


  return success;
}

static void
print_to_file_anyway_cb (GtkWidget *yes_button,
			 OutputData *output_data)
{
  gboolean success;
  DialogWindow *print_dialog;
  
  output_data->output_stream = fopen (output_data->out_file_name, "w+");
  success = output_document (output_data);
  fclose (output_data->output_stream);
  
  print_dialog = output_data->print_dialog;

  if (success)
    {
      dialog_window_destroy (output_data->err_dialog);
      dialog_window_destroy (print_dialog);
    }
  else
    {
      unlink (output_data->out_file_name);
      dialog_window_destroy (output_data->err_dialog);
    }
}

static void
file_exists_dialog_destroy_cb (GtkWidget *window, OutputData *output_data)
{
  g_free (output_data->out_file_name);
  g_free (output_data);
}

static void
file_exists_dialog (OutputData *output_data, DialogWindow *print_dialog)
{
  DialogWindow *err_dialog;
  GtkWidget *button_box, *msg_lbl, *yes_but, *no_but;
  gchar *message;
  
  message = g_strdup_printf (_("%s already exists.\n"
			       "Do you want to overwrite it?"),
			     output_data->out_file_name);

  err_dialog = dialog_window_new (_("Please answer..."));
  dialog_window_add_destroy_callback (err_dialog,
				      GTK_SIGNAL_FUNC (file_exists_dialog_destroy_cb),
				      output_data);

  msg_lbl = gtk_label_new (message);
  g_free (message);
  gtk_label_set_justify (GTK_LABEL (msg_lbl), GTK_JUSTIFY_LEFT);
  dialog_window_set_content_with_frame (err_dialog, msg_lbl);
  
  button_box = dialog_window_bbox ();
  dialog_window_set_button_box (err_dialog, button_box);
  dialog_window_set_escapable (err_dialog);

  yes_but = gtk_button_new_with_label (_("Yes, please do"));
  gtk_signal_connect (GTK_OBJECT (yes_but), "clicked",
		      GTK_SIGNAL_FUNC (print_to_file_anyway_cb),
		      output_data);
  gtk_box_pack_start (GTK_BOX (button_box), yes_but,
		      FALSE, FALSE, 5);
  GTK_WIDGET_SET_FLAGS (yes_but, GTK_CAN_DEFAULT);

  no_but = gtk_button_new_with_label (_("No thanks"));
  gtk_signal_connect (GTK_OBJECT (no_but), "clicked",
		      GTK_SIGNAL_FUNC (dialog_window_destroy_from_signal),
		      err_dialog);
  gtk_box_pack_start (GTK_BOX (button_box), no_but,
		      FALSE, FALSE, 5);
  GTK_WIDGET_SET_FLAGS(no_but, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (no_but);

  output_data->err_dialog = err_dialog;
  output_data->print_dialog = print_dialog;

  dialog_window_show (err_dialog,
		      dialog_window_get_gtkwin (print_dialog));
}

static gint
launch_print_job_cb (GtkWidget *widget,
		     PrintData *print_data)
{
  OutputData *output_data;
  PrintDirection direction;
  gchar *command, *print_command;
  gboolean progress_success;
  
  progress_success = FALSE;

  output_data = g_malloc (sizeof (OutputData));
  output_data->document = print_data->document;
  output_data->parent_window = print_data->parent_window;
  
  if (gtk_toggle_button_get_active
      (GTK_TOGGLE_BUTTON (print_data->widgets.printer_rb)))
    direction = PRINT_TO_PRINTER;
  else
    direction = PRINT_TO_FILE;
	
  if (gtk_toggle_button_get_active
      (GTK_TOGGLE_BUTTON (print_data->widgets.all_pages_rb)))
    {
      output_data->from_page = 1;
      output_data->to_page = print_data->document->nbr_pages + 1;
    }
  else if (gtk_toggle_button_get_active
	   (GTK_TOGGLE_BUTTON (print_data->widgets.from_to_rb)))
    {
      output_data->from_page = gtk_spin_button_get_value_as_int 
	(GTK_SPIN_BUTTON (print_data->widgets.from_sp_but));
      output_data->to_page = gtk_spin_button_get_value_as_int
	(GTK_SPIN_BUTTON (print_data->widgets.to_sp_but));
    } else
      output_data->from_page = output_data->to_page =
	print_data->current_page->nbr + 1;

  if (direction == PRINT_TO_PRINTER)
    {
      print_command = gtk_entry_get_text
	(GTK_ENTRY (print_data->widgets.printer_cmd_en));
      output_data->output_stream =
	create_temp_print_file (&output_data->out_file_name);

      progress_success = output_document (output_data);
      fclose (output_data->output_stream);

      if (progress_success)
	{
	  command = g_strdup_printf ("%s -P%s %s",
				     print_command,
				     print_data->printer,
				     output_data->out_file_name);
	  system (command);
	  g_free (command);

	  dialog_window_destroy (print_data->print_dialog);
	}

      unlink (output_data->out_file_name);
      g_free (output_data->out_file_name);
      g_free (output_data);
    }
  else
    {
      output_data->out_file_name = g_strdup
	(gtk_entry_get_text
	 (GTK_ENTRY (print_data->widgets.output_file_en)));
      output_data->output_stream = fopen
	(output_data->out_file_name, "wx");

      if (output_data->output_stream)
	{
	  progress_success = output_document (output_data);
	  fclose (output_data->output_stream);
	  
	  if (!progress_success)
	    unlink (output_data->out_file_name);

	  g_free (output_data->out_file_name);
	  g_free (output_data);

	  dialog_window_destroy (print_data->print_dialog);
	}
      else
	file_exists_dialog (output_data, print_data->print_dialog);
    }

  return FALSE;
}

static void
print_dialog_bbox (PrintData *print_data)
{
  GtkWidget *button_box, *print_but, *cancel_but;

  button_box = dialog_window_bbox ();
  dialog_window_set_button_box (print_data->print_dialog,
				button_box);
  dialog_window_set_escapable (print_data->print_dialog);

  print_but = gtk_button_new_with_label (_("Print"));
  GTK_WIDGET_SET_FLAGS(print_but, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (button_box), print_but, FALSE, FALSE, 0);
  gtk_widget_grab_default (print_but);
  gtk_signal_connect (GTK_OBJECT (print_but), "clicked",
		      GTK_SIGNAL_FUNC (launch_print_job_cb),
		      print_data);

  cancel_but = gtk_button_new_with_label (_("Cancel"));
  GTK_WIDGET_SET_FLAGS(cancel_but, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (button_box), cancel_but, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT (cancel_but), "clicked",
		      GTK_SIGNAL_FUNC (dialog_window_destroy_from_signal),
		      print_data->print_dialog);

  gtk_widget_show (button_box);
}

static DialogWindow *
print_dialog (ViewerData *viewer_data)
{
  DialogWindow *print_dialog;
  GtkWidget *table, *output_frame, *page_frame;
  PrintData *print_data;

  print_dialog = dialog_window_new (_("Print..."));

  print_data = g_malloc (sizeof (PrintData));
  print_data->document = viewer_data->fax_file;
  print_data->current_page = viewer_data->current_page;
  print_data->parent_window = viewer_data->viewer_window;
  print_data->print_dialog = print_dialog;

  table = gtk_table_new (3, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table), 5);
  output_frame = make_output_frame (print_data);
  page_frame = make_page_frame (print_data);

  gtk_table_attach_defaults (GTK_TABLE (table), output_frame,
			     0, 2, 0, 1); 
  gtk_table_attach_defaults (GTK_TABLE (table), page_frame,
			     0, 2, 1, 2); 

  print_dialog_bbox (print_data);

  dialog_window_set_content (print_dialog, table);
  dialog_window_add_destroy_callback (print_dialog,
				      GTK_SIGNAL_FUNC (free_data_on_destroy_cb),
				      print_data);

  return print_dialog;
}

void
print_cb (GtkWidget *widget, ViewerData *viewer_data)
{
  DialogWindow *print_dlg;
  static gboolean error_dialog_shown = FALSE;

  ensure_commands (viewer_data->viewer_window);

  print_dlg = print_dialog (viewer_data);
  dialog_window_show (print_dlg, viewer_data->viewer_window);

  if (!printer_enabled && !error_dialog_shown)
    {
      display_failure (dialog_window_get_gtkwin (print_dlg),
		       _("This is weird..."),
		       _("I was not able to determine how to work\n"
			 "with your printing system appropriately.\n\n"
			 "You will therefore only be able to export\n"
			 "your data to a PostScript file.\n\n"
			 "Please have your administrator verify if\n"
			 "a descent printing system is installed.\n\n"
			 "If this is so, there must be a bug in\n"
			 "ghfaxviewer. Please fill a bug report\n"
			 "accordingly and send it to\n"
			 "halifax-bugs@gnu.org."),
		       _("I promise!"));

      error_dialog_shown = TRUE;
    }
}

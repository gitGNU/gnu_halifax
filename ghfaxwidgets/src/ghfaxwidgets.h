/* ghfaxwidgets.h - this file is part of the GNU HaliFAX Widgets library
 *
 * Copyright (C) 2001 Wolfgang Sourdeau
 *
 * Author: Wolfgang Sourdeau <wolfgang@contre.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef GHFAXWIDGETS_H
#define GHFAXWIDGETS_H

#include <ghfaxwidgets/ghfwgdkutils.h>
#include <ghfaxwidgets/ghfwgtkutils.h>
#include <ghfaxwidgets/ghfwdlgwindow.h>
#include <ghfaxwidgets/ghfwprogress.h>
#include <ghfaxwidgets/ghfwthumbbox.h>
#include <ghfaxwidgets/ghfwurlzone.h>

/* Gtk version.
 */
extern const guint ghfw_major_version;
extern const guint ghfw_minor_version;
extern const guint ghfw_micro_version;
extern const guint ghfw_binary_age;
extern const guint ghfw_interface_age;
gchar* ghfw_check_version (guint required_major,
                           guint required_minor,
                           guint required_micro);

#endif /* GHFAXWIDGETS_H */

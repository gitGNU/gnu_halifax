/* ghfaxwidgets.c - this file is part of the GNU HaliFAX Widgets library
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <glib.h>

const guint ghfw_major_version = GHFW_MAJOR_VERSION;
const guint ghfw_minor_version = GHFW_MINOR_VERSION;
const guint ghfw_micro_version = GHFW_MICRO_VERSION;
const guint ghfw_binary_age = GHFW_BINARY_AGE;
const guint ghfw_interface_age = GHFW_INTERFACE_AGE;

gchar*
ghfw_check_version (guint required_major,
		    guint required_minor,
		    guint required_micro)
{
  if (required_major > GHFW_MAJOR_VERSION)
    return "GHfaxWidgets version too old (major mismatch)";
  if (required_major < GHFW_MAJOR_VERSION)
    return "GHfaxWidgets version too new (major mismatch)";
  if (required_minor > GHFW_MINOR_VERSION)
    return "GHfaxWidgets version too old (minor mismatch)";
  if (required_minor < GHFW_MINOR_VERSION)
    return "GHfaxWidgets version too new (minor mismatch)";
  if (required_micro < GHFW_MICRO_VERSION - GHFW_BINARY_AGE)
    return "GHfaxWidgets version too new (micro mismatch)";
  if (required_micro > GHFW_MICRO_VERSION)
    return "GHfaxWidgets version too old (micro mismatch)";
  return NULL;
}

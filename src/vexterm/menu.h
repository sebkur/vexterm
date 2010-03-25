/* VexTerm - a lightweight and fast terminal emulator
 *
 * Copyright (C) 2010  Sebastian Kuerten
 *
 * This file is part of VexTerm.
 *
 * VexTerm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VexTerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VexTerm.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MENU_H_
#define _MENU_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define VEX_TYPE_MENU           (menu_get_type ())
#define VEX_MENU(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_MENU, Menu))
#define VEX_MENU_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_MENU, MenuClass))
#define VEX_IS_MENU(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_MENU))
#define VEX_IS_MENU_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_MENU))
#define VEX_MENU_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_MENU, MenuClass))

GType   menu_get_type (void) G_GNUC_CONST;

typedef struct _Menu        Menu;
typedef struct _MenuClass   MenuClass;

struct _Menu
{
	GtkMenuBar parent;

	GtkWindow * main_window;

	/*
	 * Start auto-generated menu
	 * :r !./misc/menu_gen.py 1 misc/Menu.txt
	 */
	GtkWidget * menu_file_new_window;
	GtkWidget * menu_file_new_tab;
	GtkWidget * menu_file_quit;
	GtkWidget * menu_view_fullscreen;
	GtkWidget * menu_options_preferences;
	GtkWidget * menu_profiles;
	GtkWidget * menu_show_scrolling_region;
	GtkWidget * menu_show_status_bar;
	GtkWidget * menu_help_manual;
	GtkWidget * menu_help_about_vexterm;
	GtkWidget * menu_help_license;
	// END
	/*
	 * End auto-generated menu
	 */
};

struct _MenuClass
{
	GtkMenuBarClass parent_class;
};

GType menu_get_type (void) G_GNUC_CONST;

GtkWidget * menu_new();

#endif /* _MENU_H_ */

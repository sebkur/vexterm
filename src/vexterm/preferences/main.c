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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <math.h>
#include <popt.h>
#include <libgen.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>

#include "preferences.h"
#include "../vex_paths.h"
#include "../config/vex_config.h"
#include "../config/vex_config_reader.h"

char * executable;

int main(int argc, char *argv[])
{
	executable = g_strdup(argv[0]);

	/***************************************************************************
	 * initialization **********************************************************
	 ***************************************************************************/
	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	/***************************************************************************
	 * window setup ************************************************************
	 ***************************************************************************/
	GtkWidget * main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "vexterm");
	gtk_window_set_default_size(GTK_WINDOW(main_window), 500, 400);
	gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(exit), NULL);

	/***************************************************************************
	 * window content **********************************************************
	 ***************************************************************************/
	VexConfig * vex_config = vex_config_new();
	char * conf_file = vex_term_find_ressource("../../../res/config/config_local.xml");
	VexConfigReader * vcr = vex_config_reader_new();
	vex_config_reader_fill_config_from_file(vcr, conf_file, vex_config);

	Preferences * preferences = VEX_PREFERENCES(preferences_new(vex_config));

	GtkWidget * box_h = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_h), GTK_WIDGET(preferences), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), box_h);

	gtk_widget_show_all(main_window);
	gtk_widget_grab_focus(GTK_WIDGET(preferences));

	/***************************************************************************
	 * main loop ***************************************************************
	 ***************************************************************************/
	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();

	return 0;
}

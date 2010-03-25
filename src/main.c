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

#include "vexterm/vex_term.h"
#include "vexterm/vex_paths.h"
#include "vexterm/config/vex_config.h"
#include "vexterm/config/vex_config_reader.h"
#include "vexterm/config/vex_layered_config.h"

char * executable;

/***************************************************************************
 * these are options *******************************************************
 ***************************************************************************/
int 		vex_font_size	= 10;
char * 		vex_font_name	= "Monospace";
int 		vex_win_x;
int 		vex_win_y;
gboolean 	vex_win_x_set 	= FALSE;
gboolean 	vex_win_y_set 	= FALSE;
int 		vex_win_width	= 700;
int 		vex_win_height	= 500;
gboolean	statusbar	= TRUE;


int main(int argc, char *argv[])
{
	printf("%s\n", argv[0]);
	executable = g_strdup(argv[0]);

	/***************************************************************************
	 * POPT option parsing *****************************************************
	 ***************************************************************************/
	struct poptOption poptions[] = {
		{"fs", 			's', POPT_ARG_INT, 	&vex_font_size, 	1, "font size", "pixel"},
		{"fn", 			'n', POPT_ARG_STRING, 	&vex_font_name, 	2, "font name", "family"},
		{"x", 			'x', POPT_ARG_INT, 	&vex_win_x, 		3, "window's x-position", "pixel"},
		{"y", 			'y', POPT_ARG_INT, 	&vex_win_y, 		4, "window's y-position", "pixel"},
		{"width", 		'w', POPT_ARG_INT, 	&vex_win_width, 	5, "window's width", "pixel"},
		{"height", 		'h', POPT_ARG_INT, 	&vex_win_height, 	6, "window's height", "pixel"},
		{"show-statusbar", 	'b', POPT_ARG_NONE, 	NULL,		 	8, "show statusbar", NULL},
		POPT_AUTOHELP
		{ NULL, 0, 0, NULL, 0 }
		};
	poptContext pcontext = poptGetContext(NULL, argc, (const char **)argv, poptions, POPT_CONTEXT_NO_EXEC);
	poptSetOtherOptionHelp(pcontext, "[OPTION]... [PWD]");
	int rc;
	while ((rc = poptGetNextOpt(pcontext)) > 0) {
		if (rc == 3) vex_win_x_set = TRUE;
		if (rc == 4) vex_win_y_set = TRUE;
	}
	//const char * pwd = poptGetArg(pcontext); // will be NULL if not set //TODO: use this

	/***************************************************************************
	 * initialization **********************************************************
	 ***************************************************************************/
	g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	/***************************************************************************
	 * configuration ***********************************************************
	 ***************************************************************************/
	VexLayeredConfig * vlc = vex_layered_config_new(NULL, NULL);

	char * conf_file_local = vex_term_get_config_file_local();
	char * conf_file_global = vex_term_get_config_file_global();
	printf("GLOBAL CONFIG FILE: %s\n", conf_file_global);
	printf("LOCAL CONFIG FILE: %s\n", conf_file_local);

	VexConfigReader * vcr = vex_config_reader_new();
	vex_config_reader_fill_config_from_file(vcr, conf_file_local, vex_layered_config_get_config_local(vlc));
	vex_config_reader_fill_config_from_file(vcr, conf_file_global, vex_layered_config_get_config_global(vlc));

	/***************************************************************************
	 * window setup ************************************************************
	 ***************************************************************************/
	GtkWidget * main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), "vexterm");
	gtk_window_set_default_size(GTK_WINDOW(main_window), vex_win_width, vex_win_height);
	gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
	if (vex_win_x_set && vex_win_y_set){
		gtk_window_move(GTK_WINDOW(main_window), vex_win_x, vex_win_y);
	}
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(exit), NULL);

	char * logo = vex_term_find_ressource("res/logo.png");
	if (logo != NULL){
		gtk_window_set_icon_from_file(GTK_WINDOW(main_window), logo, NULL);
	}
	/***************************************************************************
	 * window content **********************************************************
	 ***************************************************************************/
	VexTerm * vex = VEX_VEX_TERM(vex_term_new(vlc));

	GtkWidget * box_h = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_h), GTK_WIDGET(vex), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), box_h);

	gtk_widget_show_all(main_window);
	gtk_widget_grab_focus(GTK_WIDGET(vex));

	/***************************************************************************
	 * main loop ***************************************************************
	 ***************************************************************************/
	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();

	return 0;
}

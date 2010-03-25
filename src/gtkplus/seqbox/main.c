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

#include "gtk_v_seq_box.h"

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
	gtk_window_set_default_size(GTK_WINDOW(main_window), 400, 300);
	gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
	g_signal_connect(G_OBJECT(main_window), "hide", G_CALLBACK(exit), NULL);

	/***************************************************************************
	 * window content **********************************************************
	 ***************************************************************************/
	//GtkWidget * foo = gtk_vbox_new(FALSE, 0);
	GtkWidget * foo = gtk_v_seq_box_new(FALSE, 0, strcmp, NULL, NULL);
//	GtkWidget * but1 = gtk_button_new_with_label("foo a");
//	GtkWidget * but2 = gtk_button_new_with_label("foo b");
//	gtk_seq_box_pack_start(GTK_SEQ_BOX(foo), GTK_WIDGET(but1), TRUE, TRUE, 0);
//	gtk_seq_box_pack_start(GTK_SEQ_BOX(foo), GTK_WIDGET(but2), TRUE, TRUE, 0);
//
//	int i; for (i = 0; i < 20; i++){
//		GtkWidget * but = gtk_button_new_with_label("foo c");
//		gtk_seq_box_pack_start(GTK_SEQ_BOX(foo), GTK_WIDGET(but), TRUE, TRUE, 0);
//	}

	GtkWidget * but1 = gtk_button_new_with_label("foo x");
	gtk_seq_box_insert(GTK_SEQ_BOX(foo), g_strdup("b"), GTK_WIDGET(but1), TRUE, TRUE, 0);
	GtkWidget * but2 = gtk_button_new_with_label("foo y");
	gtk_seq_box_insert(GTK_SEQ_BOX(foo), g_strdup("c"), GTK_WIDGET(but2), TRUE, TRUE, 0);

	gtk_seq_box_reorder(GTK_SEQ_BOX(foo), "c", "a");
	gtk_seq_remove(GTK_SEQ_BOX(foo), "b");
	gtk_seq_remove(GTK_SEQ_BOX(foo), "b");

	GtkWidget * box_h = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), box_h);
	gtk_box_pack_start(GTK_BOX(box_h), GTK_WIDGET(foo), TRUE, TRUE, 0);

	gtk_widget_show_all(main_window);

	/***************************************************************************
	 * main loop ***************************************************************
	 ***************************************************************************/
	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();

	return 0;
}

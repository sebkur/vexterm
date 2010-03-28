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
#include <libgen.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>

#include "gtk_splittable.h"
#include "../../vexterm/vex_single.h"

char * executable;

void foo(GtkWidget * widget)
{
	if (VEX_IS_GTK_SPLITTABLE(widget)){
		printf("splittable\n");
		GtkSplittable * s = VEX_GTK_SPLITTABLE(widget);
		if (!gtk_splittable_is_splitted(s)){
			GtkWidget * c = gtk_splittable_get_child(s);
			foo(c);
		}else{
			GtkWidget * c1 = gtk_splittable_get_child_by_position(s, GTK_SPLIT_LEFT);
			GtkWidget * c2 = gtk_splittable_get_child_by_position(s, GTK_SPLIT_RIGHT);
			foo(c1);
			foo(c2);
		}
	}else{
		printf("widget\n");
	}
}

int main(int argc, char *argv[])
{
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
	GtkWidget * a = gtk_button_new_with_label("asdf");
	GtkWidget * b = gtk_button_new_with_label("foo");
	GtkWidget * c = gtk_button_new_with_label("bar");

//	GtkWidget * split1 = gtk_splittable_new_with_child(a);
//	GtkWidget * split2 = gtk_splittable_new_with_child(c);
//	//gtk_splittable_split(VEX_GTK_SPLITTABLE(split1), b, GTK_SPLIT_LEFT);
//	gtk_splittable_split(VEX_GTK_SPLITTABLE(split1), split2, GTK_SPLIT_LEFT);
//	gtk_splittable_split(VEX_GTK_SPLITTABLE(split2), b, GTK_SPLIT_BOTTOM);

	GtkWidget * v1 = vex_single_new("/home/z/", NULL);
	GtkWidget * v2 = vex_single_new("/home/z/", NULL);
	GtkWidget * v3 = vex_single_new("/home/z/", NULL);

//	GtkWidget * split1 = gtk_splittable_new_with_child(v1);
//	GtkWidget * split2 = gtk_splittable_new_with_child(v2);
//	GtkWidget * split3 = gtk_splittable_new_with_child(v3);

	GtkWidget * split1 = gtk_splittable_new_with_child(v1);
	GtkWidget * split2 = gtk_splittable_new_with_child(b);
	//gtk_splittable_split(VEX_GTK_SPLITTABLE(split1), v2, GTK_SPLIT_LEFT);
	gtk_splittable_split(VEX_GTK_SPLITTABLE(split1), split2, GTK_SPLIT_LEFT);
	gtk_splittable_split(VEX_GTK_SPLITTABLE(split2), v2, GTK_SPLIT_BOTTOM);

	GtkWidget * box_h = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), box_h);
	gtk_box_pack_start(GTK_BOX(box_h), GTK_WIDGET(split1), TRUE, TRUE, 0);

	gtk_widget_show_all(main_window);

	// testing sth.
	foo(split1);

	/***************************************************************************
	 * main loop ***************************************************************
	 ***************************************************************************/
	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();

	return 0;
}

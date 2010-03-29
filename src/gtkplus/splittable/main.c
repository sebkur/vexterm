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
#include "splittable_collection.h"
#include "../../vexterm/vex_single.h"
#include "../../helpers/tool.h"
#include "../../helpers/util.h"

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

void split_it(SplittableCollection * sc, GtkSplitPosition pos)
{
	GtkSplittable * split = splittable_collection_get_active_splittable(VEX_SPLITTABLE_COLLECTION(sc));
	printf("acitve: %p\n", split);
	GtkWidget * v1 = vex_single_new("/home/z/", NULL);

	if (gtk_splittable_is_empty(split)){
		// empty
		printf("splittable empty\n");
		gtk_splittable_set_child(split, v1);
	}else if (!gtk_splittable_is_splitted(split)){
		// one child
		printf("splittable with a child\n");
		gtk_splittable_split(split, v1, pos);
	}else{
		// splitted
		printf("splittable splitted\n");
	}
	gtk_widget_show_all(v1);
}

static gboolean key_press_cb(GtkWidget * widget, GdkEventKey * event, SplittableCollection * sc)
{
	switch (event -> keyval){
		case GDK_F10:{
			split_it(sc, GTK_SPLIT_TOP);
			break;
		}
		case GDK_F11:{
			split_it(sc, GTK_SPLIT_LEFT);
			break;
		}
	}
	return FALSE;
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

	GtkWidget * v1 = vex_single_new("/home/z/", NULL);
	GtkWidget * v2 = vex_single_new("/home/z/", NULL);
	GtkWidget * v3 = vex_single_new("/home/z/", NULL);

	GtkWidget * sc = splittable_collection_new();	
	GtkSplittable * split = gtk_splittable_new();
	splittable_collection_set_main_component(sc, GTK_WIDGET(split));
	gtk_splittable_set_child(split, v1);

	GtkWidget * box_h = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), box_h);
	gtk_box_pack_start(GTK_BOX(box_h), GTK_WIDGET(sc), TRUE, TRUE, 0);

	gtk_widget_show_all(main_window);

	g_signal_connect(
		G_OBJECT(main_window), "key-press-event",
		G_CALLBACK(key_press_cb), sc);

	// testing sth.
	//foo(split1);

	/***************************************************************************
	 * main loop ***************************************************************
	 ***************************************************************************/
	gdk_threads_enter();	
	gtk_main();
	gdk_threads_leave();

	return 0;
}

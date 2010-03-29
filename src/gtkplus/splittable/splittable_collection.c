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
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "splittable_collection.h"
#include "../../helpers/tool.h"
#include "../../helpers/util.h"

G_DEFINE_TYPE (SplittableCollection, splittable_collection, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint splittable_collection_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, splittable_collection_signals[SIGNAL_NAME_n], 0);

static void splittable_collection_class_init(SplittableCollectionClass *class)
{
        /*splittable_collection_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SplittableCollectionClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void splittable_collection_init(SplittableCollection *splittable_collection)
{
}

void splittable_collection_child_connect_focus_handler(SplittableCollection * sc, GtkWidget * child);
void splittable_collection_splittable_connect_child_added_handler(SplittableCollection * sc, GtkSplittable * splittable);

gboolean splittable_collection_child_focus_in_cb(GtkWidget * child, GdkEventFocus * event, SplittableCollection * sc);
void splittable_collection_splittable_child_add_cb(GtkSplittable * splittable, GtkWidget * child, SplittableCollection * sc);
void splittable_collection_child_parent_notify_cb(GtkWidget * widget, GParamSpec * pspec, SplittableCollection * sc);

GtkWidget * splittable_collection_new()
{
	SplittableCollection * sc = g_object_new(VEX_TYPE_SPLITTABLE_COLLECTION, NULL);

	sc -> descendents_to_splittables = g_map_new(compare_pointers, NULL);
	sc -> splittables = g_sequence_new(NULL);
	sc -> focussed_descendent = NULL;

	return GTK_WIDGET(sc);
}

void splittable_collection_set_main_component(SplittableCollection * sc, GtkWidget * child)
{
	sc -> main = child;
	if (VEX_IS_GTK_SPLITTABLE(child)){
		GtkSplittable * split = VEX_GTK_SPLITTABLE(child);
		splittable_collection_splittable_connect_child_added_handler(sc, split);
	}
	gtk_box_pack_start(GTK_BOX(sc), GTK_WIDGET(child), TRUE, TRUE, 0);
}

GtkSplittable * splittable_collection_get_active_splittable(SplittableCollection * sc)
{
	if (sc -> focussed_descendent == NULL){
		return VEX_GTK_SPLITTABLE(sc -> main);
	}
	GtkWidget * parent = gtk_widget_get_parent(sc -> focussed_descendent);
	return VEX_GTK_SPLITTABLE(parent);
	//return (GtkSplittable*) g_map_get(sc -> descendents_to_splittables, sc -> focussed_descendent);
}

void splittable_collection_splittable_connect_child_added_handler(SplittableCollection * sc, GtkSplittable * splittable)
{
	GSequenceIter * i = g_sequence_find(sc -> splittables, splittable, compare_pointers, NULL);
	if (i == NULL){
		g_sequence_insert_sorted(sc -> splittables, splittable, compare_pointers, NULL);
		g_signal_connect(
			G_OBJECT(splittable), "child-add",
			G_CALLBACK(splittable_collection_splittable_child_add_cb), sc);
	}
}

void splittable_collection_child_connect_focus_handler(SplittableCollection * sc, GtkWidget * child)
{
	g_signal_connect(
		G_OBJECT(child), "focus-in-event",
		G_CALLBACK(splittable_collection_child_focus_in_cb), sc);
	g_signal_connect(
		G_OBJECT(child), "notify::parent",
		G_CALLBACK(splittable_collection_child_parent_notify_cb), sc);
}

void splittable_collection_splittable_child_add_cb(GtkSplittable * splittable, GtkWidget * child, SplittableCollection * sc)
{
	printf("(splittable %p): child %p added\n", splittable, child);
	//g_map_put(sc -> descendents_to_splittables, child, splittable);
	splittable_collection_child_connect_focus_handler(sc, child);
}

gboolean splittable_collection_child_focus_in_cb(GtkWidget * child, GdkEventFocus * event, SplittableCollection * sc)
{
	printf("%s\n", g_type_name(G_OBJECT_TYPE(sc)));
	printf("splittable focused %p %p\n", child, sc);
	sc -> focussed_descendent = child;
	return FALSE;
}

void splittable_collection_child_parent_notify_cb(GtkWidget * widget, GParamSpec * pspec, SplittableCollection * sc)
{
	GtkWidget * parent = gtk_widget_get_parent(widget);
	printf("REPARENT: %p\n", parent);
	if (parent != NULL){
		if (VEX_IS_GTK_SPLITTABLE(parent)){
			printf("PARENT IS A SPLITTABLE\n");
			splittable_collection_splittable_connect_child_added_handler(sc, VEX_GTK_SPLITTABLE(parent));
		}
	}
}

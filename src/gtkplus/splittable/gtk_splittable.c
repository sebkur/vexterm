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

#include "gtk_splittable.h"

G_DEFINE_TYPE (GtkSplittable, gtk_splittable, GTK_TYPE_VBOX);

enum
{
        ADD,
        REMOVE,
	SPLIT,
	UNSPLIT,
        LAST_SIGNAL
};

static guint gtk_splittable_signals[LAST_SIGNAL] = { 0 };

static void gtk_splittable_class_init(GtkSplittableClass *class)
{
        gtk_splittable_signals[ADD] = g_signal_new(
                "child-add",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (GtkSplittableClass, add),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
        gtk_splittable_signals[REMOVE] = g_signal_new(
                "child-remove",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (GtkSplittableClass, remove),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
        gtk_splittable_signals[SPLIT] = g_signal_new(
                "split",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (GtkSplittableClass, split),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        gtk_splittable_signals[UNSPLIT] = g_signal_new(
                "unsplit",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (GtkSplittableClass, unsplit),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void gtk_splittable_init(GtkSplittable *gtk_splittable)
{
}

void gtk_splittable_paned_position_cb(GtkPaned * paned, GParamSpec * pspec, GtkSplittable * splittable);
void gtk_splittable_paned_size_cb(GtkPaned * paned, GtkAllocation * allocation, GtkSplittable * splittable);
gboolean gtk_splittable_child_focus_in_cb(GtkWidget * child, GdkEventFocus * event, GtkSplittable * splittable);
void gtk_splittable_child_connect_focus_handler(GtkWidget * child, GtkSplittable * splittable);

GtkWidget * gtk_splittable_new()
{
	return gtk_splittable_new_with_child(NULL);
}

GtkWidget * gtk_splittable_new_with_child(GtkWidget * child)
{
	GtkSplittable * splittable = g_object_new(VEX_TYPE_GTK_SPLITTABLE, NULL);

	splittable -> split_mode = GTK_SPLITTED_NOT;
	splittable -> paned = NULL;
	splittable -> child1 = NULL;
	splittable -> child2 = NULL;
	splittable -> paned_width = 0;
	splittable -> paned_height = 0;
	splittable -> percentage = 0.5;
	splittable -> size_initialized = FALSE;
	splittable -> size_event = FALSE;

	if (child != NULL){
		splittable -> child1 = child;
		gtk_container_add(GTK_CONTAINER(splittable), child);
		gtk_splittable_child_connect_focus_handler(child, splittable);
	}

	if (VEX_IS_GTK_SPLITTABLE(child)){
		printf("splittable created with splittable\n");
	}

	return GTK_WIDGET(splittable);
}


void gtk_splittable_split(GtkSplittable * splittable, GtkWidget * child, GtkSplitPosition position)
{
	if (gtk_splittable_is_splitted(splittable)){
		// this should not happen
		return;
	}
	if (splittable -> child1 == NULL){
		return;
	}
	splittable -> split_mode = (position == GTK_SPLIT_LEFT || position == GTK_SPLIT_RIGHT)
		? GTK_SPLITTED_HORIZONTALLY : GTK_SPLITTED_VERTICALLY;
	GtkWidget * old_child = splittable -> child1;
	g_object_ref(old_child);
	gtk_container_remove(GTK_CONTAINER(splittable), old_child);
	GtkWidget * paned = splittable -> split_mode == GTK_SPLITTED_HORIZONTALLY
		? gtk_hpaned_new() : gtk_vpaned_new();
	gtk_container_add(GTK_CONTAINER(splittable), paned);
	g_signal_emit (splittable, gtk_splittable_signals[ADD], 0, child);

	GtkWidget * splittable1 = VEX_IS_GTK_SPLITTABLE(child) ? child : gtk_splittable_new_with_child(child);
	GtkWidget * splittable2 = VEX_IS_GTK_SPLITTABLE(old_child) ? old_child : gtk_splittable_new_with_child(old_child);

	if (position == GTK_SPLIT_RIGHT || position == GTK_SPLIT_BOTTOM){
		GtkWidget * s = splittable1;
		splittable1 = splittable2;
		splittable2 = s;
	}

	gtk_paned_add1(GTK_PANED(paned), splittable1);
	gtk_paned_add2(GTK_PANED(paned), splittable2);

	g_object_unref(old_child);

	splittable -> child1 = splittable1;
	splittable -> child2 = splittable2;

	splittable -> paned = paned;
	g_signal_connect(
		G_OBJECT(paned), "notify::position",
		G_CALLBACK (gtk_splittable_paned_position_cb), splittable);
	g_signal_connect(
		G_OBJECT(paned), "size-allocate",
		G_CALLBACK (gtk_splittable_paned_size_cb), splittable);

	gtk_splittable_child_connect_focus_handler(child, splittable);

	if (VEX_IS_GTK_SPLITTABLE(child)){
		printf("splittable splitted with splittable\n");
	}
	gtk_widget_show_all(paned);
}

void gtk_splittable_unsplit_by_position(GtkSplittable * splittable, GtkSplitPosition position)
{
}

void gtk_splittable_unsplit_by_child(GtkSplittable * splittable, GtkWidget * child)
{
}


gboolean gtk_splittable_is_empty(GtkSplittable * splittable)
{
	return splittable -> child1 == NULL;
}

gboolean gtk_splittable_is_splitted(GtkSplittable * splittable)
{
	return splittable -> split_mode != GTK_SPLITTED_NOT;
}

GtkSplitMode gtk_splittable_get_split_mode(GtkSplittable * splittable)
{
	return splittable -> split_mode;
}


GtkWidget * gtk_splittable_get_child(GtkSplittable * splittable)
{
	if (gtk_splittable_is_splitted(splittable)){
		// this should not happen
		return NULL;
	}
	return splittable -> child1;
}

void gtk_splittable_set_child(GtkSplittable * splittable, GtkWidget * child)
{
	if (gtk_splittable_is_splitted(splittable)){
		// this should not happen
		return;
	}
	splittable -> child1 = child;
	g_signal_emit (splittable, gtk_splittable_signals[ADD], 0, child);
	gtk_container_add(GTK_CONTAINER(splittable), child);
	gtk_splittable_child_connect_focus_handler(child, splittable);
}


GtkWidget * gtk_splittable_get_child_by_position(GtkSplittable * splittable, GtkSplitPosition position)
{
	if (!gtk_splittable_is_splitted(splittable)){
		// this should not happen
		return NULL;
	}
	if (position == GTK_SPLIT_LEFT || position == GTK_SPLIT_TOP){
		return splittable -> child1;
	}
	return splittable -> child2;
}

void gtk_splittable_set_child_by_position(GtkSplittable * splittable, GtkWidget * child, GtkSplitPosition position)
{
	if (!gtk_splittable_is_splitted(splittable)){
		// this should not happen
		return;
	}
	// set
}

/* internal */

void gtk_splittable_paned_position_cb(GtkPaned * paned, GParamSpec * pspec, GtkSplittable * splittable)
{
	if (splittable -> size_initialized && !splittable -> size_event){
		/* only set percentage, if 'size_cb' has been called at least once. otherwise
		   we could see 's == 1' and this would destroy the initial position given by 
		   the user-set or default percentage */
		/* additionally, only set, if the event has not been triggered by a resizement.
		   if we did this, we could distort percentages, e.g. when the paned is resized
		   very small, there will be huge influence of rounding errors */
		GtkAllocation allocation = GTK_WIDGET(paned) -> allocation;
		int s = splittable -> split_mode == GTK_SPLITTED_HORIZONTALLY ? allocation.width : allocation.height;
		int p = gtk_paned_get_position(paned);
		splittable -> percentage = ((double) p) / s;
		//printf("user move\n");
	}
	/* after size_cb always comes position_cb. Thus we don't use this flag to 
	   recognize this */
	splittable -> size_event = FALSE;
}

void gtk_splittable_paned_size_cb(GtkPaned * paned, GtkAllocation * allocation, GtkSplittable * splittable)
{
	splittable -> size_initialized = TRUE;
	if ((allocation -> width != splittable -> paned_width) || (allocation -> height != splittable -> paned_height)){
		/* size changed */
		splittable -> paned_width = allocation -> width;
		splittable -> paned_height = allocation -> height;
		int s = splittable -> split_mode == GTK_SPLITTED_HORIZONTALLY ? allocation -> width : allocation -> height;
		int target = (int) (s * splittable -> percentage);
		if (target != gtk_paned_get_position(paned)){
			gtk_paned_set_position(paned, target);
		}
		//printf("size change\n");
		splittable -> size_event = TRUE;
	}
}

gboolean gtk_splittable_child_focus_in_cb(GtkWidget * child, GdkEventFocus * event, GtkSplittable * splittable)
{
	//printf("focused %p %p\n", child, splittable);
	return FALSE;
}

void gtk_splittable_child_connect_focus_handler(GtkWidget * child, GtkSplittable * splittable)
{
	g_signal_connect(
		G_OBJECT(child), "focus-in-event",
		G_CALLBACK(gtk_splittable_child_focus_in_cb), splittable);
//	g_signal_connect(
//		G_OBJECT(child), "focus-out-event",
//		G_CALLBACK(gtk_splittable_child_grabbed_focus_cb), splittable);
}

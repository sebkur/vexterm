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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "list_v_box.h"
#include "../../helpers/tool.h"
#include "../../gtkplus/seqbox/gtk_v_seq_box.h"

G_DEFINE_TYPE (ListVBox, list_v_box, GTK_TYPE_VBOX);

enum
{
        SELECT,
        LAST_SIGNAL
};

static guint list_v_box_signals[LAST_SIGNAL] = { 0 };

void list_v_box_constructor(ListVBox * box);

GtkWidget * list_v_box_new(
			GCompareDataFunc cmp_func,
			gpointer cmp_data)
{
	ListVBox * list_v_box = g_object_new(VEX_TYPE_LIST_V_BOX, NULL);

	list_v_box -> cmp_func = cmp_func;
	list_v_box -> cmp_data = cmp_data;
	list_v_box_constructor(list_v_box);

	return GTK_WIDGET(list_v_box);
}

static void list_v_box_class_init(ListVBoxClass *class)
{
        list_v_box_signals[SELECT] = g_signal_new(
                "select",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ListVBoxClass, select),
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void list_v_box_init(ListVBox *list_v_box)
{
}

void list_v_box_select_callback(GtkWidget * widget, gpointer boolean);

void list_v_box_select(GtkWidget * widget, gboolean state)
{
	if ((GTK_IS_LABEL(widget) || GTK_IS_CONTAINER(widget)) && !GTK_IS_BUTTON(widget)){
		gtk_widget_set_state(widget, state ? GTK_STATE_SELECTED : GTK_STATE_NORMAL);
		if (GTK_IS_CONTAINER(widget)){
			gtk_container_forall(GTK_CONTAINER(widget),
				list_v_box_select_callback, GINT_TO_POINTER(state));
		}else{
		}
	}
	if (GTK_IS_BUTTON(widget) || GTK_IS_COMBO_BOX(widget)){
		gtk_widget_set_state(widget, GTK_STATE_NORMAL);
	}
}

void list_v_box_select_callback(GtkWidget * widget, gpointer state)
{
	list_v_box_select(widget, GPOINTER_TO_INT(state));
}

void list_v_box_event_box_press(GtkWidget * box, GdkEventButton * event, gpointer data)
{
	ListVBox * lbox = VEX_LIST_V_BOX(data);

	if (lbox -> selected == box){
		return;
	}

	if (lbox -> selected != NULL){
		list_v_box_select(lbox -> selected, FALSE);
		gtk_widget_queue_draw(lbox -> selected);
	}
	list_v_box_select(box, TRUE);
	gtk_widget_queue_draw(box);

	gpointer key = gtk_seq_box_get_key(lbox -> box, box);
	//printf("key: %s\n", key);

	lbox -> selected = box;
	lbox -> selected_key = key;

	g_signal_emit (lbox, list_v_box_signals[SELECT], 0, key);
}

void list_v_box_constructor(ListVBox * list_box)
{
	GtkWidget * ebox = gtk_event_box_new();
	list_box -> box = GTK_SEQ_BOX(gtk_v_seq_box_new(FALSE, 0, list_box -> cmp_func, list_box -> cmp_func, NULL));
	gtk_container_add(GTK_CONTAINER(ebox), GTK_WIDGET(list_box -> box));
	/* if we wanted a white background */
//	GdkColor color = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
//	gtk_widget_modify_bg(ebox, GTK_STATE_NORMAL, &color);
	gtk_container_add(GTK_CONTAINER(list_box), ebox);

	list_box -> selected = NULL;
	list_box -> selected_key = NULL;
}

void list_v_box_insert(ListVBox * box, gpointer key, GtkWidget * widget)
{
	GtkWidget * ebox = gtk_event_box_new();
	gtk_widget_show(ebox);
	gtk_container_add(GTK_CONTAINER(ebox), widget);
	gtk_seq_box_insert(box -> box, key, ebox, FALSE, FALSE, 0);

	g_signal_connect(
		G_OBJECT(ebox), "button-press-event",
		G_CALLBACK(list_v_box_event_box_press), box);
}

void list_v_box_remove(ListVBox * box, gpointer key)
{
	if (box -> cmp_func(box -> selected_key, key, box -> cmp_data) == 0){
		box -> selected = NULL;
		box -> selected_key = NULL;
	}
	gtk_seq_remove(box -> box, key);
	g_signal_emit (box, list_v_box_signals[SELECT], 0, NULL);
}

void list_v_box_rename(ListVBox * box, gpointer old_key, gpointer new_key)
//void list_v_box_rename(ListVBox * box, char * old_key, char * new_key)
{
	gtk_seq_box_reorder(box -> box, old_key, new_key);
	if (box -> cmp_func(box -> selected_key, old_key, box -> cmp_data) == 0){
		box -> selected_key = new_key;
	}
}

char * list_v_box_get_selected_key(ListVBox * box)
{
	return box -> selected_key;
}

GtkWidget * list_v_box_get(ListVBox * box, gpointer key)
//GtkWidget * list_v_box_get(ListVBox * box, char * key)
{
	GtkWidget * ebox = gtk_seq_box_get(box -> box, key);
	return gtk_bin_get_child(GTK_BIN(ebox));
}

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

#ifndef _LIST_V_BOX_H_
#define _LIST_V_BOX_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../../gtkplus/seqbox/gtk_seq_box.h"

#define VEX_TYPE_LIST_V_BOX           (list_v_box_get_type ())
#define VEX_LIST_V_BOX(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_LIST_V_BOX, ListVBox))
#define VEX_LIST_V_BOX_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_LIST_V_BOX, ListVBoxClass))
#define VEX_IS_LIST_V_BOX(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_LIST_V_BOX))
#define VEX_IS_LIST_V_BOX_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_LIST_V_BOX))
#define VEX_LIST_V_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_LIST_V_BOX, ListVBoxClass))

typedef struct _ListVBox        ListVBox;
typedef struct _ListVBoxClass   ListVBoxClass;

struct _ListVBox
{
	GtkVBox parent;

	GtkSeqBox * box;

	GCompareDataFunc cmp_func;
	gpointer cmp_data;

	gpointer selected_key;
	GtkWidget * selected;
};

struct _ListVBoxClass
{
	GtkVBoxClass parent_class;

	void (* select) (ListVBox *list_v_box, gpointer key);
};

GType list_v_box_get_type (void) G_GNUC_CONST;

GtkWidget * list_v_box_new(
			GCompareDataFunc cmp_func,
			gpointer cmp_data);

void list_v_box_insert(ListVBox * box, gpointer key, GtkWidget * widget);

void list_v_box_remove(ListVBox * box, gpointer key);

void list_v_box_rename(ListVBox * box, gpointer old_key, gpointer new_key);

char * list_v_box_get_selected_key(ListVBox * box);

GtkWidget * list_v_box_get(ListVBox * box, gpointer key);

#endif /* _LIST_V_BOX_H_ */

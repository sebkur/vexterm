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

#ifndef _GTK_SPLITTABLE_H_
#define _GTK_SPLITTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define VEX_TYPE_GTK_SPLITTABLE           (gtk_splittable_get_type ())
#define VEX_GTK_SPLITTABLE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_GTK_SPLITTABLE, GtkSplittable))
#define VEX_GTK_SPLITTABLE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_GTK_SPLITTABLE, GtkSplittableClass))
#define VEX_IS_GTK_SPLITTABLE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_GTK_SPLITTABLE))
#define VEX_IS_GTK_SPLITTABLE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_GTK_SPLITTABLE))
#define VEX_GTK_SPLITTABLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_GTK_SPLITTABLE, GtkSplittableClass))

GType gtk_splittable_get_type (void) G_GNUC_CONST; 

typedef struct _GtkSplittable        GtkSplittable;
typedef struct _GtkSplittableClass   GtkSplittableClass;

typedef enum {
	GTK_SPLITTED_NOT,
	GTK_SPLITTED_VERTICALLY,
	GTK_SPLITTED_HORIZONTALLY
} GtkSplitMode;

typedef enum {
	GTK_SPLIT_LEFT,
	GTK_SPLIT_RIGHT,
	GTK_SPLIT_TOP,
	GTK_SPLIT_BOTTOM
} GtkSplitPosition;

struct _GtkSplittable
{
	GtkVBox parent;

	GtkSplitMode split_mode;
	GtkWidget * paned;
	GtkWidget * child1;
	GtkWidget * child2;

	GtkAllocation last_seen_allocation;
	int paned_width;
	int paned_height;
	double percentage;
	gboolean size_initialized;
	gboolean size_event;
};

struct _GtkSplittableClass
{
	GtkBinClass parent_class;

	void (* add) (GtkSplittable * gtk_splittable, GtkWidget * child);
	void (* remove) (GtkSplittable * gtk_splittable, GtkWidget * child);
	void (* split) (GtkSplittable * gtk_splittable);
	void (* unsplit) (GtkSplittable * gtk_splittable);
};

GtkWidget * gtk_splittable_new();
GtkWidget * gtk_splittable_new_with_child(GtkWidget * child);

void gtk_splittable_split(GtkSplittable * splittable, GtkWidget * child, GtkSplitPosition position);
void gtk_splittable_unsplit_by_position(GtkSplittable * splittable, GtkSplitPosition position);
void gtk_splittable_unsplit_by_child(GtkSplittable * splittable, GtkWidget * child);

gboolean gtk_splittable_is_empty(GtkSplittable * splittable);
gboolean gtk_splittable_is_splitted(GtkSplittable * splittable);
GtkSplitMode gtk_splittable_get_split_mode(GtkSplittable * splittable);

GtkWidget * gtk_splittable_get_child(GtkSplittable * splittable);
void gtk_splittable_set_child(GtkSplittable * splittable, GtkWidget * child);

GtkWidget * gtk_splittable_get_child_by_position(GtkSplittable * splittable, GtkSplitPosition position);
void gtk_splittable_set_child_by_position(GtkSplittable * splittable, GtkWidget * child, GtkSplitPosition position);

#endif /* _GTK_SPLITTABLE_H_ */

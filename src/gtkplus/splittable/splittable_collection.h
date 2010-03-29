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

#ifndef _SPLITTABLE_COLLECTION_H_
#define _SPLITTABLE_COLLECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "gtk_splittable.h"
#include "../../helpers/g_map/g_map.h"

#define VEX_TYPE_SPLITTABLE_COLLECTION           (splittable_collection_get_type ())
#define VEX_SPLITTABLE_COLLECTION(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_SPLITTABLE_COLLECTION, SplittableCollection))
#define VEX_SPLITTABLE_COLLECTION_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_SPLITTABLE_COLLECTION, SplittableCollectionClass))
#define VEX_IS_SPLITTABLE_COLLECTION(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_SPLITTABLE_COLLECTION))
#define VEX_IS_SPLITTABLE_COLLECTION_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_SPLITTABLE_COLLECTION))
#define VEX_SPLITTABLE_COLLECTION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_SPLITTABLE_COLLECTION, SplittableCollectionClass))

GType splittable_collection_get_type (void) G_GNUC_CONST; 

typedef struct _SplittableCollection        SplittableCollection;
typedef struct _SplittableCollectionClass   SplittableCollectionClass;

struct _SplittableCollection
{
	GtkVBox parent;

	GtkWidget * main;
	GtkWidget * focussed_descendent;

	GMap * descendents_to_splittables;
	GSequence * splittables;
};

struct _SplittableCollectionClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (SplittableCollection *splittable_collection);
};

GtkWidget * splittable_collection_new();

void splittable_collection_set_main_component(SplittableCollection * sc, GtkWidget * child);
GtkSplittable * splittable_collection_get_active_splittable(SplittableCollection * sc);

#endif /* _SPLITTABLE_COLLECTION_H_ */

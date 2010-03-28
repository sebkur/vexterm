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

#ifndef _G_MAP_H_
#define _G_MAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <glib-object.h>

#define G_TYPE_MAP           (g_map_get_type ())
#define G_MAP(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_MAP, GMap))
#define G_MAP_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), G_TYPE_MAP, GMapClass))
#define G_IS_MAP(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_MAP))
#define G_IS_MAP_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), G_TYPE_MAP))
#define G_MAP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_MAP, GMapClass))

GType g_map_get_type (void) G_GNUC_CONST; 

typedef struct _GMap        GMap;
typedef struct _GMapClass   GMapClass;

typedef GSequenceIter GMapIter;

struct _GMap
{
	GObject parent;
	
	GCompareDataFunc compare_func;
	gpointer compare_data;
	GSequence * sequence;
};

struct _GMapClass
{
	GObjectClass parent_class;

	//void (* function_name) (GMap *g_map);
};

GMap * g_map_new(GCompareDataFunc compare_func, gpointer compare_data);

void g_map_put(GMap * map, gpointer key, gpointer val);
gpointer g_map_get(GMap * map, gpointer key);

GMapIter * g_map_get_begin_iter(GMap * map);
GMapIter * g_map_get_end_iter(GMap * map);
GMapIter * g_map_get_iter_at_pos(GMap * map, gint pos);
GMapIter * g_map_get_iter_for_key(GMap * map, gpointer key);

gboolean g_map_iter_is_begin(GMapIter * iter);
gboolean g_map_iter_is_end(GMapIter * iter);
GMapIter * g_map_iter_next(GMapIter * iter);
GMapIter * g_map_iter_prev(GMapIter * iter);
gint g_map_iter_get_position(GMapIter * iter);
GMapIter * g_map_iter_move(GMapIter * iter, gint delta);

gpointer g_map_iter_get(GMapIter * iter, gpointer * key);

#endif /* _G_MAP_H_ */

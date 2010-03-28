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

#include "g_map.h"
#include "../util.h"

G_DEFINE_TYPE (GMap, g_map, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint g_map_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, g_map_signals[SIGNAL_NAME_n], 0);

static void g_map_class_init(GMapClass * class)
{
        /*g_map_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (GMapClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void g_map_init(GMap * g_map)
{
}

typedef struct {
	gpointer key;
	gpointer val;
} Entry;

int compare_function(gconstpointer a, gconstpointer b, gpointer data)
{
	GMap * map = (GMap*) data;
	Entry * ea = (Entry*) a;
	Entry * eb = (Entry*) b;
	return map -> compare_func(ea -> key, eb -> key, map -> compare_data);
}

GMap * g_map_new(GCompareDataFunc compare_func, gpointer compare_data)
{
	GMap * map = g_object_new(G_TYPE_MAP, NULL);
	map -> compare_func = compare_func;
	map -> compare_data = compare_data;
	map -> sequence = g_sequence_new(NULL);
	return map;
}

void g_map_put(GMap * map, gpointer key, gpointer val)
{
	Entry * e = g_slice_alloc(sizeof(Entry));
	e -> key = key;
	e -> val = val;
	g_sequence_insert_sorted(map -> sequence, e, compare_function, map);
}

gpointer g_map_get(GMap * map, gpointer key)
{
	Entry f; f.key = key;
	GSequenceIter * iter = g_sequence_find(map -> sequence, &f, compare_function, map);
	if (iter == NULL) {
		return NULL;
	}
	Entry * e = (Entry*) g_sequence_get(iter);
	return e -> val;
}


GMapIter * g_map_get_begin_iter(GMap * map)
{
	return g_sequence_get_begin_iter(map -> sequence);
}

GMapIter * g_map_get_end_iter(GMap * map)
{
	return g_sequence_get_end_iter(map -> sequence);
}

GMapIter * g_map_get_iter_at_pos(GMap * map, gint pos)
{
	return g_sequence_get_iter_at_pos(map -> sequence, pos);
}

GMapIter * g_map_get_iter_for_key(GMap * map, gpointer key)
{
	Entry f; f.key = key;
	return g_sequence_find(map -> sequence, &f, compare_function, map);
}


gboolean g_map_iter_is_begin(GMapIter * iter)
{
	return g_sequence_iter_is_begin(iter);
}

gboolean g_map_iter_is_end(GMapIter * iter)
{
	return g_sequence_iter_is_end(iter);
}

GMapIter * g_map_iter_next(GMapIter * iter)
{
	return g_sequence_iter_next(iter);
}

GMapIter * g_map_iter_prev(GMapIter * iter)
{
	return g_sequence_iter_prev(iter);
}

gint g_map_iter_get_position(GMapIter * iter)
{
	return g_sequence_iter_get_position(iter);
}

GMapIter * g_map_iter_move(GMapIter * iter, gint delta)
{
	return g_sequence_iter_move(iter, delta);
}


gpointer g_map_iter_get(GMapIter * iter, gpointer * key)
{
	Entry * e = (Entry*) g_sequence_get(iter);
	if (e == NULL){
		if (key != NULL) *key = NULL;
		return NULL;
	}else{
		if (key != NULL) *key = e -> key;
		return e -> val;
	}
}


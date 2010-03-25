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

/*
 * Class History.
 *
 * This is mainly a stack. Hence you operate on it using 'push' and 'pop'
 * operators.
 * 
 * The stack is additionally a kind of ringbuffer. Once there are as many
 * elements in it as defined by the 'size' argument, the most ancient 
 * elements will be replaced by new ones.
 *
 * It is implemented using a growing GArray. Hence the structure will 
 * occupy minimal space in the beginning and grow as necessary.
 * NOTE: in contrast to GArray, the History structure (most propably) won't
 * shrink as elements are being removed since the alignment of the elements 
 * in the internal GArray does not necessarily allow this.
 *
 * TODO: Add an option to allow dynamic shrinking. May be implemented by
 * copying existing elements to a new array of half size once 
 * history -> elements <= history -> size / 4 
 *
 * TODO: Implement set_size
 *
 * NOTE: There are some testing methods at the end of 'history.c'
 * compile by 'gcc `pkg-config --libs --cflags glib-2.0 gthread-2.0 gobject-2.0` -o history_test history.c'
 */

#ifndef _HISTORY_H_
#define _HISTORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

#define LIBVEX_TYPE_HISTORY           (history_get_type ())
#define LIBVEX_HISTORY(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_HISTORY, History))
#define LIBVEX_HISTORY_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_HISTORY, HistoryClass))
#define LIBVEX_IS_HISTORY(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_HISTORY))
#define LIBVEX_IS_HISTORY_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_HISTORY))
#define LIBVEX_HISTORY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_HISTORY, HistoryClass))

typedef struct _History        History;
typedef struct _HistoryClass   HistoryClass;

struct _History
{
	GObject parent;

	GArray * array;
	int size;
	int start;
	int elements;
};

struct _HistoryClass
{
	GObjectClass parent_class;

	//void (* function_name) (History *history);
};

GType history_get_type (void) G_GNUC_CONST;

/* create a new History with maximum number of elements 'size' */
History * history_new(int size);

/* set the new maximum number of elements to 'size' */ // NOT IMPLEMENTED!
void history_set_size(History * history, int size);

/* get the maximum number of elements */
int history_get_size(History * history);

/* Push the element 'data' into the history.
 * If an ancient element has been replaced:
 * 	returns TRUE
 *	puts the replaced element in 'replaced'
 * Otherwise:
 * 	returns FALSE
 */
gboolean history_push(History * history, gpointer data, gpointer * replaced);

/* Pop one element from the history.
 * Returns:
 * 	TRUE on success
 * 	FALSE otherwise (no elements in history)
 * On success, puts the retrieved element into 'data'
 * and removes the element from history
 */
gboolean history_pop(History * history, gpointer * data);

/* Retrive the element at position 'index'.
 * NOTE: It's up to you to perform legal operations here.
 * Be sure not to ask for an element which is out of range.
 */
gpointer history_get(History * history, int index);

/* Return the number of elements in the history */
int history_get_number_of_elements(History * history);

#endif /* _HISTORY_H_ */

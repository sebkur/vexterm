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
#include <glib-object.h>

#include "history.h"

#define DEBUG_HISTORY 0

G_DEFINE_TYPE (History, history, G_TYPE_OBJECT);

static void history_class_init(HistoryClass *class)
{
}

static void history_init(History *history)
{
}

History * history_new(int size)
{
	History * history = g_object_new(LIBVEX_TYPE_HISTORY, NULL);
	history -> size = size;
	history -> array = g_array_new(FALSE, FALSE, sizeof(gpointer));
	history -> elements = 0;
	history -> start = 0;
	return history;
}

void history_set_size(History * history, int size)
{
	printf("NOT IMPLEMENTED: history_set_size\n");
}

int history_get_size(History * history)
{
	return history -> size;
}

gboolean history_push(History * history, gpointer data, gpointer * replaced)
{
	if (history -> elements != history -> size){
		// not reached maximum fill level
		int vpos = history -> elements; // virtual position
		int rpos = (history -> start + vpos) % history -> size;
		if (history -> array -> len <= rpos){
			// position in array doesn't exist currently
			g_array_insert_val(history -> array, rpos, data);
			#if DEBUG_HISTORY
			printf("PUSH; simple append\n");
			#endif
		}else{
			// position available
			gpointer * place = &g_array_index(history -> array, gpointer, rpos);
			*place = data;
			#if DEBUG_HISTORY
			printf("PUSH; romm available\n");
			#endif
		}
		history -> elements += 1;
		return FALSE;
	}else{
		// array is full. replace sth.
		gpointer * place = &g_array_index(history -> array, gpointer, history -> start);
		*replaced = *place;
		*place = data;
		history -> start = (history -> start + 1) % history -> size;
		#if DEBUG_HISTORY
		printf("PUSH; replace\n");
		#endif
		return TRUE;
	}
}

gboolean history_pop(History * history, gpointer * data)
{
	if (history -> elements == 0){
		#if DEBUG_HISTORY
		printf("POP; nothing to pop\n");
		#endif
		return FALSE; // nothing to pop
	}
	#if DEBUG_HISTORY
	printf("POP; something\n");
	#endif
	int vpos = history -> elements - 1; // virtual position
	int rpos = (history -> start + vpos) % history -> size;
	gpointer * place = &g_array_index(history -> array, gpointer, rpos);
	history -> elements -= 1;
	*data = *place;
	return TRUE;
}

gpointer history_get(History * history, int index)
{
	#if DEBUG_HISTORY
	printf("GET; index %d\n", index);
	#endif
	int rpos = (history -> start + index) % history -> size;
	gpointer data = g_array_index(history -> array, gpointer, rpos);
	return data;
}

int history_get_number_of_elements(History * history)
{
	return history -> elements;
}

#define TEST_HISTORY 0
#if TEST_HISTORY

void history_print(History * history)
{
	printf("-----------------------------------\n");
	printf("History has %d lines\n", history_get_number_of_elements(history));
	int i;
	for (i = 0; i < history_get_number_of_elements(history); i++){
		gpointer data = history_get(history, i);
		printf("index: %d, value %d\n", i, GPOINTER_TO_INT(data));
	}
	printf("-----------------------------------\n");
}

void pop(History * history)
{
	gpointer data;
	gboolean popped = history_pop(history, &data);
	if (popped){
		printf("Popped %d\n", GPOINTER_TO_INT(data));
	}else{
		printf("nothing to pop\n");
	}
}

void push(History * history, int n)
{
	gpointer replaced;
	gboolean rep = history_push(history, GINT_TO_POINTER(n), &replaced);
	printf("Pushed %d, ", n);
	if (rep){
		printf("replaced %d\n", GPOINTER_TO_INT(replaced));
	}else{
		printf("replaced nothing\n");
	}
}

int main(int argc, char * argv[])
{
	g_thread_init(NULL);
	g_type_init();
	History * history = history_new(4);
	gpointer rep, data;
	history_print(history);
	push(history, 1);
	history_print(history);
	pop(history);
	history_print(history);
	pop(history);
	history_print(history);
	push(history, 2);
	push(history, 3);
	push(history, 4);
	push(history, 5);
	push(history, 6);
	push(history, 8);
	push(history, 9);
	push(history, 10);
	push(history, 11);
	history_print(history);
	pop(history);
	history_print(history);
	pop(history);
	history_print(history);
	push(history, 12);
	history_print(history);
	pop(history);
	history_print(history);
	push(history, 13);
	push(history, 14);
	push(history, 15);
	push(history, 16);
	push(history, 17);
}
#endif

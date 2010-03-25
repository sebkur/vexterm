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

#ifndef _TOOL_H_
#define _TOOL_H_

#include <sys/time.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

int time_diff(struct timeval * t1, struct timeval * t2);

GtkWidget * find_containing_gtk_window(GtkWidget * widget);

char * get_abs_uri(char * filename);

void toggle_widget_visibility(GtkWidget * widget);
void set_widget_visibility(GtkWidget * widget, gboolean show);

int * int_malloc(int value);
int * int_malloc_p(int * value);

gint compare_int_pointers_easy(gconstpointer a, gconstpointer b);
gint compare_int_pointers(gconstpointer a, gconstpointer b, gpointer user_data);
gint compare_pointers(gconstpointer a, gconstpointer b, gpointer user_data);
gint compare_string_pointers(gpointer a, gpointer b);
gint compare_strings_data(gconstpointer a, gconstpointer b, gpointer data);
gint compare_strings(gconstpointer a, gconstpointer b);

void command_exec(char * command, char * args, char * path);

#endif /* _TOOL_H_ */

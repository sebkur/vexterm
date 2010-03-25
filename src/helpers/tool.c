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
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include "tool.h"

extern char ** environ;

int time_diff(struct timeval * t1, struct timeval * t2)
{
	int sec_diff  = t2 -> tv_sec - t1 -> tv_sec;
	int msec = sec_diff * 1000000 + t2 -> tv_usec - t1 -> tv_usec;
	return msec;
}

GtkWidget * find_containing_gtk_window(GtkWidget * widget)
{
	GtkWidget * current = widget;
	GtkWidget * cparent = gtk_widget_get_parent(current);
	while(cparent != NULL && !GTK_IS_WINDOW(cparent)){
		current = cparent;
		cparent = gtk_widget_get_parent(current);
	}
	return cparent;
}

char * get_abs_uri(char * filename)
{
	if (filename == NULL) return NULL;
	char * abs = realpath(filename, NULL);
	if (abs == NULL) return NULL;
	char * uri_prefix = "file://";
	char * uri = malloc(sizeof(char) * (strlen(uri_prefix) + strlen(abs) + 1));
	uri[0] = '\0';
	strcat(uri, uri_prefix);
	strcat(uri, abs);
	free(abs);
	return uri;
}

void toggle_widget_visibility(GtkWidget * widget)
{
	gboolean visible = GTK_WIDGET_VISIBLE(widget);
	if (visible){
		gtk_widget_hide(widget);
	}else{
		gtk_widget_show(widget);
	}
}

void set_widget_visibility(GtkWidget * widget, gboolean show)
{
	if (show){
		gtk_widget_show(widget);
	}else{
		gtk_widget_hide(widget);
	}
}

int * int_malloc(int value)
{
	int * i = malloc(sizeof(int));
	*i = value;
	return i;
}

int * int_malloc_p(int * value)
{
	int * i = malloc(sizeof(int));
	*i = *value;
	return i;
}

gint compare_int_pointers_easy(gconstpointer a, gconstpointer b)
{
	if (a == NULL && b == NULL) return 0;
	if (a == NULL) return -1;
	if (b == NULL) return 1;
	return *(int*)a - *(int*)b;
}

gint compare_int_pointers(gconstpointer a, gconstpointer b, gpointer user_data)
{
	if (a == NULL && b == NULL) return 0;
	if (a == NULL) return -1;
	if (b == NULL) return 1;
	return *(int*)a - *(int*)b;
}

gint compare_pointers(gconstpointer a, gconstpointer b, gpointer user_data)
{
	return a - b;
}

gint compare_string_pointers(gpointer a, gpointer b)
{
	char ** sa = (char**)a;
	char ** sb = (char**)b;
	int c = strcmp(*sa, *sb);
	return c;
}

gint compare_strings_data(gconstpointer a, gconstpointer b, gpointer data)
{
	char * sa = (char*)a;
	char * sb = (char*)b;
	int c = strcmp(sa, sb);
	return c;
}

gint compare_strings(gconstpointer a, gconstpointer b)
{
	return strcmp(a, b);
}

//gint compare_strings(gconstpointer a, gconstpointer b, gpointer user_data)
//{
//	return strcmp(a, b);
//}

char * replace_special(char * string, char * replacement)
{
	//TODO: "\%s" has to be substitued by "%s"
	GArray * places = g_array_new(FALSE, FALSE, sizeof(gpointer));
	int len = strlen(string);
	int len_r = strlen(replacement);
	int i;
	gboolean backslash = FALSE;
	gboolean percent = FALSE;
	for (i = 0; i < len; i++){
		char c = string[i];
		if (c == '\\') {
			backslash = TRUE;
			percent = FALSE;
		}else if (c == '%'){
			if (percent) backslash = FALSE;
			percent = TRUE;
		}else if (c == 'f' || c == 'F' || c == 'u' || c == 'U'){
			if (percent && !backslash){
				//printf("found %d\n", i-1);
				gpointer insert = GINT_TO_POINTER(i-1);
				g_array_append_val(places, insert);
			}
			backslash = FALSE;
			percent = FALSE;
		}else{
			backslash = FALSE;
			percent = FALSE;
		}
	}
	char * result = malloc(sizeof(char) * (len + places -> len * (len_r - 2) + 1));
	int pos = 0;
	int opos = 0;
	int p;
	for (p = 0; p < places -> len; p++){
		int rpos = GPOINTER_TO_INT(g_array_index(places, gpointer, p));
		strncpy(result + pos, string + opos, rpos - opos);
		pos += rpos - opos;
		strncpy(result + pos, replacement, len_r); 
		pos += len_r;
		opos = rpos + 2;
	}
	strncpy(result + pos, string + opos, len - opos + 1);
	return result;
}

void command_exec(char * command, char * args, char * path)
{
	if (command != NULL){
		pid_t pid = vfork();
		if (pid == 0){
			char ** argl = g_strsplit(args, " ", 256);
			int argc = g_strv_length(argl);
			printf("argc: %d, path: %s\n", argc, path);
			char ** argv = malloc(sizeof(char*) * (argc+2));
			argv[0] = command;
			int x = 0;
			char ** argp = argl;
			while(*argp != NULL){
				int this = x++;
				char * arg = *argp;
				argp++;
				argv[this + 1] = replace_special(arg, path);
				//printf("%s %s\n", arg, replace_special(arg, path));
			}
			argv[argc+1] = NULL;
			for (x = 0; x <= argc; x++){
				printf("arg %d: %s\n", x, argv[x]);
			}
			//int e = execve(command, argv, environ);
			execvp(command, argv);
			exit(0);
		}
	}
}

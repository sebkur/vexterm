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
#include <pango/pango.h>

#include "about_widget.h"
#include "about_logo.h"
#include "srt_parser.h"
#include "../vex_paths.h"

#define DOC_PATH "res/"
#define LOGO_HEIGHT_DEFAULT 40

G_DEFINE_TYPE (AboutWidget, about_widget, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint about_widget_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, about_widget_signals[SIGNAL_NAME_n], 0);

void about_widget_constructor(AboutWidget * about_widget, int preselected);

GtkWidget * about_widget_new(int preselected)
{
	AboutWidget * about_widget = g_object_new(VEX_TYPE_ABOUT_WIDGET, NULL);
	about_widget_constructor(about_widget, preselected);
	return GTK_WIDGET(about_widget);
}

static void about_widget_class_init(AboutWidgetClass *class)
{
        /*about_widget_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (AboutWidgetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void about_widget_init(AboutWidget *about_widget)
{
}

GtkWidget * about_widget_create_view_from_file(char * file, char * default_text);

void about_widget_constructor(AboutWidget * about_widget, int preselected)
{
	char * files[][2] = {
		{"Manual", 	DOC_PATH "doc_manual.srt"},
		{"About", 	DOC_PATH "doc_about.srt"},
		{"License", 	DOC_PATH "doc_gpl.srt"}
		};

	char * default_texts[] = {
		"The manual file could not be found",
		"Vexterm. Copyright (C) 2010 Sebastian Kürten\n\n"
		"(More information is not available since the about-file could not be found)",
		"Vexterm is distributed under the terms of the GNU General Public License (GPL) in version 3"
		" or (at your choise) any later version."
	};

	GtkNotebook * nb = GTK_NOTEBOOK(gtk_notebook_new());

	int p;
	for (p = 0; p < sizeof(files) / sizeof(char*) / 2; p++){
		GtkWidget * view = about_widget_create_view_from_file(files[p][1], default_texts[p]);

		GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
		gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(view));
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);

		GtkWidget * label = gtk_label_new(files[p][0]);
		gtk_widget_show_all(scrolled);
		gtk_notebook_append_page(nb, scrolled, label);
	}

	GtkWidget * logo = about_logo_new();
	int logo_height;
	gtk_widget_get_size_request(logo, NULL, &logo_height);
	if (logo_height == 1){
		gtk_widget_set_size_request(logo, -1, LOGO_HEIGHT_DEFAULT);
	}

	gtk_box_pack_start(GTK_BOX(about_widget), GTK_WIDGET(logo), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(about_widget), GTK_WIDGET(nb), TRUE, TRUE, 0);

	gtk_notebook_set_current_page(nb, preselected);
}

GtkWidget * about_widget_create_view_from_file(char * file, char * default_text)
{
	GtkTextView * view = GTK_TEXT_VIEW(gtk_text_view_new());
	gtk_text_view_set_editable(view, FALSE);
	gtk_text_view_set_cursor_visible(view, FALSE);
	gtk_text_view_set_wrap_mode(view, GTK_WRAP_WORD_CHAR);

	char * file_path = vex_term_find_ressource(file);
	if (file_path != NULL){
		SrtParser * srt = srt_parser_new();
		//TODO: need to free the SrtParser
		GtkTextBuffer * buffer = srt_parser_parse_file(srt, file_path);
		free(file_path);
		if (buffer != NULL){
			gtk_text_view_set_buffer(view, buffer);
			return GTK_WIDGET(view);
		}
	}
	GtkTextBuffer * buffer = gtk_text_buffer_new(NULL);
	GtkTextIter iter;
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
	gtk_text_view_set_buffer(view, buffer);
	gtk_text_buffer_insert(buffer, &iter, default_text, -1);
	return GTK_WIDGET(view);
}

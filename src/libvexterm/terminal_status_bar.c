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
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "terminal_status_bar.h"

G_DEFINE_TYPE (TerminalStatusBar, terminal_status_bar, GTK_TYPE_HBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint terminal_status_bar_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, terminal_status_bar_signals[SIGNAL_NAME_n], 0);

void terminal_status_bar_construct(TerminalStatusBar * tsb);

GtkWidget * terminal_status_bar_new(TerminalWidget * terminal_widget)
{
	TerminalStatusBar * terminal_status_bar = g_object_new(LIBVEX_TYPE_TERMINAL_STATUS_BAR, NULL);

	terminal_status_bar -> terminal_widget = terminal_widget;

	terminal_status_bar_construct(terminal_status_bar);

	return GTK_WIDGET(terminal_status_bar);
}

static void terminal_status_bar_class_init(TerminalStatusBarClass *class)
{
        /*terminal_status_bar_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalStatusBarClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void terminal_status_bar_init(TerminalStatusBar *terminal_status_bar)
{
}

static void terminal_status_bar_screen_changed(TerminalWidget * terminal_widget, int id, TerminalStatusBar * tsb);
static void terminal_status_bar_keypad_changed(TerminalWidget * terminal_widget, int id, TerminalStatusBar * tsb);
static void terminal_status_bar_size_changed(TerminalWidget * terminal_widget, int width, int height, TerminalStatusBar * tsb);

void terminal_status_bar_construct(TerminalStatusBar * tsb)
{
	tsb -> screen_label = gtk_label_new("Normal");
	tsb -> keypad_label = gtk_label_new("");
	tsb -> size_label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(tsb), tsb -> screen_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tsb), tsb -> size_label, FALSE, FALSE, 5);

	g_signal_connect(
		G_OBJECT(tsb -> terminal_widget), "set-screen",
		G_CALLBACK(terminal_status_bar_screen_changed), tsb);
	g_signal_connect(
		G_OBJECT(tsb -> terminal_widget), "set-keypad",
		G_CALLBACK(terminal_status_bar_keypad_changed), tsb);
	g_signal_connect(
		G_OBJECT(tsb -> terminal_widget), "set-size",
		G_CALLBACK(terminal_status_bar_size_changed), tsb);

	/* retrieve current settings and update the status-bar accordingly,
	 * since the status-bar might have been added on a terminal_widget
	 * that has already changed its state */
	int id = terminal_widget_get_current_screen(tsb -> terminal_widget);
	int width, height;
	terminal_widget_get_size(tsb -> terminal_widget, &width, &height);

	terminal_status_bar_screen_changed(tsb -> terminal_widget, id, tsb);
	terminal_status_bar_size_changed(tsb -> terminal_widget, width, height, tsb);
}

static void terminal_status_bar_screen_changed(TerminalWidget * terminal_widget, int id, TerminalStatusBar * tsb)
{
	gtk_label_set_text(GTK_LABEL(tsb -> screen_label), id == 0 ?
		 "Screen: Normal" : "Screen: Alternate");
}

static void terminal_status_bar_keypad_changed(TerminalWidget * terminal_widget, int id, TerminalStatusBar * tsb)
{
	//gtk_label_set_text(GTK_LABEL(tsb -> keypad_label), id == 0 ? "Normal" : "Alternate");
}

static void terminal_status_bar_size_changed(TerminalWidget * terminal_widget, int width, int height, TerminalStatusBar * tsb)
{
	char * text = g_strdup_printf("Size: %d x %d", width, height);
	gtk_label_set_text(GTK_LABEL(tsb -> size_label), text);
	g_free(text);
}

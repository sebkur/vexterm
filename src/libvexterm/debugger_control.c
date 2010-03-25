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

#include "debugger_control.h"

G_DEFINE_TYPE (DebuggerControl, debugger_control, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint debugger_control_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, debugger_control_signals[SIGNAL_NAME_n], 0);

static void dc_slow_toggle(GtkWidget * widget, DebuggerControl * dc);
static void dc_printing_toggle(GtkWidget * widget, DebuggerControl * dc);

GtkWidget * debugger_control_new(TerminalDebugger * debugger)
{
	DebuggerControl * debugger_control = g_object_new(LIBVEX_TYPE_DEBUGGER_CONTROL, NULL);
	debugger_control -> debugger = debugger;

	GtkWidget * box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(debugger_control), box, FALSE, FALSE, 0);

	GtkWidget * button1 = gtk_toggle_button_new_with_label("Slow");
	GtkWidget * button2 = gtk_toggle_button_new_with_label("Printing");
	gtk_box_pack_start(GTK_BOX(box), button1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), button2, FALSE, FALSE, 0);

	g_signal_connect(
		G_OBJECT(button1), "toggled",
		G_CALLBACK(dc_slow_toggle), debugger_control);

	g_signal_connect(
		G_OBJECT(button2), "toggled",
		G_CALLBACK(dc_printing_toggle), debugger_control);

	return GTK_WIDGET(debugger_control);
}

static void debugger_control_class_init(DebuggerControlClass *class)
{
        /*debugger_control_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (DebuggerControlClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void debugger_control_init(DebuggerControl *debugger_control)
{
}

static void dc_slow_toggle(GtkWidget * widget, DebuggerControl * dc)
{
	GtkToggleButton * tb = GTK_TOGGLE_BUTTON(widget);
	gboolean pressed = gtk_toggle_button_get_active(tb);
	terminal_debugger_set_slow(dc -> debugger, pressed);
}

static void dc_printing_toggle(GtkWidget * widget, DebuggerControl * dc)
{
	GtkToggleButton * tb = GTK_TOGGLE_BUTTON(widget);
	gboolean pressed = gtk_toggle_button_get_active(tb);
	terminal_debugger_set_printing(dc -> debugger, pressed);
}

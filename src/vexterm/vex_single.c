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

#include "vex_single.h"
#include "config/vex_colour_scheme.h"

#define DEFAULT_FONT "Monospace"
#define DEFAULT_FONT_SIZE 12

G_DEFINE_TYPE (VexSingle, vex_single, GTK_TYPE_VBOX);

enum
{
        EXITED,
        LAST_SIGNAL
};

static guint vex_single_signals[LAST_SIGNAL] = { 0 };

void vex_single_constructor(VexSingle * vex_single, char * pwd, VexProfile * profile);
void vex_single_terminal_config_setup_with_profile(VexSingle * vex_single, TerminalConfig * tc, VexProfile * profile);
void vex_single_grab_focus(GtkWidget * widget);
void vex_single_exit_cb(Terminal * terminal, VexSingle * vex_single);

GtkWidget * vex_single_new(char * pwd, VexProfile * profile)
{
	VexSingle * vex_single = g_object_new(VEX_TYPE_VEX_SINGLE, NULL);

	vex_single_constructor(vex_single, pwd, profile);
	GTK_OBJECT_SET_FLAGS(vex_single, GTK_CAN_FOCUS);

	return GTK_WIDGET(vex_single);
}

static void vex_single_class_init(VexSingleClass *class)
{
	GtkWidgetClass * widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> grab_focus = vex_single_grab_focus;

        vex_single_signals[EXITED] = g_signal_new(
                "exited",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexSingleClass, exited),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void vex_single_init(VexSingle *vex_single)
{
}

void vex_single_focus(GtkWidget * terminal_widget, VexSingle * vex_single)
{
	gboolean ret;
	g_signal_emit_by_name(G_OBJECT(vex_single), "focus-in-event", NULL, &ret);
}

void vex_single_set_profile(VexSingle * vex_single, VexProfile * profile)
{
	printf("VEX_SINGLE: set_profile\n");
	vex_single -> profile = profile;
	vex_single_terminal_config_setup_with_profile(vex_single, vex_single -> tc, profile);
}

VexProfile * vex_single_get_profile(VexSingle * vex_single)
{
	return vex_single -> profile;
}

static void vex_single_profiles_font_changed_cb(VexProfile * profile, VexSingle * vex_single)
{
	char * font_name = vex_profile_get_font_name(profile);
	int font_size = vex_profile_get_font_size(profile);
	terminal_config_set_font(vex_single -> tc, font_name, font_size);
}

void vex_single_colour_scheme_changed(VexColourScheme * vcs, int index, TerminalConfig * tc)
{
	printf("colour %d changed\n", index);
	TerminalColourPalette * palette = vex_colour_scheme_convert_to_terminal_palette(vcs);
	terminal_config_set_colour_palette(tc, palette);
}

static void vex_single_profiles_colour_scheme_changed_cb(VexProfile * profile, VexSingle * vex_single)
{
	VexColourScheme * vcs = vex_profile_get_active_colour_scheme(profile);
	if (vex_single -> colour_scheme != NULL){
		g_signal_handlers_disconnect_by_func(G_OBJECT(vex_single -> colour_scheme), 
			vex_single_colour_scheme_changed, vex_single -> tc);
	}
	if (vcs != NULL){
		g_signal_connect(
			G_OBJECT(vcs), "changed-colour",
			G_CALLBACK(vex_single_colour_scheme_changed), vex_single -> tc);
	}
	vex_single -> colour_scheme = vcs;
	
	TerminalColourPalette * palette;
	if (vcs != NULL){
		palette = vex_colour_scheme_convert_to_terminal_palette(vcs);
	}else{
		palette = terminal_colour_palette_new_default();
	}
	terminal_config_set_colour_palette(vex_single -> tc, palette);
}

void vex_single_constructor(VexSingle * vex_single, char * pwd, VexProfile * profile)
{
	vex_single -> profile = profile;
	vex_single -> terminal = terminal_new(pwd);

	vex_single -> tc = terminal_config_new();
	vex_single_terminal_config_setup_with_profile(vex_single, vex_single -> tc, profile);

	vex_single -> status_bar = NULL;
	vex_single -> show_status_bar = FALSE;

	g_signal_connect(
		G_OBJECT(vex_single -> terminal), "pty-exited",
		G_CALLBACK(vex_single_exit_cb), vex_single);

	vex_single ->  terminal_widget = terminal_widget_new_with_config(vex_single -> tc);
//	TerminalDebugger * terminal_debugger = terminal_debugger_new();

	vex_single -> show_scrolling_region = terminal_widget_get_show_scrolling_region(vex_single -> terminal_widget);

	terminal_set_handler(vex_single -> terminal, LIBVEX_TERMINAL_HANDLER(vex_single -> terminal_widget));

//	terminal_set_handler(terminal, terminal_debugger);
//	terminal_debugger_set_handler(terminal_debugger, terminal_widget);

	terminal_start(vex_single -> terminal);

//	GtkWidget * dc = debugger_control_new(terminal_debugger);

//	gtk_box_pack_start(GTK_BOX(vex_single), GTK_WIDGET(dc), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vex_single), GTK_WIDGET(vex_single -> terminal_widget), TRUE, TRUE, 0);

	g_signal_connect(
		G_OBJECT(vex_single -> terminal_widget), "grab-focus",
		G_CALLBACK(vex_single_focus), vex_single);
}

void vex_single_terminal_config_setup_with_profile(VexSingle * vex_single, TerminalConfig * tc, VexProfile * profile)
{
	if (profile != NULL){
		terminal_config_set_font_name(tc, vex_profile_get_font_name(profile));
		terminal_config_set_font_size(tc, vex_profile_get_font_size(profile));
		VexColourScheme * vcs = vex_profile_get_active_colour_scheme(profile);
		if (vcs != NULL){
			TerminalColourPalette * palette = vex_colour_scheme_convert_to_terminal_palette(vcs);
			terminal_config_set_colour_palette(tc, palette);

			g_signal_connect(
				G_OBJECT(vcs), "changed-colour",
				G_CALLBACK(vex_single_colour_scheme_changed), tc);
		}else{
			TerminalColourPalette * palette = terminal_colour_palette_new_default();
			terminal_config_set_colour_palette(tc, palette);
		}
		vex_single -> colour_scheme = vcs;

		g_signal_connect(G_OBJECT(profile), "changed-font", 
			G_CALLBACK(vex_single_profiles_font_changed_cb), vex_single);
		g_signal_connect(G_OBJECT(profile), "changed-colour-scheme", 
			G_CALLBACK(vex_single_profiles_colour_scheme_changed_cb), vex_single);
	}else{
		terminal_config_set_font_name(tc, DEFAULT_FONT);
		terminal_config_set_font_size(tc, DEFAULT_FONT_SIZE);
		TerminalColourPalette * palette = terminal_colour_palette_new_default();
		terminal_config_set_colour_palette(tc, palette);
	}
}

char * vex_single_get_pwd(VexSingle * vex_single)
{
	char * pwd = terminal_get_pwd(vex_single -> terminal);
	return pwd;
}

/* _private_ 
 * show / hide the status bar */
void vex_single_show_status_bar(VexSingle * vex_single, gboolean show)
{
	if (show){ // we got to show it now
		if (vex_single -> status_bar == NULL){ // first time
			vex_single -> status_bar = 
				LIBVEX_TERMINAL_STATUS_BAR(terminal_status_bar_new(vex_single -> terminal_widget));
			gtk_box_pack_start(GTK_BOX(vex_single), 
				GTK_WIDGET(vex_single -> status_bar), FALSE, FALSE, 0);
		}
		gtk_widget_show_all(GTK_WIDGET(vex_single -> status_bar));
	}else{ // we got to hide it
		if (vex_single -> status_bar != NULL)
			gtk_widget_hide_all(GTK_WIDGET(vex_single -> status_bar));
	}
	vex_single -> show_status_bar = show;
}

/* _private_ 
 * show / hide the scrolling region */
void vex_single_show_scrolling_region(VexSingle * vex_single, gboolean show)
{
	terminal_widget_set_show_scrolling_region(vex_single -> terminal_widget, show);
	vex_single -> show_scrolling_region = show;
}

/* Getter / Setter for the debug status-bar */
gboolean vex_single_get_show_status_bar(VexSingle * vex_single)
{
	return vex_single -> show_status_bar;
}

void vex_single_set_show_status_bar(VexSingle * vex_single, gboolean show)
{
	if (vex_single -> show_status_bar != show){
		vex_single_show_status_bar(vex_single, show);
	}
}

/* Getter / Setter for the debug scrolling region */
gboolean vex_single_get_show_scrolling_region(VexSingle * vex_single)
{
	return vex_single -> show_scrolling_region;
}

void vex_single_set_show_scrolling_region(VexSingle * vex_single, gboolean show)
{
	if (vex_single -> show_scrolling_region != show){
		vex_single_show_scrolling_region(vex_single, show);
	}
}

void vex_single_grab_focus(GtkWidget * widget)
{
	VexSingle * vex_single = VEX_VEX_SINGLE(widget);
	gtk_widget_grab_focus(GTK_WIDGET(vex_single -> terminal_widget));
}

void vex_single_exit_cb(Terminal * terminal, VexSingle * vex_single)
{
	g_signal_emit (vex_single, vex_single_signals[EXITED], 0);
}

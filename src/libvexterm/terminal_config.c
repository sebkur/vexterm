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

#define TERMINAL_CONFIG_DEFAULT_FONT_NAME "Monospace"
#define TERMINAL_CONFIG_DEFAULT_FONT_SIZE 10

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "terminal_config.h"

G_DEFINE_TYPE (TerminalConfig, terminal_config, G_TYPE_OBJECT);

enum
{
        CHANGED_FONT,
        CHANGED_FONT_NAME,
        CHANGED_FONT_SIZE,
        CHANGED_COLOUR_PALETTE,
        LAST_SIGNAL
};

static guint terminal_config_signals[LAST_SIGNAL] = { 0 };

TerminalConfig * terminal_config_new()
{
	TerminalConfig * tc = g_object_new(LIBVEX_TYPE_TERMINAL_CONFIG, NULL);
	return tc;
}

static void terminal_config_class_init(TerminalConfigClass *class)
{
        terminal_config_signals[CHANGED_FONT] = g_signal_new(
                "changed-font",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalConfigClass, changed_font),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        terminal_config_signals[CHANGED_FONT_NAME] = g_signal_new(
                "changed-font-name",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalConfigClass, changed_font_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        terminal_config_signals[CHANGED_FONT_SIZE] = g_signal_new(
                "changed-font-size",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalConfigClass, changed_font_size),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        terminal_config_signals[CHANGED_COLOUR_PALETTE] = g_signal_new(
                "changed-colour-palette",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalConfigClass, changed_colour_palette),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void terminal_config_init(TerminalConfig * tc)
{
	terminal_config_set_font_name(tc, TERMINAL_CONFIG_DEFAULT_FONT_NAME);
	terminal_config_set_font_size(tc, TERMINAL_CONFIG_DEFAULT_FONT_SIZE);
	terminal_config_set_colour_palette(tc, terminal_colour_palette_new_default());
}

/* Setters */

void terminal_config_set_font_name(TerminalConfig * tc, char * font_name)
{
	if(tc -> font_name != NULL){
		g_free(tc -> font_name);
	}
	tc -> font_name = g_strdup(font_name);
	g_signal_emit (tc, terminal_config_signals[CHANGED_FONT_NAME], 0);
}

void terminal_config_set_font_size(TerminalConfig * tc, int font_size)
{
	tc -> font_size = font_size;
	g_signal_emit (tc, terminal_config_signals[CHANGED_FONT_SIZE], 0);
}

void terminal_config_set_font(TerminalConfig * tc, char * font_name, int font_size)
{
	if(tc -> font_name != NULL){
		g_free(tc -> font_name);
	}
	tc -> font_name = g_strdup(font_name);
	tc -> font_size = font_size;
	g_signal_emit (tc, terminal_config_signals[CHANGED_FONT], 0);
}

void terminal_config_set_colour_palette(TerminalConfig * tc, TerminalColourPalette * palette)
{
	tc -> colour_palette = palette;
	g_signal_emit (tc, terminal_config_signals[CHANGED_COLOUR_PALETTE], 0);
}

/* Getters */

char * terminal_config_get_font_name(TerminalConfig * tc)
{
	return tc -> font_name;
}

int terminal_config_get_font_size(TerminalConfig * tc)
{
	return tc -> font_size;
}

TerminalColourPalette * terminal_config_get_colour_palette(TerminalConfig * tc)
{
	return tc -> colour_palette;
}

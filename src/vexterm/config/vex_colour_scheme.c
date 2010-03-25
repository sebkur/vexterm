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

#include "vex_colour_scheme.h"

G_DEFINE_TYPE (VexColourScheme, vex_colour_scheme, G_TYPE_OBJECT);

enum
{
        CHANGED_COLOUR,
        CHANGED_COLOURS,
        LAST_SIGNAL
};

static guint vex_colour_scheme_signals[LAST_SIGNAL] = { 0 };

VexColourScheme * vex_colour_scheme_new()
{
	VexColourScheme * vex_colour_scheme = g_object_new(VEX_TYPE_VEX_COLOUR_SCHEME, NULL);
	return vex_colour_scheme;
}

VexColourScheme * vex_colour_scheme_new_defaults()
{
	VexColourScheme * vex_colour_scheme = g_object_new(VEX_TYPE_VEX_COLOUR_SCHEME, NULL);
	VexColour fg = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
	VexColour bg = {0x0000, 0x0000, 0x0000, 0xFFFF};
	vex_colour_scheme_set_colour(vex_colour_scheme, 0, fg);
	vex_colour_scheme_set_colour(vex_colour_scheme, 1, bg);
	int i;
	for (i = 2; i < 18; i++){
		vex_colour_scheme_set_colour(vex_colour_scheme, i, fg);
	}
	return vex_colour_scheme;
}

static void vex_colour_scheme_class_init(VexColourSchemeClass *class)
{
        vex_colour_scheme_signals[CHANGED_COLOUR] = g_signal_new(
                "changed-colour",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexColourSchemeClass, changed_colour),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        vex_colour_scheme_signals[CHANGED_COLOURS] = g_signal_new(
                "changed-colours",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexColourSchemeClass, changed_colours),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void vex_colour_scheme_init(VexColourScheme *vex_colour_scheme)
{
}

TerminalColour vex_colour_convert_to_terminal_colour(VexColour * vex_colour)
{
	TerminalColour tc;
	tc.r = vex_colour -> r;
	tc.g = vex_colour -> g;
	tc.b = vex_colour -> b;
	tc.a = vex_colour -> a;
	return tc;
}

TerminalColourPalette * vex_colour_scheme_convert_to_terminal_palette(VexColourScheme * vcs)
{
	TerminalColourPalette * palette = terminal_colour_palette_new();
	palette -> foreground = vex_colour_convert_to_terminal_colour(&vcs -> foreground);
	palette -> background = vex_colour_convert_to_terminal_colour(&vcs -> background);
	int c;
	for (c = 0; c < 8; c++){
		palette -> normal[c] = vex_colour_convert_to_terminal_colour(&vcs -> normal[c]);
	}
	for (c = 0; c < 8; c++){
		palette -> highlighted[c] = vex_colour_convert_to_terminal_colour(&vcs -> highlighted[c]);
	}
	return palette;
}

VexColour * vex_colour_scheme_get_colour(VexColourScheme * vex_colour_scheme, int n)
{
	switch(n){
		case 0:{
			return &vex_colour_scheme -> foreground;
		}
		case 1:{
			return &vex_colour_scheme -> background;
		}
		default:{
			if (n < 10) return &vex_colour_scheme -> normal[n-2];
			if (n < 18) return &vex_colour_scheme -> highlighted[n-10];
			return NULL;
		}
	}
}

void vex_colour_scheme_set_colour(VexColourScheme * vex_colour_scheme, int index, VexColour colour)
{
	switch(index){
		case 0:{
			vex_colour_scheme -> foreground = colour;
			break;
		}
		case 1:{
			vex_colour_scheme -> background = colour;
			break;
		}
		default:{
			if (index < 10) vex_colour_scheme -> normal[index-2] = colour;
			if (index < 18) vex_colour_scheme -> highlighted[index-10] = colour;
			break;
		}
	}
	g_signal_emit (vex_colour_scheme, vex_colour_scheme_signals[CHANGED_COLOUR], 0, index);
}

VexColourScheme * vex_colour_scheme_duplicate(VexColourScheme * vex_colour_scheme)
{
	VexColourScheme * scheme = vex_colour_scheme_new();
	int i;
	for (i = 0; i < 18; i++){
		vex_colour_scheme_set_colour(scheme, i, *vex_colour_scheme_get_colour(vex_colour_scheme, i));
	}
	return scheme;
}

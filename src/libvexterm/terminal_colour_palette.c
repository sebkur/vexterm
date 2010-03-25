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

#include "terminal_colour_palette.h"

G_DEFINE_TYPE (TerminalColourPalette, terminal_colour_palette, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint terminal_colour_palette_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, terminal_colour_palette_signals[SIGNAL_NAME_n], 0);

TerminalColourPalette * terminal_colour_palette_new()
{
	TerminalColourPalette * terminal_colour_palette = g_object_new(LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE, NULL);
	return terminal_colour_palette;
}

static void terminal_colour_palette_class_init(TerminalColourPaletteClass *class)
{
        /*terminal_colour_palette_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalColourPaletteClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void terminal_colour_palette_init(TerminalColourPalette *terminal_colour_palette)
{
}

TerminalColourPalette * terminal_colour_palette_new_default()
{
	TerminalColourPalette * tcp = g_object_new(LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE, NULL);
	int colours[][4] = {
		{0x0000, 0x0000, 0x0000, 0xFFFF}, // foreground
		{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}, // background

		{0x0000, 0x0000, 0x0000, 0xFFFF}, // black
		{0xAA00, 0x0000, 0x0000, 0xFFFF}, // red
		{0x0000, 0xAA00, 0x0000, 0xFFFF}, // green
		{0xAA00, 0x5500, 0x0000, 0xFFFF}, // brown
		{0x0000, 0x0000, 0xAA00, 0xFFFF}, // blue
		{0xAA00, 0x0000, 0xAA00, 0xFFFF}, // purple
		{0x0000, 0xAA00, 0xAA00, 0xFFFF}, // magenta
		{0xAA00, 0xAA00, 0xAA00, 0xFFFF}, // grey

		{0x5500, 0x5500, 0x5500, 0xFFFF}, 
		{0xFFFF, 0x5500, 0x5500, 0xFFFF}, 
		{0x5500, 0xFFFF, 0x5500, 0xFFFF}, 
		{0xFFFF, 0xFFFF, 0x5500, 0xFFFF}, 
		{0x5500, 0x5500, 0xFFFF, 0xFFFF}, 
		{0xFFFF, 0x5500, 0xFFFF, 0xFFFF}, 
		{0x5500, 0xFFFF, 0xFFFF, 0xFFFF}, 
		{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}, 
	};
	int c;
	for (c = 0; c <= 17; c++){
		TerminalColour * colour;
		if (c == 0){
			colour = &tcp -> foreground;
		}else if (c == 1){
			colour = &tcp -> background;
		}else if (c <= 9){
			colour = &tcp -> normal[c - 2];
		}else if (c <= 17){
			colour = &tcp -> highlighted[c - 10];
		}
		(*colour).r = colours[c][0];
		(*colour).g = colours[c][1];
		(*colour).b = colours[c][2];
		(*colour).a = colours[c][3];
	}
	return tcp;
}

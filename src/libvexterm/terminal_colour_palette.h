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

#ifndef _TERMINAL_COLOUR_PALETTE_H_
#define _TERMINAL_COLOUR_PALETTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE           (terminal_colour_palette_get_type ())
#define LIBVEX_TERMINAL_COLOUR_PALETTE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE, TerminalColourPalette))
#define LIBVEX_TERMINAL_COLOUR_PALETTE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE, TerminalColourPaletteClass))
#define LIBVEX_IS_TERMINAL_COLOUR_PALETTE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE))
#define LIBVEX_IS_TERMINAL_COLOUR_PALETTE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE))
#define LIBVEX_TERMINAL_COLOUR_PALETTE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_TERMINAL_COLOUR_PALETTE, TerminalColourPaletteClass))

typedef struct _TerminalColourPalette        TerminalColourPalette;
typedef struct _TerminalColourPaletteClass   TerminalColourPaletteClass;

typedef struct {
	int r;
	int g;
	int b;
	int a;
} TerminalColour;

struct _TerminalColourPalette
{
	GObject parent;

	TerminalColour foreground;
	TerminalColour background;
	TerminalColour normal[8];
	TerminalColour highlighted[8];
};

struct _TerminalColourPaletteClass
{
	GObjectClass parent_class;

	//void (* function_name) (TerminalColourPalette *terminal_colour_palette);
};

GType terminal_colour_palette_get_type (void) G_GNUC_CONST;

TerminalColourPalette * terminal_colour_palette_new();

TerminalColourPalette * terminal_colour_palette_new_default();

#endif /* _TERMINAL_COLOUR_PALETTE_H_ */

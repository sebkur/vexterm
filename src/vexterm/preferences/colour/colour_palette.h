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

#ifndef _COLOUR_PALETTE_H_
#define _COLOUR_PALETTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../../config/vex_colour_scheme.h"

#define VEX_TYPE_COLOUR_PALETTE           (colour_palette_get_type ())
#define VEX_COLOUR_PALETTE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_COLOUR_PALETTE, ColourPalette))
#define VEX_COLOUR_PALETTE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_COLOUR_PALETTE, ColourPaletteClass))
#define VEX_IS_COLOUR_PALETTE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_COLOUR_PALETTE))
#define VEX_IS_COLOUR_PALETTE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_COLOUR_PALETTE))
#define VEX_COLOUR_PALETTE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_COLOUR_PALETTE, ColourPaletteClass))

typedef struct _ColourPalette        ColourPalette;
typedef struct _ColourPaletteClass   ColourPaletteClass;

struct _ColourPalette
{
	GtkVBox parent;

	VexColourScheme * colour_scheme;

	GtkWidget * buttons[18];
};

struct _ColourPaletteClass
{
	GtkVBoxClass parent_class;

	void (* changed_colour) (ColourPalette *colour_palette, int index, int r, int g, int b, int a);
};

GType colour_palette_get_type (void) G_GNUC_CONST;

GtkWidget * colour_palette_new(VexColourScheme * colour_scheme);

void colour_palette_use_colour_scheme(ColourPalette * colour_palette, VexColourScheme * colour_scheme);

#endif /* _COLOUR_PALETTE_H_ */

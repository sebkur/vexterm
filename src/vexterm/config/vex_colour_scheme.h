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

#ifndef _VEX_COLOUR_SCHEME_H_
#define _VEX_COLOUR_SCHEME_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../../libvexterm/terminal_colour_palette.h"

#define VEX_TYPE_VEX_COLOUR_SCHEME           (vex_colour_scheme_get_type ())
#define VEX_VEX_COLOUR_SCHEME(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_COLOUR_SCHEME, VexColourScheme))
#define VEX_VEX_COLOUR_SCHEME_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_COLOUR_SCHEME, VexColourSchemeClass))
#define VEX_IS_VEX_COLOUR_SCHEME(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_COLOUR_SCHEME))
#define VEX_IS_VEX_COLOUR_SCHEME_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_COLOUR_SCHEME))
#define VEX_VEX_COLOUR_SCHEME_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_COLOUR_SCHEME, VexColourSchemeClass))

typedef struct _VexColourScheme        VexColourScheme;
typedef struct _VexColourSchemeClass   VexColourSchemeClass;

typedef struct {
	int r;
	int g;
	int b;
	int a;
} VexColour;

struct _VexColourScheme
{
	GObject parent;

	VexColour foreground;
	VexColour background;
	VexColour normal[8];
	VexColour highlighted[8];
};

struct _VexColourSchemeClass
{
	GObjectClass parent_class;

	void (* changed_colour) (VexColourScheme *vex_colour_scheme, int index);
	void (* changed_colours) (VexColourScheme *vex_colour_scheme);
};

GType vex_colour_scheme_get_type (void) G_GNUC_CONST;

VexColourScheme * vex_colour_scheme_new();
VexColourScheme * vex_colour_scheme_new_defaults();

TerminalColourPalette * vex_colour_scheme_convert_to_terminal_palette(VexColourScheme * vex_colour_scheme);

VexColour * vex_colour_scheme_get_colour(VexColourScheme * vex_colour_scheme, int n);

void vex_colour_scheme_set_colour(VexColourScheme * vex_colour_scheme, int index, VexColour colour);

VexColourScheme * vex_colour_scheme_duplicate(VexColourScheme * vex_colour_scheme);

#endif /* _VEX_COLOUR_SCHEME_H_ */

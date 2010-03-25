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

#define VEX_TYPE_COLOUR_AREA           (colour_area_get_type ())
#define VEX_COLOUR_AREA(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_COLOUR_AREA, ColourArea))
#define VEX_COLOUR_AREA_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_COLOUR_AREA, ColourAreaClass))
#define VEX_IS_COLOUR_AREA(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_COLOUR_AREA))
#define VEX_IS_COLOUR_AREA_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_COLOUR_AREA))
#define VEX_COLOUR_AREA_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_COLOUR_AREA, ColourAreaClass))

typedef struct _ColourArea        ColourArea;
typedef struct _ColourAreaClass   ColourAreaClass;

struct _ColourArea
{
	GtkDrawingArea parent;
	
	double r;
	double g;
	double b;
	double a;
};

struct _ColourAreaClass
{
	GtkDrawingAreaClass parent_class;

	//void (* function_name) (ColourArea *colour_area);
};

GType colour_area_get_type (void) G_GNUC_CONST;

GtkWidget * colour_area_new();

void colour_area_set_colour(ColourArea * colour_area, double r, double g, double b, double a);

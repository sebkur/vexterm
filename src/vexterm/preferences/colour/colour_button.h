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

#ifndef _COLOUR_BUTTON_H_
#define _COLOUR_BUTTON_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "colour_area.h"

#define VEX_TYPE_COLOUR_BUTTON           (colour_button_get_type ())
#define VEX_COLOUR_BUTTON(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_COLOUR_BUTTON, ColourButton))
#define VEX_COLOUR_BUTTON_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_COLOUR_BUTTON, ColourButtonClass))
#define VEX_IS_COLOUR_BUTTON(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_COLOUR_BUTTON))
#define VEX_IS_COLOUR_BUTTON_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_COLOUR_BUTTON))
#define VEX_COLOUR_BUTTON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_COLOUR_BUTTON, ColourButtonClass))

typedef struct _ColourButton        ColourButton;
typedef struct _ColourButtonClass   ColourButtonClass;

struct _ColourButton
{
	GtkButton parent;

	double r;
	double g;
	double b;
	double a;

	GtkWidget * colour_area;
};

struct _ColourButtonClass
{
	GtkButtonClass parent_class;

	//void (* function_name) (ColourButton *colour_button);
};

GType colour_button_get_type (void) G_GNUC_CONST;

GtkWidget * colour_button_new();
GtkWidget * colour_button_new_with_rgba(double r, double g, double b, double a);

void colour_button_set_colour(ColourButton * colour_button, double r, double g, double b, double a);

#endif /* _COLOUR_BUTTON_H_ */

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

#include "colour_button.h"

G_DEFINE_TYPE (ColourButton, colour_button, GTK_TYPE_BUTTON);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint colour_button_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, colour_button_signals[SIGNAL_NAME_n], 0);

void colour_button_constructor(ColourButton * colour_button);

GtkWidget * colour_button_new_with_rgba(double r, double g, double b, double a)
{
	ColourButton * colour_button = g_object_new(VEX_TYPE_COLOUR_BUTTON, NULL);

	colour_button -> r = r;
	colour_button -> g = g;
	colour_button -> b = b;
	colour_button -> a = a;

	colour_button_constructor(colour_button);

	return GTK_WIDGET(colour_button);
}

GtkWidget * colour_button_new()
{
	return colour_button_new_with_rgba(0, 0, 0, 0);
}

static void colour_button_class_init(ColourButtonClass *class)
{
        /*colour_button_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColourButtonClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void colour_button_init(ColourButton *colour_button)
{
}

void colour_button_constructor(ColourButton * colour_button)
{
	colour_button -> colour_area = colour_area_new();
	colour_area_set_colour(VEX_COLOUR_AREA(colour_button -> colour_area), 
		colour_button -> r, colour_button -> g,
		colour_button -> b, colour_button -> a);
	gtk_container_add(GTK_CONTAINER(colour_button), colour_button -> colour_area);
}

void colour_button_set_colour(ColourButton * colour_button, double r, double g, double b, double a)
{
	colour_button -> r = r;
	colour_button -> g = g;
	colour_button -> b = b;
	colour_button -> a = a;
	colour_area_set_colour(VEX_COLOUR_AREA(colour_button -> colour_area), r, g, b, a);
}

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
#include <cairo/cairo.h>

#include "colour_area.h"

/****************************************************************************************************
* this is a simple button that displays a colour
****************************************************************************************************/
G_DEFINE_TYPE (ColourArea, colour_area, GTK_TYPE_DRAWING_AREA);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint colour_area_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, colour_area_signals[SIGNAL_NAME_n], 0);

static gboolean expose_cb (GtkWidget * area, GdkEventExpose *event);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * colour_area_new()
{
	ColourArea * colour_area = g_object_new(VEX_TYPE_COLOUR_AREA, NULL);
	colour_area -> r = 0.0; 
	colour_area -> g = 0.0; 
	colour_area -> b = 1.0; 
	colour_area -> a = 0.8; 
	g_signal_connect (colour_area, "expose-event", G_CALLBACK (expose_cb), NULL);
	return GTK_WIDGET(colour_area);
}

static void colour_area_class_init(ColourAreaClass *class)
{
        /*colour_area_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColourAreaClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void colour_area_init(ColourArea *colour_area)
{
}

/****************************************************************************************************
* set the colour the button shall show
****************************************************************************************************/
void colour_area_set_colour(ColourArea * colour_area, double r, double g, double b, double a)
{
	colour_area -> r = r;
	colour_area -> g = g;
	colour_area -> b = b;
	colour_area -> a = a;
	gtk_widget_queue_draw(GTK_WIDGET(colour_area));
}

/****************************************************************************************************
* paint the widget
* derived from gtk_color_sel.c
****************************************************************************************************/
static gboolean expose_cb (GtkWidget * area, GdkEventExpose *event)
{
	gboolean sensitive = GTK_WIDGET_IS_SENSITIVE(area);
	ColourArea * cb = VEX_COLOUR_AREA(area);
	gint x, y, wid, heig, goff;
	cairo_t *cr;
	goff = 0;
	int CHECK_SIZE = 5;
	cr = gdk_cairo_create (area->window);
	wid = area->allocation.width;
	heig = area->allocation.height;

	cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
	cairo_rectangle (cr, 0, 0, wid, heig);
	cairo_fill (cr);

	cairo_set_source_rgb (cr, 0.75, 0.75, 0.75);
	for (x = goff & -CHECK_SIZE; x < goff + wid; x += CHECK_SIZE)
		for (y = 0; y < heig; y += CHECK_SIZE)
			if ((x / CHECK_SIZE + y / CHECK_SIZE) % 2 == 0)
				cairo_rectangle (cr, x - goff, y, CHECK_SIZE, CHECK_SIZE);
	cairo_fill (cr);

	cairo_set_source_rgba (cr, cb -> r, cb -> g, cb -> b, cb -> a);
	cairo_rectangle (cr, 0, 0, wid, heig);
	cairo_fill (cr);
	if (!sensitive){
		cairo_set_source_rgba (cr, 0.5, 0.5, 0.5, 0.5);
		cairo_rectangle (cr, 0, 0, wid, heig);
		cairo_fill (cr);
	}
	cairo_destroy (cr);

	return FALSE;
}

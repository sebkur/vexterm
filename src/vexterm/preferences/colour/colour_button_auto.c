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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "colour_button_auto.h"
#include "colour_button.h"
#include "marshal.h"
#include "../../../helpers/customio.h"

/****************************************************************************************************
* this is a coloured button that can be used to let the user select a colour.
* the buttons appears in the preselected colour. when the user clicks the button, a colour-chooser-
* dialog will appear.
****************************************************************************************************/
G_DEFINE_TYPE (ColourButtonAuto, colour_button_auto, GTK_TYPE_HBOX);

enum
{
        CHANGED_COLOUR,
        LAST_SIGNAL
};

static guint colour_button_auto_signals[LAST_SIGNAL] = { 0 };

static gboolean colour_button_auto_button_cb(GtkWidget * button, ColourButtonAuto * colour_button_auto);

/****************************************************************************************************
* constructor
****************************************************************************************************/
GtkWidget * colour_button_auto_new()
{
	ColourButtonAuto * colour_button_auto = g_object_new(VEX_TYPE_COLOUR_BUTTON_AUTO, NULL);
	colour_button_auto -> button = colour_button_new();
	gtk_box_pack_start(GTK_BOX(colour_button_auto), colour_button_auto -> button, TRUE, TRUE, 0);
	g_signal_connect(
		G_OBJECT(colour_button_auto -> button), "clicked", 
		G_CALLBACK(colour_button_auto_button_cb), colour_button_auto);
	return GTK_WIDGET(colour_button_auto);
}

GtkWidget * colour_button_auto_new_with_rgba(double r, double g, double b, double a)
{
	ColourButtonAuto * colour_button_auto = VEX_COLOUR_BUTTON_AUTO(colour_button_auto_new());
	colour_button_set_colour(VEX_COLOUR_BUTTON(colour_button_auto -> button), r, g, b, a);
	return GTK_WIDGET(colour_button_auto);
}

static void colour_button_auto_class_init(ColourButtonAutoClass *class)
{
        colour_button_auto_signals[CHANGED_COLOUR] = g_signal_new(
                "changed-colour",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColourButtonAutoClass, changed_colour),
                NULL, NULL,
                g_cclosure_user_marshal_VOID__INT_INT_INT_INT,
                G_TYPE_NONE, 4, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
}

static void colour_button_auto_init(ColourButtonAuto *colour_button_auto)
{
}

/****************************************************************************************************
* get the current value
****************************************************************************************************/
void colour_button_auto_get_current_value(ColourButtonAuto * colour_button_auto, double *r, double *g, double *b, double *a)
{
	*r = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> r;
	*g = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> g;
	*b = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> b;
	*a = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> a;
}

/****************************************************************************************************
* set the current value externaly
****************************************************************************************************/
void colour_button_auto_set_current_value(ColourButtonAuto * colour_button_auto, double r, double g, double b, double a)
{
	colour_button_set_colour(
		VEX_COLOUR_BUTTON(colour_button_auto -> button), r, g, b, a);
}

/****************************************************************************************************
* show the dialog to select a colour for the button
****************************************************************************************************/
static gboolean colour_button_auto_button_cb(GtkWidget * button, ColourButtonAuto * colour_button_auto)
{
	GtkWidget * dialog = gtk_color_selection_dialog_new("Select colour");
	GtkColorSelection * sel = GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel);
	gtk_color_selection_set_has_opacity_control(sel, TRUE);

	GdkColor color;
	color.red = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> r * 65535;
	color.green = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> g * 65535;
	color.blue = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> b * 65535;
	guint16 a = VEX_COLOUR_BUTTON(colour_button_auto -> button) -> a * 65535;
	gtk_color_selection_set_current_color(sel, &color);
	gtk_color_selection_set_current_alpha(sel, a);

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response != GTK_RESPONSE_OK){
		gtk_widget_destroy(dialog);
		return FALSE;
	}
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel), &color);
	guint16 opacity = gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog) -> colorsel));
	gtk_widget_destroy(dialog);
	double red = ((double)color.red) / 65535;
	double green = ((double)color.green) / 65535;
	double blue = ((double)color.blue) / 65535;
	double alpha = ((double)opacity) / 65535;
	colour_button_set_colour (VEX_COLOUR_BUTTON(colour_button_auto -> button), red, green, blue, alpha);
	g_signal_emit (colour_button_auto, colour_button_auto_signals[CHANGED_COLOUR], 0, 
		color.red, color.green, color.blue, opacity);
	return FALSE;
}


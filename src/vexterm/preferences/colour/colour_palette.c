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

#include "colour_palette.h"
#include "colour_button_auto.h"
#include "marshal.h"

G_DEFINE_TYPE (ColourPalette, colour_palette, GTK_TYPE_VBOX);

enum
{
        CHANGED_COLOUR,
        LAST_SIGNAL
};

static guint colour_palette_signals[LAST_SIGNAL] = { 0 };

void colour_palette_constructor(ColourPalette * colour_palette);

GtkWidget * colour_palette_new(VexColourScheme * colour_scheme)
{
	ColourPalette * colour_palette = g_object_new(VEX_TYPE_COLOUR_PALETTE, NULL);

	colour_palette -> colour_scheme = colour_scheme;

	colour_palette_constructor(colour_palette);

	return GTK_WIDGET(colour_palette);
}

static void colour_palette_class_init(ColourPaletteClass *class)
{
        colour_palette_signals[CHANGED_COLOUR] = g_signal_new(
                "changed-colour",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColourPaletteClass, changed_colour),
                NULL, NULL,
                g_cclosure_user_marshal_VOID__INT_INT_INT_INT_INT,
                G_TYPE_NONE, 5, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
}

static void colour_palette_init(ColourPalette *colour_palette)
{
}

void colour_palette_constructor(ColourPalette * colour_palette)
{
	GtkWidget * box1 = gtk_hbox_new(FALSE, 0);
	GtkWidget * box2 = gtk_hbox_new(FALSE, 0);
	int b;
	for (b = 0; b < 18; b++){
		GtkWidget * button = colour_button_auto_new();
		colour_palette -> buttons[b] = button;
		gtk_widget_set_size_request(button, 40, 30);
		GtkWidget * box = (b < 10 && b != 1) ? box1 : box2;
		gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
		if (b == 0) gtk_box_pack_start(GTK_BOX(box1), gtk_vseparator_new(), FALSE, FALSE, 3);
		if (b == 9) gtk_box_pack_start(GTK_BOX(box2), gtk_vseparator_new(), FALSE, FALSE, 3);
	}
	gtk_box_pack_start(GTK_BOX(colour_palette), box1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(colour_palette), box2, FALSE, FALSE, 0);

	colour_palette_use_colour_scheme(colour_palette, NULL);
}

void colour_palette_colour_changed_cb(ColourButtonAuto * button, int r, int g, int b, int a, gpointer data)
{
	gpointer p;
	g_object_get(G_OBJECT(button), "user-data", &p, NULL);
	int n = GPOINTER_TO_INT(p);
	printf("changed colour number: %d\n", n);
	g_signal_emit (data, colour_palette_signals[CHANGED_COLOUR], 0, n, r, g, b, a);
}

void colour_palette_use_colour_scheme(ColourPalette * colour_palette, VexColourScheme * colour_scheme)
{
	colour_palette -> colour_scheme = colour_scheme;
	if (colour_scheme != NULL){
		int b;
		for (b = 0; b < 18; b++){
			VexColour * colour = vex_colour_scheme_get_colour(colour_palette -> colour_scheme, b);
			GtkWidget * button = colour_palette -> buttons[b];
			colour_button_auto_set_current_value(
				VEX_COLOUR_BUTTON_AUTO(button),
				colour -> r / (double)0xFFFF, 
				colour -> g / (double)0xFFFF, 
				colour -> b / (double)0xFFFF, 
				colour -> a / (double)0xFFFF);
			gtk_widget_set_sensitive(button, TRUE);
			g_object_set(G_OBJECT(button), "user-data", GINT_TO_POINTER(b), NULL);
			g_signal_connect(
				G_OBJECT(button), "changed-colour",
				G_CALLBACK(colour_palette_colour_changed_cb), colour_palette);
		}
	}else{
		int b;
		for (b = 0; b < 18; b++){
			GtkWidget * button = colour_palette -> buttons[b];
			colour_button_auto_set_current_value(
				VEX_COLOUR_BUTTON_AUTO(button),
				0.0, 0.0, 0.0, 0.0);
			gtk_widget_set_sensitive(button, FALSE);
		}
	}
}

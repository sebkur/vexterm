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

#define VEX_TYPE_COLOUR_BUTTON_AUTO           (colour_button_auto_get_type ())
#define VEX_COLOUR_BUTTON_AUTO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_COLOUR_BUTTON_AUTO, ColourButtonAuto))
#define VEX_COLOUR_BUTTON_AUTO_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_COLOUR_BUTTON_AUTO, ColourButtonAutoClass))
#define VEX_IS_COLOUR_BUTTON_AUTO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_COLOUR_BUTTON_AUTO))
#define VEX_IS_COLOUR_BUTTON_AUTO_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_COLOUR_BUTTON_AUTO))
#define VEX_COLOUR_BUTTON_AUTO_GET_CLASS      (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_COLOUR_BUTTON_AUTO, ColourButtonAutoClass))

typedef struct _ColourButtonAuto        ColourButtonAuto;
typedef struct _ColourButtonAutoClass   ColourButtonAutoClass;

struct _ColourButtonAuto
{
	GtkHBox parent;

	GtkWidget * button;
};

struct _ColourButtonAutoClass
{
	GtkHBoxClass parent_class;

	void (* changed_colour) (ColourButtonAuto *colour_button_auto, int r, int g, int b, int a);
};

GType colour_button_auto_get_type (void) G_GNUC_CONST;

GtkWidget * colour_button_auto_new();
GtkWidget * colour_button_auto_new_with_rgba(double r, double g, double b, double a);

void colour_button_auto_get_current_value(ColourButtonAuto * colour_button_auto, double *r, double *g, double *b, double *a);

void colour_button_auto_set_current_value(ColourButtonAuto * colour_button_auto, double r, double g, double b, double a);

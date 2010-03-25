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

#include "about_logo.h"
#include "../vex_paths.h"
#include "../../helpers/util.h"

G_DEFINE_TYPE (AboutLogo, about_logo, GTK_TYPE_DRAWING_AREA);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint about_logo_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, about_logo_signals[SIGNAL_NAME_n], 0);

void about_logo_constructor(AboutLogo * about_logo);
static gboolean about_logo_expose(GtkWidget * widget, GdkEventExpose * event);
static void about_logo_finalize(GObject * object);

GtkWidget * about_logo_new()
{
	AboutLogo * about_logo = g_object_new(VEX_TYPE_ABOUT_LOGO, NULL);
	about_logo_constructor(about_logo);
	return GTK_WIDGET(about_logo);
}

static void about_logo_class_init(AboutLogoClass *class)
{
	GObjectClass *object_class = (GObjectClass*)class;
	object_class -> finalize = about_logo_finalize;
	GtkWidgetClass * widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> expose_event = about_logo_expose;

        /*about_logo_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (AboutLogoClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void about_logo_init(AboutLogo *about_logo)
{
}

void about_logo_constructor(AboutLogo * about_logo)
{
	about_logo -> logo_width = 0;
	about_logo -> logo_height = 0;
	char * logo_path = vex_term_find_ressource("res/aterminal.png");
	if (logo_path != NULL){
		cairo_surface_t * surface_logo = cairo_surface_create_from_file_unscaled(
			logo_path, &about_logo -> logo_width, &about_logo -> logo_height);
		about_logo -> pattern_logo = cairo_pattern_create_for_surface(surface_logo);
		cairo_surface_destroy(surface_logo);
		free(logo_path);
	}else{
		about_logo -> pattern_logo = cairo_pattern_create_rgba(0.0, 0.0, 1.0, 1.0);
	}

	gtk_widget_set_size_request(GTK_WIDGET(about_logo), -1, about_logo -> logo_height);
}

static void about_logo_finalize(GObject * object)
{
	AboutLogo * about_logo = VEX_ABOUT_LOGO(object);
	cairo_pattern_destroy(about_logo -> pattern_logo);
}

char * about_logo_create_random_string(int len)
{
	char * s = malloc(sizeof(char) * len);
	long int x;
	srandom(1);
	int i;
	for (i = 0; i < len; i++){
		x = random();
		gboolean one = x % 2 == 1;
		s[i] = one ? '1' : '0';
	}
	return s;
}

static gboolean about_logo_expose(GtkWidget * widget, GdkEventExpose * event)
{
	char * font_name = "Monospace";
	int font_size = 6;

	char * name = "vexterm";
	int nlen = strlen(name);
	int font_size_name = 8;

	AboutLogo * about_logo = VEX_ABOUT_LOGO(widget);

	cairo_t * cr = gdk_cairo_create(widget -> window);

	/* logo */
	cairo_pattern_t * pattern_logo = about_logo -> pattern_logo;
	cairo_set_source(cr, pattern_logo);
	cairo_rectangle(cr, 0, 0, widget -> allocation.width, widget -> allocation.height);
	cairo_fill(cr);

	/* background */
	cairo_pattern_t * pattern_bg = cairo_pattern_create_linear(
		about_logo -> logo_width, 0.0, widget -> allocation.width, 0.0);
	cairo_pattern_add_color_stop_rgba(pattern_bg, 0.0, 0.0, 0.0, 0.0, 1.0);
	cairo_pattern_add_color_stop_rgba(pattern_bg, 1.0, 0.0, 0.75, 0.0, 1.0);
	cairo_set_source(cr, pattern_bg);
	cairo_rectangle(cr, about_logo -> logo_width, 0, 
		widget -> allocation.width - about_logo -> logo_width, widget -> allocation.height);
	cairo_fill(cr);
	cairo_pattern_destroy(pattern_bg);

	/* 011011010011101010010 */
	PangoAttrList * attrs = pango_attr_list_new(); 
	PangoAttribute * size = pango_attr_size_new(PANGO_SCALE * font_size); 
	PangoAttribute * weight = pango_attr_weight_new(400); 
	PangoAttribute * family = pango_attr_family_new(font_name); 
	pango_attr_list_insert(attrs, size); 
	pango_attr_list_insert(attrs, family); 
	pango_attr_list_insert(attrs, weight); 

	cairo_pattern_t * pattern_font = cairo_pattern_create_linear(
		about_logo -> logo_width, 0.0, widget -> allocation.width, 0.0);
	cairo_pattern_add_color_stop_rgba(pattern_font, 0.0, 0.0, 1.0, 0.0, 1.0);
	cairo_pattern_add_color_stop_rgba(pattern_font, 1.0, 0.0, 0.0, 0.0, 0.0);

	PangoLayout * layout = pango_cairo_create_layout(cr);
	pango_layout_set_attributes(layout, attrs);

	pango_layout_set_text(layout, "0", 1);
	PangoRectangle ri, rl;
	pango_layout_get_pixel_extents(layout, &ri, &rl);
	int w = rl.width;

	int slen = (widget -> allocation.width - about_logo -> logo_width) / w + 1;
	char * string = about_logo_create_random_string(slen);

	pango_layout_set_text(layout, string, slen);

	cairo_set_source(cr, pattern_font);
	cairo_move_to(cr, about_logo -> logo_width, 0);
	pango_cairo_show_layout(cr, layout);

	pango_attr_list_unref(attrs);
	cairo_pattern_destroy(pattern_font);
	free(string);

	/* tool - name */
	attrs = pango_attr_list_new(); 
	size = pango_attr_size_new(PANGO_SCALE * font_size_name); 
	weight = pango_attr_weight_new(800); 
	family = pango_attr_family_new(font_name); 
	pango_attr_list_insert(attrs, size); 
	pango_attr_list_insert(attrs, family); 
	pango_attr_list_insert(attrs, weight); 

	cairo_pattern_t * pattern_name = cairo_pattern_create_linear(
		about_logo -> logo_width, 0.0, widget -> allocation.width, 0.0);
	cairo_pattern_add_color_stop_rgba(pattern_name, 0.0, 0.0, 1.0, 0.0, 1.0);
	cairo_pattern_add_color_stop_rgba(pattern_name, 0.0, 0.0, 0.0, 0.0, 1.0);

	pango_layout_set_attributes(layout, attrs);

	pango_layout_set_text(layout, name, nlen);
	pango_layout_get_pixel_extents(layout, &ri, &rl);
	int nw = rl.width;
	//int nh = rl.height;

	pango_layout_set_text(layout, name, nlen);

	cairo_set_source(cr, pattern_name);
	cairo_move_to(cr, widget -> allocation.width - nw - 4, 0);
	pango_cairo_show_layout(cr, layout);

	pango_attr_list_unref(attrs);
	cairo_pattern_destroy(pattern_name);
	g_object_unref(layout);

	cairo_destroy(cr);

	return FALSE;
}

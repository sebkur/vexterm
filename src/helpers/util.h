#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

gunichar string_to_unichar(char * string);

gboolean g_array_contains(GArray * array, gpointer data);

GSequenceIter * g_sequence_find(GSequence * sequence, gpointer data, GCompareDataFunc cmp_func, gpointer cmp_data);

char * get_path_relative_to_home_dir(char * path);

gboolean file_exists(char * path);

char * parent_dir(char * path);

char * home_dir();

void copy_files(char ** files, char * dest);

cairo_pattern_t * cairo_pattern_create_from_gdk_color(GdkColor color, double alpha);

cairo_surface_t * cairo_surface_create_from_pixbuf(GdkPixbuf * buf, int width, int height);
cairo_surface_t * cairo_surface_create_from_stock(char * stock_id, int width, int height);
cairo_surface_t * cairo_surface_create_from_file(char * file, int width, int height);
cairo_surface_t * cairo_surface_create_from_file_unscaled(char * file, int * width, int * height);

void pango_get_font_information(char * font_name, int font_size, int font_weight,
	int * out_ascent, int * out_width, int * out_height);

#endif /* _UTIL_H_ */

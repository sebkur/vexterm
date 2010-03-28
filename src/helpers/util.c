#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <glib.h>

#include "util.h"

gunichar string_to_unichar(char * string)
{
	gunichar * chars = g_utf8_to_ucs4_fast(string, -1, NULL);
	gunichar c = *chars;
	g_free(chars);
	return c;
}

gboolean g_array_contains(GArray * array, gpointer data)
{
	int i;
	for (i = 0; i < array -> len; i++){
		gpointer v = g_array_index(array, gpointer, i);
		if (v == data) return TRUE;
	}
	return FALSE;
}

char * get_path_relative_to_home_dir(char * path)
{
	char * home = getenv("HOME");
	if (home == NULL) return NULL;
	char * ret = g_build_filename(home, path, NULL);
	printf("PATH: %s\n", ret);
	return ret;
}

char * home_dir()
{
	char * home = getenv("HOME");
	return home;
}

gboolean file_exists(char * path)
{
	struct stat st;
	int s = stat(path, &st);
	return s == 0;
}

char * parent_dir(char * path)
{
	if (path == NULL) return NULL;
	char * last = NULL;
	char * found = NULL;
	char * iter = path;
	while(*iter != '\0'){
		if (*iter == '/'){
			last = found;
			found = iter;
		}
		iter += 1;
	}
	if (iter == found + 1){ //slash was last char
		found = last;
	}
	if (found == NULL) return NULL;
	int len = found - path + 1;
	return g_strndup(path, len);
}

void copy_file(char * filename, char * dest, mode_t protection)
{
	char * fn = g_path_get_basename(filename);
	char * destfile = g_build_filename(dest, fn, NULL);
	//printf("%s -> %s\n", filename, destfile);
	int f = open(filename, O_RDONLY);
	int g = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, protection);
	//printf("f: %d, g: %d\n", f, g);
	if (f < 0 || g < 0){
		printf("error copying file\n");
		if (f >= 0) close(f);
		if (g >= 0) close(g);
	}
	int max = 0x400000;
	char buf[max];
	size_t r;
	while(TRUE){
		r = read(f, &buf, max);
		if (r == 0) break;
		write(g, &buf, r);
	}
	close(f);
	close(g);
	free(fn);
	free(destfile);
}

void copy_file_or_folder(char * filename, char * dest)
{
	struct stat st;
	int s = lstat(g_strdup(filename), &st);
	if (s != 0){
		printf("error opening file\n");
		return;
	}
	int mode = st.st_mode & S_IFMT;
	switch(mode){
		case (S_IFDIR):{
			//printf("DIR: %s\n", filename);
			char * basename = g_path_get_basename(filename);
			char * newfolder = g_build_filename(dest, basename, NULL);
			//printf("creating folder: %s\n", newfolder);
			int mk = mkdir(newfolder, st.st_mode);
			if (mk != 0){
				printf("error creating subdiretory\n");
				free(newfolder);
				break;
			}
			free(basename);
			DIR * d;
			struct dirent * dir;
			d = opendir(filename);
			if(d == NULL) {
				printf("error copying directory\n");
				free(newfolder);
				closedir(d);
				break;
			}
			while((dir = readdir(d))){
				char * name = dir -> d_name;
				if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0){
					char * full = g_build_filename(filename, name, NULL);
					//printf("subfile: %s\n", full);
					copy_file_or_folder(full, newfolder);
					free(full);
				}
			}
			free(newfolder);
			closedir(d);
			break;
		}
		case (S_IFREG):{
			//printf("REG: %s\n", filename);
			copy_file(filename, dest, st.st_mode);
			break;
		}
	}
}

void copy_files(char ** files, char * dest)
{
	printf("copying to %s\n", dest);
	char ** iter;
	for(iter = files; *iter != NULL; iter++){
		if(strcmp(*iter, "") != 0){
			char * rfn = g_filename_from_uri(*iter, NULL, NULL);
			//if (g_str_has_prefix(*iter, "file://")){
			if (rfn != NULL){
				//char * filename = &((*iter)[7]);
				copy_file_or_folder(rfn, dest);
			}
		}
	}
}

int remove_file(char * filename)
{
	struct stat st;
	int s = lstat(g_strdup(filename), &st);
	if (s != 0){
		printf("error opening file\n");
		return -1;
	}
	int mode = st.st_mode & S_IFMT;
	int success = 0;
	switch(mode){
		case (S_IFDIR):{
			//printf("deleting DIR: %s\n", filename);
			DIR * d;
			struct dirent * dir;
			d = opendir(filename);
			if(d == NULL) {
				printf("error deleting directory\n");
				closedir(d);
				return -1;
			}
			int remsuc = 0;
			while((dir = readdir(d))){
				char * name = dir -> d_name;
				if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0){
					char * full = g_build_filename(filename, name, NULL);
					//printf("subfile: %s\n", full);
					int r = remove_file(full);
					if (r != 0) remsuc++;
					free(full);
				}
			}
			closedir(d);
			int s = rmdir(filename);
			if (s != 0 || remsuc != 0) success = -1;
			break;
		}
		case (S_IFLNK):
		case (S_IFREG):{
			//printf("deleting REG: %s\n", filename);
			int s = unlink(filename);
			if (s != 0) success = -1;
			break;
		}
	}
	return success;
}

GSequenceIter * g_sequence_find(GSequence * sequence, gpointer data, GCompareDataFunc cmp_func, gpointer cmp_data)
{
	GSequenceIter * iter = g_sequence_search(sequence, data, cmp_func, cmp_data);
	GSequenceIter * iter2 = g_sequence_iter_prev(iter);
	if (!g_sequence_iter_is_end(iter2)){
		gpointer fdata = g_sequence_get(iter2);
		if (cmp_func(data, fdata, cmp_data) == 0) return iter2;
	}
	if (!g_sequence_iter_is_end(iter)){
		gpointer fdata = g_sequence_get(iter);
		if (cmp_func(data, fdata, cmp_data) == 0) return iter;
	}
	return NULL;
}

cairo_pattern_t * cairo_pattern_create_from_gdk_color(GdkColor color, double alpha)
{
	cairo_pattern_t * pattern = cairo_pattern_create_rgba(color.red/65535., color.green/65535., color.blue/65535., alpha);
	return pattern;
}

cairo_surface_t * cairo_surface_create_from_pixbuf(GdkPixbuf * buf, int width, int height)
{
	int img_w = gdk_pixbuf_get_width(buf);
	int img_h = gdk_pixbuf_get_height(buf);
	cairo_surface_t * sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img_w, img_h); 		// ? F2
	cairo_t * cr = cairo_create(sur); 								// ? F3
	gdk_cairo_set_source_pixbuf(cr, buf, 0.0, 0.0);
	cairo_rectangle(cr, 0, 0, img_w, img_h);
	cairo_fill(cr);
	cairo_destroy(cr); 										// ! F3
	cairo_surface_t * sur_x = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);	// ? F4
	cairo_t * cr_x = cairo_create(sur_x);								// ? F5
	cairo_pattern_t * pat = cairo_pattern_create_for_surface(sur);					// ? F6
	double factor = ((double)img_h) / height;
	cairo_matrix_t matrix;
	cairo_matrix_init_scale(&matrix, factor, factor);
	cairo_pattern_set_matrix(pat, &matrix);
	cairo_set_source(cr_x, pat);
	cairo_rectangle(cr_x, 0, 0, width, height);
	cairo_fill(cr_x);
	cairo_destroy(cr_x);										// ! F5
	cairo_surface_destroy(sur); 									// ! F2
	cairo_pattern_destroy(pat);									// ! F6
	return sur_x;											// . F4
}

cairo_surface_t * cairo_surface_create_from_stock(char * stock_id, int width, int height)
{
	GtkIconTheme * icon_theme = gtk_icon_theme_get_default(); 					// no need to free
	GdkPixbuf * buf = gtk_icon_theme_load_icon(icon_theme, stock_id, height, 0, NULL); 		// ? F1
	cairo_surface_t * surface = cairo_surface_create_from_pixbuf(buf, width, height);
	g_object_unref(buf); 										// ! F1
	return surface;
}

cairo_surface_t * cairo_surface_create_from_file(char * file, int width, int height)
{
	GdkPixbuf * buf = gdk_pixbuf_new_from_file(file, NULL);
	cairo_surface_t * surface = cairo_surface_create_from_pixbuf(buf, width, height);
	g_object_unref(buf); 										// ! F1
	return surface;
}

cairo_surface_t * cairo_surface_create_from_file_unscaled(char * file, int * width, int * height)
{
	GdkPixbuf * buf = gdk_pixbuf_new_from_file(file, NULL);
	int img_w = gdk_pixbuf_get_width(buf);
	int img_h = gdk_pixbuf_get_height(buf);
	cairo_surface_t * surface = cairo_surface_create_from_pixbuf(buf, img_w, img_h);
	g_object_unref(buf); 										// ! F1
	*width = img_w;
	*height = img_h;
	return surface;
}

void pango_get_font_information(char * font_name, int font_size, int font_weight,
	int * out_ascent, int * out_width, int * out_height)
{
	int len = 1;
	char * test_char = "A";

	cairo_surface_t * surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 20, 20); 	// A1
	cairo_t * cr = cairo_create(surface);							// A2

	PangoFontMap * font_map = pango_cairo_font_map_get_default(); // do not free.

	/* get normal font description */
	PangoAttrList * attrs = pango_attr_list_new();						// A3
	PangoAttribute * size = pango_attr_size_new(PANGO_SCALE * font_size); 			// A3
	PangoAttribute * weight = pango_attr_weight_new(400); 					// A3
	PangoAttribute * family = pango_attr_family_new(font_name); 				// A3
	pango_attr_list_insert(attrs, size); 							// A3
	pango_attr_list_insert(attrs, family); 							// A3
	pango_attr_list_insert(attrs, weight); 							// A3

	PangoAttrIterator * pali = pango_attr_list_get_iterator(attrs); 			// A4
	PangoFontDescription * desc = pango_font_description_new(); 				// A5
	pango_attr_iterator_get_font(pali, desc, NULL, NULL);  

	PangoContext * pc = pango_cairo_create_context(cr);					// A6
	PangoFont * font = pango_font_map_load_font(font_map, pc, desc);			// A7

	PangoFontMetrics * metrics = pango_font_get_metrics(font, NULL);			// A8
	int ascent = pango_font_metrics_get_ascent(metrics) / PANGO_SCALE;
	pango_font_metrics_unref(metrics);							// F8

	GList * list = pango_itemize(pc, test_char, 0, len, attrs, NULL);			// A9
	PangoItem * pi = (PangoItem*)list -> data;
	PangoGlyphString * glyphs = pango_glyph_string_new();					// A10
	pango_glyph_string_set_size(glyphs, len);
	pango_shape(test_char, len, &pi -> analysis, glyphs);
	PangoRectangle ink_rect, logical_rect;
	pango_font_get_glyph_extents(font, glyphs -> glyphs -> glyph, &ink_rect, &logical_rect);

	/* put values into out parameters */
	*out_width = logical_rect.width / PANGO_SCALE;
	*out_height = logical_rect.height / PANGO_SCALE;
	*out_ascent = ascent;

	/* clean up */
	cairo_destroy(cr);					// F2
	cairo_surface_destroy(surface);				// F1
	pango_attr_list_unref(attrs);				// F3
	pango_font_description_free(desc);			// F5
	pango_attr_iterator_destroy(pali);			// F4

	g_list_foreach(list, (GFunc)pango_item_free, NULL);	// F9
	g_list_free(list);					// F9
	pango_glyph_string_free(glyphs);			// F10
	g_object_unref(font);					// F7
	g_object_unref(pc);					// F6
}

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

#ifndef _TERMINAL_WIDGET_H_
#define _TERMINAL_WIDGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "terminal_config.h"
#include "csi.h"
#include "history.h"

#define LIBVEX_TYPE_TERMINAL_WIDGET           (terminal_widget_get_type ())
#define LIBVEX_TERMINAL_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_TERMINAL_WIDGET, TerminalWidget))
#define LIBVEX_TERMINAL_WIDGET_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_TERMINAL_WIDGET, TerminalWidgetClass))
#define LIBVEX_IS_TERMINAL_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_TERMINAL_WIDGET))
#define LIBVEX_IS_TERMINAL_WIDGET_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_TERMINAL_WIDGET))
#define LIBVEX_TERMINAL_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_TERMINAL_WIDGET, TerminalWidgetClass))

typedef struct _TerminalWidget        TerminalWidget;
typedef struct _TerminalWidgetClass   TerminalWidgetClass;

#define PIXEL_FLAG_HIGHLIGHTED	0x8000
#define PIXEL_FLAG_REVERSE	0x4000
#define PIXEL_FLAG_FG_BRIGHT	0x2000
#define PIXEL_FLAG_BG_BRIGHT	0x1000

typedef struct{
	gunichar uchar;
	int flags; /* saves colours and switches */
	/* bits (from the right):
		 5-0 : bg-colour
		11-6 : fg-colour
		  13 : bg_bright
		  14 : fg_bright
		  15 : reverse
		  16 : highlighted 
	*/
} Pixel;

typedef struct {
	GArray * rows;
	int crow;
	int ccol;

	int scroll_top;
	int scroll_bot;

	char character_set;
} TerminalScreen;

typedef struct {
	int col;
	int row;
} TerminalPosition;

typedef struct {
	TerminalPosition start;
	TerminalPosition end;
} TerminalSelection;

struct _TerminalWidget
{
	GtkDrawingArea parent;

	TerminalConfig * terminal_config;

	pthread_mutex_t mutex;

	int width;
	int height;

	int c_w;
	int c_h;

	int n_rows;
	int n_cols;

	TerminalScreen * screen_current;
	TerminalScreen * screen_normal;
	TerminalScreen * screen_alternate;

	cairo_pattern_t * pattern_fg;
	cairo_pattern_t * pattern_bg;
	cairo_pattern_t * patterns_normal[8];
	cairo_pattern_t * patterns_highlighted[8];

	GSequence * cache[18][36];

	int fg;
	int bg;
	gboolean highlighted;
	gboolean reverse;
	gboolean fg_bright;
	gboolean bg_bright;

	int ascent;

	int master;

	GArray * parse_bufs;
	int parse_pos;

	int state;
	Csi current_csi;

	GHashTable * map_specials;

	int debug;
	gboolean dec_ckm;

	char * unhandled_data;
	int unhandled_data_len;

	gboolean cursor_visible; // maybe into the screen??? may perform better concerning error confinement
	gboolean show_scrolling_region;

	History * history;
	int history_pos;

	/* mouse stuff */
	gboolean mouse_left_pressed;
	int mouse_press_x;
	int mouse_press_y;
	gboolean selection_active;
	TerminalPosition pos_press;
	TerminalPosition pos_motion;
	TerminalSelection selection;
};

struct _TerminalWidgetClass
{
	GtkDrawingAreaClass parent_class;

	void (* set_screen)	(TerminalWidget *terminal_widget, int id);
	void (* set_keypad)	(TerminalWidget *terminal_widget, int id);
	void (* set_size)	(TerminalWidget *terminal_widget, int width, int height);
};

GType terminal_widget_get_type (void) G_GNUC_CONST;

TerminalWidget * terminal_widget_new();
TerminalWidget * terminal_widget_new_with_config(TerminalConfig * config);

int terminal_widget_get_current_screen(TerminalWidget * terminal_widget);
void terminal_widget_get_size(TerminalWidget * terminal_widget, int * width, int * height);

gboolean terminal_widget_get_show_scrolling_region(TerminalWidget * terminal_widget);
void terminal_widget_set_show_scrolling_region(TerminalWidget * terminal_widget, gboolean show);

void terminal_widget_set_font_name(TerminalWidget * terminal_widget, char * font_name);
void terminal_widget_set_font_size(TerminalWidget * terminal_widget, int font_size);

#endif /* _TERMINAL_WIDGET_H_ */

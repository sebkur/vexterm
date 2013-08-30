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

 /* NOTES:
  #1: tab-charachters; erase in line; they ignore colour-inversion. (seen in xterm and gnome-terminal_widget)
 */

#define DEFAULT_FONT_NAME "Monospace"
#define DEFAULT_FONT_SIZE 10

#define TW_CHAR_SIZE 10
#define TW_CHAR_WIDTH 8
#define TW_CHAR_HEIGHT 16

#define MULTICORE_FRIENDLY 1 /* 1 -> copy line buffer for drawing; don't hold lock while drawing */
#define LOCK_CHUNKWISE 1 /* 0 -> lock per char / escape sequence */

#define DEBUG_AT_ALL 0
#define DEBUG_HANDLE_CHAR 0			&& DEBUG_AT_ALL
#define DEBUG_NEWLINE 0				&& DEBUG_AT_ALL
#define DEBUG_INSERT_LINES 0			&& DEBUG_AT_ALL
#define DEBUG_DELTE_LINES 0			&& DEBUG_AT_ALL
#define DEBUG_REVERSE_INDEX 0			&& DEBUG_AT_ALL

#define DEBUG_UNIMPLEMENTED 1			&& DEBUG_AT_ALL

#define DEBUG_KNOWN_CSIS 0 			&& DEBUG_AT_ALL
#define DEBUG_UNKNOWN_CSIS 1			&& DEBUG_AT_ALL
#define DEBUG_KNOWN_BUT_UNHANDLED_CSIS 1	&& DEBUG_AT_ALL

#define DEBUG_KNOWN_ESCAPES 0			&& DEBUG_AT_ALL
#define DEBUG_UNKNOWN_ESCAPES 1			&& DEBUG_AT_ALL
#define DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES 1	&& DEBUG_AT_ALL

#define DEBUG_DECSET 0				&& DEBUG_AT_ALL
#define DEBUG_SCROLLING_AREA 0			&& DEBUG_AT_ALL
#define DEBUG_COLOURS 0				&& DEBUG_AT_ALL
#define DEBUG_UNKNOWN_COLOURS 0			&& DEBUG_AT_ALL
#define DEBUG_UTF8 0				&& DEBUG_AT_ALL
#define DEBUG_SIZE 0				&& DEBUG_AT_ALL
#define DEBUG_SCREENS 0				&& DEBUG_AT_ALL
#define DEBUG_CURSOR 0				&& DEBUG_AT_ALL
#define DEBUG_HISTORY 0				&& DEBUG_AT_ALL

#define DEBUG_RENDERING_TIME 1			&& DEBUG_AT_ALL
#define DEBUG_RENDERING_FONT 1			&& DEBUG_AT_ALL

#define DEBUG_MOUSE 0
#define DEBUG_MOUSE_MOTION 0

#define DRAW_EACH_ACTION 0

#define _XOPEN_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include "terminal_widget.h"
#include "terminal_handler.h"
#include "marshal.h"
#include "../helpers/tool.h"
#include "../helpers/util.h"
#include "../helpers/stringbuffer/string_buffer.h"
#include "iso2022.h"

#define PIXEL_IS_HIGHLIGHTED(p)		((p -> flags & PIXEL_FLAG_HIGHLIGHTED) != 0)
#define PIXEL_IS_REVERSE(p)		((p -> flags & PIXEL_FLAG_REVERSE) != 0)
#define PIXEL_IS_FG_BRIGHT(p)		((p -> flags & PIXEL_FLAG_FG_BRIGHT) != 0)
#define PIXEL_IS_BG_BRIGHT(p)		((p -> flags & PIXEL_FLAG_BG_BRIGHT) != 0)

#define PIXEL_SET_HIGHLIGHTED(p, b)\
	do {if (b) { p -> flags |= PIXEL_FLAG_HIGHLIGHTED; } else { p -> flags &= ~PIXEL_FLAG_HIGHLIGHTED; }} while(0)
#define PIXEL_SET_REVERSE(p, b)\
	do {if (b) { p -> flags |= PIXEL_FLAG_REVERSE; } else { p -> flags &= ~PIXEL_FLAG_REVERSE; }} while(0)
#define PIXEL_SET_FG_BRIGHT(p, b)\
	do {if (b) { p -> flags |= PIXEL_FLAG_FG_BRIGHT; } else { p -> flags &= ~PIXEL_FLAG_FG_BRIGHT; }} while(0)
#define PIXEL_SET_BG_BRIGHT(p, b)\
	do {if (b) { p -> flags |= PIXEL_FLAG_BG_BRIGHT; } else { p -> flags &= ~PIXEL_FLAG_BG_BRIGHT; }} while(0)

#define PIXEL_SET_BG(p, i)	do { p -> flags &= ~0x001F; p -> flags |= i; } while(0)
#define PIXEL_SET_FG(p, i)	do { p -> flags &= ~0x03E0; p -> flags |= (i << 5); } while(0)
#define PIXEL_GET_BG(p)		(p -> flags & 0x001F)
#define PIXEL_GET_FG(p)		((p -> flags & 0x03E0) >> 5)

static void terminal_handler_interface_init (TerminalHandlerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (TerminalWidget, terminal_widget, GTK_TYPE_DRAWING_AREA,
	G_IMPLEMENT_INTERFACE(LIBVEX_TYPE_TERMINAL_HANDLER, terminal_handler_interface_init));

enum
{
        SET_SCREEN,
        SET_KEYPAD,
	SET_SIZE,
        LAST_SIGNAL
};

static guint terminal_widget_signals[LAST_SIGNAL] = { 0 };

static gboolean terminal_widget_configure(GtkWidget * widget, GdkEventConfigure * event);
static gboolean terminal_widget_expose(GtkWidget * widget, GdkEventExpose * event);
static gboolean terminal_widget_key_press(GtkWidget * widget, GdkEventKey * event);
static gboolean terminal_widget_button_press(GtkWidget * widget, GdkEventButton * event);
static gboolean terminal_widget_button_release(GtkWidget * widget, GdkEventButton * event);
static gboolean terminal_widget_motion_notify(GtkWidget * widget, GdkEventMotion * event);
void terminal_widget_adjust_to_size(TerminalWidget * terminal_widget, int w, int h, gboolean force);

void terminal_widget_handle_ascii(TerminalHandler * terminal_handler, char c);
void terminal_widget_handle_char(TerminalHandler * terminal_handler, gunichar uchar);
void terminal_widget_handle_csi(TerminalHandler * terminal_handler, Csi * csi);
void terminal_widget_handle_escaped(TerminalHandler * terminal_handler, char c);
void terminal_widget_set_charset(TerminalHandler * terminal_handler, char c);
void terminal_widget_set_fd(TerminalHandler * terminal_handler, int fd);
void terminal_widget_chunk_begin(TerminalHandler * terminal_handler);
void terminal_widget_chunk_done(TerminalHandler * terminal_handler);

void emit_set_screen(TerminalWidget * terminal_widget);
void emit_set_keypad(TerminalWidget * terminal_widget);
void emit_set_size(TerminalWidget * terminal_widget);

void terminal_widget_constructor(TerminalWidget * terminal_widget);
void terminal_widget_configure_colour_palette(TerminalWidget * terminal_widget, TerminalColourPalette * palette);
void terminal_widget_configure_font(TerminalWidget * terminal_widget);

gboolean terminal_widget_get_selected_text(TerminalWidget * terminal_widget, StringBuffer ** buffer);

void input_commit_cb (GtkIMContext *context, const gchar * str, gpointer data);

TerminalWidget * terminal_widget_new()
{
	TerminalConfig * config = terminal_config_new();
	return terminal_widget_new_with_config(config);
}

TerminalWidget * terminal_widget_new_with_config(TerminalConfig * config)
{
	TerminalWidget * terminal_widget = g_object_new(LIBVEX_TYPE_TERMINAL_WIDGET, NULL);
	terminal_widget -> terminal_config = config;
	terminal_widget_constructor(terminal_widget);
	return terminal_widget;
}

static void terminal_widget_class_init(TerminalWidgetClass *class)
{
	GtkWidgetClass * widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> configure_event = terminal_widget_configure;
	widget_class -> expose_event = terminal_widget_expose;
	widget_class -> key_press_event = terminal_widget_key_press;
	widget_class -> button_press_event = terminal_widget_button_press;
	widget_class -> button_release_event = terminal_widget_button_release;
	widget_class -> motion_notify_event = terminal_widget_motion_notify;

        terminal_widget_signals[SET_SCREEN] = g_signal_new(
                "set-screen",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalWidgetClass, set_screen),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        terminal_widget_signals[SET_KEYPAD] = g_signal_new(
                "set-keypad",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalWidgetClass, set_keypad),
                NULL, NULL,
                g_cclosure_marshal_VOID__INT,
                G_TYPE_NONE, 1, G_TYPE_INT);
        terminal_widget_signals[SET_SIZE] = g_signal_new(
                "set-size",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalWidgetClass, set_size),
                NULL, NULL,
                g_cclosure_user_marshal_VOID__INT_INT,
                G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);
}

static void terminal_handler_interface_init (TerminalHandlerInterface * iface)
{
	iface -> handle_ascii = terminal_widget_handle_ascii;
	iface -> handle_utf8 = terminal_widget_handle_char;
	iface -> handle_csi = terminal_widget_handle_csi;
	iface -> handle_escaped = terminal_widget_handle_escaped;
	iface -> set_charset = terminal_widget_set_charset;
	iface -> set_fd = terminal_widget_set_fd;
	iface -> chunk_begin = terminal_widget_chunk_begin;
	iface -> chunk_done = terminal_widget_chunk_done;
}

void pixel_set_colours(TerminalWidget * terminal_widget, Pixel * pixel)
{
	PIXEL_SET_FG(pixel, terminal_widget -> fg);
	PIXEL_SET_BG(pixel, terminal_widget -> bg);
	PIXEL_SET_HIGHLIGHTED	(pixel, terminal_widget -> highlighted);
	PIXEL_SET_REVERSE	(pixel, terminal_widget -> reverse);
	PIXEL_SET_FG_BRIGHT	(pixel, terminal_widget -> fg_bright);
	PIXEL_SET_BG_BRIGHT	(pixel, terminal_widget -> bg_bright);
}

void pixel_init(TerminalWidget * terminal_widget, Pixel * pixel, gunichar uchar)
{
	pixel -> uchar = uchar;
	pixel_set_colours(terminal_widget, pixel);
}

void pixel_init_opaque(TerminalWidget * terminal_widget, Pixel * pixel, gunichar uchar)
{
	pixel -> uchar = uchar;
	PIXEL_SET_FG(pixel, 16);
	PIXEL_SET_BG(pixel, 17);
	PIXEL_SET_HIGHLIGHTED	(pixel, FALSE);
	PIXEL_SET_REVERSE	(pixel, FALSE);
	PIXEL_SET_FG_BRIGHT	(pixel, FALSE);
	PIXEL_SET_BG_BRIGHT	(pixel, FALSE);
}

void pixel_change(TerminalWidget * terminal_widget, Pixel * pixel, gunichar uchar)
{
	pixel -> uchar = uchar;
	pixel_set_colours(terminal_widget, pixel);
}

// int rcc = 0; // counter for created / deleted rows

GArray * row_buffer_create(TerminalWidget * terminal_widget)
{
	GArray * buffer = g_array_new(FALSE, FALSE, sizeof(GArray*));
	return buffer;
//	// this is obsolete due to automatic adding of lines TODO: remove
//	int r, c;
//	for (r = 0; r < terminal_widget -> n_rows; r++){
//		GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
//		//printf("rows1 %d\n", rcc++);
//		g_array_append_val(buffer, row);
//		for (c = 0; c < terminal_widget -> n_cols; c++){
//			Pixel pixel;
//			pixel_init(terminal_widget, &pixel, (gunichar)' ');
//			g_array_append_val(row, pixel);
//		}
//	}
//	return buffer;
}

GArray * row_copy(GArray * row)
{
	int l = row -> len;
	GArray * array = g_array_sized_new(FALSE, FALSE, sizeof(Pixel), l);
	// shoule be a bit faster like this
	memcpy(array -> data, row -> data, sizeof(Pixel) * l);
	array -> len = l;
	// slower version
//	int i;
//	for (i = 0; i < l; i++){
//		Pixel * pixel = &g_array_index(row, Pixel, i);
//		Pixel p;
//		memcpy(&p, pixel, sizeof(Pixel));
//		g_array_append_val(array, p);
//	}
	return array;
}

void row_free(GArray * row)
{
	g_array_free(row, TRUE);
}

void row_buffer_free(GArray * array)
{
	int r;
	for (r = 0; r < array -> len; r++){
		GArray * row = g_array_index(array, GArray*, r);
		row_free(row);
		//printf("rows %d\n", rcc--);
	}
	g_array_free(array, TRUE); 
}

TerminalScreen * terminal_widget_screen_create(TerminalWidget * terminal_widget)
{
	TerminalScreen * screen = malloc(sizeof(TerminalScreen));

	screen -> rows = row_buffer_create(terminal_widget);
	screen -> crow = 1;
	screen -> ccol = 1;

	screen -> scroll_top = 1;
	screen -> scroll_bot = terminal_widget -> n_rows;

	screen -> character_set = 'B';
	return screen;
}

void terminal_widget_config_changed_cb(TerminalConfig * terminal_config, TerminalWidget * terminal_widget)
{
	printf("terminal_config changed\n");
	terminal_widget -> terminal_config = terminal_config;
	TerminalColourPalette * palette = 
		terminal_config_get_colour_palette(terminal_widget -> terminal_config);
	terminal_widget_configure_colour_palette(terminal_widget, palette);
	terminal_widget_configure_font(terminal_widget);
	terminal_widget_adjust_to_size(terminal_widget, terminal_widget -> width, terminal_widget -> height, TRUE);
	gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
}

static void terminal_widget_init(TerminalWidget *terminal_widget)
{
        GTK_OBJECT_SET_FLAGS(terminal_widget, GTK_CAN_FOCUS);
	gtk_widget_set_events(GTK_WIDGET(terminal_widget), gtk_widget_get_events(GTK_WIDGET(terminal_widget))
		| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

	terminal_widget -> width = 200;
	terminal_widget -> height = 100;

	int n_rows = 24;
	int n_cols = 80;

	terminal_widget -> n_rows = n_rows;
	terminal_widget -> n_cols = n_cols;

	terminal_widget -> map_specials = char_map_special();

	terminal_widget -> dec_ckm = FALSE;

	terminal_widget -> cursor_visible = TRUE;
	terminal_widget -> show_scrolling_region = TRUE;
	terminal_widget -> show_right_margin = FALSE;

	terminal_widget -> ascent = -1;

	/* mouse stuff */
	terminal_widget -> mouse_left_pressed = FALSE;
	terminal_widget -> selection_active = FALSE;
	terminal_widget -> mouse_press_x = 0;
	terminal_widget -> mouse_press_y = 0;
	terminal_widget -> pos_press.col = 0;
	terminal_widget -> pos_press.row = 0;
	terminal_widget -> selection.start.col = 0;
	terminal_widget -> selection.start.row = 0;
	terminal_widget -> selection.end.col = 0;
	terminal_widget -> selection.end.row = 0;
	
	terminal_widget -> im_context = gtk_im_context_simple_new();
	g_signal_connect (terminal_widget -> im_context, "commit", G_CALLBACK (input_commit_cb), terminal_widget);
}

void terminal_widget_constructor(TerminalWidget * terminal_widget)
{	
	terminal_widget -> fg = 16;
	terminal_widget -> bg = 17;
	terminal_widget -> highlighted = FALSE;
	terminal_widget -> reverse = FALSE;
	terminal_widget -> fg_bright = FALSE;
	terminal_widget -> bg_bright = FALSE;

	TerminalColourPalette * palette = 
		terminal_config_get_colour_palette(terminal_widget -> terminal_config);
	terminal_widget_configure_colour_palette(terminal_widget, palette);

	g_signal_connect(
		G_OBJECT(terminal_widget -> terminal_config), "changed-font",
		G_CALLBACK(terminal_widget_config_changed_cb), terminal_widget);
	g_signal_connect(
		G_OBJECT(terminal_widget -> terminal_config), "changed-colour-palette",
		G_CALLBACK(terminal_widget_config_changed_cb), terminal_widget);

	terminal_widget -> screen_normal = terminal_widget_screen_create(terminal_widget);
	terminal_widget -> screen_alternate = terminal_widget_screen_create(terminal_widget);
	terminal_widget -> screen_current = terminal_widget -> screen_normal;

	terminal_widget -> history = history_new(512);
	terminal_widget -> history_pos = 0;

	terminal_widget_configure_font(terminal_widget);

	terminal_widget -> margin_position = 80;

	pthread_mutexattr_t mta;
	pthread_mutexattr_init(&mta);
	pthread_mutex_init(&terminal_widget -> mutex, &mta);
}

void terminal_widget_configure_font(TerminalWidget * terminal_widget)
{
	int font_width, font_height, font_ascent;
	pango_get_font_information(
		terminal_config_get_font_name(terminal_widget -> terminal_config), 
		terminal_config_get_font_size(terminal_widget -> terminal_config), 
		400, 
		&font_ascent, &font_width, &font_height);
	#if DEBUG_RENDERING_FONT
	printf("CHAR SIZE (width, height, ascent): %d, %d, %d\n", font_width, font_height, font_ascent);
	#endif

	terminal_widget -> c_w = font_width;
	terminal_widget -> c_h = font_height;
	terminal_widget -> ascent = font_ascent;
}

void terminal_widget_configure_colour_palette(TerminalWidget * terminal_widget, TerminalColourPalette * palette)
{
	int c;
	for (c = 0; c <= 17; c++){
		TerminalColour * colour;
		cairo_pattern_t ** pattern;
		if (c == 0){
			colour = &palette -> foreground;
			pattern = &terminal_widget -> pattern_fg;
		}else if (c == 1){
			colour = &palette -> background;
			pattern = &terminal_widget -> pattern_bg;
		}else if (c <= 9){
			colour = &palette -> normal[c - 2];
			pattern = &terminal_widget -> patterns_normal[c - 2];
		}else if (c <= 17){
			colour = &palette -> highlighted[c - 10];
			pattern = &terminal_widget -> patterns_highlighted[c - 10];
		}
		*pattern = cairo_pattern_create_rgba(
			colour -> r / (double) 0xFFFF,
			colour -> g / (double) 0xFFFF,
			colour -> b / (double) 0xFFFF,
			colour -> a / (double) 0xFFFF
		);
	}
	
	int bg, fg;
	for (bg = 0; bg <= 17; bg++){
		for (fg = 0; fg <= 35; fg++){
			terminal_widget -> cache[bg][fg] = g_sequence_new(NULL);
		}
	}
}

gboolean terminal_widget_get_show_scrolling_region(TerminalWidget * terminal_widget)
{
	return terminal_widget -> show_scrolling_region;
}

void terminal_widget_set_show_scrolling_region(TerminalWidget * terminal_widget, gboolean show)
{
	if (terminal_widget -> show_scrolling_region != show){
		terminal_widget -> show_scrolling_region = show;
		gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
	}
}

gboolean terminal_widget_get_show_right_margin(TerminalWidget * terminal_widget)
{
	return terminal_widget -> show_right_margin;
}

void terminal_widget_set_show_right_margin(TerminalWidget * terminal_widget, gboolean show)
{
	if (terminal_widget -> show_right_margin != show){
		terminal_widget -> show_right_margin = show;
		gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
	}
}

void terminal_widget_set_margin_position(TerminalWidget * terminal_widget, int position)
{
	terminal_widget -> margin_position = position;
	gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
}

int terminal_widget_get_margin_position(TerminalWidget * terminal_widget)
{
	return terminal_widget -> margin_position;
}

int terminal_widget_get_current_screen(TerminalWidget * terminal_widget)
{
	int id = (terminal_widget -> screen_current == terminal_widget -> screen_normal) ? 0 : 1;
	return id;
}

void terminal_widget_get_size(TerminalWidget * terminal_widget, int * width, int * height)
{
	*width = terminal_widget -> n_cols;
	*height = terminal_widget -> n_rows;
}

/****************************************************************
Keyboard controls
****************************************************************/
void send_cursor(TerminalWidget * terminal_widget, int key); // xterm doc
void send_keypad(TerminalWidget * terminal_widget, int key); // xterm doc

void clipboard_func(GtkClipboard * cb, const gchar * text, gpointer data)
{
	TerminalWidget * terminal_widget = (TerminalWidget*)data;
	if (text != NULL){
		char * wbuf = g_strdup(text); //TODO: copying necessary?
		write(terminal_widget -> master, wbuf, strlen(wbuf));
		free(wbuf);
	}
}

static gboolean terminal_widget_key_press(GtkWidget * widget, GdkEventKey * event)
{
	TerminalWidget * terminal_widget = LIBVEX_TERMINAL_WIDGET(widget);
	//gboolean ret = FALSE;
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	if ((event -> state & modifiers) == 0){
		switch (event->keyval){
		}
	}
	if ((event -> state & modifiers) == GDK_SHIFT_MASK){
		switch(event -> keyval){
			case GDK_Up:{
				if (terminal_widget -> screen_current == terminal_widget -> screen_normal){
					if (terminal_widget -> history_pos > 0){
						terminal_widget -> history_pos -= 1;
						gtk_widget_queue_draw(widget);
					}
				}
				return TRUE;
			}
			case GDK_Down:{
				if (terminal_widget -> screen_current == terminal_widget -> screen_normal){
					int n_elems = history_get_number_of_elements(terminal_widget -> history);
					if (terminal_widget -> history_pos < n_elems){
						terminal_widget -> history_pos += 1;
						gtk_widget_queue_draw(widget);
					}
				}
				return TRUE;
			}
			case GDK_Page_Up:{
				if (terminal_widget -> screen_current == terminal_widget -> screen_normal){
					if (terminal_widget -> history_pos > 0){
						int newpos = terminal_widget -> history_pos - terminal_widget -> n_rows;
						terminal_widget -> history_pos = newpos >= 0 ? newpos : 0;
						gtk_widget_queue_draw(widget);
					}
				}
				return TRUE;
			}
			case GDK_Page_Down:{
				if (terminal_widget -> screen_current == terminal_widget -> screen_normal){
					int n_elems = history_get_number_of_elements(terminal_widget -> history);
					if (terminal_widget -> history_pos < n_elems){
						int newpos = terminal_widget -> history_pos + terminal_widget -> n_rows;
						terminal_widget -> history_pos = 
							newpos <= n_elems ? newpos : n_elems;
						gtk_widget_queue_draw(widget);
					}
				}
				return TRUE;
			}
			case GDK_Insert:{
				GtkClipboard * cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
				gtk_clipboard_request_text(cb, clipboard_func, terminal_widget);
				return TRUE;
			}
		}
	}
	// catch modifier keys
	if (event -> keyval == GDK_Shift_L 
		|| event -> keyval == GDK_Shift_R
		|| event -> keyval == GDK_Caps_Lock
		|| event -> keyval == GDK_Control_L
		|| event -> keyval == GDK_Control_R
		|| event -> keyval == GDK_Alt_L
		|| event -> keyval == GDK_Alt_R
		|| event -> keyval == GDK_ISO_Level3_Shift){
		return TRUE;
	}

	pthread_mutex_lock(&terminal_widget -> mutex);
	// scroll to bottom
	if (terminal_widget -> screen_current == terminal_widget -> screen_normal){
		int n_elems = history_get_number_of_elements(terminal_widget -> history);
		if (terminal_widget -> history_pos != n_elems){
			terminal_widget -> history_pos = n_elems;
			gtk_widget_queue_draw(widget);
		}
	}
	pthread_mutex_unlock(&terminal_widget -> mutex);

	if ((event -> state & modifiers) == 0
		|| (event -> state & modifiers) == GDK_SHIFT_MASK){
		switch (event -> keyval){
			case GDK_Up:
			case GDK_Down:
			case GDK_Right:
			case GDK_Left:
			case GDK_Home:
			case GDK_End:{
				send_cursor(terminal_widget, event -> keyval);
				break;
			}
			case GDK_Insert:
			case GDK_Delete:
			case GDK_Page_Up:
			case GDK_Page_Down:{
				send_keypad(terminal_widget, event -> keyval);
				break;
			}
			case GDK_Return:
			case GDK_Tab:
			case GDK_Escape:{
				char * wbuf = g_strdup_printf("%c", event -> keyval);
				write(terminal_widget -> master, wbuf, 1);
				break;
			}
			case GDK_ISO_Left_Tab:{
				char * wbuf = g_strdup_printf("\033[Z");
				write(terminal_widget -> master, wbuf, 3);
				break;
			}
			case GDK_BackSpace:{
				struct termios tio;
				char * wbuf;
				if (tcgetattr(terminal_widget -> master, &tio) != -1 && tio.c_cc[VERASE] != 0){
					wbuf = g_strdup_printf("%c", tio.c_cc[VERASE]);
				}else{
					wbuf = g_strdup_printf("%c", event -> keyval);
				}
				write(terminal_widget -> master, wbuf, 1);
				break;
			}
			case GDK_F1:
			case GDK_F2:
			case GDK_F3:
			case GDK_F4:
			case GDK_F5:{
				int fn = event -> keyval - GDK_F1 + 1;
				int fc = fn + 10;
				char * wbuf = g_strdup_printf("\033[%d~", fc);
				write(terminal_widget -> master, wbuf, 5);
				break;
			}
			case GDK_F6:
			case GDK_F7:
			case GDK_F8:
			case GDK_F9:
			case GDK_F10:{
				int fn = event -> keyval - GDK_F1 + 1;
				int fc = fn + 11;
				char * wbuf = g_strdup_printf("\033[%d~", fc);
				write(terminal_widget -> master, wbuf, 5);
				break;
			}
			case GDK_F11:
			case GDK_F12:{
				int fn = event -> keyval - GDK_F1 + 1;
				int fc = fn + 12;
				char * wbuf = g_strdup_printf("\033[%d~", fc);
				write(terminal_widget -> master, wbuf, 5);
				break;
			}
			default:{
				//char outbuf[6];
				//guint32 uch = gdk_keyval_to_unicode(event -> keyval);
				//gint l = g_unichar_to_utf8(uch, outbuf);
				//write(terminal_widget -> master, outbuf, l);
				//printf("%s\n", gdk_keyval_name(event -> keyval));
				gtk_im_context_filter_keypress(terminal_widget -> im_context, event);
				break;
			}
		}
	}else if ((event -> state & modifiers) == GDK_CONTROL_MASK
		|| (event -> state & modifiers) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK)){
		char ch;
		gboolean use = FALSE;
		if (event -> keyval >= 64 && event -> keyval <= 95){
			ch = event -> keyval - 64;
			use = TRUE;
		}
		if (event -> keyval >= 97 && event -> keyval <= 122){
			ch = event -> keyval - 64 - 32;
			use = TRUE;
		}
		if (use){
			char * wbuf = g_strdup_printf("%c", ch);
			write(terminal_widget -> master, wbuf, 1);
			free(wbuf);
		}
	}
	return TRUE;
}

void input_commit_cb (GtkIMContext *context, const gchar * str, gpointer data) {
	TerminalWidget * terminal_widget = data;
	gint l = strlen(str);
	char * wbuf = g_strdup_printf("%s", str);
	write(terminal_widget -> master, wbuf, l);
	free(wbuf);
}

void send_cursor(TerminalWidget * terminal_widget, int key) // xterm doc
// depends on DECCKM
{
	char letter = 'A';
	switch(key){
		case GDK_Up: 	letter = 'A'; break;
		case GDK_Down: 	letter = 'B'; break;
		case GDK_Right:	letter = 'C'; break;
		case GDK_Left:	letter = 'D'; break;
		case GDK_Home:	letter = 'H'; break;
		case GDK_End:	letter = 'F'; break;
	}
	char * wbuf;
	if (terminal_widget -> dec_ckm){
		wbuf = g_strdup_printf("\033O%c", letter); 
	}else{
		wbuf = g_strdup_printf("\033[%c", letter);
	}
	write(terminal_widget -> master, wbuf, 3);
	free(wbuf);
}

void send_keypad(TerminalWidget * terminal_widget, int key) // xterm doc
// depends on DECPNM, DECPAM TODO: not implemented
{
	char letter = '3';
	switch(key){
		case GDK_Delete: 	letter = '3'; break;
		case GDK_Insert: 	letter = '2'; break;
		case GDK_Page_Down:	letter = '6'; break;
		case GDK_Page_Up:	letter = '5'; break;
	}
	char * wbuf = g_strdup_printf("\033[%c~", letter); 
	write(terminal_widget -> master, wbuf, 4);
	free(wbuf);
}

/****************************************************************
Mouse Handling
****************************************************************/

gboolean terminal_widget_pos_smaller(TerminalPosition p1, TerminalPosition p2){
	if (p1.row == p2.row){
		return p1.col < p2.col;
	}else{
		return p1.row < p2.row;
	}
}

void terminal_widget_get_position(TerminalWidget * terminal_widget, double wx, double wy, int * x, int * y)
{
	*x = (int) (wx / terminal_widget -> c_w);
	*y = (int) (wy / terminal_widget -> c_h);
}

static gboolean terminal_widget_button_press(GtkWidget * widget, GdkEventButton * event)
{
	if (!GTK_WIDGET_HAS_FOCUS(widget)){
		gtk_widget_grab_focus(widget);
	}
	TerminalWidget * terminal_widget = (TerminalWidget*) widget;
	terminal_widget -> mouse_press_x = (int) event -> x;
	terminal_widget -> mouse_press_y = (int) event -> y;
	int x, y;
	terminal_widget_get_position(terminal_widget, event -> x, event -> y, &x, &y);
	if (event -> button == 1){
		terminal_widget -> mouse_left_pressed = TRUE;
		#if DEBUG_MOUSE
		printf("press %d %d\n", x, y);
		#endif
		terminal_widget -> pos_press.col = x;
		terminal_widget -> pos_press.row = y;
		terminal_widget -> pos_motion.row = -1;
		terminal_widget -> pos_motion.col = -1;
		terminal_widget -> selection_active = FALSE;
		gtk_widget_queue_draw(widget);
	}
	return FALSE;
}

static gboolean terminal_widget_button_release(GtkWidget * widget, GdkEventButton * event)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) widget;
	int x, y;
	terminal_widget_get_position(terminal_widget, event -> x, event -> y, &x, &y);
	if (event -> button == 1){
		terminal_widget -> mouse_left_pressed = FALSE;
		terminal_widget -> pos_motion.col = -1;
		terminal_widget -> pos_motion.row = -1;
		#if DEBUG_MOUSE
		printf("release %d %d\n", x, y);
		#endif
		GtkClipboard * cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
		StringBuffer * buffer;
		if (terminal_widget_get_selected_text(terminal_widget, &buffer)){
			int len;
			char * text = string_buffer_get_text(buffer, &len);
			gtk_clipboard_set_text(cb, text, len);
			string_buffer_free(buffer);
		}
	}
	return FALSE;
}

static gboolean terminal_widget_motion_notify(GtkWidget * widget, GdkEventMotion * event)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) widget;
	if (terminal_widget -> mouse_left_pressed){
		int x, y;
		terminal_widget_get_position(terminal_widget, event -> x, event -> y, &x, &y);
		#if DEBUG_MOUSE_MOTION
		printf("motion %d %d\n", x, y);
		#endif
		if (terminal_widget -> pos_motion.col != x || terminal_widget -> pos_motion.row != y){
			terminal_widget -> pos_motion.col = x;
			terminal_widget -> pos_motion.row = y;
			TerminalPosition pos; pos.col = x; pos.row = y;
			if (terminal_widget_pos_smaller(pos, terminal_widget -> pos_press)){
				terminal_widget -> selection.start = pos;
				terminal_widget -> selection.end = terminal_widget -> pos_press;
			}else{
				terminal_widget -> selection.start = terminal_widget -> pos_press;
				terminal_widget -> selection.end = pos;
			}
			terminal_widget -> selection_active = TRUE;
			gtk_widget_queue_draw(widget);
		}
	}
	return FALSE;
}

/****************************************************************
Drawing
****************************************************************/

typedef struct {
	gunichar uchar;
	GdkPixmap * pixmap;
}CacheElement;

gint compare_cache_elements(gconstpointer a, gconstpointer b, gpointer user_data)
{
	CacheElement * ca = (CacheElement*)a;
	CacheElement * cb = (CacheElement*)b;
	return (ca -> uchar - cb -> uchar);
}

cairo_pattern_t * pattern_from_index(TerminalWidget * terminal_widget, int index)
{
	switch(index){
		case 17: return terminal_widget -> pattern_bg;
		case 16: return terminal_widget -> pattern_fg;
		default:{
			if (index >= 8){
				return terminal_widget -> patterns_highlighted[index - 8];
			}else{
				return terminal_widget -> patterns_normal[index];
			}
		}
	}
}

void pixel_to_indices(Pixel * pixel, int * f, int * b)
{
	int fgc = PIXEL_GET_FG(pixel);
	int bgc = PIXEL_GET_BG(pixel);
	if (bgc != 17 && PIXEL_IS_BG_BRIGHT(pixel)) bgc += 8;
	if (fgc != 16 && PIXEL_IS_FG_BRIGHT(pixel)) fgc += 8;
	if (PIXEL_IS_REVERSE(pixel)){
		int t = fgc; fgc = bgc; bgc = t;
	}
	if (PIXEL_IS_HIGHLIGHTED(pixel) && fgc <= 7) fgc += 8;
	*f = fgc; *b = bgc;
}

void terminal_widget_adjust_to_size(TerminalWidget * terminal_widget, int w, int h, gboolean force)
{
	if (w != terminal_widget -> width || h != terminal_widget -> height || force){
		terminal_widget -> width = w;
		terminal_widget -> height = h;
		int n_rows = h / terminal_widget -> c_h;
		int n_cols = w / terminal_widget -> c_w;
		if (n_rows < 2 || n_cols < 10) return;

		if (n_rows != terminal_widget -> n_rows || n_cols != terminal_widget -> n_cols){
			pthread_mutex_lock(&terminal_widget -> mutex);

			gboolean smaller = n_rows < terminal_widget -> n_rows;
			gboolean bigger  = n_rows > terminal_widget -> n_rows;

			int n_rows_old = terminal_widget -> n_rows;
			terminal_widget -> n_rows = n_rows;
			terminal_widget -> n_cols = n_cols;

			struct winsize size;
			memset(&size, 0, sizeof(size));
			size.ws_row = terminal_widget -> n_rows;
			size.ws_col = terminal_widget -> n_cols;
			ioctl(terminal_widget -> master, TIOCSWINSZ, &size);
			emit_set_size(terminal_widget);

			//remove some lines if size has been set smaller (maybe: do on change of screen...)
			TerminalScreen * sn = terminal_widget -> screen_normal;
			if (smaller && (sn -> scroll_top == 1 && sn -> scroll_bot == n_rows_old)){
				// the terminal has become smaller
				int rem = sn -> rows -> len - terminal_widget -> n_rows;
				#if DEBUG_HISTORY
				printf("pushing %d lines to history\n", rem); fflush(NULL);
				#endif
				if (sn -> rows -> len > terminal_widget -> n_rows){
					int i;
					for (i = 0; i < rem; i++){
						GArray * row = g_array_index(sn -> rows, GArray*, 0);
						g_array_remove_index(sn -> rows, 0); // LX
						GArray * reprow;
						gboolean replaced = history_push(
							terminal_widget -> history, row, (gpointer*)&reprow);
						if (!replaced){
							terminal_widget -> history_pos += 1;
						}else{
							//printf("Dude, you have to free a row here!\n");
							//printf("free %p\n", reprow);
							row_free(reprow);
						}
					}
					if (sn -> crow > terminal_widget -> n_rows){
						sn -> crow -= rem;
					}
				}
				sn -> scroll_top = 1;
				sn -> scroll_bot = terminal_widget -> n_rows;
			}
			if (bigger && (sn -> scroll_top == 1 && sn -> scroll_bot == n_rows_old)){
				// the terminal has become larger
				int add = terminal_widget -> n_rows - sn -> rows -> len;
				int hlen = history_get_number_of_elements(terminal_widget -> history);
				int add_real = hlen >= add ? add : hlen;
				#if DEBUG_HISTORY
				printf("pulling %d line(s) from history of size %d\n", add_real, hlen); fflush(NULL);
				#endif
				int a;
				for (a = 0; a < add_real; a++){
					GArray * row;
					/*gboolean success = */history_pop(terminal_widget -> history, (gpointer*)&row);
					g_array_prepend_val(sn -> rows, row);
					terminal_widget -> history_pos -= 1;
					sn -> crow += 1;
				}
				sn -> scroll_top = 1;
				sn -> scroll_bot = terminal_widget -> n_rows;
			}

			if (terminal_widget -> screen_alternate -> scroll_top == 1 &&
				terminal_widget -> screen_alternate -> scroll_bot == n_rows_old){
				terminal_widget -> screen_alternate -> scroll_bot = n_rows;
			}
//			terminal_widget -> screen_normal -> scroll_top = 1;
//			terminal_widget -> screen_normal -> scroll_bot = terminal_widget -> n_rows;
//			terminal_widget -> screen_alternate -> scroll_top = 1;
//			terminal_widget -> screen_alternate -> scroll_bot = terminal_widget -> n_rows;

			pthread_mutex_unlock(&terminal_widget -> mutex);

			#if DEBUG_SIZE
			printf("size changed to %dx%d\n", terminal_widget -> n_rows, terminal_widget -> n_cols);
			#endif
		}
	}
}

static gboolean terminal_widget_configure(GtkWidget * widget, GdkEventConfigure * event)
{
	#if DEBUG_SIZE
	printf("CONFIGURE\n");
	#endif
	TerminalWidget * terminal_widget = LIBVEX_TERMINAL_WIDGET(widget);
	int w = event -> width;
	int h = event -> height;
	terminal_widget_adjust_to_size(terminal_widget, w, h, FALSE);
	return FALSE;
}

#if DEBUG_RENDERING_TIME
int time_cacheing;
int time_blitting;
#endif 

gboolean pixel_is_selected(TerminalWidget * terminal_widget, int col, int row)
{
	gboolean ret = FALSE;
	if (row > terminal_widget -> selection.start.row && row < terminal_widget -> selection.end.row){
		ret = TRUE;
	}else if (row == terminal_widget -> selection.start.row || row == terminal_widget -> selection.end.row){
		ret = TRUE;
		if (row == terminal_widget -> selection.start.row){
			ret &= col >= terminal_widget -> selection.start.col;
		}
		if (row == terminal_widget -> selection.end.row){
			ret &= col <= terminal_widget -> selection.end.col;
		}
	}
	return ret;
}

void draw_row(TerminalWidget * terminal_widget, 
	PangoFontMap * font_map, PangoFontDescription * desc, PangoFontDescription * desc_bold,
	PangoAttrList * attrs, PangoAttrList * attrs_bold, GdkGC * gc,
	GArray * row, int r)
{
	#if DEBUG_RENDERING_TIME
	struct timeval tstart, t1, t2, t3, t4, t5, t6, t7, t8, tend;
	gettimeofday(&tstart, NULL);
	#endif

	GtkWidget * widget = (GtkWidget*)(terminal_widget);
	int c_w = terminal_widget -> c_w; 
	int c_h = terminal_widget -> c_h;

	int c;
	for (c = 0; c < row -> len; c++){
		#if DEBUG_RENDERING_TIME
		gettimeofday(&t1, NULL);
		#endif
		Pixel * pixel = &g_array_index(row, Pixel, c);

		int fgc, bgc;
		pixel_to_indices(pixel, &fgc, &bgc);
		if (terminal_widget -> selection_active && pixel_is_selected(terminal_widget, c, r)){
			int t = fgc; fgc = bgc; bgc = t;
		}
		int fgc_cache = PIXEL_IS_HIGHLIGHTED(pixel) ? fgc + 18 : fgc;

		GSequence * cache = terminal_widget -> cache[bgc][fgc_cache];
		//printf("ACCESS: %d %d %d\n", pixel -> bg, PIXEL_GET_FG(pixel), PIXEL_IS_HIGHLIGHTED(pixel));

		CacheElement * entry;
		CacheElement se; se.uchar = pixel -> uchar;
		GSequenceIter * iter = g_sequence_find(cache, &se, compare_cache_elements, NULL);
		#if DEBUG_RENDERING_TIME
		gettimeofday(&t2, NULL);
		time_cacheing += time_diff(&t1, &t2);
		#endif
		if (G_LIKELY(iter != NULL)){
			entry = (CacheElement*)g_sequence_get(iter);
		}else{
			//printf("CACHE MISS\n");
			entry = g_slice_alloc(sizeof(CacheElement));
			GdkPixmap * pixmap = gdk_pixmap_new(widget -> window, c_w, c_h, -1);
			entry -> uchar = pixel -> uchar;
			entry -> pixmap = pixmap;
			g_sequence_insert_sorted(cache, entry, compare_cache_elements, NULL);

			cairo_pattern_t * pfg = pattern_from_index(terminal_widget, fgc);
			cairo_pattern_t * pbg = pattern_from_index(terminal_widget, bgc);

			char * obuf = g_slice_alloc0(sizeof(char) * 7);
			//memset(obuf, 0, 7);
			g_unichar_to_utf8(pixel -> uchar, obuf);

			int len = strlen(obuf);
			if (G_LIKELY(len > 0)){
				cairo_t * cr = gdk_cairo_create(entry -> pixmap);

				cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
				cairo_rectangle(cr, 0, 0, c_w, c_h);
				cairo_fill(cr);
				cairo_set_source(cr, pbg);
				cairo_rectangle(cr, 0, 0, c_w, c_h);
				cairo_fill(cr);

				PangoContext * pc = pango_cairo_create_context(cr);
				PangoFont * font = pango_font_map_load_font(font_map, pc, 
					PIXEL_IS_HIGHLIGHTED(pixel) ? desc_bold : desc);

				cairo_set_source(cr, pfg);

				GList * list = pango_itemize(pc, obuf, 0, len, 
					PIXEL_IS_HIGHLIGHTED(pixel) ? attrs_bold : attrs, NULL);
				PangoItem * pi = (PangoItem*)list -> data;
				PangoGlyphString * glyphs = pango_glyph_string_new();
				pango_glyph_string_set_size(glyphs, len);
				pango_shape(obuf, len, &pi -> analysis, glyphs);

				cairo_move_to(cr, 0, terminal_widget -> ascent);
				pango_cairo_show_glyph_string(cr, font, glyphs);

				g_list_foreach(list, (GFunc)pango_item_free, NULL);
				g_list_free(list);
				pango_glyph_string_free(glyphs);
				g_object_unref(font);
				g_object_unref(pc);
				cairo_destroy(cr);
			}
			g_slice_free1(sizeof(char) * 7, obuf);
		}
		#if DEBUG_RENDERING_TIME
		gettimeofday(&t3, NULL);
		#endif

		gdk_draw_drawable(widget -> window, gc, entry -> pixmap, 0, 0, 
			c * c_w, r * c_h, c_w, c_h);

		#if DEBUG_RENDERING_TIME
		gettimeofday(&t4, NULL);
		time_blitting += time_diff(&t3, &t4);
		#endif
	}
}

static gboolean terminal_widget_expose(GtkWidget * widget, GdkEventExpose * event)
{
	#if DEBUG_RENDERING_TIME
	struct timeval tstart, t1, t2, t3, t4, t5, t6, t7, t8, tend;
	gettimeofday(&tstart, NULL);
	time_cacheing = 0;
	time_blitting = 0;
	#endif

	TerminalWidget * terminal_widget = LIBVEX_TERMINAL_WIDGET(widget);
	TerminalScreen * screen = terminal_widget -> screen_current;
	int c_w = terminal_widget -> c_w; 
	int c_h = terminal_widget -> c_h;

	//TODO: initialize attributes once, on construction/config-change
	char * font_name = terminal_config_get_font_name(terminal_widget -> terminal_config);
	int font_size = terminal_config_get_font_size(terminal_widget -> terminal_config);

	PangoFontMap * font_map = pango_cairo_font_map_get_default(); // do not free.

	/* get normal font description */
	PangoAttrList * attrs = pango_attr_list_new(); // memory ok
	PangoAttribute * size = pango_attr_size_new(PANGO_SCALE * font_size); // memory ok
	PangoAttribute * weight = pango_attr_weight_new(400); // memory ok
	PangoAttribute * family = pango_attr_family_new(font_name); // memory ok
	pango_attr_list_insert(attrs, size); // memory ok
	pango_attr_list_insert(attrs, family); // memory ok
	pango_attr_list_insert(attrs, weight); // memory ok

	PangoAttrIterator * pali = pango_attr_list_get_iterator(attrs); // memory ok
	PangoFontDescription * desc = pango_font_description_new(); // memory ok
	pango_attr_iterator_get_font(pali, desc, NULL, NULL); // memory ok

	/* get bold font description */
	PangoAttrList * attrs_bold = pango_attr_list_new(); // memory ok
	PangoAttribute * size_bold = pango_attr_size_new(PANGO_SCALE * font_size); // memory ok
	PangoAttribute * weight_bold = pango_attr_weight_new(800); // memory ok
	PangoAttribute * family_bold = pango_attr_family_new(font_name); // memory ok
	pango_attr_list_insert(attrs_bold, size_bold); // memory ok
	pango_attr_list_insert(attrs_bold, family_bold); // memory ok
	pango_attr_list_insert(attrs_bold, weight_bold); // memory ok

	PangoAttrIterator * pali_bold = pango_attr_list_get_iterator(attrs_bold); // memory ok
	PangoFontDescription * desc_bold = pango_font_description_new(); // memory ok
	pango_attr_iterator_get_font(pali_bold, desc_bold, NULL, NULL); // memory ok

	cairo_t * cr = gdk_cairo_create(widget -> window); // memory ok

	pthread_mutex_lock(&terminal_widget -> mutex);

	/* background */
	cairo_pattern_t * pattern_bg = terminal_widget -> pattern_bg;
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, widget -> allocation.width, widget -> allocation.height);
	cairo_fill(cr);
	cairo_set_source(cr, pattern_bg);
	cairo_rectangle(cr, 0, 0, widget -> allocation.width, widget -> allocation.height);
	cairo_fill(cr);

	/* selected part */
	if (terminal_widget -> selection_active){
		int c1 = terminal_widget -> selection.start.col;
		int r1 = terminal_widget -> selection.start.row;
		int c2 = terminal_widget -> selection.end.col;
		int r2 = terminal_widget -> selection.end.row;
		int w = terminal_widget -> c_w;
		int h = terminal_widget -> c_h;
		cairo_set_source(cr, terminal_widget -> pattern_fg);
		if (r1 == r2){
			cairo_rectangle(cr, c1 * w, r1 * h, (c2 - c1 + 1) * w, h);
		}else{
			/* first line */
			cairo_rectangle(cr, c1 * w, r1 * h, widget -> allocation.width, h);
			/* middle */
			cairo_rectangle(cr, 0, (r1 + 1) * h, widget -> allocation.width, (r2 - r1 - 1) * h);
			/* last line */
			cairo_rectangle(cr, 0, r2 * h, (c2 + 1) * w, h);
		}
		cairo_fill(cr);
	}

	/* rows */
	GdkGC * gc = gdk_gc_new(widget -> window); // memory ok

	#if DEBUG_RENDERING_TIME
	int itemizing_time = 0;
	int glyphing_time = 0;

	gettimeofday(&t1, NULL);
	#endif

	GArray * rows = g_array_new(FALSE, FALSE, sizeof(GArray*));

	int r, hn = 0;
	if (screen == terminal_widget -> screen_alternate){
		int rc = 0;
		for (r = 0; r < terminal_widget -> screen_current -> rows -> len; r++){
			GArray * row = g_array_index(screen -> rows, GArray*, r);
			#if MULTICORE_FRIENDLY
			GArray * copy = row_copy(row);
			g_array_append_val(rows, copy);
			#else
			draw_row(terminal_widget, font_map, desc, desc_bold, attrs, attrs_bold, gc, row, r);
			#endif
			rc++;
		}
		//printf("drawn %d lines\n", rc);
	}else{
		int hlen = history_get_number_of_elements(terminal_widget -> history);
		int slen = terminal_widget -> screen_current -> rows -> len;
		int hpos = terminal_widget -> history_pos;
		hn = hlen - hpos;
		//int total = hlen + slen;
		//int disp = total >= terminal_widget -> n_rows ? terminal_widget -> n_rows : total;
		//printf("hist lines: %d, total: %d\n", hn, disp);
		int rc = 0;
//		printf("nrows: %d, hlen: %d, hpos: %d, hn: %d, rowslen: %d\n",
//			terminal_widget -> n_rows, hlen, hpos, hn, screen -> rows -> len);
		for (r = 0; r < hn && r < terminal_widget -> n_rows; r++){
			GArray * row = (GArray*) history_get(terminal_widget -> history, hpos + r);
			#if MULTICORE_FRIENDLY
			GArray * copy = row_copy(row);
			g_array_append_val(rows, copy);
			#else
			draw_row(terminal_widget, font_map, desc, desc_bold, attrs, attrs_bold, gc, row, r);
			#endif
			rc++;
		}
		for (r = hn; r < terminal_widget -> n_rows; r++){
			if (r - hn >= slen) break;
			GArray * row = g_array_index(screen -> rows, GArray*, r - hn);
			#if MULTICORE_FRIENDLY
			GArray * copy = row_copy(row);
			g_array_append_val(rows, copy);
			#else
			draw_row(terminal_widget, font_map, desc, desc_bold, attrs, attrs_bold, gc, row, r);
			#endif
			rc++;
		}
		//printf("drawn %d lines\n", rc);
	}

	#if MULTICORE_FRIENDLY
	pthread_mutex_unlock(&terminal_widget -> mutex);

	for (r = 0; r < rows -> len; r++){
		GArray * row = g_array_index(rows, GArray*, r);
		draw_row(terminal_widget, font_map, desc, desc_bold, attrs, attrs_bold, gc, row, r);
		row_free(row);
	}
	#endif
	
	#if DEBUG_RENDERING_TIME
	gettimeofday(&t2, NULL);
	#endif

	/* draw an half-opaque tiny cursor */
	if (terminal_widget -> cursor_visible && screen -> crow  + hn <= terminal_widget -> n_rows){
		cairo_pattern_t * pattern_cursor = cairo_pattern_create_rgba(1.0, 0.0, 0.0, 0.5);
		cairo_set_source(cr, pattern_cursor);
		cairo_rectangle(cr, 
			(screen -> ccol - 1) * c_w, 
			(screen -> crow - 1 + hn) * c_h, 
			c_w, c_h);
		cairo_fill(cr);
		cairo_pattern_destroy(pattern_cursor);
	}
	/* draw scrolling region */
	if (G_UNLIKELY(terminal_widget -> show_scrolling_region)){
		cairo_pattern_t * pattern_sr = cairo_pattern_create_rgba(1.0, 0.0, 0.0, 0.5);
		cairo_set_source(cr, pattern_sr);
		cairo_rectangle(cr, 
			0, (screen -> scroll_top - 1) * c_h, 
			widget -> allocation.width, (screen -> scroll_bot - screen -> scroll_top + 1) * c_h);
		cairo_stroke(cr);
		cairo_pattern_destroy(pattern_sr);
	}

	/* draw a 80-chars-margin */
	if (terminal_widget -> show_right_margin){
		int x = terminal_widget -> margin_position * terminal_widget -> c_w;
		cairo_pattern_t * pattern_sr = cairo_pattern_create_rgba(1.0, 0.0, 0.0, 0.5);
		cairo_set_source(cr, pattern_sr);
		cairo_move_to(cr, x, 0);
		cairo_line_to(cr, x, terminal_widget -> height);
		cairo_stroke(cr);
		cairo_pattern_destroy(pattern_sr);
	}

	/* clean up */
	g_object_unref(gc);
	cairo_destroy(cr);

	pango_font_description_free(desc);
	pango_font_description_free(desc_bold);
	pango_attr_iterator_destroy(pali);
	pango_attr_iterator_destroy(pali_bold);
	pango_attr_list_unref(attrs);
	pango_attr_list_unref(attrs_bold);

	#if !MULTICORE_FRIENDLY
	pthread_mutex_unlock(&terminal_widget -> mutex);
	#endif

	#if DEBUG_RENDERING_TIME
	gettimeofday(&tend, NULL);
	printf("EXPSURE ");
	printf("TOTAL: %d ", time_diff(&tstart, &tend));
	printf("C1: %d ", time_diff(&tstart, &t1));
	printf("ROWS: %d ", time_diff(&t1, &t2));
	printf("C2: %d ", time_diff(&t2, &tend));
	printf("CACHE: %d ", time_cacheing);
	printf("BLIT: %d ", time_blitting);
	printf("\n");
	#endif

	//printf("EXPOSURE TIME: %d\n", time_diff(&tstart, &tend));
	//printf("EXPOSURE TIME tstart,t4: %d\n", time_diff(&tstart, &t4));
	//printf("EXPOSURE TIME t4,tend: %d\n", time_diff(&t4, &tend));
	//printf("EXPOSURE TIME itemizing: %d\n", itemizing_time);
	//printf("EXPOSURE TIME glyphing: %d\n", glyphing_time);
	//printf("FROW: %d\n", terminal_widget -> frow);

	return FALSE;
}

/****************************************************************
Parsing / Actions
****************************************************************/

//typedef struct{
//	char prefix;
//	GArray * nums;
//	gboolean first_digit;
//	char suffix1;
//	char suffix2;
//} Csi;

void insert_blank_characters(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	GArray * row = g_array_index(terminal_widget -> screen_current -> rows, GArray*, screen -> crow - 1);
	int i;
	for (i = 0; i < n; i++){
		if (screen -> ccol <= row -> len){
			Pixel pixel;
			pixel_init(terminal_widget, &pixel, (gunichar)' ');
			g_array_insert_val(row, screen -> ccol - 1, pixel);
		}
	}
}

void erase_characters(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	if (screen -> crow <= screen -> rows -> len){
		GArray * row = g_array_index(terminal_widget -> screen_current -> rows, GArray*, screen -> crow - 1);
		int i;
		for (i = 0; i < n; i++){
			int c = screen -> ccol - 1 + i;
			if (c < row -> len){
				Pixel * pixel = &g_array_index(row, Pixel, c);
				pixel_change(terminal_widget, pixel, (gunichar)' ');
			}else{
				Pixel pixel;
				pixel_init(terminal_widget, &pixel, (gunichar)' ');
				g_array_append_val(row, pixel);
			}
		}
	}
}

void delete_characters(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	if (screen -> crow <= screen -> rows -> len){
		GArray * row = g_array_index(terminal_widget -> screen_current -> rows, GArray*, screen -> crow - 1);
		int i;
		for (i = 0; i < n; i++){
			if (screen -> ccol < row -> len){
				g_array_remove_index(row, screen -> ccol - 1); 
			}
		}
	}
}

void cursor_character_absolute(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	screen -> ccol = n >= 1 ? n : 1;
}

void cursor_up(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	screen -> crow -= n;
}

void cursor_down(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	screen -> crow += n;
}

void erase_to_the_right(TerminalWidget * terminal_widget)
{
	int pr = terminal_widget -> screen_current -> crow;
	int pc = terminal_widget -> screen_current -> ccol;
	if (terminal_widget -> screen_current -> rows -> len < pr) return;
	GArray * row = g_array_index(terminal_widget -> screen_current -> rows, GArray*, pr - 1);
	int x;
	gboolean reverse = terminal_widget -> reverse; // toggle reversion temporarily (NOTE #1)
	terminal_widget -> reverse = FALSE;
	if (row -> len >= 1){
		for (x = row -> len - 1; x >= pc - 1 && x >= 0; x--){ //TODO: check for >= 0 is silly. pc must not be < 1
			g_array_remove_index(row, x); 
		}
	}
	for (x = terminal_widget -> screen_current -> ccol; x <= terminal_widget -> n_cols; x++){
		Pixel pixel;
		pixel_init(terminal_widget, &pixel, (gunichar)' ');
		g_array_append_val(row, pixel);
	}
	terminal_widget -> reverse = reverse;
}

void erase_to_the_left(TerminalWidget * terminal_widget)
{
	int pr = terminal_widget -> screen_current -> crow;
	int pc = terminal_widget -> screen_current -> ccol;
	if (terminal_widget -> screen_current -> rows -> len < pr) return;
	GArray * row = g_array_index(terminal_widget -> screen_current -> rows, GArray*, pr - 1);
	int len = row -> len;
	int del = len > pc ? pc : len;
	printf("DELETING: %d on %d:%d\n", del, terminal_widget -> screen_current -> crow, terminal_widget -> screen_current -> ccol);
	printf("reverse is %d\n", terminal_widget -> reverse);
	printf("bg is %d\n", terminal_widget -> bg);
	int i;
	gboolean reverse = terminal_widget -> reverse; // toggle reversion temporarily (NOTE #1);
	terminal_widget -> reverse = FALSE;
	for (i = 0; i < del; i++){
		Pixel * pixel = &g_array_index(row, Pixel, i);
		pixel_change(terminal_widget, pixel, (gunichar)' ');
	}
	terminal_widget -> reverse = reverse;
}

void erase_line(TerminalWidget * terminal_widget)
{
	#if DEBUG_UNIMPLEMENTED
	int pr = terminal_widget -> screen_current -> crow;
	int pc = terminal_widget -> screen_current -> ccol;
	printf("UNIMPLEMENTED: erase line at %d %d\n", pc, pr);
	#endif
}

void insert_lines_before(TerminalWidget * terminal_widget, int n)
{
	// ignore if not within scrolling region
	TerminalScreen * screen = terminal_widget -> screen_current;
	#if DEBUG_INSERT_LINES
	printf("||INSERTING LINE BEFORE cursor:%d:%d||", screen -> crow, screen -> ccol); fflush(NULL);
	#endif
	int i;
	for (i = 0; i < n; i++){
		if (screen -> crow >= screen -> scroll_top && screen -> crow <= screen -> scroll_bot){
			// ok, we're in the scrolling region
			GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
			//printf("rows2 %d\n", rcc++);
			if (screen -> rows -> len >= screen -> scroll_bot){
				GArray * row = g_array_index(screen -> rows, GArray*, screen -> scroll_bot - 1);
				g_array_remove_index(screen -> rows, screen -> scroll_bot - 1); // LX
				//printf("rows %d\n", rcc--);
				row_free(row);
			}
			g_array_insert_val(screen -> rows, screen -> crow - 1, row);
		}
	}
}

void insert_lines(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	#if DEBUG_INSERT_LINES
	printf("||INSERTING %d LINE(S) cursor:%d:%d||", n, screen -> crow, screen -> ccol); fflush(NULL);
	#endif
	// ignore if not within scrolling region
	if (screen -> crow >= screen -> scroll_top && screen -> crow <= screen -> scroll_bot){
		// ok, we're in the scrolling region
		if (screen -> crow == screen -> scroll_bot){
			// we're on the last line, have to scroll
			#if DEBUG_INSERT_LINES
			printf("LAST nlines: %d ", screen -> rows -> len); fflush(NULL);
			#endif
			int s;
			for (s = screen -> rows -> len; s < screen -> crow; s++){
				GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
				//printf("rows3 %d\n", rcc++);
				g_array_insert_val(screen -> rows, s, row);
			}
			GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
			//printf("rows4 %d\n", rcc++);
			g_array_insert_val(screen -> rows, screen -> crow, row);

			GArray * drow = g_array_index(screen -> rows, GArray*, screen -> scroll_top - 1);
			g_array_remove_index(screen -> rows, screen -> scroll_top - 1); // LX
			if (screen == terminal_widget -> screen_normal && screen -> scroll_top == 1) {
				//printf("HISTORY\n"); fflush(NULL);
				GArray * reprow;
				gboolean replaced = history_push(terminal_widget -> history, drow, (gpointer*)&reprow);
				if (!replaced){
					terminal_widget -> history_pos += 1;
				}else{
					//printf("Dude, here's some row to free\n");
					//printf("1free %p\n", reprow);
					row_free(reprow);
				}
			}else{
				row_free(drow);
				//printf("rows %d\n", rcc--);
			}
		}else{
			int x; // n lines
			for (x = 0; x < n; x++){
				// check whether we have to retain a row at the bottom of the scrolling region
				if (screen -> rows -> len >= screen -> scroll_bot){
					// yes, there are too many rows
					GArray * row = g_array_index(screen -> rows, GArray*, screen -> scroll_bot - 1);
					g_array_remove_index(screen -> rows, screen -> scroll_bot - 1); // LX
					//printf("rows %d\n", rcc--);
					row_free(row);
				}
				// insert new row
				GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
				//printf("rows5 %d\n", rcc++);
				g_array_insert_val(screen -> rows, screen -> crow, row);
				screen -> crow += 1; // TODO: is this ok?
			}
		}
	}
}

void delete_lines(TerminalWidget * terminal_widget, int n)
{
	// ignore if not within scrolling region
	TerminalScreen * screen = terminal_widget -> screen_current;
	#if DEBUG_DELETE_LINES
	printf("\nDELETE %d:%d c:%d\n", screen -> scroll_top, screen -> scroll_bot, screen -> crow); fflush(NULL);
	#endif
	if (screen -> crow >= screen -> scroll_top && screen -> crow <= screen -> scroll_bot){
		// ok, we're in the scrolling region
		#if DEBUG_DELETE_LINES
		printf("\nDELETE\n"); fflush(NULL);
		#endif
		int i;
		for (i = 0; i < n; i++){
			GArray * drow = g_array_index(screen -> rows, GArray*, screen -> crow - 1);
			g_array_remove_index(screen -> rows, screen -> crow - 1); // LX 
			//printf("rows %d\n", rcc--);
			row_free(drow);
			// insert new row at bottom of scrolling region
			GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
			//printf("rows6 %d\n", rcc++);
			g_array_insert_val(screen -> rows, screen -> scroll_bot - 1, row);
		}
	}
}

void erase_all(TerminalWidget * terminal_widget)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	row_buffer_free(screen -> rows);
	screen -> rows = row_buffer_create(terminal_widget);
	screen -> crow = 1;
	screen -> ccol = 1;
}

void tw_index(TerminalWidget * terminal_widget)
{	//TODO
	#if DEBUG_UNIMPLEMENTED
	printf("index - UNIMPLEMENTED\n");
	#endif
}

void tw_reverse_index(TerminalWidget * terminal_widget)
{
	#if DEBUG_REVERSE_INDEX
	printf("||REVERSE INDEX||"); fflush(NULL);
	#endif
	TerminalScreen * screen = terminal_widget -> screen_current;
	// check whether we're at the top margin
	if (screen -> crow == screen -> scroll_top){
		// yes we are. scroll down
		#if DEBUG_REVERSE_INDEX
		printf("\nSCROLL\n"); fflush(NULL);
		#endif
		#if DEBUG_INSERT_LINES
		printf("IL: reverse index "); fflush(NULL);
		#endif
		insert_lines_before(terminal_widget, 1);
	}else{
		// no. just move the cursor up
		screen -> crow -= 1;
		#if DEBUG_REVERSE_INDEX
		printf("\nREPOS\n"); fflush(NULL);
		#endif
	}
}

void scroll_up(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	int i;
	for (i = 0; i < n; i++){
		GArray * drow = g_array_index(screen -> rows, GArray*, screen -> scroll_top - 1);
		g_array_remove_index(screen -> rows, screen -> scroll_top - 1); // LX
		//printf("rows %d\n", rcc--);
		row_free(drow);
		// insert new row at bottom of scrolling region
		GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
		//printf("rows7 %d\n", rcc++);
		int inspos = screen -> scroll_bot - 1;
		if (inspos > screen -> rows -> len) inspos = screen -> rows -> len;
		g_array_insert_val(screen -> rows, inspos, row);
	}
}

void scroll_down(TerminalWidget * terminal_widget, int n)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	int i;
	for (i = 0; i < n; i++){
		GArray * drow = g_array_index(screen -> rows, GArray*, screen -> scroll_bot - 1);
		g_array_remove_index(screen -> rows, screen -> scroll_bot - 1); // LX
		row_free(drow);
		// insert new row at top of scrolling region
		GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
		int inspos = screen -> scroll_top - 1;
		if (inspos > screen -> rows -> len) inspos = screen -> rows -> len;
		g_array_insert_val(screen -> rows, inspos, row);
	}
}

/* Colour codes are the following:

Foreground
1m     -     Highlight (bold)
4m     -     Underline
5m     -     Blinking
8m     -     Hidden
30m    -     Black
31m    -     Red
32m    -     Green
33m    -     Yellow
34m    -     Blue
35m    -     Magenta
36m    -     Cyan
37m    -     White

Background
40m    -     Black
41m    -     Red
42m    -     Green
43m    -     Yellow
44m    -     Blue
45m    -     Magenta
46m    -     Cyan
47m    -     White
49m    -     Default

7m     -     Reverse
0m     -     Reset attributes.		*/
void set_colours(TerminalWidget * terminal_widget, int code)
{
	#if DEBUG_COLOURS
	printf("%d ", code); fflush(NULL);
	#endif
	if (code == 0){
		terminal_widget -> fg = 16;
		terminal_widget -> bg = 17;
		terminal_widget -> highlighted = FALSE;
		terminal_widget -> reverse = FALSE;
		terminal_widget -> fg_bright = FALSE;
		terminal_widget -> bg_bright = FALSE;
	}else if(code == 1){
		terminal_widget -> highlighted = TRUE;
	}else if(code == 2){
		terminal_widget -> highlighted = FALSE;
	}else if(code == 7){ 
		terminal_widget -> reverse = TRUE;
	}else if(code == 27){ 
		terminal_widget -> reverse = FALSE;
	}else if (code >= 30 && code <= 37){
		int c = code - 30;
		terminal_widget -> fg = c;
		terminal_widget -> fg_bright = FALSE;
	}else if(code == 39){
		terminal_widget -> fg = 16;
		terminal_widget -> fg_bright = FALSE;
	}else if (code >= 40 && code <= 47){
		int c = code - 40;
		terminal_widget -> bg = c;
		terminal_widget -> bg_bright = FALSE;
	}else if(code == 49){
		terminal_widget -> bg = 17;
		terminal_widget -> bg_bright = FALSE;
	}else if (code >= 90 && code <= 97){ // should be bright
		int c = code - 90;
		terminal_widget -> fg = c;
		terminal_widget -> fg_bright = TRUE;
	}else if (code >= 100 && code <= 107){ // should be bright
		int c = code - 100;
		terminal_widget -> bg = c;
		terminal_widget -> bg_bright = TRUE;
	}else{
		#if DEBUG_UNKNOWN_COLOURS
		printf("colour: %d ", code); fflush(NULL);
		#endif
	}
}

void use_normal_screen(TerminalWidget * terminal_widget)
{
	#if DEBUG_SCREENS
	printf("normal screen ");
	#endif
	terminal_widget -> screen_current = terminal_widget -> screen_normal;
	emit_set_screen(terminal_widget);
}

void use_alternate_screen(TerminalWidget * terminal_widget)
{
	#if DEBUG_SCREENS
	printf("alternate screen ");
	#endif
	terminal_widget -> screen_current = terminal_widget -> screen_alternate;
	row_buffer_free(terminal_widget -> screen_current -> rows);
	terminal_widget -> screen_current -> rows = row_buffer_create(terminal_widget);
	terminal_widget -> screen_current -> crow = 1;
	terminal_widget -> screen_current -> ccol = 1;
	emit_set_screen(terminal_widget);
}

void cursor_goto(TerminalWidget * terminal_widget, int r, int c)
{
	TerminalScreen * screen = terminal_widget -> screen_current;
	screen -> crow = r;
	screen -> ccol = c >= 1 ? c : 1;
	if (screen -> crow > terminal_widget -> n_rows){ // if we're out of range
		//TODO: this disregards the scrolling region; guessed behavior
		// It's unclear what's supposed to happen if we're moved 'into' the margin
		int i, x = r - terminal_widget -> n_rows;
		for (i = 0; i < x; i++){
			//g_array_remove_index(screen -> rows, 0);
			screen -> crow -= 1;
		}
	}
}

void reset(TerminalWidget * terminal_widget)
{
	use_normal_screen(terminal_widget);
	TerminalScreen * screen = terminal_widget -> screen_normal;
	row_buffer_free(screen -> rows);
	screen -> rows = row_buffer_create(terminal_widget); //TODO: free old stuff
	screen -> scroll_top = 1;
	screen -> scroll_bot = terminal_widget -> n_rows;
	cursor_goto(terminal_widget, 1, 1);
}

/****************************************************************
Interface Functions
****************************************************************/

void _terminal_widget_handle_char(TerminalWidget * terminal_widget, gunichar uchar, gboolean internal)
{
	#if !LOCK_CHUNKWISE
	if (!internal){
		//pthread_mutex_lock(&terminal_widget -> mutex);
	}
	#endif

	TerminalScreen * screen = terminal_widget -> screen_current;

	/* replace char if 'special' keymap is selected */
	if (screen -> character_set == '0'){
		gpointer cp = GINT_TO_POINTER((int)uchar);
		gpointer up = g_hash_table_lookup(terminal_widget -> map_specials, cp);
		if (up != NULL){
			uchar = GPOINTER_TO_INT(up);
		}
	}
	/* movement of cursor, scrolling */
	gboolean scrolling_region = screen -> crow >= screen -> scroll_top && screen -> crow <= screen -> scroll_bot;
	if (screen -> ccol > terminal_widget -> n_cols){
		// we're 'in' the right margin.
		if (scrolling_region){
			if (screen -> crow == screen -> scroll_bot){
				insert_lines(terminal_widget, 1);
			}
			if (screen -> crow != screen -> scroll_bot){
				screen -> crow += 1;
			}
		}else{
			if (screen -> crow < terminal_widget -> n_rows){
				screen -> crow += 1;
			}
		}
		screen -> ccol = 1;
	}
	/* make sure the row below the cursor exists */
	while (screen -> crow > screen -> rows -> len){
		GArray * row = g_array_new(FALSE, FALSE, sizeof(Pixel)); // LC
		//printf("rows8 %d %d:%d\n", rcc++, screen -> ccol, screen -> crow);
		g_array_append_val(screen -> rows, row);
	}

	GArray * row = g_array_index(screen -> rows, GArray*, screen -> crow - 1);
	int nrow = row -> len;
	//printf("pr %d, pc %d, nrow %d string: %s\n", pr, pc, nrow, str);
	if (nrow < screen -> ccol - 1){ // need to fill
		int x, fill = screen -> ccol - nrow - 1;
		for (x = 0; x < fill; x++){
			#if DEBUG_HANDLE_CHAR
			printf("|>|"); fflush(NULL);
			#endif
			Pixel blank;
			pixel_init_opaque(terminal_widget, &blank, (gunichar)' ');
			g_array_append_val(row, blank);
		}
		Pixel pixel;
		pixel_init(terminal_widget, &pixel, uchar);
		g_array_append_val(row, pixel);
	}else if(nrow == screen -> ccol - 1){ // fits perfectly. just append
		#if DEBUG_HANDLE_CHAR
		printf("|+|"); fflush(NULL);
		#endif
		Pixel pixel;
		pixel_init(terminal_widget, &pixel, uchar);
		g_array_append_val(row, pixel);
	}else{ // overwrite
		#if DEBUG_HANDLE_CHAR
		printf("|$|"); fflush(NULL);
		#endif
		if (screen -> ccol - 1 < row -> len){
			Pixel * pixel = &g_array_index(row, Pixel, screen -> ccol - 1);
			pixel_change(terminal_widget, pixel, uchar);
		}
	}
	screen -> ccol += 1;
	if(!internal){
		#if !LOCK_CHUNKWISE
		pthread_mutex_unlock(&terminal_widget -> mutex);
		#endif

		#if DRAW_EACH_ACTION
		gdk_threads_enter();
		gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
		gdk_threads_leave();
		#endif
	}
}

void terminal_widget_handle_char(TerminalHandler * terminal_handler, gunichar uchar)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	_terminal_widget_handle_char(terminal_widget, uchar, FALSE);
}

void terminal_widget_handle_csi(TerminalHandler * terminal_handler, Csi * csi)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	#if !LOCK_CHUNKWISE
	pthread_mutex_lock(&terminal_widget -> mutex);
	#endif
	TerminalScreen * screen = terminal_widget -> screen_current;
	//printf("||CSI:%c||", csi -> suffix1);
	if((csi -> suffix1 == 'h' || csi -> suffix1 == 'l') && csi -> prefix == '\0'){ 
		// set mode (<None>) / reset mode (RM)
		gboolean set = csi -> suffix1 == 'h'; // SET / RESET 
		if (csi -> nums -> len > 0){ // we expect a parameter here
			gpointer num = g_array_index(csi -> nums, gpointer, 0);
			int n = GPOINTER_TO_INT(num);
			switch(n){
				case 2:{ // keyboard action mode (AM)
					// turn off / on keyboard (KBD LOCKED)
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("TODO: %s ACTION MODE\n", set ? "SET" : "RST"); fflush(NULL);
					#endif
					break;
				}
				case 4:{ // insert mode (IRM)
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("TODO: %s INSERT MODE\n", set ? "SET" : "RST"); fflush(NULL);
					#endif
					break;
				}
				case 12:{ // send/receive (SRM)
					// local echo. set: disable, reset: enable
					// automatically display transmitted chars
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("TODO: %s SEND/RECEIVE MODE\n", set ? "SET" : "RST"); fflush(NULL);
					#endif
					break;
				}
				case 20:{ // automatic newline (NLM)
					// influences received linefeeds, vertical tabs, transmission of RETURN
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("TODO: %s AUTOMATIC NEWLINE MODE\n", set ? "SET" : "RST"); fflush(NULL);
					#endif
					printf("AUTO NEWLIN MODE\n");
					break;
				}
				default:{
					#if DEBUG_UNKNOWN_CSIS
					printf("||SET / RESET a %d||", n);
					#endif
					break;
				}
			}
		}
	}else if((csi -> suffix1 == 'h' || csi -> suffix1 == 'l') && csi -> prefix == '?'){ // DECSET / DECRST
		gboolean set = csi -> suffix1 == 'h'; // SET / RESET 
		int i;
		for (i = 0; i < csi -> nums -> len; i++){
			gpointer num = g_array_index(csi -> nums, gpointer, i);
			int n = GPOINTER_TO_INT(num);
			#if DEBUG_DECSET
			char * s = set ? "DECSET": "DECRST";
			printf("||%s %d||", s, n); fflush(NULL);
			#endif
			switch(n){
				case 1:{ // DECCKM
					// set: cursor keys transmit control (application) functions
					// reset: cursor keys transmit ANSI control sequences
					terminal_widget -> dec_ckm = set;
					break;
				}
				case 2:{ // VT52 Mode (DECANM)
					if (csi -> suffix1 == 'h'); ; // only 'h'
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("|TODO: DECANM|"); fflush(NULL);
					#endif
					break;
				}
				case 3:{ // Column Mode (DECCOLM)
					// set: 132 cols/line; reset: 80 cols/line
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("|TODO: DECCOLM|"); fflush(NULL);
					#endif
					break;
				}
				case 4:{ // Scroll Mode (DECSCLM)
					// set: smooth (6 lines/sec); reset: jump (fast as possible)
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("|TODO: DECSCLM|"); fflush(NULL);
					#endif
					break;
				}
				case 5:{ // Screen Mode (DECSCNM)
					// set: reverse screen (white screen, black chars)
					// reset: normal screen (black screen, white chars)
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("|TODO: DECSCNM|"); fflush(NULL);
					#endif
					break;
				}
				case 6:{ // Origin Mode (DECOM)
					// set: relative to scrolling region; reset: absolute
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("|TODO: DECOM|"); fflush(NULL);
					#endif
					break;
				}
				case 7:{ // Wrap Mode (DECAWM)
					// set: auto wrap (goto next line; scroll if neccessary)
					// reset: diable auto wrap (overwrite chars at end of line)
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("|TODO: DECAWM|"); fflush(NULL);
					#endif
					break;
				}
				case 8:{ // Auto Repeat Mode (DECARM)
					// enable / disable auto repeat of pressed keys
					#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
					printf("|TODO: DECARM|"); fflush(NULL);
					#endif
					break;
				}
				case 12:{
					#if DEBUG_KNOWN_CSIS || DEBUG_DECSET
					char * s = set ? "START" : "STOP";
					printf("||%s BLINKING||", s); fflush(NULL);
					#endif
					break;
				}
				case 25:{
					#if DEBUG_KNOWN_CSIS || DEBUG_DECSET
					char * s = set ? "SHOW" : "HIDE";
					printf("||%s CURSOR||", s); fflush(NULL);
					#endif
					terminal_widget -> cursor_visible = set;
					break;
				}
				case 1049:{
					#if DEBUG_KNOWN_CSIS || DEBUG_SCREENS
					char * s = set ? "alternate" : "normal";
					printf("||%s SCREEN||", s); fflush(NULL);
					#endif
					if (set){
						use_alternate_screen(terminal_widget);
					}else{
						use_normal_screen(terminal_widget);
					}
					break;
				}
				default:{
					#if DEBUG_UNKNOWN_CSIS
					printf("||SET / RESET b %d||", n);
					#endif
					break;
				}
			}
		}
	}else if (csi -> suffix1 == 'H'){ // goto
		int r = 1, c = 1; // appropriate default values
		if (csi -> nums -> len >= 1){
			gpointer num = g_array_index(csi -> nums, gpointer, 0);
			r = GPOINTER_TO_INT(num);
		}
		if (csi -> nums -> len >= 2){
			gpointer num = g_array_index(csi -> nums, gpointer, 1);
			c = GPOINTER_TO_INT(num);
		}
		#if DEBUG_KNOWN_CSIS || DEBUG_CURSOR
		printf("||GOTO:%d,%d||", r, c); fflush(NULL);
		#endif
		cursor_goto(terminal_widget, r, c);
	}else if(csi -> suffix1 == 'd'){ //this is bogus?
		int r = 1, c = 1; // appropriate default values
		if (csi -> nums -> len == 1){
			gpointer num = g_array_index(csi -> nums, gpointer, 0);
			r = GPOINTER_TO_INT(num);
		}
		if (csi -> nums -> len >= 2){
			gpointer numr = g_array_index(csi -> nums, gpointer, 0);
			r = GPOINTER_TO_INT(numr);
			gpointer numc = g_array_index(csi -> nums, gpointer, 0);
			c = GPOINTER_TO_INT(numc);
		}
		#if DEBUG_KNOWN_CSIS || DEBUG_CURSOR
		printf("||GOTO:%d,%d||", r, c); fflush(NULL);
		#endif
		terminal_widget -> screen_current -> crow = r;
		terminal_widget -> screen_current -> ccol = c >= 1 ? c : 1;
	}else if(csi -> suffix1 == '@'){ // insert n blank characters
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||insert %d blank chars||", n); fflush(NULL);
		#endif
		insert_blank_characters(terminal_widget, n);
	}else if(csi -> suffix1 == 'X'){ // erase n characters
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||erase %d chars||", n); fflush(NULL);
		#endif
		erase_characters(terminal_widget, n);
	}else if(csi -> suffix1 == 'P'){ // delete n characters 
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||delete %d chars||", n); fflush(NULL);
		#endif
		delete_characters(terminal_widget, n);
	}else if(csi -> suffix1 == 'G'){ // cursor character absolute
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||cursor char absolute %d||", n); fflush(NULL);
		#endif
		cursor_character_absolute(terminal_widget, n);
	}else if(csi -> suffix1 == 'A'){ // cursor up n times
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||cursor %d up||", n); fflush(NULL);
		#endif
		cursor_up(terminal_widget, n);
	}else if(csi -> suffix1 == 'B'){ // cursor down n times
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||cursor %d down||", n); fflush(NULL);
		#endif
		cursor_down(terminal_widget, n);
	}else if(csi -> suffix1 == 'C'){ // cursor forward n times
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||cursor %d forward||", n); fflush(NULL);
		#endif
		terminal_widget -> screen_current -> ccol += n;
	}else if(csi -> suffix1 == 'D'){ // cursor backward n times
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||cursor %d backwards (%d)||", n, screen -> ccol);
		#endif
		screen -> ccol -= n;
		if (screen -> ccol < 1) screen -> ccol = 1;
	}else if(csi -> suffix1 == 'K'){ // erase in line
		int n = get_value_or_default_from_csi(csi, 0);
		switch(n){
			case 0:{
				#if DEBUG_KNOWN_CSIS
				printf("||ERASE TO RIGHT %d:%d||", screen->ccol, screen->crow); fflush(NULL);
				#endif
				erase_to_the_right(terminal_widget);
				break;
			}
			case 1:{
				#if DEBUG_KNOWN_CSIS
				printf("||ERASE TO LEFT||"); fflush(NULL);
				#endif
				erase_to_the_left(terminal_widget);
				break;
			}
			case 2:{
				#if DEBUG_KNOWN_CSIS
				printf("||ERASE LINE||"); fflush(NULL);
				#endif
				erase_line(terminal_widget);
				break;
			}
		}
	}else if(csi -> suffix1 == 'L'){ // insert n lines
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_INSERT_LINES || DEBUG_KNOWN_CSIS
		printf("IL: CSI.L"); fflush(NULL);
		#endif
		insert_lines_before(terminal_widget, n);
	}else if(csi -> suffix1 == 'S'){ // scroll up n lines
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||scroll %d up||", n); fflush(NULL);
		#endif
		scroll_up(terminal_widget, n);
	}else if(csi -> suffix1 == 'T'){ // scroll down n lines
		int n = get_value_or_default_from_csi(csi, 1);
		//scroll_down(terminal_widget, n);
		#if DEBUG_KNOWN_CSIS
		printf("||scroll %d down||", n); fflush(NULL);
		#endif
		scroll_down(terminal_widget, n);
	}else if(csi -> suffix1 == 'M'){ // delete n lines
		int n = get_value_or_default_from_csi(csi, 1);
		#if DEBUG_KNOWN_CSIS
		printf("||delete %d lines||", n); fflush(NULL);
		#endif
		delete_lines(terminal_widget, n);
	}else if(csi -> suffix1 == 'J'){ // erase in display
		int n = get_value_or_default_from_csi(csi, 1);
		switch(n){ // 0: below, 1: above, 2: all, 3: saved lines (xterm)
			case 2:{
				#if DEBUG_KNOWN_CSIS
				printf("||ERASE IN DISPLAY, ALL||"); fflush(NULL);
				#endif
				erase_all(terminal_widget);
				break;
			}
			default:{
				#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
				printf("||TODO: ERASE IN DISPLAY %d\n||", n);
				#endif
			}
		}
	}else if(csi -> suffix1 == 'r' && csi -> prefix == '\0'){ //set scrolling region
		int t, b;
		t = 1;
		b = terminal_widget -> n_rows;
		if (csi -> nums -> len >= 2){
			gpointer numt = g_array_index(csi -> nums, gpointer, 0);
			gpointer numb = g_array_index(csi -> nums, gpointer, 1);
			t = GPOINTER_TO_INT(numt);
			b = GPOINTER_TO_INT(numb);
		}
		#if DEBUG_KNOWN_CSIS || DEBUG_SCROLLING_AREA
		printf("||SCROLLING AREA: %d:%d||", t, b); fflush(NULL);
		#endif
		terminal_widget -> screen_current -> scroll_top = t;
		terminal_widget -> screen_current -> scroll_bot = b;
	}else if(csi -> suffix1 == 'm'){
		int n = csi -> nums -> len;
		if (n == 0){
			// default value: 0
			set_colours(terminal_widget, 0);
		}else{
			int i;
			for (i = 0; i < n; i++){
				gpointer num = g_array_index(csi -> nums, gpointer, i);
				int x = GPOINTER_TO_INT(num);
				set_colours(terminal_widget, x);
			}
		}
	}else if(csi -> suffix1 == 'c' && csi -> prefix == '>'){ //send device attributes (secondary DA)
		#if DEBUG_KNOWN_BUT_UNHANDLED_CSIS
		printf("||TODO: DEVICE ATTRIBUTES, PLEASE||"); fflush(NULL);
		#endif
	}else{
		#if DEBUG_UNKNOWN_CSIS
		printf("||UNKNOWN Prefix: %c, Suffix1: %c, Suffix2: %c, Nums: ", csi -> prefix, csi -> suffix1, csi -> suffix2); fflush(NULL);
		int i;
		for (i = 0; i < csi -> nums -> len; i++){
			if (i != 0) printf(";");
			gpointer num = g_array_index(csi -> nums, gpointer, i);
			int n = GPOINTER_TO_INT(num);
			printf("%d", n);
		}
		printf("||");
		#endif
	}
	#if !LOCK_CHUNKWISE
	pthread_mutex_unlock(&terminal_widget -> mutex);
	#endif

	#if DRAW_EACH_ACTION
	gdk_threads_enter();
	gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
	gdk_threads_leave();
	#endif
}

void terminal_widget_handle_escaped(TerminalHandler * terminal_handler, char c)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	switch(c){
		case 'c':{ // Reset (RIS)
			#if DEBUG_KNOWN_ESCAPES
			printf("||Reset||"); fflush(NULL);
			#endif
			reset(terminal_widget);
			break;
		}
		case 'H':{ // Reset (RIS)
			#if DEBUG_KNOWN_ESCAPES
			printf("||Home Position||"); fflush(NULL);
			#endif
			cursor_goto(terminal_widget, 1, 1);
			break;
		}
		case 'D':{ // Index (IND)
			#if DEBUG_KNOWN_ESCAPES
			printf("||Index||"); fflush(NULL);
			#endif
			tw_index(terminal_widget);
			break;
		}
		case 'M':{ // Reverse Index (RI)
			#if DEBUG_KNOWN_ESCAPES
			printf("||Reverse Index||"); fflush(NULL);
			#endif
			tw_reverse_index(terminal_widget);
			break;
		}
		case 'E':{ // Next Line (NEL)
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("||TODO: Next Line||"); fflush(NULL);
			#endif
			break;
		}
		case '7':{ // Save Cursor (DECSC)
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("||TODO: Save Cursor||"); fflush(NULL);
			#endif
			break;
		}
		case '8':{ // Restore Cursor (DECRC)
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("||TODO: Restore Cursor||"); fflush(NULL);
			#endif
			break;
		}
		case '=':{ // Application Keypad Mode (DECKPAM)
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("\n\n\n");
			printf("||TODO: Application Keypad Mode||"); fflush(NULL);
			#endif
			break;
		}
		case '>':{ // Numeric Keypad Mode (DECKPNM)
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("\n\n\n");
			printf("||TODO: Numeric Keypad Mode||"); fflush(NULL);
			#endif
			break;
		}
		case 'N':{ // Single Shift 2 (SS2)
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("||TODO: Single Shift 2||"); fflush(NULL);
			#endif
			break;
		}
		case 'O':{ // Single Shift 3 (SS3)
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("||TODO: Single Shift 3||"); fflush(NULL);
			#endif
			break;
		}
		default:{
			#if DEBUG_KNOWN_BUT_UNHANDLED_ESCAPES
			printf("||UNKNOWN ESC<c>%c||", c); fflush(NULL);
			#endif
		}
	}
}

void terminal_widget_set_charset(TerminalHandler * terminal_handler, char c)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	//TODO: set charset
	switch(c){
	}
	TerminalScreen * screen = terminal_widget -> screen_current;
	screen -> character_set = c;
}

void terminal_widget_handle_ascii(TerminalHandler * terminal_handler, char c)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	#if !LOCK_CHUNKWISE
	pthread_mutex_lock(&terminal_widget -> mutex);
	#endif
	TerminalScreen * screen = terminal_widget -> screen_current;
	switch(c){
		case '\0':{
			break;
		}
		case '\a':{	// ^G : Bell 
			break;
		}
		case '\b':{	// ^H : Backspace
			screen -> ccol--;
			#if DEBUG_NEWLINE
			printf("BACKSPACE\n");
			#endif 
			break;
		}
		case '\t':{	// ^I : Tab Stop
			gboolean reverse = terminal_widget -> reverse; // toggle reversion temporarily (NOTE #1)
			terminal_widget -> reverse = FALSE;
			int x = screen -> ccol;
			int m = (x - 1) % 8;
			//x = x + (8 - m);
			//screen -> ccol = x;
			int i;
			for (i = 0; i < (8 - m); i++){
				_terminal_widget_handle_char(terminal_widget, (gunichar)' ', TRUE);
			}
			terminal_widget -> reverse = reverse;
			break;
		}
		case '\r':{	// ^M : Carriage Return
			//terminal_widget -> screen_current -> crow++;
			terminal_widget -> screen_current -> ccol = 1;
			break;
		}
		case '\n':{	// ^J : Line Feed
			#if DEBUG_NEWLINE
			printf("||NEWLINE cursor:%d:%d||", screen -> crow, screen -> ccol); fflush(NULL);
			#endif
			if (screen -> crow < screen -> scroll_bot){
				screen -> crow += 1;
				#if DEBUG_INSERT_LINES
				printf(" nrows:%d ", screen -> rows -> len); fflush(NULL);
				#endif 
			}else{
				#if DEBUG_INSERT_LINES
				printf(" IL: Linefeed"); fflush(NULL);
				#endif
				insert_lines(terminal_widget, 1);
			}
			break;
		}
		default:{
			if (((unsigned char)c) <= 31){
				_terminal_widget_handle_char(terminal_widget, (gunichar)'?', TRUE);
			}else{
				_terminal_widget_handle_char(terminal_widget, (gunichar)c, TRUE);
			}
			break;
		}
	}
	#if !LOCK_CHUNKWISE
	pthread_mutex_unlock(&terminal_widget -> mutex);
	#endif

	#if DRAW_EACH_ACTION
	gdk_threads_enter();
	gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
	gdk_threads_leave();
	#endif
}

void terminal_widget_set_fd(TerminalHandler * terminal_handler, int fd)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	terminal_widget -> master = fd;
}

void terminal_widget_chunk_begin(TerminalHandler * terminal_handler)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	#if LOCK_CHUNKWISE
	pthread_mutex_lock(&terminal_widget -> mutex);
	#endif
}

void terminal_widget_chunk_done(TerminalHandler * terminal_handler)
{
	TerminalWidget * terminal_widget = (TerminalWidget*) terminal_handler;
	#if LOCK_CHUNKWISE
	pthread_mutex_unlock(&terminal_widget -> mutex);
	#endif
	gdk_threads_enter();
	gtk_widget_queue_draw(GTK_WIDGET(terminal_widget));
	gdk_threads_leave();
}

/* Signals */

gboolean _emit_set_screen(TerminalWidget * terminal_widget)
{
	int id = (terminal_widget -> screen_current == terminal_widget -> screen_normal) ? 0 : 1;
	g_signal_emit (terminal_widget, terminal_widget_signals[SET_SCREEN], 0, id);
	return FALSE;
}

void emit_set_screen(TerminalWidget * terminal_widget)
{
	g_idle_add((GSourceFunc)_emit_set_screen, terminal_widget);
}

gboolean _emit_set_keypad(TerminalWidget * terminal_widget)
{
	int id = 0;
	g_signal_emit (terminal_widget, terminal_widget_signals[SET_KEYPAD], 0, id);
	return FALSE;
}

void emit_set_keypad(TerminalWidget * terminal_widget)
{
	g_idle_add((GSourceFunc)_emit_set_keypad, terminal_widget);
}

gboolean _emit_set_size(TerminalWidget * terminal_widget)
{
	int rows = terminal_widget -> n_rows;
	int cols = terminal_widget -> n_cols;
	g_signal_emit (terminal_widget, terminal_widget_signals[SET_SIZE], 0, cols, rows);
	return FALSE;
}

void emit_set_size(TerminalWidget * terminal_widget)
{
	g_idle_add((GSourceFunc)_emit_set_size, terminal_widget);
}

/* Selection */

void terminal_widget_selection_add(TerminalScreen * screen, StringBuffer * buffer, int r, int c_start, int c_stop)
{
	//printf("%d %d\n", r, screen -> rows -> len);
	if (r >= screen -> rows -> len){
		return;
	}
	GArray * row = g_array_index(screen -> rows, GArray*, r);
	int c;
	if (c_stop == -1) c_stop = row -> len - 1;
	for (c = c_start; c <= c_stop && c < row -> len; c++){
		Pixel * pixel = &g_array_index(row, Pixel, c);
		string_buffer_append_unichar(buffer, pixel -> uchar);
	}
}

gboolean terminal_widget_get_selected_text(TerminalWidget * terminal_widget, StringBuffer ** buffer)
{
	if (!terminal_widget -> selection_active){
		return FALSE;
	}

	TerminalScreen * screen = terminal_widget -> screen_current;

	int c1 = terminal_widget -> selection.start.col;
	int r1 = terminal_widget -> selection.start.row;
	int c2 = terminal_widget -> selection.end.col;
	int r2 = terminal_widget -> selection.end.row;

	*buffer = string_buffer_new(1);
	if (r1 == r2){
		terminal_widget_selection_add(screen, *buffer, r1, c1, c2);
	}else{
		terminal_widget_selection_add(screen, *buffer, r1, c1, -1);
		string_buffer_append_string(*buffer, "\n", 1);
		int r;
		for (r = r1 + 1; r < r2; r++){
			terminal_widget_selection_add(screen, *buffer, r, 0, -1);
			string_buffer_append_string(*buffer, "\n", 1);
		}
		terminal_widget_selection_add(screen, *buffer, r2, 0, c2);
	}

	return TRUE;
}

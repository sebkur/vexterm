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

#ifndef _TERMINAL_CONFIG_H_
#define _TERMINAL_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "terminal_colour_palette.h"

#define LIBVEX_TYPE_TERMINAL_CONFIG           (terminal_config_get_type ())
#define LIBVEX_TERMINAL_CONFIG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_TERMINAL_CONFIG, TerminalConfig))
#define LIBVEX_TERMINAL_CONFIG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_TERMINAL_CONFIG, TerminalConfigClass))
#define LIBVEX_IS_TERMINAL_CONFIG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_TERMINAL_CONFIG))
#define LIBVEX_IS_TERMINAL_CONFIG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_TERMINAL_CONFIG))
#define LIBVEX_TERMINAL_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_TERMINAL_CONFIG, TerminalConfigClass))

typedef struct _TerminalConfig        TerminalConfig;
typedef struct _TerminalConfigClass   TerminalConfigClass;

struct _TerminalConfig
{
	GObject parent;

	char * font_name;
	int font_size;
	TerminalColourPalette * colour_palette;
};

struct _TerminalConfigClass
{
	GObjectClass parent_class;

	void (* changed_font) (TerminalConfig *terminal_config);
	void (* changed_font_name) (TerminalConfig *terminal_config);
	void (* changed_font_size) (TerminalConfig *terminal_config);
	void (* changed_colour_palette) (TerminalConfig *terminal_config);
};

GType terminal_config_get_type (void) G_GNUC_CONST;

TerminalConfig * terminal_config_new();

void 		terminal_config_set_font(TerminalConfig * tc, char * font_name, int font_size);
void 		terminal_config_set_font_name(TerminalConfig * tc, char * font_name);
void 		terminal_config_set_font_size(TerminalConfig * tc, int font_size);
void 		terminal_config_set_colour_palette(TerminalConfig * tc, TerminalColourPalette * palette);

char * 		terminal_config_get_font_name(TerminalConfig * tc);
int 		terminal_config_get_font_size(TerminalConfig * tc);
TerminalColourPalette * terminal_config_get_colour_palette(TerminalConfig * tc);

#endif /* _TERMINAL_CONFIG_H_ */

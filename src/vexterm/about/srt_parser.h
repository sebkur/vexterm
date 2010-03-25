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

#ifndef _SRT_PARSER_H_
#define _SRT_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define VEX_TYPE_SRT_PARSER           (srt_parser_get_type ())
#define VEX_SRT_PARSER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_SRT_PARSER, SrtParser))
#define VEX_SRT_PARSER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_SRT_PARSER, SrtParserClass))
#define VEX_IS_SRT_PARSER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_SRT_PARSER))
#define VEX_IS_SRT_PARSER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_SRT_PARSER))
#define VEX_SRT_PARSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_SRT_PARSER, SrtParserClass))

typedef struct _SrtParser        SrtParser;
typedef struct _SrtParserClass   SrtParserClass;

struct _SrtParser
{
	GObject parent;

	GtkTextBuffer * buffer;
	GtkTextIter iter;
	GArray * styles;

	char * sbuffer;
	int sbuffer_len;
};

struct _SrtParserClass
{
	GObjectClass parent_class;

	//void (* function_name) (SrtParser *srt_parser);
};

GType srt_parser_get_type (void) G_GNUC_CONST;

SrtParser * srt_parser_new();

GtkTextBuffer * srt_parser_parse_file(SrtParser * srt, char * filename);

#endif /* _SRT_PARSER_H_ */

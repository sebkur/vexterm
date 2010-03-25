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

#define DEBUG_XML 0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <expat.h>

#include "srt_parser.h"

G_DEFINE_TYPE (SrtParser, srt_parser, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint srt_parser_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, srt_parser_signals[SIGNAL_NAME_n], 0);

SrtParser * srt_parser_new()
{
	SrtParser * srt_parser = g_object_new(VEX_TYPE_SRT_PARSER, NULL);
	srt_parser -> sbuffer = NULL;
	srt_parser -> sbuffer_len = 0;
	return srt_parser;
}

static void srt_parser_class_init(SrtParserClass *class)
{
        /*srt_parser_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SrtParserClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void srt_parser_init(SrtParser *srt_parser)
{
}

gboolean style_any_active(SrtParser * srt_parser)
{
	return srt_parser -> styles -> len > 0;
}

void style_push(SrtParser * srt_parser, char * style)
{
	g_array_append_val(srt_parser -> styles, style);
}

char * style_top(SrtParser * srt_parser)
{
	if (srt_parser -> styles -> len >= 1){
		int n = srt_parser -> styles -> len - 1;
		return g_array_index(srt_parser -> styles, char*, n);
	}
	return NULL;
}

char * style_pop(SrtParser * srt_parser)
{
	if (srt_parser -> styles -> len >= 1){
		int n = srt_parser -> styles -> len - 1;
		char * s = g_array_index(srt_parser -> styles, char*, n);
		g_array_remove_index(srt_parser -> styles, n);
		return s;
	}
	return NULL;
}

void srt_parser_process_stringbuffer(SrtParser * srt)
{
	/* put received chars into textbuffer */
	if (srt -> sbuffer != NULL){
		char * source = srt -> sbuffer;
		int len = srt -> sbuffer_len;
		//printf("sbuffer: %d\n", len);
		char * n, * c = source;
		char * t = malloc(sizeof(char) * (len + 1));
		char * u = t;
		int rlen = 0;
		while (c - source < len){
			n = g_utf8_find_next_char(c, NULL);
			//printf("%p\n", n);
			if (n == NULL) printf("NULL\n");
			int l = n - c;
			if (c[0] != '\n' && c[0] != '\0'){
				const char * valide;
				//printf("validate %p %p\n", c, l);
				gboolean v = g_utf8_validate(c, l, &valide);
				if(v){
					memcpy(u, c, l);
					u += l;
					rlen += l;
				}else{
					printf("invalid %d %d\n", rlen, l); fflush(NULL);
					int i;
					for (i = 0; i < rlen; i++){
						//printf("%d%c ", source[i], source[i]);
					}
				}
			}
			c = n;
			//printf("%d\n", c - source);
		}
		t[rlen] = '\0';
		//printf("rlen %d\n", rlen);
		if (rlen > 0){
			GtkTextMark * mark = gtk_text_mark_new(NULL, TRUE);
			gtk_text_buffer_add_mark(srt -> buffer, mark, &srt -> iter);
			gtk_text_buffer_insert(srt -> buffer, 
				&srt -> iter, t, rlen);

			GtkTextIter old;
			gtk_text_buffer_get_iter_at_mark(srt -> buffer, &old, mark);
			gtk_text_buffer_delete_mark(srt -> buffer, mark);
			g_object_unref(mark);

			char * style = style_top(srt);
			if (style != NULL){
				gtk_text_buffer_apply_tag_by_name(srt -> buffer, 
					style, &old, &srt -> iter);
			}
		}
		free(t);

		free(srt -> sbuffer);
		srt -> sbuffer_len = 0;
		srt -> sbuffer = NULL;
	}
}

const char * srt_parser_get_value(const XML_Char ** atts, char * search_key)
{
	const XML_Char ** ptr = atts;
	while(*ptr != NULL){
		const XML_Char * key = *ptr;
		const XML_Char * value = *(ptr + 1);
		if (strcmp(key, search_key) == 0){
			return value;
		}
		ptr = ptr + 2;
	}
	return NULL;
}

static void XMLCALL srt_parser_StartElementCallback(
						void * userData,
						const XML_Char * name,
						const XML_Char ** atts)
{
	SrtParser * srt = (SrtParser*)userData;
	#if DEBUG_XML
	printf("start %s\n", name);
	#endif

	srt_parser_process_stringbuffer(srt);

	if (strcmp(name, "font") == 0){
		const char * style = srt_parser_get_value(atts, "style");
		if (style == NULL){
			style_push(srt, NULL);
		}else{
			style_push(srt, g_strdup(style));
		}
	}else if (strcmp(name, "br") == 0){
		gtk_text_buffer_insert(srt -> buffer, 
			&srt -> iter, "\n", 1);
	}else if (strcmp(name, "style") == 0){
		//name, size, font, weight
		const char * name = srt_parser_get_value(atts, "name");
		const char * size = srt_parser_get_value(atts, "size");
		const char * font = srt_parser_get_value(atts, "font");
		const char * weight = srt_parser_get_value(atts, "weight");
		if (name == NULL) return;
		#if DEBUG_XML
		printf("adding style: %s\n", name);
		#endif
		GtkTextTag * tag = gtk_text_buffer_create_tag(srt -> buffer, name, NULL);
		if (size != NULL){
			g_object_set(tag, "size", atoi(size) * PANGO_SCALE, NULL);
		}
		if (font != NULL){
			g_object_set(tag, "font", font, NULL);
		}
		if (weight != NULL){
			if (strcmp(weight, "bold") == 0){
				g_object_set(tag, "weight", PANGO_WEIGHT_BOLD, NULL);
			}
		}
	}/*else if(strcmp(name, "p") == 0){
		GtkTextTag * tag = gtk_text_buffer_create_tag(srt -> buffer, NULL, NULL);
		g_object_set(tag, "left-margin", 20, NULL);
		gtk_text_buffer_insert_with_tags(srt -> buffer, 
			&srt -> iter, " ", 1, tag, NULL);
	}*/
}

static void XMLCALL srt_parser_EndElementCallback(
					void * userData,
					const XML_Char * name)
{
	SrtParser * srt = (SrtParser*)userData;
	#if DEBUG_XML
	printf("end %s\n", name);
	#endif

	srt_parser_process_stringbuffer(srt);

	/* update style information */
	if (strcmp(name, "font") == 0){
		char * s = style_pop(srt);
		if (s != NULL) free(s);
	}
}

static void XMLCALL srt_parser_CharacterDataCallback(
					void * userData,
					const XML_Char * s,
					int len)
{
	SrtParser * srt = (SrtParser*)userData;
	//printf("%d new chars\n", len);
	if (!style_any_active(srt)){
		return;
	}

	char * destination;
	if (srt -> sbuffer == NULL){
		srt -> sbuffer = malloc(sizeof(char) * (len + 1));
		srt -> sbuffer_len = len;
		destination = srt -> sbuffer;
		//printf("new len %d\n", len);
	}else{
		int sbuffer_len_old = srt -> sbuffer_len;
		int sbuffer_len_new = srt -> sbuffer_len + len;
		char * sbuffer_new = malloc(sizeof(char) * (sbuffer_len_new + 1));
		memcpy(sbuffer_new, srt -> sbuffer, srt -> sbuffer_len);
		free(srt -> sbuffer);
		srt -> sbuffer = sbuffer_new;
		srt -> sbuffer_len = sbuffer_len_new;
		destination = &srt -> sbuffer[sbuffer_len_old];
		//printf("widened len %d\n", srt -> sbuffer_len);
	}
	memcpy(destination, s, len);
	destination[len] = '\0';
}

GtkTextBuffer * srt_parser_parse_file(SrtParser * srt, char * filename)
{
	srt -> styles = g_array_new(FALSE, FALSE, sizeof(char*));
	srt -> buffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_get_iter_at_offset(srt -> buffer, &srt -> iter, 0);

	struct stat sb;
	int s = stat(filename, &sb);
	if (s < 0){
		printf("could not open file, stat failed\n");
		return NULL;
	}
	int f = open(filename, O_RDONLY);
	if (f == 0) {
		printf("could not open file, open failed\n");
		return NULL;
	}

	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, srt);
	XML_SetStartElementHandler(parser, srt_parser_StartElementCallback);
	XML_SetEndElementHandler(parser, srt_parser_EndElementCallback);
	XML_SetCharacterDataHandler(parser, srt_parser_CharacterDataCallback);

	ssize_t size;
	int n = 1024 * 4;
	char buf[n];
	do{
		size = read(f, buf, n);
		XML_Parse(parser, buf, size, 0);
	}while(size > 0);
	XML_Parse(parser, NULL, 0, 1);
	XML_ParserFree(parser);

	return srt -> buffer;
}

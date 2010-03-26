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

#ifndef _STRING_BUFFER_H_
#define _STRING_BUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <glib-object.h>

#define VEX_TYPE_STRING_BUFFER           (string_buffer_get_type ())
#define VEX_STRING_BUFFER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_STRING_BUFFER, StringBuffer))
#define VEX_STRING_BUFFER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_STRING_BUFFER, StringBufferClass))
#define VEX_IS_STRING_BUFFER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_STRING_BUFFER))
#define VEX_IS_STRING_BUFFER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_STRING_BUFFER))
#define VEX_STRING_BUFFER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_STRING_BUFFER, StringBufferClass))

GType string_buffer_get_type (void) G_GNUC_CONST; 

typedef struct _StringBuffer        StringBuffer;
typedef struct _StringBufferClass   StringBufferClass;

struct _StringBuffer
{
	GObject parent;

	char * buffer;
	int buffer_len;
	int text_len;
};

struct _StringBufferClass
{
	GObjectClass parent_class;

	//void (* function_name) (StringBuffer *string_buffer);
};

StringBuffer * string_buffer_new(int initial_size);

void string_buffer_append_string(StringBuffer * sbuffer, char * string, int len);
void string_buffer_append_unichar(StringBuffer * sbuffer, gunichar c);

char * string_buffer_get_text(StringBuffer * sbuffer, int * len);

void string_buffer_free(StringBuffer * sbuffer);

#endif /* _STRING_BUFFER_H_ */

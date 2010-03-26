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
#include <string.h>

#include "string_buffer.h"

G_DEFINE_TYPE (StringBuffer, string_buffer, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint string_buffer_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, string_buffer_signals[SIGNAL_NAME_n], 0);

static void string_buffer_class_init(StringBufferClass *class)
{
        /*string_buffer_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (StringBufferClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void string_buffer_init(StringBuffer *string_buffer)
{
}

StringBuffer * string_buffer_new(int initial_size)
{
	StringBuffer * sbuffer = g_object_new(VEX_TYPE_STRING_BUFFER, NULL);

	if (initial_size < 1) initial_size = 1;
	sbuffer -> buffer = malloc(sizeof(char) * initial_size);
	sbuffer -> buffer_len = initial_size;
	sbuffer -> text_len = 0;

	return sbuffer;
}

void string_buffer_append_string(StringBuffer * sbuffer, char * string, int len)
{
	int sum = sbuffer -> text_len + len;
	if (sum > sbuffer -> buffer_len){
		// buffer is too small. we're going to double size
		int buffer_len_new = sbuffer -> buffer_len;
		while(sum > buffer_len_new){
			// or even 2^n if necessary
			buffer_len_new *= 2;
		}
		char * buffer_new = malloc(sizeof(char) * buffer_len_new);
		memcpy(buffer_new, sbuffer -> buffer, sbuffer -> text_len);
		free(sbuffer -> buffer);
		sbuffer -> buffer = buffer_new;
		sbuffer -> buffer_len = buffer_len_new;
	}
	memcpy(&sbuffer -> buffer[sbuffer -> text_len], string, len);
	sbuffer -> text_len += len;
}

void string_buffer_append_unichar(StringBuffer * sbuffer, gunichar c)
{
	char buf[6];
	int bytes = g_unichar_to_utf8(c, &buf);
	string_buffer_append_string(sbuffer, &buf, bytes);
}

char * string_buffer_get_text(StringBuffer * sbuffer, int * len)
{
	*len = sbuffer -> text_len;
	return sbuffer -> buffer;
}

void string_buffer_free(StringBuffer * sbuffer)
{
	free(sbuffer -> buffer);
}

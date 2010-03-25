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

#include "xml_document.h"

G_DEFINE_TYPE (XmlDocument, xml_document, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint xml_document_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, xml_document_signals[SIGNAL_NAME_n], 0);

XmlDocument * xml_document_new()
{
	XmlDocument * xml_document = g_object_new(VEX_TYPE_XML_DOCUMENT, NULL);
	return xml_document;
}

static void xml_document_class_init(XmlDocumentClass *class)
{
        /*xml_document_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (XmlDocumentClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void xml_document_init(XmlDocument *xml_document)
{
}

XmlNode * xml_document_get_root(XmlDocument * document)
{
	return document -> root;
}

void xml_document_set_root(XmlDocument * document, XmlNode * root)
{
	document -> root = root;
}

void xml_document_print(XmlDocument * document)
{
	xml_document_write(document, 1);
}

void xml_document_write_node(XmlNode * node, int fd, int intendation);

void xml_document_write(XmlDocument * document, int fd)
{
	if (document -> root == NULL) return;

	xml_document_write_node(document -> root, fd, 0);
}

void write_tabs(int fd, int intendation)
{
	int i;
	for (i = 0; i < intendation; i++){
		write(fd, "\t", 1);
	}
}

void xml_document_write_node(XmlNode * node, int fd, int intendation)
{
	char * buf;

	write_tabs(fd, intendation);
	buf = g_strdup_printf("<%s", node -> name);
	write(fd, buf, strlen(buf));
	free(buf);
	GSequenceIter * i;
	for (i = g_sequence_get_begin_iter(node -> tags); !g_sequence_iter_is_end(i); i = g_sequence_iter_next(i)){
		XmlKeyValue * kv = (XmlKeyValue*)g_sequence_get(i);
		buf = g_strdup_printf(" %s=\"%s\"", kv -> key, kv -> value);
		write(fd, buf, strlen(buf));
	}
	if (g_sequence_get_length(node -> childs) == 0){
		write(fd, " />\n", 4);
	}else{
		write(fd, ">\n", 2);
		for (i = g_sequence_get_begin_iter(node -> childs); !g_sequence_iter_is_end(i); i = g_sequence_iter_next(i)){
			XmlNode * child = (XmlNode*)g_sequence_get(i);
			xml_document_write_node(child, fd, intendation + 1);
		}
		write_tabs(fd, intendation);
		buf = g_strdup_printf("</%s>\n", node -> name);
		write(fd, buf, strlen(buf));
		free(buf);
	}
}

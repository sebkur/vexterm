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

#include "xml_node.h"

G_DEFINE_TYPE (XmlNode, xml_node, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint xml_node_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, xml_node_signals[SIGNAL_NAME_n], 0);

XmlNode * xml_node_new(char * name)
{
	XmlNode * xml_node = g_object_new(VEX_TYPE_XML_NODE, NULL);
	xml_node -> name = g_strdup(name);
	return xml_node;
}

static void xml_node_class_init(XmlNodeClass *class)
{
        /*xml_node_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (XmlNodeClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void xml_node_init(XmlNode *xml_node)
{
	xml_node -> tags = g_sequence_new(NULL);
	xml_node -> childs = g_sequence_new(NULL);
}

void xml_node_add_tag(XmlNode * node, char * key, char * value)
{
	XmlKeyValue * kv = g_slice_alloc(sizeof(XmlKeyValue));
	kv -> key = g_strdup(key);
	kv -> value = g_strdup(value);
	g_sequence_append(node -> tags, kv);
}

void xml_node_add_child(XmlNode * node, XmlNode * child)
{
	g_sequence_append(node -> childs, child);
}


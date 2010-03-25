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

#ifndef _XML_NODE_H_
#define _XML_NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <glib-object.h>

#define VEX_TYPE_XML_NODE           (xml_node_get_type ())
#define VEX_XML_NODE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_XML_NODE, XmlNode))
#define VEX_XML_NODE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_XML_NODE, XmlNodeClass))
#define VEX_IS_XML_NODE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_XML_NODE))
#define VEX_IS_XML_NODE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_XML_NODE))
#define VEX_XML_NODE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_XML_NODE, XmlNodeClass))

GType xml_node_get_type (void) G_GNUC_CONST; 

typedef struct _XmlNode        XmlNode;
typedef struct _XmlNodeClass   XmlNodeClass;

typedef struct {
	char * key;
	char * value;
} XmlKeyValue;

struct _XmlNode
{
	GObject parent;

	char * name;
	GSequence * tags;
	GSequence * childs;
};

struct _XmlNodeClass
{
	GObjectClass parent_class;

	//void (* function_name) (XmlNode *xml_node);
};

XmlNode * xml_node_new(char * name);

void xml_node_add_tag(XmlNode * node, char * key, char * value);
void xml_node_add_child(XmlNode * node, XmlNode * child);

#endif /* _XML_NODE_H_ */

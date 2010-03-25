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

#ifndef _XML_DOCUMENT_H_
#define _XML_DOCUMENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <glib-object.h>

#include "xml_node.h"

#define VEX_TYPE_XML_DOCUMENT           (xml_document_get_type ())
#define VEX_XML_DOCUMENT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_XML_DOCUMENT, XmlDocument))
#define VEX_XML_DOCUMENT_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_XML_DOCUMENT, XmlDocumentClass))
#define VEX_IS_XML_DOCUMENT(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_XML_DOCUMENT))
#define VEX_IS_XML_DOCUMENT_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_XML_DOCUMENT))
#define VEX_XML_DOCUMENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_XML_DOCUMENT, XmlDocumentClass))

GType xml_document_get_type (void) G_GNUC_CONST; 

typedef struct _XmlDocument        XmlDocument;
typedef struct _XmlDocumentClass   XmlDocumentClass;

struct _XmlDocument
{
	GObject parent;

	XmlNode * root;
};

struct _XmlDocumentClass
{
	GObjectClass parent_class;

	//void (* function_name) (XmlDocument *xml_document);
};

XmlDocument * xml_document_new();

XmlNode * xml_document_get_root(XmlDocument * document);
void xml_document_set_root(XmlDocument * document, XmlNode * root);

void xml_document_print(XmlDocument * document);
void xml_document_write(XmlDocument * document, int fd);

#endif /* _XML_DOCUMENT_H_ */

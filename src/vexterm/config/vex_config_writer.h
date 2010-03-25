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

#ifndef _VEX_CONFIG_WRITER_H_
#define _VEX_CONFIG_WRITER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_config.h"

#define VEX_TYPE_VEX_CONFIG_WRITER           (vex_config_writer_get_type ())
#define VEX_VEX_CONFIG_WRITER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_CONFIG_WRITER, VexConfigWriter))
#define VEX_VEX_CONFIG_WRITER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_CONFIG_WRITER, VexConfigWriterClass))
#define VEX_IS_VEX_CONFIG_WRITER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_CONFIG_WRITER))
#define VEX_IS_VEX_CONFIG_WRITER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_CONFIG_WRITER))
#define VEX_VEX_CONFIG_WRITER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_CONFIG_WRITER, VexConfigWriterClass))

GType vex_config_writer_get_type (void) G_GNUC_CONST; 

typedef struct _VexConfigWriter        VexConfigWriter;
typedef struct _VexConfigWriterClass   VexConfigWriterClass;

struct _VexConfigWriter
{
	GObject parent;
};

struct _VexConfigWriterClass
{
	GObjectClass parent_class;

	//void (* function_name) (VexConfigWriter *vex_config_writer);
};

VexConfigWriter * vex_config_writer_new();

void vex_config_writer_write_config_to_file(VexConfigWriter * vcw, char * file, VexConfig * config);

#endif /* _VEX_CONFIG_WRITER_H_ */

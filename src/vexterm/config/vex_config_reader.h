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

#ifndef _VEX_CONFIG_READER_H_
#define _VEX_CONFIG_READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_config.h"
#include "vex_profile.h"
#include "vex_colour_scheme.h"

#define LIBVEX_TYPE_VEX_CONFIG_READER           (vex_config_reader_get_type ())
#define LIBVEX_VEX_CONFIG_READER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_VEX_CONFIG_READER, VexConfigReader))
#define LIBVEX_VEX_CONFIG_READER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_VEX_CONFIG_READER, VexConfigReaderClass))
#define LIBVEX_IS_VEX_CONFIG_READER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_VEX_CONFIG_READER))
#define LIBVEX_IS_VEX_CONFIG_READER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_VEX_CONFIG_READER))
#define LIBVEX_VEX_CONFIG_READER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_VEX_CONFIG_READER, VexConfigReaderClass))

typedef struct _VexConfigReader        VexConfigReader;
typedef struct _VexConfigReaderClass   VexConfigReaderClass;

struct _VexConfigReader
{
	GObject parent;

	VexConfig * vex_config;

	GArray * nesting;

	VexColourScheme * colour_scheme;
	char * 	 colour_scheme_name;
	int colour_scheme_colour_count;

	VexProfile * profile;
	char * profile_name;
};

struct _VexConfigReaderClass
{
	GObjectClass parent_class;

	//void (* function_name) (VexConfigReader *vex_config_reader);
};

GType vex_config_reader_get_type (void) G_GNUC_CONST;

VexConfigReader * vex_config_reader_new();

void vex_config_reader_fill_config_from_file(VexConfigReader * vcr, char * file, VexConfig * vex_config);

#endif /* _VEX_CONFIG_READER_H_ */

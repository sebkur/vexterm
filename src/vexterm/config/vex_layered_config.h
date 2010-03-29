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

#ifndef _VEX_LAYERED_CONFIG_H_
#define _VEX_LAYERED_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_config.h"
#include "vex_layered_config.h"

#define VEX_TYPE_VEX_LAYERED_CONFIG           (vex_layered_config_get_type ())
#define VEX_VEX_LAYERED_CONFIG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_LAYERED_CONFIG, VexLayeredConfig))
#define VEX_VEX_LAYERED_CONFIG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_LAYERED_CONFIG, VexLayeredConfigClass))
#define VEX_IS_VEX_LAYERED_CONFIG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_LAYERED_CONFIG))
#define VEX_IS_VEX_LAYERED_CONFIG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_LAYERED_CONFIG))
#define VEX_VEX_LAYERED_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_LAYERED_CONFIG, VexLayeredConfigClass))

GType vex_layered_config_get_type (void) G_GNUC_CONST; 

typedef struct _VexLayeredConfig        VexLayeredConfig;
typedef struct _VexLayeredConfigClass   VexLayeredConfigClass;

struct _VexLayeredConfig
{
	GObject parent;

	VexConfig * config_global;
	VexConfig * config_local;

	GSequence * profiles;
	GSequence * colour_schemes;
};

struct _VexLayeredConfigClass
{
	GObjectClass parent_class;

	void (* profile_added) (
		VexLayeredConfig *vex_layered_config, ConfigLevel level, char * name);
	void (* profile_renamed) (
		VexLayeredConfig *vex_layered_config, ConfigLevel level, char * old_name, char * new_name);
	void (* profile_removed) (
		VexLayeredConfig *vex_layered_config, ConfigLevel level, char * name, VexProfile * vex_profile);

	void (* colour_scheme_added) (
		VexLayeredConfig *vex_layered_config, ConfigLevel level, char * name);
	void (* colour_scheme_renamed) (
		VexLayeredConfig *vex_layered_config, ConfigLevel level, char * old_name, char * new_name);
	void (* colour_scheme_removed) (
		VexLayeredConfig *vex_layered_config, ConfigLevel level, char * name, VexColourScheme * vex_colour_scheme);
};

VexLayeredConfig * vex_layered_config_new(VexConfig * config_global, VexConfig * config_local);

/* GETTERS */
VexConfig * vex_layered_config_get_config_global(VexLayeredConfig * vex_layered_config);
VexConfig * vex_layered_config_get_config_local(VexLayeredConfig * vex_layered_config);

gboolean vex_layered_config_get_show_scrolling_region(VexLayeredConfig * vex_layered_config);
gboolean vex_layered_config_get_show_status_bar(VexLayeredConfig * vex_layered_config);
GtkPositionType vex_layered_config_get_tabs_position(VexLayeredConfig * vex_layered_config);
char * vex_layered_config_get_active_profile(VexLayeredConfig * vex_layered_config);

int vex_layered_config_get_number_of_profiles(VexLayeredConfig * vex_layered_config);
int vex_layered_config_get_number_of_colour_schemes(VexLayeredConfig * vex_layered_config);

VexProfile * vex_layered_config_get_profile_by_index(
	VexLayeredConfig * vex_layered_config, int index, char ** name, ConfigLevel * level);

VexColourScheme * vex_layered_config_get_colour_scheme_by_index(
	VexLayeredConfig * vex_layered_config, int index, char ** name, ConfigLevel * level);

VexProfile * vex_layered_config_get_profile_by_name(VexLayeredConfig * vlc, char * name);
VexColourScheme * vex_layered_config_get_colour_scheme_by_name(VexLayeredConfig * vlc, char * name);

#endif /* _VEX_LAYERED_CONFIG_H_ */

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

#ifndef _VEX_CONFIG_H_
#define _VEX_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_profile.h"
#include "vex_colour_scheme.h"

#define VEX_TYPE_VEX_CONFIG           (vex_config_get_type ())
#define VEX_VEX_CONFIG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_CONFIG, VexConfig))
#define VEX_VEX_CONFIG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_CONFIG, VexConfigClass))
#define VEX_IS_VEX_CONFIG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_CONFIG))
#define VEX_IS_VEX_CONFIG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_CONFIG))
#define VEX_VEX_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_CONFIG, VexConfigClass))

typedef struct _VexConfig        VexConfig;
typedef struct _VexConfigClass   VexConfigClass;

typedef enum{
	CONFIG_LEVEL_GLOBAL,
	CONFIG_LEVEL_LOCAL
} ConfigLevel;

struct _VexConfig
{
	GObject parent;

	gboolean	show_scrolling_region;
	gboolean	show_status_bar;

	char * 		active_profile;
	GSequence *	profiles;
	GSequence *	colour_schemes;
};

struct _VexConfigClass
{
	GObjectClass parent_class;

	void (* profile_added) (VexConfig *vex_config, char * name);
	void (* profile_renamed) (VexConfig *vex_config, char * old_name, char * new_name);
	void (* profile_removed) (VexConfig *vex_config, char * name, VexProfile * vex_profile);

	void (* colour_scheme_added) (VexConfig *vex_config, char * name);
	void (* colour_scheme_renamed) (VexConfig *vex_config, char * old_name, char * new_name);
	void (* colour_scheme_removed) (VexConfig *vex_config, char * name, VexColourScheme * vex_colour_scheme);
};

GType vex_config_get_type (void) G_GNUC_CONST;

VexConfig * vex_config_new();

void vex_config_deepcopy(VexConfig * config_from, VexConfig * config_to);

/* SETTERS / ADDERS / REMOVERS */
void vex_config_set_show_scrolling_region(VexConfig * vex_config, gboolean show);
void vex_config_set_show_status_bar(VexConfig * vex_config, gboolean show);
void vex_config_set_active_profile(VexConfig * vex_config, char * name);
void vex_config_add_profile(VexConfig * vex_config, const char * name, VexProfile * profile);
void vex_config_add_profile_defaults(VexConfig * vex_config, const char * name);
void vex_config_add_colour_scheme(VexConfig * vex_config, const char * name, VexColourScheme * colour_scheme);
void vex_config_add_colour_scheme_defaults(VexConfig * vex_config, const char * name);
void vex_config_remove_profile(VexConfig * vex_config, const char * name);
void vex_config_remove_colour_scheme(VexConfig * vex_config, const char * name);
void vex_config_rename_profile(VexConfig * vex_config, const char * name, const char * new_name);
void vex_config_rename_colour_scheme(VexConfig * vex_config, const char * name, const char * new_name);

/* GETTERS */
gboolean vex_config_get_show_scrolling_region(VexConfig * vex_config);
gboolean vex_config_get_show_status_bar(VexConfig * vex_config);
char * vex_config_get_active_profile(VexConfig * vex_config);

int vex_config_get_number_of_profiles(VexConfig * vex_config);
int vex_config_get_number_of_colour_schemes(VexConfig * vex_config);

VexProfile * vex_config_get_profile_by_index(VexConfig * vex_config, int index, char ** name);
VexProfile * vex_config_get_profile_by_name(VexConfig * vex_config, const char * name);

VexColourScheme * vex_config_get_colour_scheme_by_index(VexConfig * vex_config, int index, char ** name);
VexColourScheme * vex_config_get_colour_scheme_by_name(VexConfig * vex_config, const char * name);

#endif /* _VEX_CONFIG_H_ */

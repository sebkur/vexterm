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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_layered_config.h"

G_DEFINE_TYPE (VexLayeredConfig, vex_layered_config, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint vex_layered_config_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, vex_layered_config_signals[SIGNAL_NAME_n], 0);

void vex_layered_config_constructor(VexLayeredConfig * vlc);

VexLayeredConfig * vex_layered_config_new(VexConfig * config_global, VexConfig * config_local)
{
	VexLayeredConfig * vlc = g_object_new(VEX_TYPE_VEX_LAYERED_CONFIG, NULL);
	vlc -> config_global = config_global;
	vlc -> config_local = config_local;
	vex_layered_config_constructor(vlc);
	return vlc;
}

static void vex_layered_config_class_init(VexLayeredConfigClass *class)
{
        /*vex_layered_config_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexLayeredConfigClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

void vex_layered_config_update_profiles_due_colour_scheme(VexLayeredConfig * vlc, VexConfig * vex_config, char * name)
{
	int i;
	for (i = 0; i < vex_config_get_number_of_profiles(vex_config); i++){
		VexProfile * profile = vex_config_get_profile_by_index(vex_config, i, NULL);
		char * active = vex_profile_get_active_colour_scheme_name(profile);
		if (strcmp(name, active) == 0){
			VexColourScheme * scheme = vex_config_get_colour_scheme_by_name(vlc -> config_local, name);
			if (scheme == NULL){
				scheme = vex_config_get_colour_scheme_by_name(vlc -> config_global, name);
			}
			vex_profile_set_active_colour_scheme(profile, scheme);
		}
	}
}

static void vex_layered_config_colour_scheme_added_cb(VexConfig * vex_config, char * name, VexLayeredConfig * vlc)
{
	vex_layered_config_update_profiles_due_colour_scheme(vlc, vlc -> config_local, name);
	vex_layered_config_update_profiles_due_colour_scheme(vlc, vlc -> config_global, name);
}

static void vex_layered_config_colour_scheme_removed_cb(VexConfig * vex_config, char * name, 
	VexColourScheme * scheme, VexLayeredConfig * vlc)
{
	gboolean is_local = vex_config == vlc -> config_local;
	printf("REMOVAL of colourscheme REGISTERED WITHIN LAYERED CONFIG\n");
	if (is_local || vex_config_get_colour_scheme_by_name(vlc -> config_local, name) == NULL){
		vex_layered_config_update_profiles_due_colour_scheme(vlc, vlc -> config_local, name);
		vex_layered_config_update_profiles_due_colour_scheme(vlc, vlc -> config_global, name);
	}
}

static void vex_layered_config_profiles_colour_scheme_name_changed_cb(VexProfile * profile, VexLayeredConfig * vlc)
{
	char * name = vex_profile_get_active_colour_scheme_name(profile);
	VexColourScheme * scheme = vex_config_get_colour_scheme_by_name(vlc -> config_local, name);
	if (scheme == NULL){
		scheme = vex_config_get_colour_scheme_by_name(vlc -> config_global, name);
	}
	vex_profile_set_active_colour_scheme(profile, scheme);
}

static void vex_layered_config_profile_added_cb(VexConfig * vex_config, char * name, VexLayeredConfig * vlc)
{
	VexProfile * profile = vex_config_get_profile_by_name(vex_config, name);
	g_signal_connect(
		G_OBJECT(profile), "changed-colour-scheme-name",
		G_CALLBACK(vex_layered_config_profiles_colour_scheme_name_changed_cb), vlc);
	gboolean is_local = vex_config == vlc -> config_local;
	if (is_local || vex_config_get_profile_by_name(vlc -> config_local, name) == NULL){
	}
}

static void vex_layered_config_profile_removed_cb(VexConfig * vex_config, char * name, 
	VexProfile * profile, VexLayeredConfig * vlc)
{
	//gboolean is_local = vex_config == vlc -> config_local;
	printf("REMOVAL of profile REGISTERED WITHIN LAYERED CONFIG\n");
}

static void vex_layered_config_init(VexLayeredConfig * vex_layered_config)
{
}

void vex_layered_config_constructor(VexLayeredConfig * vlc)
{
	if (vlc -> config_local == NULL)
		vlc -> config_local = vex_config_new();
	if (vlc -> config_global == NULL)
		vlc -> config_global = vex_config_new();

	g_signal_connect(
		G_OBJECT(vlc -> config_local), "colour-scheme-added",
		G_CALLBACK(vex_layered_config_colour_scheme_added_cb), vlc);
	g_signal_connect(
		G_OBJECT(vlc -> config_global), "colour-scheme-added",
		G_CALLBACK(vex_layered_config_colour_scheme_added_cb), vlc);
	g_signal_connect(
		G_OBJECT(vlc -> config_local), "colour-scheme-removed",
		G_CALLBACK(vex_layered_config_colour_scheme_removed_cb), vlc);
	g_signal_connect(
		G_OBJECT(vlc -> config_global), "colour-scheme-removed",
		G_CALLBACK(vex_layered_config_colour_scheme_removed_cb), vlc);

	g_signal_connect(
		G_OBJECT(vlc -> config_local), "profile-added",
		G_CALLBACK(vex_layered_config_profile_added_cb), vlc);
	g_signal_connect(
		G_OBJECT(vlc -> config_global), "profile-added",
		G_CALLBACK(vex_layered_config_profile_added_cb), vlc);
	g_signal_connect(
		G_OBJECT(vlc -> config_local), "profile-removed",
		G_CALLBACK(vex_layered_config_profile_removed_cb), vlc);
	g_signal_connect(
		G_OBJECT(vlc -> config_global), "profile-removed",
		G_CALLBACK(vex_layered_config_profile_removed_cb), vlc);
}

VexConfig * vex_layered_config_get_config_global(VexLayeredConfig * vex_layered_config)
{
	return vex_layered_config -> config_global;
}

VexConfig * vex_layered_config_get_config_local(VexLayeredConfig * vex_layered_config)
{
	return vex_layered_config -> config_local;
}


gboolean vex_layered_config_get_show_scrolling_region(VexLayeredConfig * vex_layered_config)
{
	return FALSE;
}

gboolean vex_layered_config_get_show_status_bar(VexLayeredConfig * vex_layered_config)
{
	return FALSE;
}

GtkPositionType vex_layered_config_get_tabs_position(VexLayeredConfig * vex_layered_config)
{
	return GTK_POS_TOP;
}

char * vex_layered_config_get_active_profile(VexLayeredConfig * vex_layered_config)
{
	return NULL;
}


int vex_layered_config_get_number_of_profiles(VexLayeredConfig * vex_layered_config)
{
	return 0;
}

int vex_layered_config_get_number_of_colour_schemes(VexLayeredConfig * vex_layered_config)
{
	return 0;
}


VexProfile * vex_layered_config_get_profile_by_index(
	VexLayeredConfig * vex_layered_config, int index, char ** name, ConfigLevel * level)
{
	return NULL;
}


VexColourScheme * vex_layered_config_get_colour_scheme_by_index(
	VexLayeredConfig * vex_layered_config, int index, char ** name, ConfigLevel * level)
{
	return NULL;
}

VexProfile * vex_layered_config_get_profile_by_name(VexLayeredConfig * vlc, char * name)
{
	VexProfile * profile = vex_config_get_profile_by_name(vlc -> config_local, name);
	if (profile == NULL){
		profile = vex_config_get_profile_by_name(vlc -> config_global, name);
	}
	return profile;
}

VexColourScheme * vex_layered_config_get_colour_scheme_by_name(VexLayeredConfig * vlc, char * name)
{
	VexColourScheme * cs = vex_config_get_colour_scheme_by_name(vlc -> config_local, name);
	if (cs == NULL){
		cs = vex_config_get_colour_scheme_by_name(vlc -> config_global, name);
	}
	return cs;
}

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

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_config.h"
#include "../../helpers/util.h"
#include "marshal.h"

G_DEFINE_TYPE (VexConfig, vex_config, G_TYPE_OBJECT);

enum
{
        PROFILE_ADDED,
	PROFILE_RENAMED,
	PROFILE_REMOVED,
        COLOUR_SCHEME_ADDED,
	COLOUR_SCHEME_RENAMED,
	COLOUR_SCHEME_REMOVED,
        LAST_SIGNAL
};

static guint vex_config_signals[LAST_SIGNAL] = { 0 };

typedef struct {
	char * name;
	VexProfile * profile;
}ProfileEntry;

typedef struct {
	char * name;
	VexColourScheme * colour_scheme;
}ColourSchemeEntry;

gint compare_profile_entries(gconstpointer a, gconstpointer b, gpointer user)
{
	ProfileEntry * peA = (ProfileEntry*)a;
	ProfileEntry * peB = (ProfileEntry*)b;
	return strcmp(peA -> name, peB -> name);
}

gint compare_colour_scheme_entries(gconstpointer a, gconstpointer b, gpointer user)
{
	ColourSchemeEntry * cseA = (ColourSchemeEntry*)a;
	ColourSchemeEntry * cseB = (ColourSchemeEntry*)b;
	return strcmp(cseA -> name, cseB -> name);
}

void free_profile_entry(gpointer data)
{
	return;
}

void free_colour_scheme_entry(gpointer data)
{
	return;
}

VexConfig * vex_config_new()
{
	VexConfig * vex_config = g_object_new(VEX_TYPE_VEX_CONFIG, NULL);
	return vex_config;
}

static void vex_config_class_init(VexConfigClass *class)
{
        vex_config_signals[PROFILE_ADDED] = g_signal_new(
                "profile-added",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigClass, profile_added),
                NULL, NULL,
                g_cclosure_marshal_VOID__STRING,
                G_TYPE_NONE, 1, G_TYPE_STRING);
        vex_config_signals[PROFILE_RENAMED] = g_signal_new(
                "profile-renamed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigClass, profile_renamed),
                NULL, NULL,
                g_cclosure_user_marshal_VOID__STRING_STRING,
                G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_STRING);
        vex_config_signals[PROFILE_REMOVED] = g_signal_new(
                "profile-removed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigClass, profile_removed),
                NULL, NULL,
                g_cclosure_user_marshal_VOID__STRING_OBJECT,
                G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_OBJECT);
        vex_config_signals[COLOUR_SCHEME_ADDED] = g_signal_new(
                "colour-scheme-added",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigClass, colour_scheme_added),
                NULL, NULL,
                g_cclosure_marshal_VOID__STRING,
                G_TYPE_NONE, 1, G_TYPE_STRING);
        vex_config_signals[COLOUR_SCHEME_RENAMED] = g_signal_new(
                "colour-scheme-renamed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigClass, colour_scheme_renamed),
                NULL, NULL,
                g_cclosure_user_marshal_VOID__STRING_STRING,
                G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_STRING);
        vex_config_signals[COLOUR_SCHEME_REMOVED] = g_signal_new(
                "colour-scheme-removed",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigClass, colour_scheme_removed),
                NULL, NULL,
                g_cclosure_user_marshal_VOID__STRING_OBJECT,
                G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_OBJECT);
}

static void vex_config_init(VexConfig *vex_config)
{
	vex_config -> active_profile = NULL;
	vex_config -> profiles = g_sequence_new(free_profile_entry);
	vex_config -> colour_schemes = g_sequence_new(free_colour_scheme_entry);
}

void vex_config_deepcopy(VexConfig * config_from, VexConfig * config_to)
{
	vex_config_set_show_scrolling_region(config_to, vex_config_get_show_scrolling_region(config_from));
	vex_config_set_show_status_bar(config_to, vex_config_get_show_status_bar(config_from));
	if (vex_config_get_active_profile(config_from) != NULL){
		vex_config_set_active_profile(config_to, vex_config_get_active_profile(config_from));
	}
	int i;
	for (i = 0; i < vex_config_get_number_of_profiles(config_from); i++){
		char * profile_name;
		VexProfile * profile = vex_config_get_profile_by_index(config_from, i, &profile_name);
		vex_config_add_profile(config_to, profile_name, 
			vex_profile_duplicate(profile, FALSE));
	}
	for (i = 0; i < vex_config_get_number_of_colour_schemes(config_from); i++){
		char * colour_scheme_name;
		VexColourScheme * colour_scheme = vex_config_get_colour_scheme_by_index(config_from, i, &colour_scheme_name);
		vex_config_add_colour_scheme(config_to, colour_scheme_name, 
			vex_colour_scheme_duplicate(colour_scheme));
	}
}

void vex_config_set_show_scrolling_region(VexConfig * vex_config, gboolean show)
{
	vex_config -> show_scrolling_region = show;
}

void vex_config_set_show_status_bar(VexConfig * vex_config, gboolean show)
{
	vex_config -> show_status_bar = show;
}

void vex_config_set_active_profile(VexConfig * vex_config, char * name)
{
	g_free(vex_config -> active_profile);
	vex_config -> active_profile = g_strdup(name);
}

void vex_config_add_profile(VexConfig * vex_config, const char * name, VexProfile * profile)
{
	ProfileEntry * pe = malloc(sizeof(ProfileEntry));
	pe -> name = g_strdup(name);
	pe -> profile = profile;
	g_sequence_insert_sorted(vex_config -> profiles, pe, compare_profile_entries, NULL);
	g_signal_emit (vex_config, vex_config_signals[PROFILE_ADDED], 0, name);
}

void vex_config_add_profile_defaults(VexConfig * vex_config, const char * name)
{
	VexProfile * profile = vex_profile_new();
	vex_config_add_profile(vex_config, name, profile);
}

void vex_config_add_colour_scheme(VexConfig * vex_config, const char * name, VexColourScheme * colour_scheme)
{
	ColourSchemeEntry * cse = malloc(sizeof(ColourSchemeEntry));
	cse -> name = g_strdup(name);
	cse -> colour_scheme = colour_scheme;
	g_sequence_insert_sorted(vex_config -> colour_schemes, cse, compare_colour_scheme_entries, NULL);
	g_signal_emit (vex_config, vex_config_signals[COLOUR_SCHEME_ADDED], 0, name);
}

void vex_config_add_colour_scheme_defaults(VexConfig * vex_config, const char * name)
{
	VexColourScheme * cs = vex_colour_scheme_new_defaults();
	vex_config_add_colour_scheme(vex_config, name, cs);
}

void vex_config_remove_profile(VexConfig * vex_config, const char * name)
{
	ProfileEntry pe;
	pe.name = (char*)name;
	GSequenceIter * iter = g_sequence_find(vex_config -> profiles, &pe, compare_profile_entries, NULL);
	if (iter != NULL){
		ProfileEntry * pe = (ProfileEntry*) g_sequence_get(iter);
		VexProfile * profile = pe -> profile;
		g_sequence_remove(iter);
		g_signal_emit (vex_config, vex_config_signals[PROFILE_REMOVED], 0, name, profile);
	}
}

void vex_config_remove_colour_scheme(VexConfig * vex_config, const char * name)
{
	ColourSchemeEntry cse;
	cse.name = (char*)name;
	GSequenceIter * iter = g_sequence_find(vex_config -> colour_schemes, &cse, compare_colour_scheme_entries, NULL);
	if (iter != NULL){
		ColourSchemeEntry * cse = (ColourSchemeEntry*) g_sequence_get(iter);
		VexColourScheme * colour_scheme = cse -> colour_scheme;
		g_sequence_remove(iter);
		g_signal_emit (vex_config, vex_config_signals[COLOUR_SCHEME_REMOVED], 0, name, colour_scheme);
	}
}

void vex_config_rename_profile(VexConfig * vex_config, const char * name, const char * new_name)
{
	ProfileEntry pe;
	pe.name = (char*)name;
	GSequenceIter * iter = g_sequence_find(vex_config -> profiles, &pe, compare_profile_entries, NULL);
	if (iter != NULL){
		ProfileEntry * pe = (ProfileEntry*)g_sequence_get(iter);
		pe -> name = g_strdup(new_name);
		g_sequence_sort_changed(iter, compare_profile_entries, NULL);
		g_signal_emit (vex_config, vex_config_signals[PROFILE_RENAMED], 0, name, new_name);
	}
}

void vex_config_rename_colour_scheme(VexConfig * vex_config, const char * name, const char * new_name)
{
	ColourSchemeEntry cse;
	cse.name = (char*)name;
	GSequenceIter * iter = g_sequence_find(vex_config -> colour_schemes, &cse, compare_colour_scheme_entries, NULL);
	if (iter != NULL){
		ColourSchemeEntry * cse = (ColourSchemeEntry*)g_sequence_get(iter);
		cse -> name = g_strdup(new_name);
		g_sequence_sort_changed(iter, compare_colour_scheme_entries, NULL);
		g_signal_emit (vex_config, vex_config_signals[COLOUR_SCHEME_RENAMED], 0, name, new_name);
	}
}


gboolean vex_config_get_show_scrolling_region(VexConfig * vex_config)
{
	return vex_config -> show_scrolling_region;
}

gboolean vex_config_get_show_status_bar(VexConfig * vex_config)
{
	return vex_config -> show_status_bar;
}

char * vex_config_get_active_profile(VexConfig * vex_config)
{
	return vex_config -> active_profile;
}

int vex_config_get_number_of_profiles(VexConfig * vex_config)
{
	return g_sequence_get_length(vex_config -> profiles);
}

int vex_config_get_number_of_colour_schemes(VexConfig * vex_config)
{
	return g_sequence_get_length(vex_config -> colour_schemes);
}

VexProfile * vex_config_get_profile_by_index(VexConfig * vex_config, int index, char ** name)
{
	GSequenceIter * iter = g_sequence_get_iter_at_pos(vex_config -> profiles, index);
	ProfileEntry * pe = (ProfileEntry*)g_sequence_get(iter);
	if (name != NULL)
		*name = pe -> name;
	return pe -> profile;
}

VexProfile * vex_config_get_profile_by_name(VexConfig * vex_config, const char * name)
{
	ProfileEntry pe_search;
	pe_search.name = (char*)name;
	GSequenceIter * iter = g_sequence_find(
		vex_config -> profiles, &pe_search, compare_profile_entries, NULL);
	if (iter != NULL){
		ProfileEntry * pe = (ProfileEntry*)g_sequence_get(iter);
		return pe -> profile;
	}
	return NULL;
}

VexColourScheme * vex_config_get_colour_scheme_by_index(VexConfig * vex_config, int index, char ** name)
{
	GSequenceIter * iter = g_sequence_get_iter_at_pos(vex_config -> colour_schemes, index);
	ColourSchemeEntry * cse = (ColourSchemeEntry*)g_sequence_get(iter);
	if (name != NULL)
		*name = cse -> name;
	return cse -> colour_scheme;
}

VexColourScheme * vex_config_get_colour_scheme_by_name(VexConfig * vex_config, const char * name)
{
	ColourSchemeEntry cse_search;
	cse_search.name = (char*)name;
	GSequenceIter * iter = g_sequence_find(
		vex_config -> colour_schemes, &cse_search, compare_colour_scheme_entries, NULL);
	if (iter != NULL){
		ColourSchemeEntry * cse = (ColourSchemeEntry*)g_sequence_get(iter);
		return cse -> colour_scheme;
	}
	return NULL;
}

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

#include "settings_editor.h"
#include "../../helpers/tool.h"
#include "../../helpers/util.h"

G_DEFINE_TYPE (SettingsEditor, settings_editor, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint settings_editor_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, settings_editor_signals[SIGNAL_NAME_n], 0);

void settings_editor_constructor(SettingsEditor * settings_editor);
void settings_editor_finalize(GObject * object);

void settings_editor_box_profiles_changed_cb(GtkComboBox * box_profiles, SettingsEditor * se);
void settings_editor_show_scrolling_region_toggled_cb(GtkToggleButton * button, SettingsEditor * se);
void settings_editor_show_status_bar_toggled_cb(GtkToggleButton * button, SettingsEditor * se);

static void settings_editor_profile_added_cb(VexConfig * config, char * name, SettingsEditor * se);
static void settings_editor_profile_renamed_cb(VexConfig * config, char * name, char * new_name, SettingsEditor * se);
static void settings_editor_profile_removed_cb(VexConfig * config, char * name, VexProfile * profile, SettingsEditor * se);

GtkWidget * settings_editor_new(VexLayeredConfig * vlc)
{
	SettingsEditor * settings_editor = g_object_new(VEX_TYPE_SETTINGS_EDITOR, NULL);

	settings_editor -> vlc = vlc;

	settings_editor_constructor(settings_editor);

	return GTK_WIDGET(settings_editor);
}

static void settings_editor_class_init(SettingsEditorClass *class)
{
	GObjectClass * object_class = G_OBJECT_CLASS(class);
	object_class -> finalize = settings_editor_finalize;
        /*settings_editor_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SettingsEditorClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void settings_editor_init(SettingsEditor *settings_editor)
{
}

void settings_editor_finalize(GObject * object)
{
	SettingsEditor * se = (SettingsEditor*) object;
	VexConfig * local = vex_layered_config_get_config_local(se -> vlc);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(settings_editor_profile_added_cb), se);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(settings_editor_profile_renamed_cb), se);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(settings_editor_profile_removed_cb), se);
}
void settings_editor_constructor(SettingsEditor * se)
{
	GtkWidget * table = gtk_table_new(2, 3, FALSE);
	gtk_box_pack_start(GTK_BOX(se), table, TRUE, TRUE, 0);

	GtkWidget * label_profile = gtk_label_new("Profile:");
	GtkWidget * label_scrolling_region = gtk_label_new("Show Scrolling Region:");
	GtkWidget * label_status_bar = gtk_label_new("Show Status Bar:");
	gtk_misc_set_alignment(GTK_MISC(label_profile), 0.0, 0.0);
	gtk_misc_set_alignment(GTK_MISC(label_scrolling_region), 0.0, 0.0);
	gtk_misc_set_alignment(GTK_MISC(label_status_bar), 0.0, 0.0);

	VexConfig * global = vex_layered_config_get_config_global(se -> vlc);
	VexConfig * local = vex_layered_config_get_config_local(se -> vlc);

	se -> box_profiles = gtk_combo_box_entry_new_text();
	int n_profiles, i;
	se -> profile_names = g_sequence_new(NULL);

	n_profiles = vex_config_get_number_of_profiles(global);
	for (i = 0; i < n_profiles; i++){
		char * name;
		vex_config_get_profile_by_index(global, i, &name);
		GSequenceIter * iter = g_sequence_insert_sorted(
			se -> profile_names, g_strdup(name), compare_strings_data, NULL);
		int pos = g_sequence_iter_get_position(iter);
		gtk_combo_box_insert_text(GTK_COMBO_BOX(se -> box_profiles), pos, name);
	}
	n_profiles = vex_config_get_number_of_profiles(local);
	for (i = 0; i < n_profiles; i++){
		char * name;
		vex_config_get_profile_by_index(local, i, &name);
		if (g_sequence_find(se -> profile_names, name, compare_strings_data, NULL) == NULL){
			GSequenceIter * iter = 
				g_sequence_insert_sorted(se -> profile_names, g_strdup(name), compare_strings_data, NULL);
			int pos = g_sequence_iter_get_position(iter);
			gtk_combo_box_insert_text(GTK_COMBO_BOX(se -> box_profiles), pos, name);
		}
	}
	char * active_profile = vex_config_get_active_profile(local);
	if (active_profile == NULL){
		active_profile = vex_config_get_active_profile(global);
	}
	if (active_profile == NULL){
		active_profile = "";
	}
	gtk_entry_set_text (GTK_ENTRY (GTK_BIN (se -> box_profiles)->child), active_profile);

	g_signal_connect(
		G_OBJECT(local), "profile-added",
		G_CALLBACK(settings_editor_profile_added_cb), se);
	g_signal_connect(
		G_OBJECT(local), "profile-renamed",
		G_CALLBACK(settings_editor_profile_renamed_cb), se);
	g_signal_connect(
		G_OBJECT(local), "profile-removed",
		G_CALLBACK(settings_editor_profile_removed_cb), se);

	GtkWidget * check_1  = gtk_check_button_new();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_1), 
		vex_config_get_show_scrolling_region(local));

	GtkWidget * check_2  = gtk_check_button_new();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_2), 
		vex_config_get_show_status_bar(local));


	gtk_table_attach(GTK_TABLE(table), label_profile,	    0, 1, 0, 1, GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), label_scrolling_region,  0, 1, 1, 2, GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), label_status_bar, 	    0, 1, 2, 3, GTK_FILL, 0, 2, 0);

	gtk_table_attach(GTK_TABLE(table), se -> box_profiles,      1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), check_1,	   	    1, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), check_2,   		    1, 2, 2, 3, GTK_EXPAND | GTK_FILL, 0, 2, 0);

	g_signal_connect(
		G_OBJECT(se -> box_profiles), "changed",
		G_CALLBACK(settings_editor_box_profiles_changed_cb), se);
	g_signal_connect(
		G_OBJECT(check_1), "toggled",
		G_CALLBACK(settings_editor_show_scrolling_region_toggled_cb), se);
	g_signal_connect(
		G_OBJECT(check_2), "toggled",
		G_CALLBACK(settings_editor_show_status_bar_toggled_cb), se);
}

void settings_editor_box_profiles_changed_cb(GtkComboBox * box_profiles, SettingsEditor * se)
{
	VexConfig * config_local = vex_layered_config_get_config_local(se -> vlc);
	const char * text = gtk_entry_get_text (GTK_ENTRY (GTK_BIN (se -> box_profiles)->child));
	vex_config_set_active_profile(config_local, text);
}

void settings_editor_show_scrolling_region_toggled_cb(GtkToggleButton * button, SettingsEditor * se)
{
	VexConfig * config_local = vex_layered_config_get_config_local(se -> vlc);
	gboolean state = gtk_toggle_button_get_active(button);
	vex_config_set_show_scrolling_region(config_local, state);
}

void settings_editor_show_status_bar_toggled_cb(GtkToggleButton * button, SettingsEditor * se)
{
	VexConfig * config_local = vex_layered_config_get_config_local(se -> vlc);
	gboolean state = gtk_toggle_button_get_active(button);
	vex_config_set_show_status_bar(config_local, state);
}

static void settings_editor_profile_added_cb(VexConfig * config, char * name, SettingsEditor * se)
{
	GSequenceIter * iter = g_sequence_find(se -> profile_names, name, compare_strings_data, NULL);
	if (iter == NULL){
		GSequenceIter * iter = 
			g_sequence_insert_sorted(se -> profile_names, g_strdup(name), compare_strings_data, NULL);
		int pos = g_sequence_iter_get_position(iter);
		gtk_combo_box_insert_text(GTK_COMBO_BOX(se -> box_profiles), pos, name);
	}
}

static void settings_editor_profile_renamed_cb(VexConfig * config, char * name, char * new_name, SettingsEditor * se)
{
	VexConfig * config_local = vex_layered_config_get_config_local(se -> vlc);
	VexConfig * config_global = vex_layered_config_get_config_global(se -> vlc);

	if (vex_config_get_profile_by_name(config_local, name) == NULL
			&& vex_config_get_profile_by_name(config_global, name) == NULL){
		GSequenceIter * iter = g_sequence_find(se -> profile_names, name, compare_strings_data, NULL);
		if (iter != NULL){
			int pos = g_sequence_iter_get_position(iter);
			gtk_combo_box_remove_text(GTK_COMBO_BOX(se -> box_profiles), pos);
			g_sequence_remove(iter);
		}
	}
	GSequenceIter * iter = g_sequence_find(se -> profile_names, new_name, compare_strings_data, NULL);
	if (iter == NULL){
		GSequenceIter * iter = 
			g_sequence_insert_sorted(se -> profile_names, 
				g_strdup(new_name), compare_strings_data, NULL);
		int pos = g_sequence_iter_get_position(iter);
		gtk_combo_box_insert_text(GTK_COMBO_BOX(se -> box_profiles), pos, new_name);
	}
	if(strcmp(gtk_entry_get_text (GTK_ENTRY (GTK_BIN (se -> box_profiles)->child)), name) == 0){
		gtk_entry_set_text (GTK_ENTRY (GTK_BIN (se -> box_profiles)->child), new_name);
	}
}

static void settings_editor_profile_removed_cb(VexConfig * config, char * name, VexProfile * profile, SettingsEditor * se)
{
	VexConfig * config_local = vex_layered_config_get_config_local(se -> vlc);
	VexConfig * config_global = vex_layered_config_get_config_global(se -> vlc);

	if (vex_config_get_profile_by_name(config_local, name) == NULL
			&& vex_config_get_colour_scheme_by_name(config_global, name) == NULL){
		GSequenceIter * iter = g_sequence_find(se -> profile_names, name, compare_strings_data, NULL);
		if (iter != NULL){
			int pos = g_sequence_iter_get_position(iter);
			gtk_combo_box_remove_text(GTK_COMBO_BOX(se -> box_profiles), pos);
			g_sequence_remove(iter);
		}
	}
}

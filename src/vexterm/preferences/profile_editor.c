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
#include <pango/pango.h>

#include "profile_editor.h"
#include "../../helpers/tool.h"
#include "../../helpers/util.h"

G_DEFINE_TYPE (ProfileEditor, profile_editor, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint profile_editor_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, profile_editor_signals[SIGNAL_NAME_n], 0);

void profile_editor_constructor(ProfileEditor * profile_editor, char * name);
void profile_editor_finalize(GObject * object);
void profile_editor_font_set_cb(GtkFontButton * button, ProfileEditor * profile_editor);
void profile_editor_colour_scheme_added_cb(VexConfig * config, char * name, ProfileEditor * profile_editor);
void profile_editor_colour_scheme_renamed_cb(VexConfig * config, char * name, char * new_name, ProfileEditor * profile_editor);
void profile_editor_colour_scheme_removed_cb(
	VexConfig * config, char * name, VexColourScheme * vcs, ProfileEditor * profile_editor);
void profile_editor_box_schemes_changed_cb(GtkComboBox * box_schemes, ProfileEditor * profile_editor);

GtkWidget * profile_editor_new(VexConfig * config_global, VexConfig * config_local, VexProfile * profile, char * name)
{
	ProfileEditor * profile_editor = g_object_new(VEX_TYPE_PROFILE_EDITOR, NULL);
	profile_editor -> config_global = config_global;
	profile_editor -> config_local = config_local;
	profile_editor -> profile = profile;
	profile_editor_constructor(profile_editor, name);
	return GTK_WIDGET(profile_editor);
}

static void profile_editor_class_init(ProfileEditorClass *class)
{
        /*profile_editor_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ProfileEditorClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
	GObjectClass * object_class = G_OBJECT_CLASS(class);
	object_class -> finalize = profile_editor_finalize;
}

static void profile_editor_init(ProfileEditor *profile_editor)
{
}

void profile_editor_finalize(GObject * object)
{
	ProfileEditor * profile_editor = (ProfileEditor*) object;
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(profile_editor -> box_schemes),
		G_CALLBACK(profile_editor_box_schemes_changed_cb), profile_editor);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(profile_editor -> config_local),
		G_CALLBACK(profile_editor_colour_scheme_added_cb), profile_editor);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(profile_editor -> config_local),
		G_CALLBACK(profile_editor_colour_scheme_renamed_cb), profile_editor);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(profile_editor -> config_local),
		G_CALLBACK(profile_editor_colour_scheme_removed_cb), profile_editor);
	g_object_unref(profile_editor -> box_schemes);
}

void profile_editor_constructor(ProfileEditor * profile_editor, char * name)
{
	profile_editor -> label = gtk_label_new(name);
	gtk_misc_set_alignment(GTK_MISC(profile_editor -> label), 0.0, 0.0);
	gtk_box_pack_start(GTK_BOX(profile_editor), profile_editor -> label, FALSE, FALSE, 0);

	char * font_name = vex_profile_get_font_name(profile_editor -> profile);
	int font_size = vex_profile_get_font_size(profile_editor -> profile);
	char * string = g_strdup_printf("%s %d", font_name, font_size);
	char * active_cs = vex_profile_get_active_colour_scheme_name(profile_editor -> profile);

	GtkWidget * font_button = gtk_font_button_new_with_font(string);
	gtk_font_button_set_use_font(GTK_FONT_BUTTON(font_button), TRUE);

	profile_editor -> box_schemes = gtk_combo_box_entry_new_text();
	g_object_ref(profile_editor -> box_schemes);
	int i, n_schemes;

	profile_editor -> scheme_names = g_sequence_new(free);

	n_schemes = vex_config_get_number_of_colour_schemes(profile_editor -> config_global);
	for (i = 0; i < n_schemes; i++){
		char * name;
		vex_config_get_colour_scheme_by_index(
			profile_editor -> config_global, i, &name);
		char * copy = g_strdup(name);
		g_sequence_insert_sorted(profile_editor -> scheme_names, copy, compare_strings_data, NULL);
	}
	n_schemes = vex_config_get_number_of_colour_schemes(profile_editor -> config_local);
	for (i = 0; i < n_schemes; i++){
		char * name;
		vex_config_get_colour_scheme_by_index(
			profile_editor -> config_local, i, &name);
		char * copy = g_strdup(name);
		if (g_sequence_find(profile_editor -> scheme_names, copy, compare_strings_data, NULL) == NULL){
			g_sequence_insert_sorted(profile_editor -> scheme_names, copy, compare_strings_data, NULL);
		}
	}

	GSequenceIter * iter;
	for (iter = g_sequence_get_begin_iter(profile_editor -> scheme_names); 
		!g_sequence_iter_is_end(iter); iter = g_sequence_iter_next(iter)){
		char * name = (char*) g_sequence_get(iter);
		gtk_combo_box_append_text(GTK_COMBO_BOX(profile_editor -> box_schemes), name);
	}

	gtk_entry_set_text (GTK_ENTRY (GTK_BIN (profile_editor -> box_schemes)->child), active_cs);
	
	g_signal_connect(
		G_OBJECT(profile_editor -> box_schemes), "changed",
		G_CALLBACK(profile_editor_box_schemes_changed_cb), profile_editor);
	g_signal_connect(
		G_OBJECT(profile_editor -> config_local), "colour-scheme-added",
		G_CALLBACK(profile_editor_colour_scheme_added_cb), profile_editor);
	g_signal_connect(
		G_OBJECT(profile_editor -> config_local), "colour-scheme-renamed",
		G_CALLBACK(profile_editor_colour_scheme_renamed_cb), profile_editor);
	g_signal_connect(
		G_OBJECT(profile_editor -> config_local), "colour-scheme-removed",
		G_CALLBACK(profile_editor_colour_scheme_removed_cb), profile_editor);

	GtkWidget * label_font = gtk_label_new("Font:");
	GtkWidget * label_colours = gtk_label_new("Colours:");
	gtk_misc_set_alignment(GTK_MISC(label_font), 0.0, 0.0);
	gtk_misc_set_alignment(GTK_MISC(label_colours), 0.0, 0.0);

	GtkWidget * table = gtk_table_new(2, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(profile_editor), table, FALSE, FALSE, 0);
	gtk_table_attach(GTK_TABLE(table), label_font,	  0, 1, 0, 1, GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), label_colours, 0, 1, 1, 2, GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), font_button,   1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), profile_editor -> box_schemes,   1, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 2, 0);

	g_signal_connect(
		G_OBJECT(font_button), "font-set",
		G_CALLBACK(profile_editor_font_set_cb), profile_editor);
}

void profile_editor_colour_scheme_added_cb(VexConfig * config, char * name, ProfileEditor * profile_editor)
{
	GSequenceIter * iter = g_sequence_find(profile_editor -> scheme_names, name, compare_strings_data, NULL);
	if (iter == NULL){
		GSequenceIter * iter = 
			g_sequence_insert_sorted(profile_editor -> scheme_names, g_strdup(name), compare_strings_data, NULL);
		int pos = g_sequence_iter_get_position(iter);
		gtk_combo_box_insert_text(GTK_COMBO_BOX(profile_editor -> box_schemes), pos, name);
	}
}

void profile_editor_colour_scheme_renamed_cb(VexConfig * config, char * name, char * new_name, ProfileEditor * profile_editor)
{
	if (vex_config_get_colour_scheme_by_name(profile_editor -> config_local, name) == NULL
			&& vex_config_get_colour_scheme_by_name(profile_editor -> config_global, name) == NULL){
		GSequenceIter * iter = g_sequence_find(profile_editor -> scheme_names, name, compare_strings_data, NULL);
		if (iter != NULL){
			int pos = g_sequence_iter_get_position(iter);
			gtk_combo_box_remove_text(GTK_COMBO_BOX(profile_editor -> box_schemes), pos);
			g_sequence_remove(iter);
		}
	}
	GSequenceIter * iter = g_sequence_find(profile_editor -> scheme_names, new_name, compare_strings_data, NULL);
	if (iter == NULL){
		GSequenceIter * iter = 
			g_sequence_insert_sorted(profile_editor -> scheme_names, 
				g_strdup(new_name), compare_strings_data, NULL);
		int pos = g_sequence_iter_get_position(iter);
		gtk_combo_box_insert_text(GTK_COMBO_BOX(profile_editor -> box_schemes), pos, new_name);
	}
	if(strcmp(gtk_entry_get_text (GTK_ENTRY (GTK_BIN (profile_editor -> box_schemes)->child)), name) == 0){
		gtk_entry_set_text (GTK_ENTRY (GTK_BIN (profile_editor -> box_schemes)->child), new_name);
	}
}

void profile_editor_colour_scheme_removed_cb(
	VexConfig * config, char * name, VexColourScheme * vcs, ProfileEditor * profile_editor)
{
	if (vex_config_get_colour_scheme_by_name(profile_editor -> config_local, name) == NULL
			&& vex_config_get_colour_scheme_by_name(profile_editor -> config_global, name) == NULL){
		GSequenceIter * iter = g_sequence_find(profile_editor -> scheme_names, name, compare_strings_data, NULL);
		if (iter != NULL){
			int pos = g_sequence_iter_get_position(iter);
			gtk_combo_box_remove_text(GTK_COMBO_BOX(profile_editor -> box_schemes), pos);
			g_sequence_remove(iter);
		}
	}
}

void profile_editor_set_name(ProfileEditor * profile_editor, char * name)
{
	gtk_label_set_text(GTK_LABEL(profile_editor -> label), name);
}

void profile_editor_font_set_cb(GtkFontButton * button, ProfileEditor * profile_editor)
{
	const char * font_name = gtk_font_button_get_font_name(button);
	PangoFontDescription * desc = pango_font_description_from_string(font_name);
	const char * font_family = pango_font_description_get_family(desc);
	int font_size = pango_font_description_get_size(desc) / PANGO_SCALE;
	printf("family: %s, size: %d\n", font_family, font_size);
	vex_profile_set_font(profile_editor -> profile, font_family, font_size);
}

void profile_editor_box_schemes_changed_cb(GtkComboBox * box_schemes, ProfileEditor * profile_editor)
{
	const char * scheme_name = gtk_entry_get_text (GTK_ENTRY (GTK_BIN (profile_editor -> box_schemes)->child));
	vex_profile_set_active_colour_scheme_name(profile_editor -> profile, scheme_name);
}

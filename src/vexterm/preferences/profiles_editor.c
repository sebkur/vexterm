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

#include "profiles_editor.h"
#include "profile_editor.h"
#include "../../gtkplus/dialog/choose_name_dialog.h"

G_DEFINE_TYPE (ProfilesEditor, profiles_editor, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint profiles_editor_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, profiles_editor_signals[SIGNAL_NAME_n], 0);

void profiles_editor_constructor(ProfilesEditor * profiles_editor);
void profiles_editor_finalize(GObject * object);
void profiles_editor_add_profile_box(ProfilesEditor * pe, char * name, VexProfile * profile, ConfigLevel config_level);
void profiles_editor_added(VexConfig * vex_config, char * name, ProfilesEditor * profiles_editor);
void profiles_editor_renamed(VexConfig * vex_config, char * name, char * new_name, ProfilesEditor * pe);
void profiles_editor_removed(VexConfig * vex_config, char * name, VexProfile * profiles, ProfilesEditor * profiles_editor);

typedef struct {
	char * name;
	ConfigLevel config_level;
} ProfileEntry;

gint compare_profiles(gconstpointer a, gconstpointer b, gpointer data)
{
	ProfileEntry * pa = (ProfileEntry*)a;
	ProfileEntry * pb = (ProfileEntry*)b;
	int c = strcmp(pa -> name, pb -> name);
	if (c == 0){
		if (pa -> config_level == pb -> config_level) return 0;
		c = pa -> config_level == CONFIG_LEVEL_GLOBAL ? -1 : 1;
	}
	return c;
}

gboolean profiles_editor_profile_name_ok_function(const char * name, gpointer data)
{
	VexConfig * vex_config = (VexConfig*) data;
	VexProfile * profile = vex_config_get_profile_by_name(vex_config, name);
	return profile == NULL;
}

GtkWidget * profiles_editor_new(VexLayeredConfig * vlc)
{
	ProfilesEditor * profiles_editor = g_object_new(VEX_TYPE_PROFILES_EDITOR, NULL);

	profiles_editor -> vlc = vlc;

	profiles_editor_constructor(profiles_editor);

	return GTK_WIDGET(profiles_editor);
}

static void profiles_editor_class_init(ProfilesEditorClass *class)
{
	GObjectClass * object_class = G_OBJECT_CLASS(class);
	object_class -> finalize = profiles_editor_finalize;
        /*profiles_editor_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ProfilesEditorClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void profiles_editor_init(ProfilesEditor *profiles_editor)
{
}

void profiles_editor_select(ListVBox * box, gpointer key, ProfilesEditor * pe);
void profiles_editor_button_new_clicked(GtkButton * button, ProfilesEditor * pe);
void profiles_editor_button_ren_clicked(GtkButton * button, ProfilesEditor * pe);
void profiles_editor_button_dup_clicked(GtkButton * button, ProfilesEditor * pe);
void profiles_editor_button_rem_clicked(GtkButton * button, ProfilesEditor * pe);

void profiles_editor_finalize(GObject * object)
{
	ProfilesEditor * pe = (ProfilesEditor*) object;
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(profiles_editor_added), pe);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(profiles_editor_renamed), pe);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(profiles_editor_removed), pe);
}

void profiles_editor_constructor(ProfilesEditor * pe)
{
	VexConfig * global = vex_layered_config_get_config_global(pe -> vlc);
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);

	/* list of profiles */
	pe -> list_box = VEX_LIST_V_BOX(list_v_box_new(compare_profiles, NULL));

	g_signal_connect(
		G_OBJECT(pe -> list_box), "select",
		G_CALLBACK(profiles_editor_select), pe);

	g_signal_connect(
		G_OBJECT(local), "profile-added",
		G_CALLBACK(profiles_editor_added), pe);
	g_signal_connect(
		G_OBJECT(local), "profile-renamed",
		G_CALLBACK(profiles_editor_renamed), pe);
	g_signal_connect(
		G_OBJECT(local), "profile-removed",
		G_CALLBACK(profiles_editor_removed), pe);

	int i, n_profiles;

	n_profiles = vex_config_get_number_of_profiles(local);
	for (i = 0; i < n_profiles; i++){
		char * name;
		VexProfile * profile = vex_config_get_profile_by_index(local, i, &name);
		profiles_editor_add_profile_box(pe, name, profile, CONFIG_LEVEL_LOCAL);
	}
	n_profiles = vex_config_get_number_of_profiles(global);
	for (i = 0; i < n_profiles; i++){
		char * name;
		VexProfile * profile = vex_config_get_profile_by_index(global, i, &name);
		profiles_editor_add_profile_box(pe, name, profile, CONFIG_LEVEL_GLOBAL);
	}

        GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), GTK_WIDGET(pe -> list_box));
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);

	/* buttons */
	GtkWidget * box_s_buttons = gtk_hbox_new(FALSE, 0);

	GtkWidget * image_new = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
	GtkWidget * image_ren = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
	GtkWidget * image_dup = gtk_image_new_from_stock("gtk-copy", GTK_ICON_SIZE_MENU);
	GtkWidget * image_rem = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);

	pe -> button_s_new = gtk_button_new_with_label("new");
	pe -> button_s_ren = gtk_button_new_with_label("rename");
	pe -> button_s_dup = gtk_button_new_with_label("duplicate");
	pe -> button_s_rem = gtk_button_new_with_label("remove");

	gtk_button_set_image(GTK_BUTTON(pe -> button_s_new), image_new);
	gtk_button_set_image(GTK_BUTTON(pe -> button_s_ren), image_ren);
	gtk_button_set_image(GTK_BUTTON(pe -> button_s_dup), image_dup);
	gtk_button_set_image(GTK_BUTTON(pe -> button_s_rem), image_rem);

	gtk_box_pack_start(GTK_BOX(box_s_buttons), pe -> button_s_new, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_s_buttons), pe -> button_s_ren, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_s_buttons), pe -> button_s_dup, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_s_buttons), pe -> button_s_rem, FALSE, FALSE, 0);

	gtk_widget_set_sensitive(pe -> button_s_ren, FALSE);
	gtk_widget_set_sensitive(pe -> button_s_dup, FALSE);
	gtk_widget_set_sensitive(pe -> button_s_rem, FALSE);

	g_signal_connect(G_OBJECT(pe -> button_s_new), "clicked", G_CALLBACK(profiles_editor_button_new_clicked), pe);
	g_signal_connect(G_OBJECT(pe -> button_s_ren), "clicked", G_CALLBACK(profiles_editor_button_ren_clicked), pe);
	g_signal_connect(G_OBJECT(pe -> button_s_dup), "clicked", G_CALLBACK(profiles_editor_button_dup_clicked), pe);
	g_signal_connect(G_OBJECT(pe -> button_s_rem), "clicked", G_CALLBACK(profiles_editor_button_rem_clicked), pe);

	/* overall layout */
	gtk_box_pack_start(GTK_BOX(pe), box_s_buttons, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pe), scrolled, TRUE, TRUE, 0);
}

void profiles_editor_add_profile_box(ProfilesEditor * pe, char * name, VexProfile * profile, ConfigLevel config_level)
{
	VexConfig * global = vex_layered_config_get_config_global(pe -> vlc);
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);

	GtkWidget * profile_editor = profile_editor_new(global, local, profile, name);
	gtk_widget_set_sensitive(profile_editor, config_level == CONFIG_LEVEL_LOCAL);
	GtkWidget * frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(frame), profile_editor);
	gtk_container_set_border_width(GTK_CONTAINER(profile_editor), 4);

	ProfileEntry * pre = malloc(sizeof(ProfileEntry));
	pre -> name = g_strdup(name);
	pre -> config_level = config_level;
	list_v_box_insert(VEX_LIST_V_BOX(pe -> list_box), pre, frame);

	gtk_widget_show_all(frame);
}

void profiles_editor_select(ListVBox * box, gpointer key, ProfilesEditor * pe)
{
	//printf("selected %p\n", key);
	gboolean ren_rem = FALSE;
	gboolean dup = key != NULL;
	if (key != NULL){
		ProfileEntry * pre = (ProfileEntry*) key;
		ren_rem = pre -> config_level == CONFIG_LEVEL_LOCAL;
	}
	gtk_widget_set_sensitive(pe -> button_s_ren, ren_rem);
	gtk_widget_set_sensitive(pe -> button_s_dup, dup);
	gtk_widget_set_sensitive(pe -> button_s_rem, ren_rem);
}

/* button callbacks */

void profiles_editor_button_new_clicked(GtkButton * button, ProfilesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);

	GtkWidget * d = choose_name_dialog_new(
		"New Profile", FALSE, "", "Name:", "", "new", 
		profiles_editor_profile_name_ok_function, local);
	int response = gtk_dialog_run(GTK_DIALOG(d));
	if (response == GTK_RESPONSE_OK){
		const char * new_name = choose_name_dialog_get_name(VEX_CHOOSE_NAME_DIALOG(d));
		vex_config_add_profile_defaults(local, new_name);
	}
	gtk_widget_destroy(d);
}

void profiles_editor_button_ren_clicked(GtkButton * button, ProfilesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);

	ProfileEntry * pre = (ProfileEntry*) list_v_box_get_selected_key(pe -> list_box);
	char * key = pre -> name;
	GtkWidget * d = choose_name_dialog_new(
		"Rename Profile", TRUE, "Old name:", "New name:", 
		key, key, profiles_editor_profile_name_ok_function, local);
	int response = gtk_dialog_run(GTK_DIALOG(d));
	if (response == GTK_RESPONSE_OK){
		const char * new_name = choose_name_dialog_get_name(VEX_CHOOSE_NAME_DIALOG(d));
		printf("renaming %s -> %s\n", key, new_name);
		vex_config_rename_profile(local, key, new_name);
	}
	gtk_widget_destroy(d);
}

void profiles_editor_button_dup_clicked(GtkButton * button, ProfilesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	VexConfig * global = vex_layered_config_get_config_global(pe -> vlc);

	ProfileEntry * pre = (ProfileEntry*) list_v_box_get_selected_key(pe -> list_box);
	char * key = pre -> name;
	GtkWidget * d = choose_name_dialog_new(
		"Duplicate Profile", TRUE, "Source name:", "Copy name:", 
		key, key, profiles_editor_profile_name_ok_function, local);
	int response = gtk_dialog_run(GTK_DIALOG(d));
	if (response == GTK_RESPONSE_OK){
		const char * new_name = choose_name_dialog_get_name(VEX_CHOOSE_NAME_DIALOG(d));
		printf("duplicating %s -> %s\n", key, new_name);
		VexConfig * config = pre -> config_level == CONFIG_LEVEL_LOCAL ? local : global;
		VexProfile * profile = vex_config_get_profile_by_name(config, key);
		VexProfile * dup = vex_profile_duplicate(profile, TRUE);
		vex_config_add_profile(local, new_name, dup);
	}
	gtk_widget_destroy(d);
}

void profiles_editor_button_rem_clicked(GtkButton * button, ProfilesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);

	ProfileEntry * pre = (ProfileEntry*) list_v_box_get_selected_key(pe -> list_box);
	char * key = pre -> name;
	printf("removing %s\n", key);
	vex_config_remove_profile(local, key);
}

/* VexConfig callbacks */

void profiles_editor_added(VexConfig * vex_config, char * name, ProfilesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	//ProfileEntry * pre = (ProfileEntry*) list_v_box_get_selected_key(pe -> list_box);
	//char * key = pre -> name;
	printf("added %s\n", name);
	VexProfile * profile = vex_config_get_profile_by_name(local, name);
	profiles_editor_add_profile_box(pe, name, profile, CONFIG_LEVEL_LOCAL);
}

void profiles_editor_renamed(VexConfig * vex_config, char * name, char * new_name, ProfilesEditor * pe)
{
	printf("renamed %s -> %s\n", name, new_name);

	ProfileEntry pe_old; pe_old.name = name; pe_old.config_level = CONFIG_LEVEL_LOCAL;
	ProfileEntry * pe_new = malloc(sizeof(ProfileEntry));
	pe_new -> name = g_strdup(new_name);
	pe_new -> config_level = CONFIG_LEVEL_LOCAL;

	GtkWidget * frame = list_v_box_get(pe -> list_box, &pe_old);
	GtkWidget * content = gtk_bin_get_child(GTK_BIN(frame));
	list_v_box_rename(pe -> list_box, &pe_old, pe_new);
	profile_editor_set_name(VEX_PROFILE_EDITOR(content), new_name);
}

void profiles_editor_removed(VexConfig * vex_config, char * name, VexProfile * profiles, ProfilesEditor * pe)
{
	printf("removed %s\n", name);
	ProfileEntry pe_old; pe_old.name = name; pe_old.config_level = CONFIG_LEVEL_LOCAL;
	list_v_box_remove(pe -> list_box, &pe_old);
}

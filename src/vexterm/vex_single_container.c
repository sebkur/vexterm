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

#include "vex_single_container.h"
#include "../helpers/util.h"

#define DEFAULT_PROFILE "default"

G_DEFINE_TYPE (VexSingleContainer, vex_single_container, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint vex_single_container_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, vex_single_container_signals[SIGNAL_NAME_n], 0);

typedef struct{
	char * name;
	GtkWidget * item;
} ProfileItem;

int compare_profile_items(gconstpointer a, gconstpointer b, gpointer data)
{
	ProfileItem * pia = (ProfileItem*) a;
	ProfileItem * pib = (ProfileItem*) b;
	return strcmp(pia -> name, pib -> name);
}

void vex_single_container_build_profiles_menu(VexSingleContainer * vsc);

static void vex_single_container_grab_focus(GtkWidget * widget);

static void vex_single_container_profile_added_cb(
	VexConfig * config, char * name, VexSingleContainer * vex_single_container);
static void vex_single_container_profile_renamed_cb(
	VexConfig * config, char * name, char * new_name, VexSingleContainer * vex_single_container);
static void vex_single_container_profile_removed_cb(
	VexConfig * config, char * name, VexProfile * profile, VexSingleContainer * vex_single_container);

void vex_single_container_add_profile_entry(VexSingleContainer * vex_single_container, char * name);
void vex_single_container_remove_profile_entry(VexSingleContainer * vex_single_container, char * name);
void vex_single_container_rename_profile_entry(VexSingleContainer * vex_single_container, char * name, char * new_name);
static void vex_single_container_profile_entry_toggled_cb(GtkCheckMenuItem * item, VexSingleContainer * vex_single_container);

GtkWidget * vex_single_container_new(VexLayeredConfig * vlc, char * profile_name, char * pwd)
{
	VexSingleContainer * vsc = g_object_new(VEX_TYPE_VEX_SINGLE_CONTAINER, NULL);
	vsc -> vlc = vlc;
	vsc -> profile_name = g_strdup(profile_name);
	VexProfile * profile = vex_layered_config_get_profile_by_name(vlc, profile_name);
	vsc -> profile_state = profile != NULL ? PROFILE_FOUND_AND_ACTIVE : PROFILE_NOT_FOUND_FELL_BACK_TO_DEFAULT;
	vsc -> vex_single = VEX_VEX_SINGLE(vex_single_new(pwd, profile));
	gtk_container_add(GTK_CONTAINER(vsc), GTK_WIDGET(vsc -> vex_single));

	/* add profiles to the profiles-menu */
	vsc -> menu_profiles = gtk_menu_new();
	g_object_ref(vsc -> menu_profiles);
	//TODO: we need finalize to unref this
	vex_single_container_build_profiles_menu(vsc);

	return GTK_WIDGET(vsc);
}

static void vex_single_container_class_init(VexSingleContainerClass *class)
{
	GtkWidgetClass * widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> grab_focus = vex_single_container_grab_focus;
        /*vex_single_container_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexSingleContainerClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void vex_single_container_init(VexSingleContainer *vex_single_container)
{
}

void vex_single_container_build_profiles_menu(VexSingleContainer * vsc)
{
	/* first: remove all items from menu and make 'profile_names' a new sequence */
	if (vsc -> profile_names != NULL){
		GSequenceIter * i;
		for (i = g_sequence_get_begin_iter(vsc -> profile_names);
				!g_sequence_iter_is_end(i); i = g_sequence_iter_next(i)){
			ProfileItem * pi = (ProfileItem*) g_sequence_get(i);
			gtk_container_remove(GTK_CONTAINER(vsc -> menu_profiles), pi -> item);
		}
		g_sequence_free(vsc -> profile_names);
	}
	vsc -> profile_names = g_sequence_new(NULL);

	/* then: add items to menu */
	VexConfig * config_global = vex_layered_config_get_config_global(vsc -> vlc);
	VexConfig * config_local = vex_layered_config_get_config_local(vsc -> vlc);

	int c, p;
	VexConfig * configs[2] = {config_global, config_local};
	vsc -> menu_profiles_group = NULL;
	for (c = 0; c < 2; c++){
		VexConfig * config = configs[c];
		for (p = 0; p < vex_config_get_number_of_profiles(config); p++){
			char * name;
			vex_config_get_profile_by_index(config, p, &name);
			vex_single_container_add_profile_entry(vsc, name);
		}
	}
	/* this operation has no effect if DEFAULT_PROFILE is already existing */
	vex_single_container_add_profile_entry(vsc, DEFAULT_PROFILE);
	
	/* toggle the active profile */
	ProfileItem pis; 
	pis.name = vsc -> profile_state == PROFILE_FOUND_AND_ACTIVE ? vsc -> profile_name : DEFAULT_PROFILE;
	GSequenceIter * iter = g_sequence_find(
		vsc -> profile_names, &pis, compare_profile_items, NULL);
	if (iter != NULL){
		ProfileItem * pi = (ProfileItem*) g_sequence_get(iter);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(pi -> item), TRUE);
	}

	/* connect to config to recognize added / removed profiles */
	g_signal_connect(
		G_OBJECT(config_local), "profile-added",
		G_CALLBACK(vex_single_container_profile_added_cb), vsc);
	g_signal_connect(
		G_OBJECT(config_local), "profile-renamed",
		G_CALLBACK(vex_single_container_profile_renamed_cb), vsc);
	g_signal_connect(
		G_OBJECT(config_local), "profile-removed",
		G_CALLBACK(vex_single_container_profile_removed_cb), vsc);

}

VexSingle * vex_single_container_get_vex_single(VexSingleContainer * vsc)
{
	return vsc -> vex_single;
}

GtkWidget * vex_single_container_get_menu_profiles(VexSingleContainer * vsc)
{
	return vsc -> menu_profiles;
}

static void vex_single_container_grab_focus(GtkWidget * widget)
{
	VexSingleContainer * vsc = VEX_VEX_SINGLE_CONTAINER(widget);
	gtk_widget_grab_focus(GTK_WIDGET(vsc -> vex_single));
}

void vex_single_container_set_config(VexSingleContainer * vsc, VexLayeredConfig * vlc)
{
	vsc -> vlc = vlc;
	VexProfile * profile = vex_layered_config_get_profile_by_name(
		vlc, vsc -> profile_name);
	vex_single_set_profile(vsc -> vex_single, profile);
	vsc -> profile_state = profile != NULL ? PROFILE_FOUND_AND_ACTIVE : PROFILE_NOT_FOUND_FELL_BACK_TO_DEFAULT;
	vex_single_container_build_profiles_menu(vsc);
}

/* config callbacks */

static void vex_single_container_profile_added_cb(
	VexConfig * config, char * name, VexSingleContainer * vsc)
{
	if (strcmp(name, vsc -> profile_name) == 0){
		VexProfile * profile = vex_layered_config_get_profile_by_name(vsc -> vlc, name);
		if (profile != vex_single_get_profile(vsc -> vex_single)){
			vex_single_set_profile(vsc -> vex_single, profile);
		}
	}
	vex_single_container_add_profile_entry(vsc, name);
}

static void vex_single_container_profile_renamed_cb(
	VexConfig * config, char * name, char * new_name, VexSingleContainer * vsc)
{
	gboolean equals_active_old = strcmp(name, vsc -> profile_name) == 0;
	gboolean equals_active_new = strcmp(new_name, vsc -> profile_name) == 0;
	if (equals_active_new){
		VexProfile * profile = vex_layered_config_get_profile_by_name(vsc -> vlc, new_name);
		if (profile != vex_single_get_profile(vsc -> vex_single)){
			vex_single_set_profile(vsc -> vex_single, profile);
		}
	}
	if (equals_active_old){
		VexProfile * profile = vex_layered_config_get_profile_by_name(vsc -> vlc, name);
		if (profile != vex_single_get_profile(vsc -> vex_single)){
			vex_single_set_profile(vsc -> vex_single, profile);
		}
	}
	vex_single_container_rename_profile_entry(vsc, name, new_name);
}

static void vex_single_container_profile_removed_cb(
	VexConfig * config, char * name, VexProfile * profile, VexSingleContainer * vsc)
{
	if (strcmp(name, vsc -> profile_name) == 0){
		VexProfile * profile = vex_layered_config_get_profile_by_name(vsc -> vlc, name);
		if (profile == NULL){
			profile = vex_layered_config_get_profile_by_name(vsc -> vlc, DEFAULT_PROFILE);
		}
		if (profile != vex_single_get_profile(vsc -> vex_single)){
			vex_single_set_profile(vsc -> vex_single, profile);
		}
	}
	vex_single_container_remove_profile_entry(vsc, name);
}

/* profiles menu */

void vex_single_container_add_profile_entry(VexSingleContainer * vsc, char * name)
{
	ProfileItem pis; pis.name = name;
	GSequenceIter * iter = g_sequence_find(vsc -> profile_names, &pis, compare_profile_items, NULL);
	if (iter == NULL){
		GtkWidget * item = gtk_radio_menu_item_new_with_label(vsc -> menu_profiles_group, name);
		ProfileItem * pi = g_slice_alloc(sizeof(ProfileItem));
		pi -> name = g_strdup(name);
		pi -> item = item;
		GSequenceIter * iter = g_sequence_insert_sorted(vsc -> profile_names, pi, compare_profile_items, NULL);
		int pos = g_sequence_iter_get_position(iter);
		vsc -> menu_profiles_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
		gtk_menu_shell_insert(GTK_MENU_SHELL(vsc -> menu_profiles), item, pos);

		g_signal_connect(
			G_OBJECT(item), "toggled",
			G_CALLBACK(vex_single_container_profile_entry_toggled_cb), vsc);

		gtk_widget_show_all(item);

		if (vsc -> profile_state == PROFILE_NOT_FOUND_FELL_BACK_TO_DEFAULT){
			if (strcmp(name, vsc -> profile_name) == 0){
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);
			}
		}
	}
}

void vex_single_container_remove_profile_entry(VexSingleContainer * vsc, char * name)
{
	if (vex_layered_config_get_profile_by_name(vsc -> vlc, name) == NULL){
		if (strcmp(name, DEFAULT_PROFILE) != 0){
			ProfileItem pis; pis.name = name;
			GSequenceIter * iter = g_sequence_find(
				vsc -> profile_names, &pis, compare_profile_items, NULL);
			if (iter != NULL){
				ProfileItem * pi = (ProfileItem*) g_sequence_get(iter);
				gtk_container_remove(GTK_CONTAINER(vsc -> menu_profiles), pi -> item);
				if (strcmp(vsc -> profile_name, name) == 0){
					VexSingle * vex = vex_single_container_get_vex_single(vsc);
					//TODO:set default instead of NULL
					vex_single_set_profile(vex, NULL);
					vsc -> profile_state = PROFILE_NOT_FOUND_FELL_BACK_TO_DEFAULT;
					ProfileItem pis; pis.name = DEFAULT_PROFILE;
					GSequenceIter * iter = g_sequence_find(
						vsc -> profile_names, &pis, compare_profile_items, NULL);
					if (iter != NULL){
						ProfileItem * pi = (ProfileItem*) g_sequence_get(iter);
						gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(pi -> item), TRUE);
					}
				}
				g_sequence_remove(iter);
			}
		}
	}
}

void vex_single_container_rename_profile_entry(VexSingleContainer * vsc, char * name, char * new_name)
{
	if (vex_layered_config_get_profile_by_name(vsc -> vlc, name) == NULL){
		vex_single_container_remove_profile_entry(vsc, name);
	}
	vex_single_container_add_profile_entry(vsc, new_name);
}

static void vex_single_container_profile_entry_toggled_cb(GtkCheckMenuItem * item, VexSingleContainer * vsc)
{
	if (!gtk_check_menu_item_get_active(item)){
		return;
	}
	char * selected = NULL;
	/* find selected profile */
	GSequenceIter * i;
	for (i = g_sequence_get_begin_iter(vsc -> profile_names); !g_sequence_iter_is_end(i); i = g_sequence_iter_next(i)){
		ProfileItem * pi = (ProfileItem*) g_sequence_get(i);
		if (pi -> item == GTK_WIDGET(item)){
			selected = pi -> name;
			break;
		}
	}
	VexProfile * profile = vex_layered_config_get_profile_by_name(vsc -> vlc, selected);
	VexSingle * vex_single = vsc -> vex_single;
	vex_single_set_profile(vex_single, profile);
	if (vsc -> profile_state != PROFILE_NOT_FOUND_FELL_BACK_TO_DEFAULT || strcmp(selected, DEFAULT_PROFILE) != 0){
		vsc -> profile_name = g_strdup(selected);
	}
}


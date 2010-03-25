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

#include "colour_schemes_editor.h"
#include "colour/colour_palette.h"
#include "../../gtkplus/dialog/choose_name_dialog.h"

#include "../config/vex_colour_scheme.h"

G_DEFINE_TYPE (ColourSchemesEditor, colour_schemes_editor, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint colour_schemes_editor_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, colour_schemes_editor_signals[SIGNAL_NAME_n], 0);

void colour_schemes_editor_constructor(ColourSchemesEditor * cse);
void colour_schemes_editor_finalize(GObject * object);
void colour_schemes_editor_list_selection_changed(ColourSchemesEditor * cse, int index);
void colour_schemes_editor_add_palette_box(ColourSchemesEditor * cse,
	 char * name, VexColourScheme * vcs, ConfigLevel config_level);
void colour_schemes_editor_added(VexConfig * vex_config, char * name, ColourSchemesEditor * pe);
void colour_schemes_editor_renamed(VexConfig * vex_config, char * name, char * new_name, ColourSchemesEditor * pe);
void colour_schemes_editor_removed(
	VexConfig * vex_config, char * name, VexColourScheme * colour_schemes, ColourSchemesEditor * pe);

typedef struct {
	char * name;
	ConfigLevel config_level;
} ColourSchemeEntry;

gint compare_colour_schemes(gconstpointer a, gconstpointer b, gpointer data)
{
	ColourSchemeEntry * csa = (ColourSchemeEntry*)a;
	ColourSchemeEntry * csb = (ColourSchemeEntry*)b;
	int c = strcmp(csa -> name, csb -> name);
	if (c == 0){
		if (csa -> config_level == csb -> config_level) return 0;
		c = csa -> config_level == CONFIG_LEVEL_GLOBAL ? -1 : 1;
	}
	return c;
}

gboolean colour_schemes_editor_colour_scheme_name_ok_function(const char * name, gpointer data)
{
	VexConfig * vex_config = (VexConfig*) data;
	VexColourScheme * colour_scheme = vex_config_get_colour_scheme_by_name(vex_config, name);
	return colour_scheme == NULL;
}

GtkWidget * colour_schemes_editor_new(VexLayeredConfig * vlc)
{
	ColourSchemesEditor * colour_schemes_editor = g_object_new(VEX_TYPE_COLOUR_SCHEMES_EDITOR, NULL);

	colour_schemes_editor -> vlc = vlc;

	colour_schemes_editor_constructor(colour_schemes_editor);
	return GTK_WIDGET(colour_schemes_editor);
}

static void colour_schemes_editor_class_init(ColourSchemesEditorClass *class)
{
	GObjectClass * object_class = G_OBJECT_CLASS(class);
	object_class -> finalize = colour_schemes_editor_finalize;
        /*colour_schemes_editor_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ColourSchemesEditorClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void colour_schemes_editor_init(ColourSchemesEditor *colour_schemes_editor)
{
}

void colour_schemes_editor_palette_changed_cb(
	ColourPalette * palette, int index, int r, int g, int b, int a, VexColourScheme * vcs)
{
	printf("colour #%d: %d %d %d %d\n", index, r, g, b, a);
	VexColour colour;
	colour.r = r;
	colour.g = g;
	colour.b = b;
	colour.a = a;
	vex_colour_scheme_set_colour(vcs, index, colour);
}

void colour_schemes_editor_select(ListVBox * box, gpointer key, ColourSchemesEditor * pe);
void colour_schemes_editor_button_new_clicked(GtkButton * button, ColourSchemesEditor * pe);
void colour_schemes_editor_button_ren_clicked(GtkButton * button, ColourSchemesEditor * pe);
void colour_schemes_editor_button_dup_clicked(GtkButton * button, ColourSchemesEditor * pe);
void colour_schemes_editor_button_rem_clicked(GtkButton * button, ColourSchemesEditor * pe);

void colour_schemes_editor_finalize(GObject * object)
{
	ColourSchemesEditor * cse = (ColourSchemesEditor*) object;
	VexConfig * local = vex_layered_config_get_config_local(cse -> vlc);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(colour_schemes_editor_added), cse);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(colour_schemes_editor_renamed), cse);
	g_signal_handlers_disconnect_by_func(
		G_OBJECT(local),
		G_CALLBACK(colour_schemes_editor_removed), cse);
}

void colour_schemes_editor_constructor(ColourSchemesEditor * cse)
{
	VexConfig * global = vex_layered_config_get_config_global(cse -> vlc);
	VexConfig * local = vex_layered_config_get_config_local(cse -> vlc);

	/* colour palettes */
	cse -> list_box = VEX_LIST_V_BOX(list_v_box_new(compare_colour_schemes, NULL));

	g_signal_connect(
		G_OBJECT(cse -> list_box), "select",
		G_CALLBACK(colour_schemes_editor_select), cse);

	g_signal_connect(
		G_OBJECT(local), "colour-scheme-added",
		G_CALLBACK(colour_schemes_editor_added), cse);
	g_signal_connect(
		G_OBJECT(local), "colour-scheme-renamed",
		G_CALLBACK(colour_schemes_editor_renamed), cse);
	g_signal_connect(
		G_OBJECT(local), "colour-scheme-removed",
		G_CALLBACK(colour_schemes_editor_removed), cse);

        GtkWidget * scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), GTK_WIDGET(cse -> list_box));
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);

	int i, n_schemes;

	n_schemes = vex_config_get_number_of_colour_schemes(local);
	for (i = 0; i < n_schemes; i++){
		char * name;
		VexColourScheme * vcs = vex_config_get_colour_scheme_by_index(local, i, &name);
		colour_schemes_editor_add_palette_box(cse, name, vcs, CONFIG_LEVEL_LOCAL);
	}
	n_schemes = vex_config_get_number_of_colour_schemes(global);
	for (i = 0; i < n_schemes; i++){
		char * name;
		VexColourScheme * vcs = vex_config_get_colour_scheme_by_index(global, i, &name);
		colour_schemes_editor_add_palette_box(cse, name, vcs, CONFIG_LEVEL_GLOBAL);
	}

	/* buttons */
	GtkWidget * box_s_buttons = gtk_hbox_new(FALSE, 0);

	GtkWidget * image_new = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
	GtkWidget * image_ren = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
	GtkWidget * image_dup = gtk_image_new_from_stock("gtk-copy", GTK_ICON_SIZE_MENU);
	GtkWidget * image_rem = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);

	cse -> button_s_new = gtk_button_new_with_label("new");
	cse -> button_s_ren = gtk_button_new_with_label("rename");
	cse -> button_s_dup = gtk_button_new_with_label("duplicate");
	cse -> button_s_rem = gtk_button_new_with_label("remove");

	gtk_button_set_image(GTK_BUTTON(cse -> button_s_new), image_new);
	gtk_button_set_image(GTK_BUTTON(cse -> button_s_ren), image_ren);
	gtk_button_set_image(GTK_BUTTON(cse -> button_s_dup), image_dup);
	gtk_button_set_image(GTK_BUTTON(cse -> button_s_rem), image_rem);

	gtk_box_pack_start(GTK_BOX(box_s_buttons), cse -> button_s_new, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_s_buttons), cse -> button_s_ren, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_s_buttons), cse -> button_s_dup, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_s_buttons), cse -> button_s_rem, FALSE, FALSE, 0);

	gtk_widget_set_sensitive(cse -> button_s_ren, FALSE);
	gtk_widget_set_sensitive(cse -> button_s_dup, FALSE);
	gtk_widget_set_sensitive(cse -> button_s_rem, FALSE);

	g_signal_connect(G_OBJECT(cse -> button_s_new), "clicked", G_CALLBACK(colour_schemes_editor_button_new_clicked), cse);
	g_signal_connect(G_OBJECT(cse -> button_s_ren), "clicked", G_CALLBACK(colour_schemes_editor_button_ren_clicked), cse);
	g_signal_connect(G_OBJECT(cse -> button_s_dup), "clicked", G_CALLBACK(colour_schemes_editor_button_dup_clicked), cse);
	g_signal_connect(G_OBJECT(cse -> button_s_rem), "clicked", G_CALLBACK(colour_schemes_editor_button_rem_clicked), cse);

	/* overall layout */
	gtk_box_pack_start(GTK_BOX(cse), box_s_buttons, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(cse), scrolled, TRUE, TRUE, 0);
}

void colour_schemes_editor_add_palette_box(ColourSchemesEditor * cse,
	 char * name, VexColourScheme * vcs, ConfigLevel config_level)
{
	GtkWidget * palette = colour_palette_new(vcs);
	colour_palette_use_colour_scheme(VEX_COLOUR_PALETTE(palette), vcs);

	GtkWidget * pbox = gtk_vbox_new(FALSE, 0);
	GtkWidget * frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(frame), pbox);
	gtk_container_set_border_width(GTK_CONTAINER(pbox), 4);

	GtkWidget * label = gtk_label_new(name);
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);

	gtk_box_pack_start(GTK_BOX(pbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(pbox), palette, FALSE, FALSE, 0);

	g_signal_connect(
		G_OBJECT(palette), "changed-colour",
		G_CALLBACK(colour_schemes_editor_palette_changed_cb), vcs);

	gtk_widget_set_sensitive(pbox, config_level == CONFIG_LEVEL_LOCAL);

	ColourSchemeEntry * csentry = malloc(sizeof(ColourSchemeEntry));
	csentry -> name = g_strdup(name);
	csentry -> config_level = config_level;
	list_v_box_insert(VEX_LIST_V_BOX(cse -> list_box), csentry, frame);

	gtk_widget_show_all(frame);

	g_object_set(G_OBJECT(frame), "user-data", label, NULL);
}

void colour_schemes_editor_select(ListVBox * box, gpointer key, ColourSchemesEditor * pe)
{
	printf("selected %p\n", key);
	gboolean ren_rem = FALSE;
	gboolean dup = key != NULL;
	if (key != NULL){
		ColourSchemeEntry * pre = (ColourSchemeEntry*) key;
		ren_rem = pre -> config_level == CONFIG_LEVEL_LOCAL;
	}
	gtk_widget_set_sensitive(pe -> button_s_ren, ren_rem);
	gtk_widget_set_sensitive(pe -> button_s_dup, dup);
	gtk_widget_set_sensitive(pe -> button_s_rem, ren_rem);
}

/* button callbacks */

void colour_schemes_editor_button_new_clicked(GtkButton * button, ColourSchemesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	GtkWidget * d = choose_name_dialog_new(
		"New Colour Scheme", FALSE, "", "Name:", "", "new", 
		colour_schemes_editor_colour_scheme_name_ok_function, local);
	int response = gtk_dialog_run(GTK_DIALOG(d));
	if (response == GTK_RESPONSE_OK){
		const char * new_name = choose_name_dialog_get_name(VEX_CHOOSE_NAME_DIALOG(d));
		vex_config_add_colour_scheme_defaults(local, new_name);
	}
	gtk_widget_destroy(d);
}

void colour_schemes_editor_button_ren_clicked(GtkButton * button, ColourSchemesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	ColourSchemeEntry * pre = (ColourSchemeEntry*) list_v_box_get_selected_key(pe -> list_box);
	char * key = pre -> name;
	GtkWidget * d = choose_name_dialog_new(
		"Rename Colour Scheme", TRUE, "Old name:", "New name:", 
		key, key, colour_schemes_editor_colour_scheme_name_ok_function, local);
	int response = gtk_dialog_run(GTK_DIALOG(d));
	if (response == GTK_RESPONSE_OK){
		const char * new_name = choose_name_dialog_get_name(VEX_CHOOSE_NAME_DIALOG(d));
		printf("renaming %s -> %s\n", key, new_name);
		vex_config_rename_colour_scheme(local, key, new_name);
	}
	gtk_widget_destroy(d);
}

void colour_schemes_editor_button_dup_clicked(GtkButton * button, ColourSchemesEditor * pe)
{
	VexConfig * global = vex_layered_config_get_config_global(pe -> vlc);
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	ColourSchemeEntry * pre = (ColourSchemeEntry*) list_v_box_get_selected_key(pe -> list_box);
	char * key = pre -> name;
	GtkWidget * d = choose_name_dialog_new(
		"Duplicate Colour Scheme", TRUE, "Source name:", "Copy name:", 
		key, key, colour_schemes_editor_colour_scheme_name_ok_function, local);
	int response = gtk_dialog_run(GTK_DIALOG(d));
	if (response == GTK_RESPONSE_OK){
		const char * new_name = choose_name_dialog_get_name(VEX_CHOOSE_NAME_DIALOG(d));
		printf("duplicating %s -> %s\n", key, new_name);
		VexConfig * config = pre -> config_level == CONFIG_LEVEL_LOCAL ? local : global;
		VexColourScheme * vcs = vex_config_get_colour_scheme_by_name(config, key);
		VexColourScheme * dup = vex_colour_scheme_duplicate(vcs);
		vex_config_add_colour_scheme(local, new_name, dup);
	}
	gtk_widget_destroy(d);
}

void colour_schemes_editor_button_rem_clicked(GtkButton * button, ColourSchemesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	ColourSchemeEntry * pre = (ColourSchemeEntry*) list_v_box_get_selected_key(pe -> list_box);
	char * key = pre -> name;
	printf("removing %s\n", key);
	vex_config_remove_colour_scheme(local, key);
}

/* VexConfig callbacks */

void colour_schemes_editor_added(VexConfig * vex_config, char * name, ColourSchemesEditor * pe)
{
	VexConfig * local = vex_layered_config_get_config_local(pe -> vlc);
	printf("added %s\n", name);
	VexColourScheme * vcs = vex_config_get_colour_scheme_by_name(local, name);
	colour_schemes_editor_add_palette_box(pe, name, vcs, CONFIG_LEVEL_LOCAL);
}

void colour_schemes_editor_renamed(VexConfig * vex_config, char * name, char * new_name, ColourSchemesEditor * pe)
{
	printf("renamed %s -> %s\n", name, new_name);

	ColourSchemeEntry pe_old; pe_old.name = name; pe_old.config_level = CONFIG_LEVEL_LOCAL;
	ColourSchemeEntry * pe_new = malloc(sizeof(ColourSchemeEntry));
	pe_new -> name = g_strdup(new_name);
	pe_new -> config_level = CONFIG_LEVEL_LOCAL;

	GtkWidget * frame = list_v_box_get(pe -> list_box, &pe_old);
	printf("frame: %p\n", frame);
	GtkWidget * content = gtk_bin_get_child(GTK_BIN(frame));
	list_v_box_rename(pe -> list_box, &pe_old, pe_new);
	//TODO:
	//colour_schemes_editor_set_name(VEX_COLOUR_SCHEMES_EDITOR(content), new_name);
	GtkWidget * label;
	g_object_get(G_OBJECT(frame), "user-data", &label, NULL);
	gtk_label_set_text(GTK_LABEL(label), new_name);
	printf("%p %p\n", frame, content);
}

void colour_schemes_editor_removed(
	VexConfig * vex_config, char * name, VexColourScheme * colour_schemes, ColourSchemesEditor * pe)
{
	printf("removed %s\n", name);
	ColourSchemeEntry pe_old; pe_old.name = name; pe_old.config_level = CONFIG_LEVEL_LOCAL;
	list_v_box_remove(pe -> list_box, &pe_old);
}

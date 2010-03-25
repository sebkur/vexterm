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

#include "preferences.h"
#include "settings_editor.h"
#include "colour_schemes_editor.h"
#include "profiles_editor.h"

G_DEFINE_TYPE (Preferences, preferences, GTK_TYPE_DIALOG);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint preferences_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, preferences_signals[SIGNAL_NAME_n], 0);

void preferences_constructor(Preferences * preferences);

GtkWidget * preferences_new(VexLayeredConfig * vlc)
{
	Preferences * preferences = g_object_new(VEX_TYPE_PREFERENCES, NULL);

	preferences -> vlc = vlc;

	preferences_constructor(preferences);
	return GTK_WIDGET(preferences);
}

static void preferences_class_init(PreferencesClass *class)
{
        /*preferences_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (PreferencesClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void preferences_init(Preferences *preferences)
{
}

void preferences_constructor(Preferences * preferences)
{
	GtkWidget * notebook = gtk_notebook_new();

	/* notebook */
	GtkWidget * se = settings_editor_new(preferences -> vlc);
	GtkWidget * pe = profiles_editor_new(preferences -> vlc);
	GtkWidget * cse = colour_schemes_editor_new(preferences -> vlc);

	GtkWidget * label_settings = gtk_label_new("Settings");
	GtkWidget * label_font= gtk_label_new("Profiles");
	GtkWidget * label_colours = gtk_label_new("Colours");

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), se, label_settings);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pe, label_font);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), cse, label_colours);

	GtkWidget * content = gtk_dialog_get_content_area(GTK_DIALOG(preferences));
	gtk_box_pack_start(GTK_BOX(content), notebook, TRUE, TRUE, 0);

	/* buttons */
//	GtkWidget * button_cancel = gtk_button_new_from_stock("gtk-cancel");
//	GtkWidget * button_apply = gtk_button_new_from_stock("gtk-ok");

	gtk_dialog_add_button(GTK_DIALOG(preferences), "Cancel", GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button(GTK_DIALOG(preferences), "Ok", GTK_RESPONSE_OK);
}

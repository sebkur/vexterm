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

#include "choose_name_dialog.h"

G_DEFINE_TYPE (ChooseNameDialog, choose_name_dialog, GTK_TYPE_DIALOG);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint choose_name_dialog_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, choose_name_dialog_signals[SIGNAL_NAME_n], 0);

void choose_name_dialog_constructor(ChooseNameDialog * dialog, 
	char * title, gboolean show_old, char * old_title, char * new_title, char * old_name, char * new_name);
void entry_changed_cb(GtkEditable * editable, gpointer user_data);

GtkWidget * choose_name_dialog_new(
	char * title, gboolean show_old_name,
	char * old_title, char * new_title,
	char * old_name, char * new_name,
	choose_name_dialog_name_ok_function name_ok_fun, 
	gpointer name_ok_data)
{
	ChooseNameDialog * choose_name_dialog = g_object_new(VEX_TYPE_CHOOSE_NAME_DIALOG, NULL);
	choose_name_dialog -> name_ok_function = name_ok_fun;
	choose_name_dialog -> name_ok_data = name_ok_data;
	choose_name_dialog_constructor(
		choose_name_dialog, title, show_old_name, old_title, new_title, old_name, new_name);
	return GTK_WIDGET(choose_name_dialog);
}

static void choose_name_dialog_class_init(ChooseNameDialogClass *class)
{
        /*choose_name_dialog_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (ChooseNameDialogClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void choose_name_dialog_init(ChooseNameDialog *choose_name_dialog)
{
}

void choose_name_dialog_constructor(ChooseNameDialog * dialog, 
	char * title, gboolean show_old, char * old_title, char * new_title, char * old_name, char * new_name)
{
	gtk_window_set_title(GTK_WINDOW(dialog), title);

	gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
	dialog -> button_ok = gtk_dialog_add_button(GTK_DIALOG(dialog), "Ok", GTK_RESPONSE_OK);

	GtkWidget * content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	GtkWidget * table = gtk_table_new(2, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(content), table, FALSE, FALSE, 0);

	GtkWidget * label_old_name = gtk_label_new(old_title);
	GtkWidget * label_new_name = gtk_label_new(new_title);
	gtk_misc_set_alignment(GTK_MISC(label_old_name), 0.0, 0.0);
	gtk_misc_set_alignment(GTK_MISC(label_new_name), 0.0, 0.0);
	GtkWidget * entry_old_name = gtk_entry_new();
	dialog -> entry_new_name = gtk_entry_new();

	g_signal_connect(
		G_OBJECT(dialog -> entry_new_name), "changed",
		G_CALLBACK(entry_changed_cb), dialog);

	gtk_entry_set_text(GTK_ENTRY(entry_old_name), old_name);
	gtk_entry_set_text(GTK_ENTRY(dialog -> entry_new_name), new_name);
	gtk_editable_set_editable(GTK_EDITABLE(entry_old_name), FALSE);
	gtk_widget_set_sensitive(entry_old_name, FALSE);

	if (show_old){
		gtk_table_attach(GTK_TABLE(table), label_old_name, 0, 1, 0, 1, GTK_FILL, 0, 2, 0);
		gtk_table_attach(GTK_TABLE(table), entry_old_name, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 2, 0);
	}
	gtk_table_attach(GTK_TABLE(table), label_new_name, 0, 1, 1, 2, GTK_FILL, 0, 2, 0);
	gtk_table_attach(GTK_TABLE(table), dialog -> entry_new_name, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 2, 0);

	gtk_widget_show_all(content);
}

void entry_changed_cb(GtkEditable * editable, gpointer user_data)
{
	ChooseNameDialog * cnd = (ChooseNameDialog*) user_data;
	const char * name = gtk_entry_get_text(GTK_ENTRY(editable));
	gboolean ok = cnd -> name_ok_function(name, cnd -> name_ok_data);
	gtk_widget_set_sensitive(cnd -> button_ok, ok);
}

const char * choose_name_dialog_get_name(ChooseNameDialog * dialog)
{
	return gtk_entry_get_text(GTK_ENTRY(dialog -> entry_new_name));
}

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

#ifndef _CHOOSE_NAME_DIALOG_H_
#define _CHOOSE_NAME_DIALOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define VEX_TYPE_CHOOSE_NAME_DIALOG           (choose_name_dialog_get_type ())
#define VEX_CHOOSE_NAME_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_CHOOSE_NAME_DIALOG, ChooseNameDialog))
#define VEX_CHOOSE_NAME_DIALOG_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_CHOOSE_NAME_DIALOG, ChooseNameDialogClass))
#define VEX_IS_CHOOSE_NAME_DIALOG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_CHOOSE_NAME_DIALOG))
#define VEX_IS_CHOOSE_NAME_DIALOG_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_CHOOSE_NAME_DIALOG))
#define VEX_CHOOSE_NAME_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_CHOOSE_NAME_DIALOG, ChooseNameDialogClass))

typedef struct _ChooseNameDialog        ChooseNameDialog;
typedef struct _ChooseNameDialogClass   ChooseNameDialogClass;

typedef gboolean (*choose_name_dialog_name_ok_function) (const char * name, gpointer user_data);

struct _ChooseNameDialog
{
	GtkDialog parent;

	GtkWidget * button_ok;
	GtkWidget * entry_new_name;

	choose_name_dialog_name_ok_function name_ok_function;
	gpointer name_ok_data;
};

struct _ChooseNameDialogClass
{
	GtkDialogClass parent_class;

	//void (* function_name) (ChooseNameDialog *choose_name_dialog);
};

GType choose_name_dialog_get_type (void) G_GNUC_CONST;

GtkWidget * choose_name_dialog_new(
	char * title, gboolean show_old_name,
	char * old_title, char * new_title,
	char * old_name, char * new_name,
	choose_name_dialog_name_ok_function name_ok_fun, 
	gpointer name_ok_data);

const char * choose_name_dialog_get_name(ChooseNameDialog * dialog);

#endif /* _CHOOSE_NAME_DIALOG_H_ */

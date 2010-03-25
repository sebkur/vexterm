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

#ifndef _SETTINGS_EDITOR_H_
#define _SETTINGS_EDITOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../config/vex_config.h"
#include "../config/vex_layered_config.h"

#define VEX_TYPE_SETTINGS_EDITOR           (settings_editor_get_type ())
#define VEX_SETTINGS_EDITOR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_SETTINGS_EDITOR, SettingsEditor))
#define VEX_SETTINGS_EDITOR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_SETTINGS_EDITOR, SettingsEditorClass))
#define VEX_IS_SETTINGS_EDITOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_SETTINGS_EDITOR))
#define VEX_IS_SETTINGS_EDITOR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_SETTINGS_EDITOR))
#define VEX_SETTINGS_EDITOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_SETTINGS_EDITOR, SettingsEditorClass))

typedef struct _SettingsEditor        SettingsEditor;
typedef struct _SettingsEditorClass   SettingsEditorClass;

struct _SettingsEditor
{
	GtkVBox parent;

	VexLayeredConfig * vlc;

	GtkWidget * box_profiles;
	GSequence * profile_names;
};

struct _SettingsEditorClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (SettingsEditor *settings_editor);
};

GType settings_editor_get_type (void) G_GNUC_CONST;

GtkWidget * settings_editor_new(VexLayeredConfig * vlc);

#endif /* _SETTINGS_EDITOR_H_ */

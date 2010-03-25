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

#ifndef _PROFILES_EDITOR_H_
#define _PROFILES_EDITOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../config/vex_config.h"
#include "../config/vex_layered_config.h"
#include "list_v_box.h"

#define VEX_TYPE_PROFILES_EDITOR           (profiles_editor_get_type ())
#define VEX_PROFILES_EDITOR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_PROFILES_EDITOR, ProfilesEditor))
#define VEX_PROFILES_EDITOR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_PROFILES_EDITOR, ProfilesEditorClass))
#define VEX_IS_PROFILES_EDITOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_PROFILES_EDITOR))
#define VEX_IS_PROFILES_EDITOR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_PROFILES_EDITOR))
#define VEX_PROFILES_EDITOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_PROFILES_EDITOR, ProfilesEditorClass))

typedef struct _ProfilesEditor        ProfilesEditor;
typedef struct _ProfilesEditorClass   ProfilesEditorClass;

struct _ProfilesEditor
{
	GtkVBox parent;

	VexLayeredConfig * vlc;

	GtkWidget * profile_editor;

	GtkWidget * button_s_new;
	GtkWidget * button_s_ren;
	GtkWidget * button_s_dup;
	GtkWidget * button_s_rem;

	ListVBox * list_box;
};

struct _ProfilesEditorClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (ProfilesEditor *profiles_editor);
};

GType profiles_editor_get_type (void) G_GNUC_CONST;

GtkWidget * profiles_editor_new(VexLayeredConfig * vlc);

#endif /* _PROFILES_EDITOR_H_ */

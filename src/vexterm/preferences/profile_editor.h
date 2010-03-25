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

#ifndef _PROFILE_EDITOR_H_
#define _PROFILE_EDITOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../config/vex_config.h"
#include "../config/vex_profile.h"

#define VEX_TYPE_PROFILE_EDITOR           (profile_editor_get_type ())
#define VEX_PROFILE_EDITOR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_PROFILE_EDITOR, ProfileEditor))
#define VEX_PROFILE_EDITOR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_PROFILE_EDITOR, ProfileEditorClass))
#define VEX_IS_PROFILE_EDITOR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_PROFILE_EDITOR))
#define VEX_IS_PROFILE_EDITOR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_PROFILE_EDITOR))
#define VEX_PROFILE_EDITOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_PROFILE_EDITOR, ProfileEditorClass))

typedef struct _ProfileEditor        ProfileEditor;
typedef struct _ProfileEditorClass   ProfileEditorClass;

struct _ProfileEditor
{
	GtkVBox parent;

	VexConfig * config_global;
	VexConfig * config_local;
	VexProfile * profile;

	GSequence * scheme_names;

	GtkWidget * label;
	GtkWidget * box_schemes;
};

struct _ProfileEditorClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (ProfileEditor *profile_editor);
};

GType profile_editor_get_type (void) G_GNUC_CONST;

GtkWidget * profile_editor_new(VexConfig * config_global, VexConfig * config_local, VexProfile * profile, char * name);

void profile_editor_set_name(ProfileEditor * profile_editor, char * name);

#endif /* _PROFILE_EDITOR_H_ */

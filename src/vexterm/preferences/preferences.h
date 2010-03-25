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

#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "../config/vex_config.h"
#include "../config/vex_layered_config.h"

#define VEX_TYPE_PREFERENCES           (preferences_get_type ())
#define VEX_PREFERENCES(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_PREFERENCES, Preferences))
#define VEX_PREFERENCES_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_PREFERENCES, PreferencesClass))
#define VEX_IS_PREFERENCES(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_PREFERENCES))
#define VEX_IS_PREFERENCES_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_PREFERENCES))
#define VEX_PREFERENCES_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_PREFERENCES, PreferencesClass))

typedef struct _Preferences        Preferences;
typedef struct _PreferencesClass   PreferencesClass;

struct _Preferences
{
	GtkDialog parent;

	VexLayeredConfig * vlc;
};

struct _PreferencesClass
{
	GtkDialogClass parent_class;

	//void (* function_name) (Preferences *preferences);
};

GType preferences_get_type (void) G_GNUC_CONST;

GtkWidget * preferences_new(VexLayeredConfig * vlc);

#endif /* _PREFERENCES_H_ */

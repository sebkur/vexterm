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

#ifndef _VEX_TERM_H_
#define _VEX_TERM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config/vex_config.h"
#include "config/vex_layered_config.h"
#include "menu.h"

#define VEX_TYPE_VEX_TERM           (vex_term_get_type ())
#define VEX_VEX_TERM(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_TERM, VexTerm))
#define VEX_VEX_TERM_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_TERM, VexTermClass))
#define VEX_IS_VEX_TERM(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_TERM))
#define VEX_IS_VEX_TERM_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_TERM))
#define VEX_VEX_TERM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_TERM, VexTermClass))

GType   vex_term_get_type (void) G_GNUC_CONST;

typedef struct _VexTerm        VexTerm;
typedef struct _VexTermClass   VexTermClass;

struct _VexTerm
{
	GtkVBox parent;

	VexLayeredConfig * vlc;

	GArray * terminals;
	GtkWidget * notebook;

	Menu * menu;
	gboolean menu_visible;
	gboolean show_status_bar;
	gboolean show_scrolling_region;
	GtkPositionType tabs_position;

	GtkWidget * preferences;
	VexLayeredConfig * vlc_copy;

	GSequence * profile_names;
	GtkWidget * menu_profiles;
	GSList * menu_profiles_group;
	int current_index;
};

struct _VexTermClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (VexTerm *vex_term);
};

GType vex_term_get_type (void) G_GNUC_CONST;

GtkWidget * vex_term_new(VexLayeredConfig * vlc);

#endif /* _VEX_TERM_H_ */

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

#ifndef _VEX_PROFILE_H_
#define _VEX_PROFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_colour_scheme.h"

#define VEX_TYPE_VEX_PROFILE           (vex_profile_get_type ())
#define VEX_VEX_PROFILE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_PROFILE, VexProfile))
#define VEX_VEX_PROFILE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_PROFILE, VexProfileClass))
#define VEX_IS_VEX_PROFILE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_PROFILE))
#define VEX_IS_VEX_PROFILE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_PROFILE))
#define VEX_VEX_PROFILE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_PROFILE, VexProfileClass))

typedef struct _VexProfile        VexProfile;
typedef struct _VexProfileClass   VexProfileClass;

struct _VexProfile
{
	GObject parent;

	char *		font_name;
	int 		font_size;
	char * 		active_colour_scheme;
	VexColourScheme * colour_scheme;
};

struct _VexProfileClass
{
	GObjectClass parent_class;

	void (* changed_font) (VexProfile *vex_profile);
	void (* changed_font_name) (VexProfile *vex_profile);
	void (* changed_font_size) (VexProfile *vex_profile);
	void (* changed_colour_scheme) (VexProfile *vex_profile);
	void (* changed_colour_scheme_name) (VexProfile *vex_profile);
};

GType vex_profile_get_type (void) G_GNUC_CONST;

VexProfile * vex_profile_new();

void vex_profile_set_font_name(VexProfile * vex_profile, char * font_name);
void vex_profile_set_font_size(VexProfile * vex_profile, int font_size);
void vex_profile_set_font(VexProfile * vex_profile, char * font_name, int font_size);
void vex_profile_set_active_colour_scheme(VexProfile * vex_profile, VexColourScheme * scheme);
void vex_profile_set_active_colour_scheme_name(VexProfile * vex_profile, char * name);

char * vex_profile_get_font_name(VexProfile * vex_profile);
int vex_profile_get_font_size(VexProfile * vex_profile);
VexColourScheme * vex_profile_get_active_colour_scheme(VexProfile * vex_profile);
char * vex_profile_get_active_colour_scheme_name(VexProfile * vex_profile);

VexProfile * vex_profile_duplicate(VexProfile * vex_profile, gboolean dup_weak_ref);

#endif /* _VEX_PROFILE_H_ */

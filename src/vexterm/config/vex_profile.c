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

#include "vex_profile.h"

G_DEFINE_TYPE (VexProfile, vex_profile, G_TYPE_OBJECT);

enum
{
	CHANGED_FONT,
	CHANGED_FONT_NAME,
	CHANGED_FONT_SIZE,
	CHANGED_COLOUR_SCHEME,
	CHANGED_COLOUR_SCHEME_NAME,
        LAST_SIGNAL
};

static guint vex_profile_signals[LAST_SIGNAL] = { 0 };

VexProfile * vex_profile_new()
{
	VexProfile * vex_profile = g_object_new(VEX_TYPE_VEX_PROFILE, NULL);
	return vex_profile;
}

static void vex_profile_class_init(VexProfileClass *class)
{
        vex_profile_signals[CHANGED_FONT] = g_signal_new(
                "changed-font",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexProfileClass, changed_font),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        vex_profile_signals[CHANGED_FONT_NAME] = g_signal_new(
                "changed-font-name",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexProfileClass, changed_font_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        vex_profile_signals[CHANGED_FONT_SIZE] = g_signal_new(
                "changed-font-size",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexProfileClass, changed_font_size),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        vex_profile_signals[CHANGED_COLOUR_SCHEME] = g_signal_new(
                "changed-colour-scheme",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexProfileClass, changed_colour_scheme),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
        vex_profile_signals[CHANGED_COLOUR_SCHEME_NAME] = g_signal_new(
                "changed-colour-scheme-name",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexProfileClass, changed_colour_scheme_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void vex_profile_init(VexProfile *vex_profile)
{
	vex_profile -> font_name = g_strdup("Monospace");
	vex_profile -> font_size = 10;
	vex_profile -> active_colour_scheme = g_strdup("default");
}

VexProfile * vex_profile_duplicate(VexProfile * vex_profile, gboolean dup_weak_ref)
{
	VexProfile * p = vex_profile_new();
	vex_profile_set_font_name(p, vex_profile_get_font_name(vex_profile));
	vex_profile_set_font_size(p, vex_profile_get_font_size(vex_profile));
	if (dup_weak_ref){
		vex_profile_set_active_colour_scheme(p, vex_profile_get_active_colour_scheme(vex_profile));
	}
	vex_profile_set_active_colour_scheme_name(p, vex_profile_get_active_colour_scheme_name(vex_profile));
	return p;
}

void vex_profile_set_font_name(VexProfile * vex_profile, char * font_name)
{
	g_free(vex_profile -> font_name);
	vex_profile -> font_name = g_strdup(font_name);
	g_signal_emit (vex_profile, vex_profile_signals[CHANGED_FONT_NAME], 0);
}

void vex_profile_set_font_size(VexProfile * vex_profile, int font_size)
{
	vex_profile -> font_size = font_size;
	g_signal_emit (vex_profile, vex_profile_signals[CHANGED_FONT_SIZE], 0);
}

void vex_profile_set_font(VexProfile * vex_profile, char * font_name, int font_size)
{
	g_free(vex_profile -> font_name);
	vex_profile -> font_name = g_strdup(font_name);
	vex_profile -> font_size = font_size;
	g_signal_emit (vex_profile, vex_profile_signals[CHANGED_FONT], 0);
}

void vex_profile_set_active_colour_scheme_name(VexProfile * vex_profile, char * name)
{
	g_free(vex_profile -> active_colour_scheme);
	vex_profile -> active_colour_scheme = g_strdup(name);
	g_signal_emit (vex_profile, vex_profile_signals[CHANGED_COLOUR_SCHEME_NAME], 0);
}

void vex_profile_set_active_colour_scheme(VexProfile * vex_profile, VexColourScheme * scheme)
{
	vex_profile -> colour_scheme = scheme;
	g_signal_emit (vex_profile, vex_profile_signals[CHANGED_COLOUR_SCHEME], 0);
}

char * vex_profile_get_font_name(VexProfile * vex_profile)
{
	return vex_profile -> font_name;
}

int vex_profile_get_font_size(VexProfile * vex_profile)
{
	return vex_profile -> font_size;
}

char * vex_profile_get_active_colour_scheme_name(VexProfile * vex_profile)
{
	return vex_profile -> active_colour_scheme;
}

VexColourScheme * vex_profile_get_active_colour_scheme(VexProfile * vex_profile)
{
	return vex_profile -> colour_scheme;
}

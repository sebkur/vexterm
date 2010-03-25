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

#ifndef _ABOUT_LOGO_H_
#define _ABOUT_LOGO_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo/cairo.h>
#include <pango/pango.h>

#define VEX_TYPE_ABOUT_LOGO           (about_logo_get_type ())
#define VEX_ABOUT_LOGO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_ABOUT_LOGO, AboutLogo))
#define VEX_ABOUT_LOGO_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_ABOUT_LOGO, AboutLogoClass))
#define VEX_IS_ABOUT_LOGO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_ABOUT_LOGO))
#define VEX_IS_ABOUT_LOGO_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_ABOUT_LOGO))
#define VEX_ABOUT_LOGO_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_ABOUT_LOGO, AboutLogoClass))

typedef struct _AboutLogo        AboutLogo;
typedef struct _AboutLogoClass   AboutLogoClass;

struct _AboutLogo
{
	GtkDrawingArea parent;

	cairo_pattern_t * pattern_logo;
	int logo_width;
	int logo_height;
};

struct _AboutLogoClass
{
	GtkDrawingAreaClass parent_class;

	//void (* function_name) (AboutLogo *about_logo);
};

GType about_logo_get_type (void) G_GNUC_CONST;

GtkWidget * about_logo_new();

#endif /* _ABOUT_LOGO_H_ */

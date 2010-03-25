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

#ifndef _VEX_SINGLE_H_
#define _VEX_SINGLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config/vex_profile.h"
#include "config/vex_colour_scheme.h"
#include "../libvexterm/terminal.h"
#include "../libvexterm/terminal_config.h"
#include "../libvexterm/terminal_debugger.h"
#include "../libvexterm/terminal_status_bar.h"
#include "../libvexterm/terminal_widget.h"

#define VEX_TYPE_VEX_SINGLE           (vex_single_get_type ())
#define VEX_VEX_SINGLE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_SINGLE, VexSingle))
#define VEX_VEX_SINGLE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_SINGLE, VexSingleClass))
#define VEX_IS_VEX_SINGLE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_SINGLE))
#define VEX_IS_VEX_SINGLE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_SINGLE))
#define VEX_VEX_SINGLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_SINGLE, VexSingleClass))

GType   vex_single_get_type (void) G_GNUC_CONST;

typedef struct _VexSingle        VexSingle;
typedef struct _VexSingleClass   VexSingleClass;

struct _VexSingle
{
	GtkVBox parent;

	Terminal * terminal;
	TerminalWidget * terminal_widget;
	TerminalStatusBar * status_bar;

	TerminalConfig * tc;
	VexProfile * profile;
	VexColourScheme * colour_scheme;
	gboolean show_status_bar;
	gboolean show_scrolling_region;
};

struct _VexSingleClass
{
	GtkVBoxClass parent_class;

	void (* exited) (VexSingle *vex_single);
};

GType vex_single_get_type (void) G_GNUC_CONST;

GtkWidget * vex_single_new(char * pwd, VexProfile * profile);

void vex_single_set_profile(VexSingle * vex_single, VexProfile * profile);
VexProfile * vex_single_get_profile(VexSingle * vex_single);

char * vex_single_get_pwd(VexSingle * vex_single);

gboolean vex_single_get_show_status_bar(VexSingle * vex_single);
void vex_single_set_show_status_bar(VexSingle * vex_single, gboolean show);

gboolean vex_single_get_show_scrolling_region(VexSingle * vex_single);
void vex_single_set_show_scrolling_region(VexSingle * vex_single, gboolean show);

#endif /* _VEX_SINGLE_H_ */

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

#ifndef _DEBUGGER_CONTROL_H_
#define _DEBUGGER_CONTROL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "terminal_debugger.h"

#define LIBVEX_TYPE_DEBUGGER_CONTROL           (debugger_control_get_type ())
#define LIBVEX_DEBUGGER_CONTROL(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_DEBUGGER_CONTROL, DebuggerControl))
#define LIBVEX_DEBUGGER_CONTROL_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_DEBUGGER_CONTROL, DebuggerControlClass))
#define LIBVEX_IS_DEBUGGER_CONTROL(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_DEBUGGER_CONTROL))
#define LIBVEX_IS_DEBUGGER_CONTROL_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_DEBUGGER_CONTROL))
#define LIBVEX_DEBUGGER_CONTROL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_DEBUGGER_CONTROL, DebuggerControlClass))

typedef struct _DebuggerControl        DebuggerControl;
typedef struct _DebuggerControlClass   DebuggerControlClass;

struct _DebuggerControl
{
	GtkVBox parent;

	TerminalDebugger * debugger;
};

struct _DebuggerControlClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (DebuggerControl *debugger_control);
};

GType debugger_control_get_type (void) G_GNUC_CONST;

GtkWidget * debugger_control_new(TerminalDebugger * debugger);

#endif /* _DEBUGGER_CONTROL_H_ */

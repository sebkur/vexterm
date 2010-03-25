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

#ifndef _TERMINAL_DEBUGGER_H_
#define _TERMINAL_DEBUGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "csi.h"
#include "terminal_handler.h"

#define LIBVEX_TYPE_TERMINAL_DEBUGGER           (terminal_debugger_get_type ())
#define LIBVEX_TERMINAL_DEBUGGER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_TERMINAL_DEBUGGER, TerminalDebugger))
#define LIBVEX_TERMINAL_DEBUGGER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_TERMINAL_DEBUGGER, TerminalDebuggerClass))
#define LIBVEX_IS_TERMINAL_DEBUGGER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_TERMINAL_DEBUGGER))
#define LIBVEX_IS_TERMINAL_DEBUGGER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_TERMINAL_DEBUGGER))
#define LIBVEX_TERMINAL_DEBUGGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_TERMINAL_DEBUGGER, TerminalDebuggerClass))

typedef struct _TerminalDebugger        TerminalDebugger;
typedef struct _TerminalDebuggerClass   TerminalDebuggerClass;

struct _TerminalDebugger
{
	GObject parent;

	TerminalHandler * handler;
	gboolean slow;
	gboolean printing;
};

struct _TerminalDebuggerClass
{
	GObjectClass parent_class;

	//void (* function_name) (TerminalDebugger *terminal_debugger);
};

GType terminal_debugger_get_type (void) G_GNUC_CONST;

TerminalDebugger * terminal_debugger_new();

void terminal_debugger_set_handler(TerminalDebugger * terminal_debugger, TerminalHandler * handler);

void terminal_debugger_set_slow(TerminalDebugger * terminal_debugger, gboolean slow);

void terminal_debugger_set_printing(TerminalDebugger * terminal_debugger, gboolean printing);

#endif /* _TERMINAL_DEBUGGER_H_ */

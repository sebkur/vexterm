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

#include "terminal_handler.h"

//enum
//{
//        LAST_SIGNAL
//};

//static guint terminal_handler_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, terminal_handler_signals[SIGNAL_NAME_n], 0);

static void terminal_handler_base_init (gpointer g_class)
{
	static gboolean is_initialized = FALSE;
	if (!is_initialized){
		/* add properties and signals to the interface here */
//		terminal_handler_signals[REQUEST_CLOSE] = g_signal_new(
//			"request-close",
//			LIBVEX_TYPE_TERMINAL_HANDLER,
//			G_SIGNAL_RUN_FIRST,
//			G_STRUCT_OFFSET (TerminalHandlerInterface, request_close),
//			NULL, NULL,
//			g_cclosure_marshal_VOID__VOID,
//			G_TYPE_NONE, 0);

		is_initialized = TRUE;
	}
}

GType terminal_handler_get_type (void)
{
	static GType iface_type = 0;
	if (iface_type == 0){
		static const GTypeInfo info = {
			sizeof (TerminalHandlerInterface),
			terminal_handler_base_init,   /* base_init */
			NULL,   /* base_finalize */
		};
		iface_type = g_type_register_static (G_TYPE_INTERFACE, "TerminalHandler",
		&info, 0);
	}
	return iface_type;
}

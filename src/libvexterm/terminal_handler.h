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

#ifndef _TERMINAL_HANDLER_H_
#define _TERMINAL_HANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib-object.h>

#include "csi.h"

#define LIBVEX_TYPE_TERMINAL_HANDLER           (terminal_handler_get_type ())
#define LIBVEX_TERMINAL_HANDLER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_TERMINAL_HANDLER, TerminalHandler))
#define LIBVEX_IS_TERMINAL_HANDLER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_TERMINAL_HANDLER))
#define LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), LIBVEX_TYPE_TERMINAL_HANDLER, TerminalHandlerInterface))

typedef struct _TerminalHandler        TerminalHandler;
typedef struct _TerminalHandlerInterface TerminalHandlerInterface;

struct _TerminalHandlerInterface
{
	GTypeInterface parent_iface;

	/* vtable */
	void (* handle_ascii) (TerminalHandler * terminal_handler, char c);
	void (* handle_utf8) (TerminalHandler * terminal_handler, gunichar uchar);
	void (* handle_csi) (TerminalHandler * terminal_handler, Csi * csi);
	void (* handle_escaped) (TerminalHandler * terminal_handler, char c);
	void (* set_charset) (TerminalHandler * terminal_handler, char c);
	void (* set_fd) (TerminalHandler * terminal_handler, int fd);
	void (* chunk_begin) (TerminalHandler * terminal_handler);
	void (* chunk_done) (TerminalHandler * terminal_handler);

	/* signals */
	//void (* request_close) (TerminalHandler *terminal_handler);
};

GType terminal_handler_get_type(void);

#endif /* _TERMINAL_HANDLER_H_ */

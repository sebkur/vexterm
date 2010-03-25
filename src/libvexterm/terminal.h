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

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "csi.h"
#include "terminal_handler.h"

#define LIBVEX_TYPE_TERMINAL           (terminal_get_type ())
#define LIBVEX_TERMINAL(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_TERMINAL, Terminal))
#define LIBVEX_TERMINAL_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_TERMINAL, TerminalClass))
#define LIBVEX_IS_TERMINAL(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_TERMINAL))
#define LIBVEX_IS_TERMINAL_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_TERMINAL))
#define LIBVEX_TERMINAL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_TERMINAL, TerminalClass))

typedef struct _Terminal        Terminal;
typedef struct _TerminalClass   TerminalClass;

struct _Terminal
{
	GObject parent;

	char * initial_pwd;

	TerminalHandler * handler;

	pthread_mutex_t mutex;

	int n_rows;
	int n_cols;

	int master;

	int state;
	Csi current_csi;

	int debug;
	gboolean dec_ckm;

	char * unhandled_data;
	int unhandled_data_len;

	pid_t pty_pid;
};

struct _TerminalClass
{
	GtkDrawingAreaClass parent_class;

	void (* pty_exited) (Terminal *terminal);
};

GType terminal_get_type (void) G_GNUC_CONST;

Terminal * terminal_new(char * pwd);

void terminal_start(Terminal * terminal);
void terminal_stop(Terminal * terminal);

void terminal_set_handler(Terminal * terminal, TerminalHandler * handler);

int terminal_get_master(Terminal * terminal);

char * terminal_get_pwd(Terminal * terminal);

#endif /* _TERMINAL_H_ */

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

 /* NOTES:
  #1: tab-charachters; erase in line; they ignore colour-inversion. (seen in xterm and gnome-terminal_debugger)
 */

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include "terminal_debugger.h"
#include "terminal_handler.h"
#include "../helpers/tool.h"
#include "../helpers/util.h"
#include "iso2022.h"

static void terminal_handler_interface_init (TerminalHandlerInterface *iface);

G_DEFINE_TYPE_WITH_CODE (TerminalDebugger, terminal_debugger, G_TYPE_OBJECT,
	G_IMPLEMENT_INTERFACE(LIBVEX_TYPE_TERMINAL_HANDLER, terminal_handler_interface_init));

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint terminal_debugger_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, terminal_debugger_signals[SIGNAL_NAME_n], 0);

void terminal_debugger_handle_ascii(TerminalHandler * terminal_handler, char c);
void terminal_debugger_handle_char(TerminalHandler * terminal_handler, gunichar uchar);
void terminal_debugger_handle_csi(TerminalHandler * terminal_handler, Csi * csi);
void terminal_debugger_handle_escaped(TerminalHandler * terminal_handler, char c);
void terminal_debugger_set_charset(TerminalHandler * terminal_handler, char c);
void terminal_debugger_set_fd(TerminalHandler * terminal_handler, int fd);
void terminal_debugger_chunk_begin(TerminalHandler * terminal_handler);
void terminal_debugger_chunk_done(TerminalHandler * terminal_handler);

TerminalDebugger * terminal_debugger_new()
{
	TerminalDebugger * terminal_debugger = g_object_new(LIBVEX_TYPE_TERMINAL_DEBUGGER, NULL);
	return terminal_debugger;
}

static void terminal_debugger_class_init(TerminalDebuggerClass *class)
{
        /*terminal_debugger_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalDebuggerClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void terminal_handler_interface_init (TerminalHandlerInterface * iface)
{
	iface -> handle_ascii = terminal_debugger_handle_ascii;
	iface -> handle_utf8 = terminal_debugger_handle_char;
	iface -> handle_csi = terminal_debugger_handle_csi;
	iface -> handle_escaped = terminal_debugger_handle_escaped;
	iface -> set_charset = terminal_debugger_set_charset;
	iface -> set_fd = terminal_debugger_set_fd;
	iface -> chunk_done = terminal_debugger_chunk_begin;
	iface -> chunk_done = terminal_debugger_chunk_done;
}

static void terminal_debugger_init(TerminalDebugger *terminal_debugger)
{
}

void terminal_debugger_set_handler(TerminalDebugger * terminal_debugger, TerminalHandler * handler)
{
	terminal_debugger -> handler = handler;
}

/****************************************************************
Interface Functions
****************************************************************/

#define PAUSE 500

void terminal_debugger_handle_char(TerminalHandler * terminal_handler, gunichar uchar)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> handle_utf8(terminal_debugger -> handler, uchar);
}

void terminal_debugger_handle_csi(TerminalHandler * terminal_handler, Csi * csi)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	if (terminal_debugger -> printing) {
		if (csi -> suffix1 != 'm')
		printf("|csi %c:%c| ", csi -> prefix, csi -> suffix1); fflush(NULL);
	}
	//if (csi -> suffix1 == 'K') return;
	if (terminal_debugger -> slow) usleep(PAUSE);
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> handle_csi(terminal_debugger -> handler, csi);
}

void terminal_debugger_handle_escaped(TerminalHandler * terminal_handler, char c)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	if (terminal_debugger -> printing) {
		printf("|esc %c| ", c); fflush(NULL);
	}
	if (terminal_debugger -> slow) usleep(PAUSE);
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> handle_escaped(terminal_debugger -> handler, c);
}

void terminal_debugger_handle_ascii(TerminalHandler * terminal_handler, char c)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	if (terminal_debugger -> printing) {
		if (c == ' ') {
			printf("<_> ");
		}else if (c == '\n'){
			printf("<\\n>");
		}else if (c == '\r'){
			printf("<\\r>");
		}else{
			printf("<%c> ", c); fflush(NULL);
		}
	}
	if (terminal_debugger -> slow) usleep(PAUSE);
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> handle_ascii(terminal_debugger -> handler, c);
}

void terminal_debugger_set_charset(TerminalHandler * terminal_handler, char c)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> set_charset(terminal_debugger -> handler, c);
}

void terminal_debugger_set_fd(TerminalHandler * terminal_handler, int fd)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> set_fd(terminal_debugger -> handler, fd);
}

void terminal_debugger_chunk_begin(TerminalHandler * terminal_handler)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> chunk_begin(terminal_debugger -> handler);
}

void terminal_debugger_chunk_done(TerminalHandler * terminal_handler)
{
	TerminalDebugger * terminal_debugger = (TerminalDebugger*) terminal_handler;
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal_debugger -> handler);
	class -> chunk_done(terminal_debugger -> handler);
}

// control

void terminal_debugger_set_slow(TerminalDebugger * terminal_debugger, gboolean slow)
{
	terminal_debugger -> slow = slow;
}

void terminal_debugger_set_printing(TerminalDebugger * terminal_debugger, gboolean printing)
{
	terminal_debugger -> printing = printing;
}
